use aes_gcm::Aes256Gcm;
use aes_gcm::aead::{Aead, KeyInit};
use alloc::vec::Vec;

pub fn encrypt(key: &[u8], nonce: &[u8], plaintext: &[u8], _aad: &[u8]) -> Result<Vec<u8>, &'static str> {
    let cipher = Aes256Gcm::new(key.into());
    let nonce = nonce.into();
    cipher.encrypt(nonce, plaintext)
        .map_err(|_| "Encryption failed")
}

pub fn decrypt(key: &[u8], nonce: &[u8], ciphertext: &[u8], _aad: &[u8]) -> Result<Vec<u8>, &'static str> {
    let cipher = Aes256Gcm::new(key.into());
    let nonce = nonce.into();
    cipher.decrypt(nonce, ciphertext)
        .map_err(|_| "Decryption failed")
}
