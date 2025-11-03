//! DLMS/COSEM object metadata and utilities.

pub mod cosem;

/// Common object identification used throughout DLMS/COSEM stacks.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct ObisCode(pub u8, pub u8, pub u8, pub u8, pub u8, pub u8);

impl ObisCode {
    /// Checks whether profile generic objects are disabled in the current configuration.
    pub const fn profile_generic_disabled() -> bool {
        cfg!(feature = "dlms_ignore_profile_generic")
    }

    /// Creates an OBIS code filled with zeros.
    pub const fn zero() -> Self {
        Self(0, 0, 0, 0, 0, 0)
    }

    /// Returns the canonical byte representation of the OBIS code.
    pub const fn to_bytes(self) -> [u8; 6] {
        [self.0, self.1, self.2, self.3, self.4, self.5]
    }

    /// Returns whether the OBIS code does not contain any identifying information.
    pub const fn is_zero(&self) -> bool {
        self.0 == 0 && self.1 == 0 && self.2 == 0 && self.3 == 0 && self.4 == 0 && self.5 == 0
    }
}

impl From<[u8; 6]> for ObisCode {
    fn from(value: [u8; 6]) -> Self {
        Self(value[0], value[1], value[2], value[3], value[4], value[5])
    }
}

impl From<ObisCode> for [u8; 6] {
    fn from(value: ObisCode) -> Self {
        value.to_bytes()
    }
}
