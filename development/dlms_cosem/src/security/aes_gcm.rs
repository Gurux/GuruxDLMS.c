//! High level AES-GCM helpers mirroring the behaviour of the legacy C helpers.

use aes_gcm::{
    aead::{AeadInPlace, KeyInit},
    Aes128Gcm, Aes256Gcm, Nonce, Tag,
};

use crate::error::{Error, Result};
use crate::types::{ByteBuffer, Security, SecuritySuite};

/// Length of authentication tags used by DLMS/COSEM profiles.
pub const AUTH_TAG_LEN: usize = 12;

const SUPPORTED_KEY_LENGTHS: &[usize] = &[16, 32];
const SUPPORTED_TAG_LENGTHS: &[usize] = &[12, 16];

/// Calculates the security control byte that prefixes every secured APDU.
pub fn security_control_byte(broadcast: bool, security: Security, suite: SecuritySuite) -> u8 {
    let mut value = security.bits();
    value |= u8::from(suite);
    if broadcast {
        value |= 0x40;
    }
    value
}

/// Builds the 96-bit nonce combining the system title and invocation counter.
pub fn nonce_from(system_title: &[u8; 8], invocation_counter: u32) -> [u8; 12] {
    let mut nonce = [0u8; 12];
    nonce[..8].copy_from_slice(system_title);
    nonce[8..].copy_from_slice(&invocation_counter.to_be_bytes());
    nonce
}

/// Builds the additional authenticated data used by AES-GCM.
pub fn build_authenticated_data(
    control_byte: u8,
    authentication_key: &[u8],
    security: Security,
    payload: &[u8],
) -> Vec<u8> {
    let mut aad = Vec::with_capacity(1 + authentication_key.len() + payload.len());
    aad.push(control_byte);
    aad.extend_from_slice(authentication_key);
    if security == Security::AUTHENTICATION {
        aad.extend_from_slice(payload);
    }
    aad
}

/// Helper that appends the authenticated payload to the provided buffer following the DLMS rules.
pub fn append_authenticated_payload(
    buffer: &mut ByteBuffer,
    control_byte: u8,
    invocation_counter: u32,
    ciphertext: &[u8],
    tag: &[u8; AUTH_TAG_LEN],
) {
    buffer.push(control_byte);
    buffer.push_u32(invocation_counter);
    buffer.extend_from_slice(ciphertext);
    buffer.extend_from_slice(tag);
}

enum CipherVariant {
    Aes128(Box<Aes128Gcm>),
    Aes256(Box<Aes256Gcm>),
}

impl CipherVariant {
    fn new(key: &[u8]) -> Result<Self> {
        match key.len() {
            16 => Aes128Gcm::new_from_slice(key)
                .map(|cipher| Self::Aes128(Box::new(cipher)))
                .map_err(|_| Error::InvalidKeyLength {
                    expected: SUPPORTED_KEY_LENGTHS,
                    actual: key.len(),
                }),
            32 => Aes256Gcm::new_from_slice(key)
                .map(|cipher| Self::Aes256(Box::new(cipher)))
                .map_err(|_| Error::InvalidKeyLength {
                    expected: SUPPORTED_KEY_LENGTHS,
                    actual: key.len(),
                }),
            actual => Err(Error::InvalidKeyLength {
                expected: SUPPORTED_KEY_LENGTHS,
                actual,
            }),
        }
    }

    fn encrypt_detached(
        &self,
        nonce: &[u8; 12],
        aad: &[u8],
        payload: &mut [u8],
    ) -> core::result::Result<Tag, aes_gcm::Error> {
        let nonce = Nonce::from(*nonce);
        match self {
            CipherVariant::Aes128(cipher) => cipher.encrypt_in_place_detached(&nonce, aad, payload),
            CipherVariant::Aes256(cipher) => cipher.encrypt_in_place_detached(&nonce, aad, payload),
        }
    }

    fn decrypt_detached(
        &self,
        nonce: &[u8; 12],
        aad: &[u8],
        payload: &mut [u8],
        tag: &Tag,
    ) -> core::result::Result<(), aes_gcm::Error> {
        let nonce = Nonce::from(*nonce);
        match self {
            CipherVariant::Aes128(cipher) => {
                cipher.decrypt_in_place_detached(&nonce, aad, payload, tag)
            }
            CipherVariant::Aes256(cipher) => {
                cipher.decrypt_in_place_detached(&nonce, aad, payload, tag)
            }
        }
    }
}

fn expand_tag(tag: &[u8]) -> Result<[u8; 16]> {
    match tag.len() {
        12 => {
            let mut buf = [0u8; 16];
            buf[..12].copy_from_slice(tag);
            Ok(buf)
        }
        16 => {
            let mut buf = [0u8; 16];
            buf.copy_from_slice(tag);
            Ok(buf)
        }
        actual => Err(Error::InvalidTagLength {
            expected: SUPPORTED_TAG_LENGTHS,
            actual,
        }),
    }
}

fn truncate_tag(tag: Tag) -> [u8; AUTH_TAG_LEN] {
    let mut truncated = [0u8; AUTH_TAG_LEN];
    truncated.copy_from_slice(&tag[..AUTH_TAG_LEN]);
    truncated
}

/// Encrypts the provided payload in-place and returns the authentication tag.
#[allow(clippy::too_many_arguments)]
pub fn encrypt_payload(
    key: &[u8],
    security: Security,
    suite: SecuritySuite,
    broadcast: bool,
    authentication_key: &[u8],
    system_title: &[u8; 8],
    invocation_counter: u32,
    payload: &mut [u8],
) -> Result<(u8, [u8; AUTH_TAG_LEN])> {
    let control_byte = security_control_byte(broadcast, security, suite);
    let nonce = nonce_from(system_title, invocation_counter);
    let aad = build_authenticated_data(control_byte, authentication_key, security, payload);
    let cipher = CipherVariant::new(key)?;
    let do_encrypt = security.contains(Security::ENCRYPTION);
    let tag = if do_encrypt {
        cipher
            .encrypt_detached(&nonce, &aad, payload)
            .map_err(|_| Error::EncryptionFailed)?
    } else {
        let mut empty: [u8; 0] = [];
        cipher
            .encrypt_detached(&nonce, &aad, &mut empty)
            .map_err(|_| Error::EncryptionFailed)?
    };
    Ok((control_byte, truncate_tag(tag)))
}

/// Decrypts the payload in-place while verifying the provided authentication tag.
#[allow(clippy::too_many_arguments)]
pub fn decrypt_payload(
    key: &[u8],
    security: Security,
    control_byte: u8,
    authentication_key: &[u8],
    system_title: &[u8; 8],
    invocation_counter: u32,
    payload: &mut [u8],
    tag: &[u8],
) -> Result<()> {
    let nonce = nonce_from(system_title, invocation_counter);
    let aad = build_authenticated_data(control_byte, authentication_key, security, payload);
    let cipher = CipherVariant::new(key)?;
    let tag_bytes = expand_tag(tag)?;
    let tag = Tag::from(tag_bytes);
    let do_encrypt = security.contains(Security::ENCRYPTION);
    if do_encrypt {
        cipher
            .decrypt_detached(&nonce, &aad, payload, &tag)
            .map_err(|_| Error::AuthenticationFailed)
    } else {
        let mut empty: [u8; 0] = [];
        cipher
            .decrypt_detached(&nonce, &aad, &mut empty, &tag)
            .map_err(|_| Error::AuthenticationFailed)
    }
}
