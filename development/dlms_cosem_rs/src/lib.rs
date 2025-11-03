#![doc = "Core crates for building DLMS/COSEM compatible applications in Rust."]
#![warn(missing_docs)]
#![cfg_attr(docsrs, feature(doc_cfg))]

pub mod client;
pub mod objects;
pub mod security;
pub mod server;
pub mod transport;

/// Feature flag helpers mirroring the legacy `DLMS_IGNORE_*` defines.
pub mod features {
    /// Legacy feature toggle that hides large parts of the classic C client implementation.
    pub const IGNORE_CLIENT: bool = cfg!(feature = "dlms_ignore_client");
    /// Legacy feature toggle that hides the embedded server implementation.
    pub const IGNORE_SERVER: bool = cfg!(feature = "dlms_ignore_server");
    /// Indicates whether memory allocation support should be compiled out.
    pub const IGNORE_MALLOC: bool = cfg!(feature = "dlms_ignore_malloc");
    /// Indicates whether object pointer helpers are removed.
    pub const IGNORE_OBJECT_POINTERS: bool = cfg!(feature = "dlms_ignore_object_pointers");
    /// Indicates whether the security setup objects are skipped.
    pub const IGNORE_SECURITY_SETUP: bool = cfg!(feature = "dlms_ignore_security_setup");
    /// Indicates whether advanced ciphering support is disabled.
    pub const IGNORE_GENERAL_CIPHERING: bool = cfg!(feature = "dlms_ignore_general_ciphering");
}

/// Representation of a Gurux DLMS/COSEM stack assembled from feature gated modules.
#[derive(Debug, Default)]
pub struct Stack {
    /// Tracks whether client functionality is part of the current build.
    pub client_enabled: bool,
    /// Tracks whether server functionality is part of the current build.
    pub server_enabled: bool,
    /// Tracks whether security layers are available.
    pub security_enabled: bool,
}

impl Stack {
    /// Creates a new stack description reflecting the active Cargo feature flags.
    pub fn detect() -> Self {
        Self {
            client_enabled: !features::IGNORE_CLIENT,
            server_enabled: !features::IGNORE_SERVER,
            security_enabled: !features::IGNORE_SECURITY_SETUP,
        }
    }
}
