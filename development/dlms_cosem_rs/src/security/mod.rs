//! Security primitives shared by the client and server stacks.

/// High level security suites supported by the Gurux ecosystem.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SecuritySuite {
    /// Basic authentication without link-layer encryption.
    Low,
    /// High security suite applying authenticated encryption.
    HighGmac,
    /// High security suite applying elliptic curve signatures.
    HighEcdsa,
}

impl SecuritySuite {
    /// Returns `true` when the selected suite requires features that are disabled.
    pub const fn is_conflicting(self) -> bool {
        match self {
            SecuritySuite::Low => false,
            SecuritySuite::HighGmac => cfg!(feature = "dlms_ignore_high_gmac"),
            SecuritySuite::HighEcdsa => cfg!(feature = "dlms_ignore_high_ecdsa"),
        }
    }
}

/// Indicates whether any security material should be compiled into the crate.
pub const fn is_enabled() -> bool {
    !cfg!(feature = "dlms_ignore_security_setup")
}
