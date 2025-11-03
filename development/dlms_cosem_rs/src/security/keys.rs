//! Key derivation and random number generation helpers.

use digest::Digest;
use rand::rngs::OsRng;
use rand_core::{CryptoRng, RngCore};
use sha2::Sha256;
use sha3::Sha3_384;

use crate::types::SecuritySuite;

/// Algorithms supported by the deterministic key derivation helper.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum KeyDerivationAlgorithm {
    /// Derive keys using SHA-256 in counter mode.
    Sha256,
    /// Derive keys using SHA3-384 in counter mode.
    Sha3_384,
}

fn derive_with_digest<D>(key_material: &[u8], context: &[u8], output_len: usize) -> Vec<u8>
where
    D: Digest + Default,
{
    let mut output = Vec::with_capacity(output_len);
    let mut counter: u32 = 1;
    while output.len() < output_len {
        let mut hasher = D::new();
        hasher.update(counter.to_be_bytes());
        hasher.update(key_material);
        hasher.update(context);
        let block = hasher.finalize();
        output.extend_from_slice(&block);
        counter += 1;
    }
    output.truncate(output_len);
    output
}

/// Derives deterministic key material using the selected hash algorithm.
pub fn derive_key(
    algorithm: KeyDerivationAlgorithm,
    key_material: &[u8],
    context: &[u8],
    output_len: usize,
) -> Vec<u8> {
    match algorithm {
        KeyDerivationAlgorithm::Sha256 => {
            derive_with_digest::<Sha256>(key_material, context, output_len)
        }
        KeyDerivationAlgorithm::Sha3_384 => {
            derive_with_digest::<Sha3_384>(key_material, context, output_len)
        }
    }
}

/// Chooses the appropriate key derivation algorithm for a security suite.
pub fn derive_suite_key(
    suite: SecuritySuite,
    key_material: &[u8],
    context: &[u8],
    output_len: usize,
) -> Vec<u8> {
    let algorithm = match suite {
        SecuritySuite::V0 | SecuritySuite::V1 => KeyDerivationAlgorithm::Sha256,
        SecuritySuite::V2 => KeyDerivationAlgorithm::Sha3_384,
    };
    derive_key(algorithm, key_material, context, output_len)
}

/// Thin wrapper around [`rand_core::RngCore`] with an API tailored for DLMS needs.
#[derive(Debug)]
pub struct RandomGenerator<R>
where
    R: RngCore + CryptoRng,
{
    rng: R,
}

impl<R> RandomGenerator<R>
where
    R: RngCore + CryptoRng,
{
    /// Creates a new random generator from an arbitrary RNG implementation.
    pub fn new(rng: R) -> Self {
        Self { rng }
    }

    /// Fills the provided slice with random bytes.
    pub fn fill_bytes(&mut self, dest: &mut [u8]) {
        self.rng.fill_bytes(dest);
    }

    /// Generates a random fixed-size byte array.
    pub fn generate_array<const N: usize>(&mut self) -> [u8; N] {
        let mut buf = [0u8; N];
        self.fill_bytes(&mut buf);
        buf
    }

    /// Returns the next random 32-bit integer.
    pub fn next_u32(&mut self) -> u32 {
        self.rng.next_u32()
    }
}

/// Creates a random generator backed by the operating system RNG.
pub fn system_random_generator() -> RandomGenerator<OsRng> {
    RandomGenerator::new(OsRng)
}
