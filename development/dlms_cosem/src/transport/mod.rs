//! Transport profiles for DLMS/COSEM integrations.

pub mod framing;
pub mod hdlc;
pub mod wrapper;

/// Supported link layer profiles.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Profile {
    /// HDLC serial link.
    Hdlc,
    /// WRAPPER profile used on TCP/UDP.
    Wrapper,
    /// PLC transports such as G3-PLC.
    Plc,
}

impl Profile {
    /// Checks whether the profile is permitted by the current feature set.
    pub const fn is_available(self) -> bool {
        match self {
            Profile::Hdlc => !cfg!(feature = "dlms_ignore_hdlc"),
            Profile::Wrapper => !cfg!(feature = "dlms_ignore_wrapper"),
            Profile::Plc => !cfg!(feature = "dlms_ignore_plc"),
        }
    }
}
