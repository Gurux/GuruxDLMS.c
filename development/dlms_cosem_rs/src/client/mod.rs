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

use crate::error::{Error, Result};
use crate::transport::framing::{build_snrm_information, FrameCodec};
use crate::types::{Command, ConnectionState, DlmsSettings, InterfaceType};

/// High level DLMS client that mirrors the legacy C implementation behaviour.
#[derive(Debug)]
pub struct DlmsClient<O = ()> {
    settings: DlmsSettings<O>,
    options: ClientOptions,
}

impl<O> DlmsClient<O> {
    /// Creates a new client with the provided DLMS settings and options.
    pub fn new(settings: DlmsSettings<O>, options: ClientOptions) -> Self {
        Self { settings, options }
    }

    /// Returns an immutable reference to the underlying settings.
    pub fn settings(&self) -> &DlmsSettings<O> {
        &self.settings
    }

    /// Returns a mutable reference to the underlying settings.
    pub fn settings_mut(&mut self) -> &mut DlmsSettings<O> {
        &mut self.settings
    }

    /// Builds a SNRM request frame for HDLC based transports.
    pub fn snrm_request(&mut self) -> Result<Vec<u8>> {
        self.settings.initialize_max_info_tx = self.settings.max_info_tx;
        self.settings.initialize_max_info_rx = self.settings.max_info_rx;
        self.settings.initialize_window_size_tx = self.settings.window_size_tx;
        self.settings.initialize_window_size_rx = self.settings.window_size_rx;
        self.settings.connected = ConnectionState::NONE;
        self.settings.authentication_required = false;
        self.settings.reset_frame_sequence();
        self.settings.reset_block_index();

        match self.settings.interface_type {
            InterfaceType::Wrapper | InterfaceType::Pdu => Ok(Vec::new()),
            InterfaceType::Hdlc | InterfaceType::HdlcWithModeE => {
                let information = build_snrm_information(&self.settings)?;
                FrameCodec::hdlc(&mut self.settings, Command::Snrm, &information)
            }
            InterfaceType::Plc => FrameCodec::plc_raw(&self.settings, 0, &[]),
            InterfaceType::PlcHdlc => {
                FrameCodec::plc_mac_hdlc(&mut self.settings, Command::Snrm, 0, &[])
            }
            _ => Ok(Vec::new()),
        }
    }

    /// Calculates the full frame length based on the configured interface type.
    pub fn frame_size(&self, frame: &[u8]) -> Result<usize> {
        match self.settings.interface_type {
            InterfaceType::Hdlc | InterfaceType::HdlcWithModeE | InterfaceType::PlcHdlc => {
                if frame.len() < 3 {
                    return Err(Error::UnexpectedEof);
                }
                let total = (((frame[1] & 0x7) as usize) << 8) | frame[2] as usize;
                Ok(total + 2)
            }
            InterfaceType::Wrapper => {
                if frame.len() < 8 {
                    return Err(Error::UnexpectedEof);
                }
                let length = u16::from_be_bytes([frame[6], frame[7]]) as usize;
                Ok(8 + length)
            }
            InterfaceType::Plc => {
                if frame.len() < 8 {
                    return Err(Error::UnexpectedEof);
                }
                let declared = frame[1] as usize;
                let pad_len = frame.get(7).copied().unwrap_or(0) as usize;
                Ok(2 + declared + pad_len + 2)
            }
            _ => Ok(frame.len()),
        }
    }

    /// Returns the amount of bytes still required to complete the frame.
    pub fn remaining_frame_size(&self, frame: &[u8]) -> Result<usize> {
        let total = self.frame_size(frame)?;
        Ok(total.saturating_sub(frame.len()))
    }

    /// Builds an AARQ request frame from the provided APDU payload.
    pub fn aarq_request(&mut self, apdu: &[u8]) -> Result<Vec<u8>> {
        self.settings.initialize_pdu_size = self.settings.max_pdu_size;
        self.settings.connected.remove(ConnectionState::DLMS);
        self.settings.reset_block_index();
        if self.settings.auto_increase_invoke_id {
            self.settings.invoke_id = 0;
        }
        match self.settings.interface_type {
            InterfaceType::Wrapper => FrameCodec::wrapper(&self.settings, apdu),
            InterfaceType::Plc => FrameCodec::plc_raw(&self.settings, 0, apdu),
            InterfaceType::PlcHdlc => {
                FrameCodec::plc_mac_hdlc(&mut self.settings, Command::Aarq, 0, apdu)
            }
            _ => FrameCodec::hdlc(&mut self.settings, Command::Aarq, apdu),
        }
    }
}

impl<O> DlmsClient<O> {
    /// Reports whether secure messaging is requested for the client instance.
    pub const fn requires_security(&self) -> bool {
        self.options.secure && !cfg!(feature = "dlms_ignore_security_setup")
    }
}

/// Backwards compatible alias for the new `DlmsClient` type.
pub type Client<O = ()> = DlmsClient<O>;

#[cfg(test)]
mod tests {
    use super::*;
    use crate::types::{
        Apdu, DEFAULT_MAX_INFO_RX, DEFAULT_MAX_INFO_TX, DEFAULT_MAX_WINDOW_SIZE_RX,
        DEFAULT_MAX_WINDOW_SIZE_TX,
    };

    fn default_settings() -> DlmsSettings<()> {
        let mut settings = DlmsSettings::<()>::default().with_addresses(0x10, 1);
        settings.interface_type = InterfaceType::Hdlc;
        settings.max_info_tx = DEFAULT_MAX_INFO_TX;
        settings.max_info_rx = DEFAULT_MAX_INFO_RX;
        settings.window_size_tx = DEFAULT_MAX_WINDOW_SIZE_TX;
        settings.window_size_rx = DEFAULT_MAX_WINDOW_SIZE_RX;
        settings
    }

    #[test]
    fn snrm_request_matches_known_vector() {
        let options = ClientOptions { secure: false };
        let settings = default_settings();
        let mut client = DlmsClient::new(settings, options);
        let frame = client.snrm_request().unwrap();
        assert_eq!(
            frame,
            vec![0x7E, 0xA0, 0x07, 0x03, 0x21, 0x93, 0x0F, 0x01, 0x7E]
        );
    }

    #[test]
    fn frame_size_and_remaining_are_consistent() {
        let options = ClientOptions { secure: false };
        let settings = default_settings();
        let mut client = DlmsClient::new(settings, options);
        let frame = client.snrm_request().unwrap();
        let total = client.frame_size(&frame).unwrap();
        assert_eq!(total, frame.len());
        assert_eq!(client.remaining_frame_size(&frame[..4]).unwrap(), total - 4);
    }

    #[test]
    fn aarq_request_wraps_apdu() {
        let options = ClientOptions { secure: false };
        let settings = default_settings();
        let mut client = DlmsClient::new(settings, options);
        let apdu = Apdu::new(Command::Aarq, vec![0x01, 0x02]);
        let frame = client.aarq_request(&apdu.to_bytes()).unwrap();
        assert_eq!(frame[0], 0x7E);
        assert_eq!(frame[frame.len() - 1], 0x7E);
    }
}
