//! DLMS/COSEM object metadata and utilities.

/// Common object identification used throughout DLMS/COSEM stacks.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct ObisCode(pub u8, pub u8, pub u8, pub u8, pub u8, pub u8);

impl ObisCode {
    /// Checks whether profile generic objects are disabled in the current configuration.
    pub const fn profile_generic_disabled() -> bool {
        cfg!(feature = "dlms_ignore_profile_generic")
    }
}
