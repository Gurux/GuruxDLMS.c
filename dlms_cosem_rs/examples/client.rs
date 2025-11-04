extern crate alloc;

use alloc::boxed::Box;
use dlms_cosem_rs::byte_buffer::{ByteBuffer, Error};
use dlms_cosem_rs::client::{Client, Transport};
use dlms_cosem_rs::cosem::data::Data;
use dlms_cosem_rs::variant::Variant;

/// A mock transport layer that simulates a meter, switching its response based on client state.
struct MockTransport {
    /// True if the client has successfully associated via AARQ/AARE handshake.
    associated: bool,
}

impl MockTransport {
    fn new() -> Self {
        Self { associated: false }
    }
}

impl Transport for MockTransport {
    fn send(&mut self, data: &[u8]) -> Result<(), Error> {
        println!("-> Sending: {:?}", data);
        Ok(())
    }

    fn receive(&mut self) -> Result<ByteBuffer, Error> {
        if !self.associated {
            // The first call to `receive` is from `client.connect()`.
            // We reply with a hardcoded, correctly-encoded AARE to accept the connection.
            self.associated = true;

            let aare_pdu = vec![
                0x61, 0x1D, // AARE APDU, total length 29
                // [1] Application Context Name (length 9)
                0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01,
                // [2] Association Result (length 3)
                0xA2, 0x03, 0x02, 0x01, 0x00, // INTEGER 0 (Accepted)
                // [30] User Information (length 11)
                0xBE, 0x0B, // OCTET STRING (length 9)
                0x04, 0x09, // SEQUENCE (length 7)
                0x30, 0x07, // Conformance (INTEGER 1, dummy)
                0x02, 0x01, 0x01, // Max PDU size (INTEGER 1024)
                0x02, 0x02, 0x04, 0x00,
            ];

            println!("<- Receiving AARE: {:?}", aare_pdu);
            Ok(ByteBuffer::from_vec(aare_pdu))
        } else {
            // Subsequent calls are for data. Reply with a GetResponse.
            let response_data = vec![
                1,    // GetResponse.Normal tag
                0x41, // Invoke ID and Priority
                0,    // DataAccessResult.Success tag
                3,    // Data.Boolean tag
                1,    // Length of boolean value
                0xff, // Boolean value `true`
            ];
            println!("<- Receiving GetResponse: {:?}", response_data);
            Ok(ByteBuffer::from_vec(response_data))
        }
    }
}

fn main() {
    let transport = Box::new(MockTransport::new());
    let mut client = Client::new(transport);

    println!("Connecting to meter...");
    client.connect().unwrap();
    println!("Connected.");

    // Create a COSEM Data object representing a value we want to read.
    let obj = Data::new(
        [0, 0, 1, 0, 0, 255], // Logical name (OBIS code)
        Variant::None,        // The client doesn't need the value, just the definition.
    );

    // Read attribute 2 (value) of the Data object.
    println!("Reading value from object...");
    let value = client.read(&obj, 2).unwrap();

    println!("Read value: {:?}", value);

    match value {
        Variant::Boolean(b) => assert!(b),
        _ => panic!("Unexpected variant type"),
    }
    println!("Successfully read and verified the value.");
}
