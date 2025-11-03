//! Error definitions shared across the safe DLMS/COSEM helpers.

use core::fmt;

use crate::types::Command;

/// Convenience result type for DLMS/COSEM operations.
pub type Result<T> = core::result::Result<T, Error>;

/// Errors that can be emitted by the safe Rust re-implementation of the
/// DLMS/COSEM helpers.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Error {
    /// Attempted to access or reserve more space than available in a buffer.
    BufferTooSmall {
        /// Number of bytes that were requested.
        requested: usize,
        /// Number of bytes that were actually available.
        available: usize,
    },
    /// Encountered a HDLC address that cannot be represented on the link layer.
    InvalidAddress(u32),
    /// Encountered a frame that violates the profile specific rules.
    InvalidFrame(&'static str),
    /// Unexpected end of data while decoding.
    UnexpectedEof,
    /// Encountered an unknown or unsupported DLMS data type tag.
    UnknownDataType(u8),
    /// Reported when a payload exceeds the negotiated limits.
    PayloadTooLarge {
        /// Maximum allowed payload size.
        max: usize,
        /// Actual payload length.
        actual: usize,
    },
    /// Encountered a cryptographic key with an unsupported length.
    InvalidKeyLength {
        /// Supported key sizes in bytes.
        expected: &'static [usize],
        /// Actual key length encountered.
        actual: usize,
    },
    /// Encountered an authentication tag with an unexpected length.
    InvalidTagLength {
        /// Supported tag sizes in bytes.
        expected: &'static [usize],
        /// Actual tag length encountered.
        actual: usize,
    },
    /// Authentication tag verification failed.
    AuthenticationFailed,
    /// Encryption failed due to invalid inputs.
    EncryptionFailed,
    /// Signing failed because the provided key or message was invalid.
    SigningFailed,
    /// Signature verification failed.
    SignatureVerificationFailed,
    /// The requested security suite is not supported by the active configuration.
    UnsupportedSecuritySuite(u8),
    /// Server or client configuration violated fundamental expectations.
    InvalidConfiguration(&'static str),
    /// Encountered a DLMS command that is not implemented by the safe bindings.
    UnsupportedCommand(Command),
    /// A parsing error occurred.
    Parse,
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Error::BufferTooSmall {
                requested,
                available,
            } => write!(
                f,
                "buffer too small: requested {} bytes but only {} available",
                requested, available
            ),
            Error::InvalidAddress(value) => write!(f, "invalid HDLC address {:X}", value),
            Error::InvalidFrame(reason) => write!(f, "invalid frame: {}", reason),
            Error::UnexpectedEof => write!(f, "unexpected end of buffer"),
            Error::UnknownDataType(tag) => write!(f, "unknown DLMS data type tag 0x{tag:02X}"),
            Error::PayloadTooLarge { max, actual } => {
                write!(f, "payload too large: {actual} bytes (max {max})")
            }
            Error::InvalidKeyLength { expected, actual } => {
                write!(
                    f,
                    "invalid key length {actual} bytes (expected one of {})",
                    expected
                        .iter()
                        .map(|len| len.to_string())
                        .collect::<Vec<_>>()
                        .join(", ")
                )
            }
            Error::InvalidTagLength { expected, actual } => {
                write!(
                    f,
                    "invalid authentication tag length {actual} bytes (expected one of {})",
                    expected
                        .iter()
                        .map(|len| len.to_string())
                        .collect::<Vec<_>>()
                        .join(", ")
                )
            }
            Error::AuthenticationFailed => write!(f, "authentication failed"),
            Error::EncryptionFailed => write!(f, "encryption failed"),
            Error::SigningFailed => write!(f, "failed to create signature"),
            Error::SignatureVerificationFailed => {
                write!(f, "failed to verify signature")
            }
            Error::UnsupportedSecuritySuite(value) => {
                write!(f, "unsupported security suite {value}")
            }
            Error::InvalidConfiguration(reason) => {
                write!(f, "invalid configuration: {reason}")
            }
            Error::UnsupportedCommand(command) => {
                write!(f, "unsupported command {:?}", command)
            }
            Error::Parse => write!(f, "parsing error"),
        }
    }
}

impl core::error::Error for Error {}
