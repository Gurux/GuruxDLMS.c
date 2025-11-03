//! HDLC transport profile.

use crate::crc;
use crate::error::{Error, Result};

const HDLC_FRAME_START_END: u8 = 0x7E;

/// Checks if the given interface type uses HDLC.
pub fn uses_hdlc(interface_type: u8) -> bool {
    // Corresponds to DLMS_INTERFACE_TYPE_HDLC, DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E,
    // and DLMS_INTERFACE_TYPE_PLC_HDLC in the C library.
    matches!(interface_type, 1 | 6 | 8)
}

/// Parses an HDLC address from the given buffer.
pub fn get_hdlc_address(buf: &mut &[u8]) -> Result<u32> {
    let mut address = 0;
    let mut len = 0;
    for &byte in buf.iter() {
        len += 1;
        if (byte & 0x01) == 1 {
            break;
        }
    }

    if len == 0 || len > 4 || len > buf.len() {
        return Err(Error::Parse);
    }

    let (address_bytes, rest) = buf.split_at(len);
    *buf = rest;

    match len {
        1 => address = (address_bytes[0] as u32 & 0xFE) >> 1,
        2 => {
            let val = u16::from_be_bytes(address_bytes.try_into().unwrap());
            address = ((val & 0xFE) >> 1 | (val & 0xFE00) >> 2) as u32;
        }
        4 => {
            let val = u32::from_be_bytes(address_bytes.try_into().unwrap());
            address = ((val & 0xFE) >> 1)
                | ((val & 0xFE00) >> 2)
                | ((val & 0xFE0000) >> 3)
                | ((val & 0xFE000000) >> 4);
        }
        _ => return Err(Error::Parse),
    }

    Ok(address)
}

/// Writes an HDLC address to the given buffer.
pub fn get_address_bytes(value: u32, bytes: &mut [u8]) -> Result<usize> {
    let (address, size) = if value < 0x80 {
        ((value << 1 | 1), 1)
    } else if value < 0x4000 {
        (((value & 0x3F80) << 2 | (value & 0x7F) << 1 | 1), 2)
    } else if value < 0x10000000 {
        (
            (value & 0xFE00000) << 4
                | (value & 0x1FC000) << 3
                | (value & 0x3F80) << 2
                | (value & 0x7F) << 1
                | 1,
            4,
        )
    } else {
        return Err(Error::InvalidAddress(value));
    };

    if bytes.len() < size {
        return Err(Error::BufferTooSmall {
            requested: size,
            available: bytes.len(),
        });
    }

    match size {
        1 => bytes[0] = address as u8,
        2 => bytes[..2].copy_from_slice(&(address as u16).to_be_bytes()),
        4 => bytes[..4].copy_from_slice(&address.to_be_bytes()),
        _ => unreachable!(),
    }

    Ok(size)
}

/// HDLC frame.
#[derive(Debug)]
pub struct HdlcFrame<'a> {
    /// Primary address.
    pub primary_address: &'a [u8],
    /// Secondary address.
    pub secondary_address: &'a [u8],
    /// Frame ID.
    pub frame_id: u8,
    /// Frame data.
    pub data: Option<&'a [u8]>,
}

impl<'a> HdlcFrame<'a> {
    /// Creates a new `HdlcFrame`.
    pub fn new(
        primary_address: &'a [u8],
        secondary_address: &'a [u8],
        frame_id: u8,
        data: Option<&'a [u8]>,
    ) -> Self {
        Self {
            primary_address,
            secondary_address,
            frame_id,
            data,
        }
    }

    /// Converts the `HdlcFrame` to bytes.
    pub fn to_bytes(&self, buf: &mut [u8]) -> Result<usize> {
        let mut offset = 0;
        buf[offset] = HDLC_FRAME_START_END;
        offset += 1;

        let data_len = self.data.map(|d| d.len()).unwrap_or(0);
        let frame_len = 7 + self.primary_address.len() + self.secondary_address.len() + data_len;
        let frame_len_bytes = (frame_len as u16).to_be_bytes();
        buf[offset] = 0xA0 | ((frame_len_bytes[0] >> 4) & 0x7);
        offset += 1;
        buf[offset] = frame_len_bytes[1];
        offset += 1;

        buf[offset..offset + self.primary_address.len()].copy_from_slice(self.primary_address);
        offset += self.primary_address.len();

        buf[offset..offset + self.secondary_address.len()].copy_from_slice(self.secondary_address);
        offset += self.secondary_address.len();

        buf[offset] = self.frame_id;
        offset += 1;

        let header_crc = crc::count_crc(&buf[1..offset]);
        buf[offset..offset + 2].copy_from_slice(&header_crc.to_be_bytes());
        offset += 2;

        if let Some(data) = self.data {
            buf[offset..offset + data.len()].copy_from_slice(data);
            offset += data.len();

            let data_crc = crc::count_crc(&buf[1..offset]);
            buf[offset..offset + 2].copy_from_slice(&data_crc.to_be_bytes());
            offset += 2;
        }

        buf[offset] = HDLC_FRAME_START_END;
        offset += 1;

        Ok(offset)
    }

    /// Creates a `HdlcFrame` from bytes.
    pub fn from_bytes(buf: &'a [u8]) -> Result<Self> {
        if buf.len() < 9
            || buf[0] != HDLC_FRAME_START_END
            || buf[buf.len() - 1] != HDLC_FRAME_START_END
        {
            return Err(Error::InvalidFrame("Invalid HDLC frame format"));
        }

        let frame_len = (((buf[1] & 0x7) as u16) << 8) | buf[2] as u16;
        if frame_len as usize != buf.len() - 2 {
            return Err(Error::InvalidFrame("Invalid HDLC frame length"));
        }

        let mut offset = 3;
        let primary_address_len = get_hdlc_address_len(&buf[offset..]);
        let primary_address = &buf[offset..offset + primary_address_len];
        offset += primary_address_len;

        let secondary_address_len = get_hdlc_address_len(&buf[offset..]);
        let secondary_address = &buf[offset..offset + secondary_address_len];
        offset += secondary_address_len;

        let frame_id = buf[offset];
        offset += 1;

        let header_crc = u16::from_be_bytes(buf[offset..offset + 2].try_into().unwrap());
        if header_crc != crc::count_crc(&buf[1..offset]) {
            return Err(Error::InvalidFrame("Invalid HDLC header CRC"));
        }
        offset += 2;

        let data = if offset < buf.len() - 3 {
            let data_len = buf.len() - offset - 3;
            let data = &buf[offset..offset + data_len];
            let data_crc =
                u16::from_be_bytes(buf[offset + data_len..offset + data_len + 2].try_into().unwrap());
            if data_crc != crc::count_crc(&buf[1..offset + data_len]) {
                return Err(Error::InvalidFrame("Invalid HDLC data CRC"));
            }
            Some(data)
        } else {
            None
        };

        Ok(Self {
            primary_address,
            secondary_address,
            frame_id,
            data,
        })
    }
}

fn get_hdlc_address_len(buf: &[u8]) -> usize {
    let mut len = 0;
    for &byte in buf.iter() {
        len += 1;
        if (byte & 0x01) == 1 {
            break;
        }
    }
    len
}
