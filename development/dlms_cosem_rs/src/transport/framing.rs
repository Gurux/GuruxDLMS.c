//! Helpers for assembling and parsing DLMS/COSEM link layer frames.

use core::convert::TryFrom;

use crate::error::{Error, Result};
use crate::transport::Profile;
use crate::types::{ByteBuffer, Command, DlmsSettings};

const HDLC_FRAME_START_END: u8 = 0x7E;
const HDLC_INFO_MAX_INFO_TX: u8 = 0x05;
const HDLC_INFO_MAX_INFO_RX: u8 = 0x06;
const HDLC_INFO_WINDOW_SIZE_TX: u8 = 0x07;
const HDLC_INFO_WINDOW_SIZE_RX: u8 = 0x08;
const DLMS_PLC_DATA_LINK_DATA_REQUEST: u8 = 0x90;
const PLC_STX: u8 = 0x02;
const PLC_HEADER_MARKER: u8 = 0x50;

const PLC_SUB_FRAMES: [u16; 7] = [0x6C6C, 0x3A3A, 0x5656, 0x7171, 0x1D1D, 0x4B4B, 0x2727];

/// Parsed representation of a transport frame.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ParsedFrame {
    /// Transport profile used for the frame.
    pub profile: Profile,
    /// Optional DLMS command extracted from the payload.
    pub command: Option<Command>,
    /// Frame control byte (HDLC) or equivalent discriminator.
    pub control: u8,
    /// Destination address extracted from the frame.
    pub destination_address: u32,
    /// Source address extracted from the frame.
    pub source_address: u32,
    /// Optional credit field for PLC frames.
    pub credit_fields: Option<u8>,
    /// Raw payload bytes that follow the link layer headers.
    pub payload: Vec<u8>,
}

/// Aggregated helpers mirroring the legacy C implementation.
pub struct FrameCodec;

impl FrameCodec {
    /// Builds a HDLC frame using the provided settings and payload.
    pub fn hdlc<O>(
        settings: &mut DlmsSettings<O>,
        command: Command,
        payload: &[u8],
    ) -> Result<Vec<u8>> {
        let primary_address = if settings.server {
            encode_hdlc_address(u32::from(settings.client_address))?
        } else {
            encode_hdlc_address(settings.server_address)?
        };
        let secondary_address = if settings.server {
            encode_hdlc_address(settings.server_address)?
        } else {
            encode_hdlc_address(u32::from(settings.client_address))?
        };

        let mut frame = Vec::with_capacity(16 + payload.len());
        frame.push(HDLC_FRAME_START_END);

        let total_payload = payload.len();
        let max_info = if settings.max_info_tx == 0 {
            total_payload as u16
        } else {
            settings.max_info_tx
        } as usize;
        if total_payload > max_info {
            return Err(Error::PayloadTooLarge {
                max: max_info,
                actual: total_payload,
            });
        }

        let mut body_len = 5 + primary_address.len() + secondary_address.len() + total_payload;
        if total_payload != 0 {
            body_len += 2; // Account for payload CRC.
        }
        let high = if total_payload == 0 {
            0xA0
        } else {
            0xA0 | (((body_len >> 8) & 0x7) as u8)
        };
        frame.push(high);
        frame.push((body_len & 0xFF) as u8);
        frame.extend_from_slice(&primary_address);
        frame.extend_from_slice(&secondary_address);

        let control = control_from_command(settings, command);
        frame.push(control);

        let header_crc = count_crc(&frame[1..]);
        frame.push((header_crc >> 8) as u8);
        frame.push((header_crc & 0xFF) as u8);

        if !payload.is_empty() {
            frame.extend_from_slice(payload);
            let payload_crc = count_crc(&frame[1..]);
            frame.push((payload_crc >> 8) as u8);
            frame.push((payload_crc & 0xFF) as u8);
        }

        frame.push(HDLC_FRAME_START_END);
        Ok(frame)
    }

    /// Parses a HDLC frame verifying the CRCs and extracting payload data.
    pub fn parse_hdlc<O>(_settings: &mut DlmsSettings<O>, frame: &[u8]) -> Result<ParsedFrame> {
        if frame.len() < 8 {
            return Err(Error::InvalidFrame("HDLC frame too short"));
        }
        if frame[0] != HDLC_FRAME_START_END || *frame.last().unwrap() != HDLC_FRAME_START_END {
            return Err(Error::InvalidFrame("missing HDLC frame delimiters"));
        }
        let declared = (((frame[1] & 0x7) as usize) << 8) | frame[2] as usize;
        if declared + 2 != frame.len() {
            return Err(Error::InvalidFrame("length field mismatch"));
        }
        let (destination_address, offset) = decode_hdlc_address(&frame[3..])?;
        let (source_address, offset2) = decode_hdlc_address(&frame[3 + offset..])?;
        let control_index = 3 + offset + offset2;
        if control_index + 3 > frame.len() {
            return Err(Error::InvalidFrame("truncated HDLC header"));
        }
        let control = frame[control_index];
        let header_crc = u16::from_be_bytes([frame[control_index + 1], frame[control_index + 2]]);
        if header_crc != count_crc(&frame[1..=control_index]) {
            return Err(Error::InvalidFrame("HDLC header CRC mismatch"));
        }
        let mut payload = Vec::new();
        if control_index + 3 < frame.len() - 3 {
            let crc_index = frame.len() - 3;
            let data_crc = u16::from_be_bytes([frame[crc_index], frame[crc_index + 1]]);
            if data_crc != count_crc(&frame[1..crc_index]) {
                return Err(Error::InvalidFrame("HDLC payload CRC mismatch"));
            }
            payload.extend_from_slice(&frame[control_index + 3..crc_index]);
        }

        let command = command_from_control_and_payload(control, &payload);
        Ok(ParsedFrame {
            profile: Profile::Hdlc,
            command,
            control,
            destination_address,
            source_address,
            credit_fields: None,
            payload,
        })
    }

    /// Builds a WRAPPER frame around the provided payload.
    pub fn wrapper<O>(settings: &DlmsSettings<O>, payload: &[u8]) -> Result<Vec<u8>> {
        if payload.len() > u16::MAX as usize {
            return Err(Error::PayloadTooLarge {
                max: u16::MAX as usize,
                actual: payload.len(),
            });
        }
        let mut frame = Vec::with_capacity(8 + payload.len());
        frame.extend_from_slice(&1u16.to_be_bytes());
        if settings.server {
            frame.extend_from_slice(&(settings.server_address as u16).to_be_bytes());
            frame.extend_from_slice(&settings.client_address.to_be_bytes());
        } else {
            frame.extend_from_slice(&settings.client_address.to_be_bytes());
            frame.extend_from_slice(&(settings.server_address as u16).to_be_bytes());
        }
        frame.extend_from_slice(&(payload.len() as u16).to_be_bytes());
        frame.extend_from_slice(payload);
        Ok(frame)
    }

    /// Parses a WRAPPER frame returning the embedded APDU.
    pub fn parse_wrapper(frame: &[u8]) -> Result<ParsedFrame> {
        if frame.len() < 8 {
            return Err(Error::InvalidFrame("WRAPPER frame too short"));
        }
        let version = u16::from_be_bytes([frame[0], frame[1]]);
        if version != 1 {
            return Err(Error::InvalidFrame("unsupported WRAPPER version"));
        }
        let destination_address = u16::from_be_bytes([frame[2], frame[3]]) as u32;
        let source_address = u16::from_be_bytes([frame[4], frame[5]]) as u32;
        let length = u16::from_be_bytes([frame[6], frame[7]]) as usize;
        if frame.len() != 8 + length {
            return Err(Error::InvalidFrame("WRAPPER length mismatch"));
        }
        let payload = frame[8..].to_vec();
        let command = payload
            .first()
            .and_then(|b| Command::try_from(*b as u16).ok());
        Ok(ParsedFrame {
            profile: Profile::Wrapper,
            command,
            control: 0,
            destination_address,
            source_address,
            credit_fields: None,
            payload,
        })
    }

    /// Builds a raw PLC frame (DLMS/COSEM G3) around the provided payload.
    pub fn plc_raw<O>(
        settings: &DlmsSettings<O>,
        credit_fields: u8,
        payload: &[u8],
    ) -> Result<Vec<u8>> {
        let plc = settings
            .plc_settings
            .as_ref()
            .ok_or(Error::InvalidFrame("PLC settings not configured"))?;
        let frame_size = payload.len().min(134);
        let pad_len = (36 - ((11 + frame_size) % 36)) % 36;
        let mut frame = Vec::with_capacity(15 + frame_size + pad_len);
        frame.push(PLC_STX);
        frame.push((11 + frame_size) as u8);
        frame.push(PLC_HEADER_MARKER);
        frame.push(credit_fields);
        frame.push((plc.mac_source_address >> 4) as u8);
        let combined =
            ((plc.mac_source_address as u32) << 12) | (plc.mac_destination_address as u32 & 0x0FFF);
        frame.extend_from_slice(&(combined as u16).to_be_bytes());
        frame.push(pad_len as u8);
        frame.push(DLMS_PLC_DATA_LINK_DATA_REQUEST);
        frame.push(settings.server_address as u8);
        frame.push(settings.client_address as u8);
        frame.extend_from_slice(&payload[..frame_size]);
        frame.resize(frame.len() + pad_len, 0);
        let crc = count_crc(&frame);
        frame.push((crc >> 8) as u8);
        frame.push((crc & 0xFF) as u8);
        Ok(frame)
    }

    /// Parses a raw PLC frame returning the MAC payload.
    pub fn parse_plc_raw(frame: &[u8]) -> Result<ParsedFrame> {
        if frame.len() < 15 {
            return Err(Error::InvalidFrame("PLC frame too short"));
        }
        if frame[0] != PLC_STX {
            return Err(Error::InvalidFrame("invalid PLC start flag"));
        }
        let declared = frame[1] as usize;
        if frame.len() < 2 + declared + 2 {
            return Err(Error::InvalidFrame("PLC length mismatch"));
        }
        if frame[2] != PLC_HEADER_MARKER {
            return Err(Error::InvalidFrame("unsupported PLC marker"));
        }
        let credit_fields = frame[3];
        let source_high = frame[4] as u16;
        let combined = u16::from_be_bytes([frame[5], frame[6]]);
        let _mac_source = ((source_high as u32) << 4) | ((combined as u32) >> 12);
        let _mac_destination = (combined as u32) & 0x0FFF;
        let pad_len = frame[7] as usize;
        let control = frame[8];
        let destination = frame[9] as u32;
        let source = frame[10] as u32;
        let payload_len = declared.saturating_sub(11);
        let payload_start = 11;
        let payload_end = payload_start + payload_len;
        if payload_end + pad_len + 2 > frame.len() {
            return Err(Error::InvalidFrame("PLC payload truncated"));
        }
        let payload = frame[payload_start..payload_end].to_vec();
        let crc = u16::from_be_bytes([
            frame[payload_end + pad_len],
            frame[payload_end + pad_len + 1],
        ]);
        if crc != count_crc(&frame[..payload_end + pad_len]) {
            return Err(Error::InvalidFrame("PLC CRC mismatch"));
        }
        let command = payload
            .first()
            .and_then(|b| Command::try_from(*b as u16).ok());
        Ok(ParsedFrame {
            profile: Profile::Plc,
            command,
            control,
            destination_address: destination,
            source_address: source,
            credit_fields: Some(credit_fields),
            payload,
        })
    }

    /// Parses a PLC MAC frame that encapsulates a HDLC frame.
    pub fn parse_plc_mac_hdlc<O>(
        settings: &mut DlmsSettings<O>,
        frame: &[u8],
    ) -> Result<ParsedFrame> {
        if frame.len() < 12 {
            return Err(Error::InvalidFrame("PLC MAC frame too short"));
        }
        let marker = u16::from_be_bytes([frame[0], frame[1]]);
        if !PLC_SUB_FRAMES.contains(&marker) {
            return Err(Error::InvalidFrame("unknown PLC sub-frame marker"));
        }
        let credit_fields = frame[2];
        let pad_len = frame[6] as usize;
        if frame.len() < 7 + pad_len + 3 {
            return Err(Error::InvalidFrame("PLC MAC payload truncated"));
        }
        let crc_index = frame
            .len()
            .checked_sub(3)
            .ok_or(Error::InvalidFrame("PLC MAC frame missing FCS"))?;
        let expected_crc = count_fcs24(&frame[2..crc_index]);
        let provided_crc = ((frame[crc_index] as u32) << 16)
            | ((frame[crc_index + 1] as u32) << 8)
            | frame[crc_index + 2] as u32;
        if expected_crc != provided_crc {
            return Err(Error::InvalidFrame("PLC MAC frame FCS mismatch"));
        }
        let payload_end = crc_index
            .checked_sub(pad_len)
            .ok_or(Error::InvalidFrame("PLC MAC padding exceeds frame"))?;
        if payload_end < 7 {
            return Err(Error::InvalidFrame("PLC MAC payload too short"));
        }
        let inner = &frame[7..payload_end];
        let mut parsed = Self::parse_hdlc(settings, inner)?;
        parsed.profile = Profile::Plc;
        parsed.credit_fields = Some(credit_fields);
        Ok(parsed)
    }

    /// Builds a PLC MAC frame encapsulating a HDLC frame.
    pub fn plc_mac_hdlc<O>(
        settings: &mut DlmsSettings<O>,
        command: Command,
        credit_fields: u8,
        payload: &[u8],
    ) -> Result<Vec<u8>> {
        let plc = settings
            .plc_settings
            .clone()
            .ok_or(Error::InvalidFrame("PLC settings not configured"))?;
        let inner = Self::hdlc(settings, command, payload)?;
        if settings.max_info_tx > 126 {
            settings.max_info_tx = 86;
        }
        let mut frame = Vec::with_capacity(64 + inner.len());
        frame.extend_from_slice(&[0, 0]);
        frame.push(credit_fields);
        frame.push((plc.mac_source_address >> 4) as u8);
        let combined =
            ((plc.mac_source_address as u32) << 12) | (plc.mac_destination_address as u32 & 0x0FFF);
        frame.extend_from_slice(&(combined as u16).to_be_bytes());
        let pad_len = (36 - ((10 + inner.len()) % 36)) % 36;
        frame.push(pad_len as u8);
        frame.extend_from_slice(&inner);
        frame.resize(frame.len() + pad_len, 0);
        let crc = count_fcs24(&frame[2..]);
        frame.push(((crc >> 16) & 0xFF) as u8);
        frame.push((crc >> 8) as u8);
        frame.push((crc & 0xFF) as u8);
        let mut sub_frames = frame.len() / 36;
        if frame.len() % 36 != 0 {
            sub_frames += 1;
        }
        let value = *PLC_SUB_FRAMES
            .get(sub_frames.saturating_sub(1))
            .ok_or(Error::InvalidFrame("too many PLC sub-frames"))?;
        frame[0] = (value >> 8) as u8;
        frame[1] = (value & 0xFF) as u8;
        Ok(frame)
    }
}

fn command_from_control_and_payload(control: u8, payload: &[u8]) -> Option<Command> {
    match control {
        0x93 => Some(Command::Snrm),
        0x73 => Some(Command::Ua),
        0x53 => Some(Command::Disc),
        0x1F => Some(Command::DisconnectMode),
        _ => payload
            .first()
            .and_then(|b| Command::try_from(*b as u16).ok()),
    }
}

fn control_from_command<O>(settings: &mut DlmsSettings<O>, command: Command) -> u8 {
    match command {
        Command::Snrm => 0x93,
        Command::Ua => 0x73,
        Command::Disc => 0x53,
        Command::DisconnectMode => 0x1F,
        _ => settings.next_send_frame(true),
    }
}

fn encode_hdlc_address(value: u32) -> Result<Vec<u8>> {
    if value < 0x80 {
        Ok(vec![(value as u8) << 1 | 0x01])
    } else if value < 0x4000 {
        let address = ((value & 0x3F80) << 2) | ((value & 0x7F) << 1) | 0x01;
        Ok(u16::to_be_bytes(address as u16).to_vec())
    } else if value < 0x10000000 {
        let address = ((value & 0x0FE0_0000) << 4)
            | ((value & 0x001F_C000) << 3)
            | ((value & 0x0000_3F80) << 2)
            | ((value & 0x0000_007F) << 1)
            | 0x01;
        Ok(u32::to_be_bytes(address).to_vec())
    } else {
        Err(Error::InvalidAddress(value))
    }
}

fn decode_hdlc_address(data: &[u8]) -> Result<(u32, usize)> {
    let mut value: u32 = 0;
    for (index, byte) in data.iter().enumerate() {
        value = (value << 7) | u32::from(byte >> 1);
        if byte & 0x1 == 0x1 {
            return Ok((value, index + 1));
        }
    }
    Err(Error::InvalidFrame("unterminated HDLC address"))
}

fn count_crc(data: &[u8]) -> u16 {
    const FCS16_TABLE: [u16; 256] = [
        0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF, 0x8C48, 0x9DC1, 0xAF5A,
        0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7, 0x1081, 0x0108, 0x3393, 0x221A, 0x56A5, 0x472C,
        0x75B7, 0x643E, 0x9CC9, 0x8D40, 0xBFDB, 0xAE52, 0xDAED, 0xCB64, 0xF9FF, 0xE876, 0x2102,
        0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD, 0xAD4A, 0xBCC3, 0x8E58, 0x9FD1,
        0xEB6E, 0xFAE7, 0xC87C, 0xD9F5, 0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5,
        0x453C, 0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD, 0xC974, 0x4204, 0x538D,
        0x6116, 0x709F, 0x0420, 0x15A9, 0x2732, 0x36BB, 0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868,
        0x99E1, 0xAB7A, 0xBAF3, 0x5285, 0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A,
        0xDECD, 0xCF44, 0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72, 0x6306, 0x728F, 0x4014,
        0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9, 0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3,
        0x8A78, 0x9BF1, 0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738, 0xFFCF,
        0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862, 0x9AF9, 0x8B70, 0x8408, 0x9581, 0xA71A, 0xB693,
        0xC22C, 0xD3A5, 0xE13E, 0xF0B7, 0x0840, 0x19C9, 0x2B52, 0x3ADB, 0x4E64, 0x5FED, 0x6D76,
        0x7CFF, 0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036, 0x18C1, 0x0948,
        0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E, 0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E,
        0xF2A7, 0xC03C, 0xD1B5, 0x2942, 0x38CB, 0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD,
        0xB58B, 0xA402, 0x9699, 0x8710, 0xF3AF, 0xE226, 0xD0BD, 0xC134, 0x39C3, 0x284A, 0x1AD1,
        0x0B58, 0x7FE7, 0x6E6E, 0x5CF5, 0x4D7C, 0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1,
        0xA33A, 0xB2B3, 0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB, 0xD68D,
        0xC704, 0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232, 0x5AC5, 0x4B4C, 0x79D7, 0x685E,
        0x1CE1, 0x0D68, 0x3FF3, 0x2E7A, 0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238,
        0x93B1, 0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9, 0xF78F, 0xE606,
        0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330, 0x7BC7, 0x6A4E, 0x58D5, 0x495C, 0x3DE3,
        0x2C6A, 0x1EF1, 0x0F78,
    ];
    let mut fcs: u16 = 0xFFFF;
    for &byte in data {
        fcs = (fcs >> 8) ^ FCS16_TABLE[(fcs ^ byte as u16) as usize & 0xFF];
    }
    fcs = !fcs;
    ((fcs & 0xFF) << 8) | (fcs >> 8)
}

fn count_fcs24(data: &[u8]) -> u32 {
    const CRCPOLY: u32 = 0xD3B6BA00;
    let mut crc: u32 = 0;
    for &byte in data {
        let mut b = byte;
        for _ in 0..8 {
            crc >>= 1;
            if (b & 0x80) != 0 {
                crc |= 0x8000_0000;
            }
            if (crc & 0x80) != 0 {
                crc ^= CRCPOLY;
            }
            b <<= 1;
        }
    }
    crc >> 8
}

/// Builds the payload for a SNRM request based on the current HDLC parameters.
pub fn build_snrm_information<O>(settings: &DlmsSettings<O>) -> Result<Vec<u8>> {
    let mut payload = ByteBuffer::with_capacity(32);
    payload.push(0x81);
    payload.push(0x80);
    payload.push(0);
    let mut has_parameters = false;
    if settings.max_info_tx != crate::types::DEFAULT_MAX_INFO_TX
        || settings.max_info_rx != crate::types::DEFAULT_MAX_INFO_RX
        || settings.window_size_tx != crate::types::DEFAULT_MAX_WINDOW_SIZE_TX
        || settings.window_size_rx != crate::types::DEFAULT_MAX_WINDOW_SIZE_RX
    {
        has_parameters = true;
        payload.push(HDLC_INFO_MAX_INFO_TX);
        append_hdlc_parameter(&mut payload, settings.max_info_tx);
        payload.push(HDLC_INFO_MAX_INFO_RX);
        append_hdlc_parameter(&mut payload, settings.max_info_rx);
        payload.push(HDLC_INFO_WINDOW_SIZE_TX);
        payload.push(4);
        payload.push_u32(u32::from(settings.window_size_tx));
        payload.push(HDLC_INFO_WINDOW_SIZE_RX);
        payload.push(4);
        payload.push_u32(u32::from(settings.window_size_rx));
    }
    if has_parameters {
        let len = payload.len() - 3;
        payload.set(2, len as u8)?;
        Ok(payload.to_vec())
    } else {
        Ok(Vec::new())
    }
}

fn append_hdlc_parameter(buffer: &mut ByteBuffer, value: u16) {
    if value < 0x100 {
        buffer.push(1);
        buffer.push(value as u8);
    } else {
        buffer.push(2);
        buffer.push_u16(value);
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::types::{
        Authentication, DlmsSettings, InterfaceType, PlcSettings, DEFAULT_MAX_INFO_RX,
        DEFAULT_MAX_INFO_TX, DEFAULT_MAX_WINDOW_SIZE_RX, DEFAULT_MAX_WINDOW_SIZE_TX,
    };

    #[test]
    fn hdlc_snrm_matches_legacy_vector() {
        let mut settings = DlmsSettings::<()>::default()
            .with_addresses(0x10, 1)
            .with_authentication(Authentication::None);
        settings.max_info_tx = DEFAULT_MAX_INFO_TX;
        settings.max_info_rx = DEFAULT_MAX_INFO_RX;
        settings.window_size_tx = DEFAULT_MAX_WINDOW_SIZE_TX;
        settings.window_size_rx = DEFAULT_MAX_WINDOW_SIZE_RX;
        settings.interface_type = InterfaceType::Hdlc;

        let frame = FrameCodec::hdlc(&mut settings, Command::Snrm, &[]).unwrap();
        let expected = vec![0x7E, 0xA0, 0x07, 0x03, 0x21, 0x93, 0x0F, 0x01, 0x7E];
        assert_eq!(frame, expected);

        let parsed = FrameCodec::parse_hdlc(&mut settings, &frame).unwrap();
        assert_eq!(parsed.command, Some(Command::Snrm));
        assert_eq!(parsed.destination_address, 1);
        assert_eq!(parsed.source_address, 0x10);
        assert!(parsed.payload.is_empty());
    }

    #[test]
    fn wrapper_frame_roundtrip() {
        let mut settings = DlmsSettings::<()>::default().with_addresses(0x01, 0x10);
        settings.interface_type = InterfaceType::Wrapper;
        let payload = vec![0x60, 0x01];
        let frame = FrameCodec::wrapper(&settings, &payload).unwrap();
        let expected = vec![0x00, 0x01, 0x00, 0x01, 0x00, 0x10, 0x00, 0x02, 0x60, 0x01];
        assert_eq!(frame, expected);

        let parsed = FrameCodec::parse_wrapper(&frame).unwrap();
        assert_eq!(parsed.command, Some(Command::Aarq));
        assert_eq!(parsed.destination_address, 0x0001);
        assert_eq!(parsed.source_address, 0x0010);
        assert_eq!(parsed.payload, payload);
    }

    #[test]
    fn plc_raw_roundtrip() {
        let mut settings = DlmsSettings::<()>::default().with_addresses(0x10, 0x01);
        settings.interface_type = InterfaceType::Plc;
        settings.plc_settings = Some(PlcSettings {
            mac_source_address: 0x1234,
            mac_destination_address: 0x567,
            ..Default::default()
        });
        let payload = vec![Command::Aarq as u16 as u8, 0x01, 0x02];
        let frame = FrameCodec::plc_raw(&settings, 0x90, &payload).unwrap();
        let parsed = FrameCodec::parse_plc_raw(&frame).unwrap();
        assert_eq!(parsed.command, Some(Command::Aarq));
        assert_eq!(parsed.payload, payload);
        assert_eq!(parsed.control, DLMS_PLC_DATA_LINK_DATA_REQUEST);
    }

    #[test]
    fn plc_mac_hdlc_roundtrip() {
        let plc_settings = PlcSettings {
            mac_source_address: 0x2222,
            mac_destination_address: 0x111,
            ..Default::default()
        };
        let mut settings = DlmsSettings::<()>::default().with_addresses(0x10, 0x01);
        settings.interface_type = InterfaceType::PlcHdlc;
        settings.plc_settings = Some(plc_settings.clone());
        let payload = vec![0x60, 0x01, 0x02];
        let frame = FrameCodec::plc_mac_hdlc(&mut settings, Command::Aarq, 0x55, &payload).unwrap();

        let mut parse_settings = DlmsSettings::<()>::default().with_addresses(0x10, 0x01);
        parse_settings.interface_type = InterfaceType::PlcHdlc;
        parse_settings.plc_settings = Some(plc_settings);
        let pad_len = frame[6] as usize;
        let crc_index = frame.len() - 3;
        let inner = &frame[7..crc_index - pad_len];
        let data_crc = u16::from_be_bytes([inner[inner.len() - 3], inner[inner.len() - 2]]);
        let computed = super::count_crc(&inner[1..inner.len() - 3]);
        assert_eq!(data_crc, computed);
        let parsed = FrameCodec::parse_plc_mac_hdlc(&mut parse_settings, &frame).unwrap();

        assert_eq!(parsed.command, Some(Command::Aarq));
        assert_eq!(parsed.credit_fields, Some(0x55));
        assert_eq!(parsed.payload, payload);
        assert_eq!(parsed.profile, Profile::Plc);
    }
}
