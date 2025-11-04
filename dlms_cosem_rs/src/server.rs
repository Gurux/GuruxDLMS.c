use crate::apdu::{Command, GetRequest};
use crate::byte_buffer::{ByteBuffer, Error};
use crate::cosem::CosemObject;
use alloc::boxed::Box;
use alloc::vec::Vec;

pub struct Server {
    objects: Vec<Box<dyn CosemObject>>,
}

impl Server {
    pub fn new() -> Self {
        Self {
            objects: Vec::new(),
        }
    }

    pub fn add_object(&mut self, object: Box<dyn CosemObject>) {
        self.objects.push(object);
    }

    pub fn handle_request(&mut self, request: &mut ByteBuffer) -> Result<ByteBuffer, Error> {
        let command = Command::from_bytes(request)?;

        match command {
            Command::GetRequest => {
                let _get_request = GetRequest::from_bytes(request)?;
                // Find the object and get the attribute
                // For now, return an empty GetResponse
                let mut response = ByteBuffer::new();
                response.put_u8(0x01)?; // GetResponseNormal
                response.put_u8(0x41)?; // InvokeIdAndPriority
                response.put_u8(0)?; // Result: success
                Ok(response)
            }
            _ => {
                // Placeholder for other commands
                unimplemented!()
            }
        }
    }
}

impl Command {
    pub fn from_bytes(buffer: &mut ByteBuffer) -> Result<Self, Error> {
        let command_byte = buffer.get_u8()?;
        match command_byte {
            192 => Ok(Command::GetRequest),    // C0 GetRequest
            193 => Ok(Command::SetRequest),    // C1 SetRequest
            195 => Ok(Command::ActionRequest), // C3 ActionRequest
            _ => Err(Error::UnexpectedEof),    // Placeholder
        }
    }
}
