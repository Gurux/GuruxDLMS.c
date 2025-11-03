//! DLMS/COSEM reply data.

use crate::types::Command;
use crate::variant::Variant;

/// DLMS/COSEM reply data.
pub struct ReplyData {
    /// DLMS command.
    pub command: Command,
    /// Encrypted DLMS command.
    pub encrypted_command: Command,
    /// Command type.
    pub command_type: u8,
    /// Reply data.
    pub data: Vec<u8>,
    /// Is reply data complete.
    pub complete: bool,
    /// Reply data value.
    pub data_value: Variant,
    /// Total count of items.
    pub total_count: u16,
    /// Current read position.
    pub read_position: usize,
    /// Packet length.
    pub packet_length: usize,
    /// Is peeking.
    pub peek: bool,
    /// Is ignoring value.
    pub ignore_value: bool,
    /// Data type.
    pub data_type: u8, // Placeholder for DLMS_DATA_TYPE
    /// Cipher index.
    pub cipher_index: u16,
    /// Time.
    pub time: u32, // Placeholder for struct tm
    /// Is pre-established.
    pub pre_established: bool,
    /// Invoke ID.
    pub invoke_id: u8,
    /// Block number.
    pub block_number: u16,
    /// Block number ack.
    pub block_number_ack: u16,
    /// Is streaming.
    pub streaming: bool,
    /// Window size.
    pub window_size: u32,
    /// Server address.
    pub server_address: u16,
    /// Client address.
    pub client_address: u16,
}

impl ReplyData {
    /// Creates a new `ReplyData`.
    pub fn new() -> Self {
        Self {
            command: Command::None,
            encrypted_command: Command::None,
            command_type: 0,
            data: Vec::new(),
            complete: false,
            data_value: Variant::new(),
            total_count: 0,
            read_position: 0,
            packet_length: 0,
            peek: false,
            ignore_value: false,
            data_type: 0,
            cipher_index: 0,
            time: 0,
            pre_established: false,
            invoke_id: 0,
            block_number: 0,
            block_number_ack: 0,
            streaming: false,
            window_size: 0,
            server_address: 0,
            client_address: 0,
        }
    }

    /// Clears the `ReplyData`.
    pub fn clear(&mut self) {
        self.clear2(true);
    }

    /// Clears the `ReplyData`.
    pub fn clear2(&mut self, clear_data: bool) {
        if clear_data {
            self.data.clear();
        }
        self.command = Command::None;
        self.encrypted_command = Command::None;
        self.command_type = 0;
        self.complete = false;
        self.data_value.clear();
        self.total_count = 0;
        self.read_position = 0;
        self.packet_length = 0;
        self.peek = false;
        self.ignore_value = false;
        self.data_type = 0;
        self.cipher_index = 0;
        self.time = 0;
        self.pre_established = false;
        self.invoke_id = 0;
        self.block_number = 0;
        self.block_number_ack = 0;
        self.streaming = false;
        self.window_size = 0;
        self.server_address = 0;
        self.client_address = 0;
    }
}
