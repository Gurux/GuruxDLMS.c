use std::fmt;

use super::buffer::ByteBuffer;
use super::enums::{
    Authentication, Conformance, ConnectionState, InterfaceType, Priority, Security,
    SecurityPolicy, SecuritySuite, ServiceClass,
};
use super::object::ObjectArray;

#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};

/// Representation of the PLC specific configuration block in DLMS settings.
#[derive(Clone, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
pub struct PlcSettings {
    /// System title used by the PLC medium.
    pub system_title: ByteBuffer,
    /// Initial credit allocated for PLC sessions.
    pub initial_credit: u8,
    /// Current credit available for PLC sessions.
    pub current_credit: u8,
    /// Delta credit requested from peers.
    pub delta_credit: u8,
    /// MAC source address used on the PLC network.
    pub mac_source_address: u16,
    /// MAC destination address used on the PLC network.
    pub mac_destination_address: u16,
    /// Probability for responding to contention slots.
    pub response_probability: u8,
    /// Allowed PLC time slots expressed in bitmap form.
    pub allowed_time_slots: u16,
    /// Client system title used for PLC communication.
    pub client_system_title: ByteBuffer,
}

impl Default for PlcSettings {
    fn default() -> Self {
        Self {
            system_title: ByteBuffer::new(),
            initial_credit: 0,
            current_credit: 0,
            delta_credit: 0,
            mac_source_address: 0,
            mac_destination_address: 0,
            response_probability: 0,
            allowed_time_slots: 0,
            client_system_title: ByteBuffer::new(),
        }
    }
}

/// Minimal ciphering state exposed to higher level APIs.
#[derive(Clone, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
pub struct CipheringState {
    pub security: Security,
    pub suite: Option<SecuritySuite>,
    pub policy: SecurityPolicy,
    pub encrypt: bool,
    pub block_cipher_key: Option<ByteBuffer>,
    pub broadcast_block_cipher_key: Option<ByteBuffer>,
    pub system_title: Option<ByteBuffer>,
    pub invocation_counter: u32,
    pub authentication_key: Option<ByteBuffer>,
    pub dedicated_key: Option<ByteBuffer>,
    pub broadcast: bool,
}

impl Default for CipheringState {
    fn default() -> Self {
        Self {
            security: Security::NONE,
            suite: None,
            policy: SecurityPolicy::empty(),
            encrypt: false,
            block_cipher_key: None,
            broadcast_block_cipher_key: None,
            system_title: None,
            invocation_counter: 0,
            authentication_key: None,
            dedicated_key: None,
            broadcast: false,
        }
    }
}

/// Invocation counter used by secure DLMS connections.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
pub enum InvocationCounter {
    /// 32-bit invocation counter.
    Bits32(u32),
    /// 64-bit invocation counter.
    Bits64(u64),
}

impl InvocationCounter {
    /// Returns the counter value as a `u64` regardless of the native representation.
    pub fn value(&self) -> u64 {
        match self {
            InvocationCounter::Bits32(v) => *v as u64,
            InvocationCounter::Bits64(v) => *v,
        }
    }
}

impl From<u32> for InvocationCounter {
    fn from(value: u32) -> Self {
        InvocationCounter::Bits32(value)
    }
}

impl From<u64> for InvocationCounter {
    fn from(value: u64) -> Self {
        InvocationCounter::Bits64(value)
    }
}

impl fmt::Display for InvocationCounter {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.value())
    }
}

/// Safe translation of the C `dlmsSettings` structure.
#[derive(Clone, Debug, PartialEq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
pub struct DlmsSettings<O = ()> {
    /// Indicates whether custom challenges are used.
    pub custom_challenges: bool,
    /// Challenge sent from client to server.
    pub client_to_server_challenge: ByteBuffer,
    /// Challenge sent from server to client.
    pub server_to_client_challenge: ByteBuffer,
    /// Source system title (server).
    pub source_system_title: [u8; 8],
    /// Short invoke identifier.
    pub invoke_id: u8,
    /// Long invoke identifier.
    pub long_invoke_id: i32,
    /// Priority used for requests.
    pub priority: Priority,
    /// Service class requested.
    pub service_class: ServiceClass,
    /// Client address used on the link layer.
    pub client_address: u16,
    /// Server address used on the link layer.
    pub server_address: u32,
    /// Whether logical name referencing is active.
    pub use_logical_name_referencing: bool,
    /// Interface type in use.
    pub interface_type: InterfaceType,
    /// Authentication mechanism in use.
    pub authentication: Authentication,
    /// Password or secret associated with the authentication.
    pub password: ByteBuffer,
    /// Optional key-encrypting-key.
    pub kek: Option<ByteBuffer>,
    /// DLMS version number.
    pub dlms_version_number: u8,
    /// Maximum negotiated PDU size.
    pub max_pdu_size: u16,
    /// PDU size proposed by the client.
    pub client_pdu_size: u16,
    /// Maximum PDU size accepted by the server.
    pub max_server_pdu_size: u16,
    /// HDLC sender frame sequence.
    pub sender_frame: u8,
    /// HDLC receiver frame sequence.
    pub receiver_frame: u8,
    /// Indicates whether the context is server side.
    pub server: bool,
    /// Indicates whether authentication is required.
    pub authentication_required: bool,
    /// Conformance proposed by the current side.
    pub proposed_conformance: Conformance,
    /// Conformance negotiated between client and server.
    pub negotiated_conformance: Conformance,
    /// Conformance proposed by the counterpart.
    pub client_proposed_conformance: Conformance,
    /// Maximum information field for transmitted frames.
    pub max_info_tx: u16,
    /// Maximum information field for received frames.
    pub max_info_rx: u16,
    /// Window size for transmitted frames.
    pub window_size_tx: u8,
    /// Window size for received frames.
    pub window_size_rx: u8,
    /// Initial PDU size used when resetting state.
    pub initialize_pdu_size: u16,
    /// Initial max info TX.
    pub initialize_max_info_tx: u16,
    /// Initial max info RX.
    pub initialize_max_info_rx: u16,
    /// Initial window size for TX.
    pub initialize_window_size_tx: u8,
    /// Initial window size for RX.
    pub initialize_window_size_rx: u8,
    /// Optional PLC configuration block.
    pub plc_settings: Option<PlcSettings>,
    /// Internal COSEM objects not visible in association view.
    pub internal_objects: ObjectArray<O>,
    /// COSEM objects available through the association.
    pub objects: ObjectArray<O>,
    /// Current block index for block transfer.
    pub block_index: u32,
    /// Connection state of the stack.
    pub connected: ConnectionState,
    /// Optional ciphering state information.
    pub cipher: Option<CipheringState>,
    /// Optional user identifier.
    pub user_id: i16,
    /// DLMS protocol version.
    pub protocol_version: u8,
    /// Quality of service indicator.
    pub quality_of_service: u8,
    /// Optional pre-established system title.
    pub pre_established_system_title: Option<ByteBuffer>,
    /// Optional serialized PDU buffer.
    pub serialized_pdu: Option<ByteBuffer>,
    /// Whether invoke identifier is incremented automatically.
    pub auto_increase_invoke_id: bool,
    /// Objects released but still tracked for cleanup.
    pub released_objects: ObjectArray<O>,
    /// Expected security suite if constrained.
    pub expected_security_suite: Option<SecuritySuite>,
    /// Expected security policy if constrained.
    pub expected_security_policy: Option<SecurityPolicy>,
    /// Expected invocation counter value.
    pub expected_invocation_counter: Option<InvocationCounter>,
    /// Expected client system title.
    pub expected_client_system_title: Option<ByteBuffer>,
}

impl<O> Default for DlmsSettings<O> {
    fn default() -> Self {
        Self {
            custom_challenges: false,
            client_to_server_challenge: ByteBuffer::new(),
            server_to_client_challenge: ByteBuffer::new(),
            source_system_title: [0; 8],
            invoke_id: 0,
            long_invoke_id: 0,
            priority: Priority::Normal,
            service_class: ServiceClass::Unconfirmed,
            client_address: 0,
            server_address: 0,
            use_logical_name_referencing: true,
            interface_type: InterfaceType::Hdlc,
            authentication: Authentication::None,
            password: ByteBuffer::new(),
            kek: None,
            dlms_version_number: 6,
            max_pdu_size: 0,
            client_pdu_size: 0,
            max_server_pdu_size: 0,
            sender_frame: 0,
            receiver_frame: 0,
            server: false,
            authentication_required: false,
            proposed_conformance: Conformance::empty(),
            negotiated_conformance: Conformance::empty(),
            client_proposed_conformance: Conformance::empty(),
            max_info_tx: 0,
            max_info_rx: 0,
            window_size_tx: 0,
            window_size_rx: 0,
            initialize_pdu_size: 0,
            initialize_max_info_tx: 0,
            initialize_max_info_rx: 0,
            initialize_window_size_tx: 0,
            initialize_window_size_rx: 0,
            plc_settings: None,
            internal_objects: ObjectArray::new(),
            objects: ObjectArray::new(),
            block_index: 0,
            connected: ConnectionState::NONE,
            cipher: None,
            user_id: 0,
            protocol_version: 0,
            quality_of_service: 0,
            pre_established_system_title: None,
            serialized_pdu: None,
            auto_increase_invoke_id: true,
            released_objects: ObjectArray::new(),
            expected_security_suite: None,
            expected_security_policy: None,
            expected_invocation_counter: None,
            expected_client_system_title: None,
        }
    }
}

impl<O> DlmsSettings<O> {
    /// Configures the instance for logical name referencing.
    pub fn with_logical_name_referencing(mut self, enabled: bool) -> Self {
        self.use_logical_name_referencing = enabled;
        self
    }

    /// Sets the client and server addresses.
    pub fn with_addresses(mut self, client: u16, server: u32) -> Self {
        self.client_address = client;
        self.server_address = server;
        self
    }

    /// Configures the authentication mechanism.
    pub fn with_authentication(mut self, authentication: Authentication) -> Self {
        self.authentication = authentication;
        self
    }

    /// Resets the HDLC frame sequence counters to their initial values.
    pub fn reset_frame_sequence(&mut self) {
        if self.server {
            self.sender_frame = SERVER_START_SENDER_FRAME_SEQUENCE;
            self.receiver_frame = SERVER_START_RECEIVER_FRAME_SEQUENCE;
        } else {
            self.sender_frame = CLIENT_START_SENDER_FRAME_SEQUENCE;
            self.receiver_frame = CLIENT_START_RECEIVER_FRAME_SEQUENCE;
        }
    }

    /// Returns the next send frame according to the HDLC rules.
    pub fn next_send_frame(&mut self, first: bool) -> u8 {
        if first {
            self.sender_frame =
                increase_receiver_sequence(increase_send_sequence(self.sender_frame));
        } else {
            self.sender_frame = increase_send_sequence(self.sender_frame);
        }
        self.sender_frame
    }

    /// Returns the receiver ready control field value updating internal state.
    pub fn receiver_ready(&mut self) -> u8 {
        self.sender_frame = increase_receiver_sequence(self.sender_frame | 1);
        self.sender_frame & 0xF1
    }

    /// Resets the block index used for block transfers.
    pub fn reset_block_index(&mut self) {
        self.block_index = 1;
    }
}

const SERVER_START_SENDER_FRAME_SEQUENCE: u8 = 0x1E;
const SERVER_START_RECEIVER_FRAME_SEQUENCE: u8 = 0xFE;
const CLIENT_START_SENDER_FRAME_SEQUENCE: u8 = 0xFE;
const CLIENT_START_RECEIVER_FRAME_SEQUENCE: u8 = 0x0E;

fn increase_send_sequence(value: u8) -> u8 {
    (value & 0xF0) | ((value.wrapping_add(0x02)) & 0x0E)
}

fn increase_receiver_sequence(value: u8) -> u8 {
    (value.wrapping_add(0x20)) | 0x10 | (value & 0x0E)
}
/// Default maximum HDLC information field for transmitted frames.
pub const DEFAULT_MAX_INFO_TX: u16 = 128;
/// Default maximum HDLC information field for received frames.
pub const DEFAULT_MAX_INFO_RX: u16 = 128;
/// Default HDLC transmit window size.
pub const DEFAULT_MAX_WINDOW_SIZE_TX: u8 = 1;
/// Default HDLC receive window size.
pub const DEFAULT_MAX_WINDOW_SIZE_RX: u8 = 1;
