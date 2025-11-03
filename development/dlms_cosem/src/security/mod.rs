//! Security primitives shared by the client and server stacks.

pub mod aes_gcm;
pub mod ecdsa;
pub mod high_gmac;
pub mod keys;

pub use crate::types::SecuritySuite;
pub use aes_gcm::{
    append_authenticated_payload, build_authenticated_data, decrypt_payload, encrypt_payload,
    nonce_from, security_control_byte, AUTH_TAG_LEN,
};
pub use ecdsa::{public_key_to_sec1_p256, public_key_to_sec1_p384, sign_message, verify_message};
pub use keys::{
    derive_key, derive_suite_key, system_random_generator, KeyDerivationAlgorithm, RandomGenerator,
};

/// Security profiles grouped by the required feature set.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SecurityProfile {
    /// Basic authentication without link-layer encryption.
    Low,
    /// High security suite applying authenticated encryption.
    HighGmac,
    /// High security suite applying elliptic curve signatures.
    HighEcdsa,
}

impl SecurityProfile {
    /// Returns `true` when the selected suite requires features that are disabled.
    pub const fn is_conflicting(self) -> bool {
        match self {
            SecurityProfile::Low => false,
            SecurityProfile::HighGmac => cfg!(feature = "dlms_ignore_high_gmac"),
            SecurityProfile::HighEcdsa => cfg!(feature = "dlms_ignore_high_ecdsa"),
        }
    }
}

/// Indicates whether any security material should be compiled into the crate.
pub const fn is_enabled() -> bool {
    !cfg!(feature = "dlms_ignore_security_setup")
}
