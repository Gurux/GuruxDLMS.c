//! Server side helpers for exposing DLMS/COSEM objects.

/// Placeholder description of a DLMS/COSEM logical device served by Gurux stacks.
#[derive(Debug, Default)]
pub struct LogicalDevice {
    /// Optional name for documentation and diagnostics.
    pub name: Option<&'static str>,
    /// Indicates that persistent storage is required by the device.
    pub requires_persistence: bool,
}

impl LogicalDevice {
    /// Checks whether the logical device should be compiled into the binary.
    pub const fn is_available() -> bool {
        !cfg!(feature = "dlms_ignore_server")
    }
}
