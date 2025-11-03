//! WRAPPER transport profile.

use crate::error::{Error, Result};

/// WRAPPER frame.
#[derive(Debug)]
pub struct WrapperFrame<'a> {
    /// Client address.
    pub client_address: u16,
    /// Server address.
    pub server_address: u16,
    /// Frame data.
    pub data: &'a [u8],
}

impl<'a> WrapperFrame<'a> {
    /// Creates a new `WrapperFrame`.
    pub fn new(client_address: u16, server_address: u16, data: &'a [u8]) -> Self {
        Self {
            client_address,
            server_address,
            data,
        }
    }

    /// Converts the `WrapperFrame` to bytes.
    pub fn to_bytes(&self, buf: &mut [u8]) -> Result<usize> {
        let len = 8 + self.data.len();
        if buf.len() < len {
            return Err(Error::BufferTooSmall {
                requested: len,
                available: buf.len(),
            });
        }

        buf[0..2].copy_from_slice(&1u16.to_be_bytes());
        buf[2..4].copy_from_slice(&self.client_address.to_be_bytes());
        buf[4..6].copy_from_slice(&self.server_address.to_be_bytes());
        buf[6..8].copy_from_slice(&(self.data.len() as u16).to_be_bytes());
        buf[8..len].copy_from_slice(self.data);

        Ok(len)
    }

    /// Creates a `WrapperFrame` from bytes.
    pub fn from_bytes(buf: &'a [u8]) -> Result<Self> {
        if buf.len() < 8 {
            return Err(Error::InvalidFrame("Invalid WRAPPER frame format"));
        }

        let version = u16::from_be_bytes(buf[0..2].try_into().unwrap());
        if version != 1 {
            return Err(Error::InvalidFrame("Unsupported WRAPPER version"));
        }

        let client_address = u16::from_be_bytes(buf[2..4].try_into().unwrap());
        let server_address = u16::from_be_bytes(buf[4..6].try_into().unwrap());
        let data_len = u16::from_be_bytes(buf[6..8].try_into().unwrap()) as usize;

        if buf.len() != 8 + data_len {
            return Err(Error::InvalidFrame("Invalid WRAPPER frame length"));
        }

        let data = &buf[8..8 + data_len];

        Ok(Self {
            client_address,
            server_address,
            data,
        })
    }
}
