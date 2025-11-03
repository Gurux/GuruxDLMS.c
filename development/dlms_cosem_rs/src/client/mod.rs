//! Client side abstractions for DLMS/COSEM messaging.

/// Configuration options for the high level client interface.
#[derive(Debug, Default, Clone)]
pub struct ClientOptions {
    /// Enables security context negotiation when available.
    pub secure: bool,
}

impl ClientOptions {
    /// Returns `true` when the legacy build configuration excludes client logic.
    pub const fn is_disabled() -> bool {
        cfg!(feature = "dlms_ignore_client")
    }
}

/// Placeholder type for the high level client façade.
#[derive(Debug, Default)]
pub struct Client {
    options: ClientOptions,
}

impl Client {
    /// Creates a new client facade with the provided options.
    pub const fn new(options: ClientOptions) -> Self {
        Self { options }
    }

    /// Reports whether secure messaging is requested for the client instance.
    pub const fn requires_security(&self) -> bool {
        self.options.secure && !cfg!(feature = "dlms_ignore_security_setup")
    }
}
