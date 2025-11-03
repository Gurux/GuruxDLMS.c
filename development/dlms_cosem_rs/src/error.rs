//! Error definitions shared across the safe DLMS/COSEM helpers.

use core::fmt;

/// Convenience result type for DLMS/COSEM operations.
pub type Result<T> = core::result::Result<T, Error>;

/// Errors that can be emitted by the safe Rust re-implementation of the
/// Gurux DLMS/COSEM helpers.
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
        }
    }
}

impl core::error::Error for Error {}
