//! Safe wrappers around P-256/P-384 ECDSA operations.

use p256::ecdsa::{
    DerSignature as P256DerSignature, Signature as P256Signature, SigningKey as P256SigningKey,
    VerifyingKey as P256VerifyingKey,
};
use p384::ecdsa::{
    DerSignature as P384DerSignature, Signature as P384Signature, SigningKey as P384SigningKey,
    VerifyingKey as P384VerifyingKey,
};
use rand_core::{CryptoRng, RngCore};
use sha2::{Digest as Sha2Digest, Sha256};
use sha3::{Digest as Sha3Digest, Sha3_384};
use signature::hazmat::{PrehashVerifier, RandomizedPrehashSigner};

use crate::error::{Error, Result};
use crate::types::SecuritySuite;

const P256_KEY_LENGTH: &[usize] = &[32];
const P384_KEY_LENGTH: &[usize] = &[48];

fn digest_for_suite(suite: SecuritySuite, message: &[u8]) -> Result<Vec<u8>> {
    match suite {
        SecuritySuite::V1 => {
            let mut hasher = Sha256::new();
            Sha2Digest::update(&mut hasher, message);
            Ok(hasher.finalize().to_vec())
        }
        SecuritySuite::V2 => {
            let mut hasher = Sha3_384::new();
            Sha3Digest::update(&mut hasher, message);
            Ok(hasher.finalize().to_vec())
        }
        SecuritySuite::V0 => Err(Error::UnsupportedSecuritySuite(u8::from(suite))),
    }
}

fn ensure_key_length(expected: &'static [usize], actual: usize) -> Result<()> {
    if expected.contains(&actual) {
        Ok(())
    } else {
        Err(Error::InvalidKeyLength { expected, actual })
    }
}

/// Signs a message using the suite specific hash function and curve.
pub fn sign_message<R>(
    suite: SecuritySuite,
    private_key: &[u8],
    message: &[u8],
    rng: &mut R,
) -> Result<Vec<u8>>
where
    R: RngCore + CryptoRng,
{
    let digest = digest_for_suite(suite, message)?;
    match suite {
        SecuritySuite::V1 => {
            ensure_key_length(P256_KEY_LENGTH, private_key.len())?;
            let signing_key =
                P256SigningKey::from_slice(private_key).map_err(|_| Error::SigningFailed)?;
            let signature: P256Signature = signing_key
                .sign_prehash_with_rng(rng, &digest)
                .map_err(|_| Error::SigningFailed)?;
            let der: P256DerSignature = signature.into();
            Ok(der.as_bytes().to_vec())
        }
        SecuritySuite::V2 => {
            ensure_key_length(P384_KEY_LENGTH, private_key.len())?;
            let signing_key =
                P384SigningKey::from_slice(private_key).map_err(|_| Error::SigningFailed)?;
            let signature: P384Signature = signing_key
                .sign_prehash_with_rng(rng, &digest)
                .map_err(|_| Error::SigningFailed)?;
            let der: P384DerSignature = signature.into();
            Ok(der.as_bytes().to_vec())
        }
        SecuritySuite::V0 => Err(Error::UnsupportedSecuritySuite(u8::from(suite))),
    }
}

/// Verifies a DER encoded signature against the provided public key.
pub fn verify_message(
    suite: SecuritySuite,
    public_key: &[u8],
    message: &[u8],
    signature: &[u8],
) -> Result<()> {
    let digest = digest_for_suite(suite, message)?;
    match suite {
        SecuritySuite::V1 => {
            let verifying_key = P256VerifyingKey::from_sec1_bytes(public_key)
                .map_err(|_| Error::SignatureVerificationFailed)?;
            let signature = P256Signature::from_der(signature)
                .map_err(|_| Error::SignatureVerificationFailed)?;
            verifying_key
                .verify_prehash(&digest, &signature)
                .map_err(|_| Error::SignatureVerificationFailed)
        }
        SecuritySuite::V2 => {
            let verifying_key = P384VerifyingKey::from_sec1_bytes(public_key)
                .map_err(|_| Error::SignatureVerificationFailed)?;
            let signature = P384Signature::from_der(signature)
                .map_err(|_| Error::SignatureVerificationFailed)?;
            verifying_key
                .verify_prehash(&digest, &signature)
                .map_err(|_| Error::SignatureVerificationFailed)
        }
        SecuritySuite::V0 => Err(Error::UnsupportedSecuritySuite(u8::from(suite))),
    }
}

/// Serialises a P-256 public key to uncompressed SEC1 representation.
pub fn public_key_to_sec1_p256(verifying_key: &P256VerifyingKey) -> Vec<u8> {
    verifying_key.to_encoded_point(false).as_bytes().to_vec()
}

/// Serialises a P-384 public key to uncompressed SEC1 representation.
pub fn public_key_to_sec1_p384(verifying_key: &P384VerifyingKey) -> Vec<u8> {
    verifying_key.to_encoded_point(false).as_bytes().to_vec()
}
