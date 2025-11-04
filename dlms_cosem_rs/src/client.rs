use crate::cosem::CosemObject;
use crate::variant::Variant;
use crate::byte_buffer::{ByteBuffer, Error};
use crate::apdu::{
    GetRequest, GetResponse, CosemAttributeDescriptor, DataAccessResult,
    Aarq, Aare, ApplicationContextName, AssociationResult,
};
use alloc::boxed::Box;
use alloc::vec::Vec;


pub trait Transport {
    fn send(&mut self, data: &[u8]) -> Result<(), Error>;
    fn receive(&mut self) -> Result<ByteBuffer, Error>;
}

pub struct Client {
    transport: Box<dyn Transport>,
    buffer: ByteBuffer,
}

impl Client {
    pub fn new(transport: Box<dyn Transport>) -> Self {
        Self {
            transport,
            buffer: ByteBuffer::with_capacity(1024),
        }
    }

    pub fn connect(&mut self) -> Result<(), Error> {
        let aarq = Aarq {
            application_context_name: ApplicationContextName::LogicalName,
            sender_acse_requirements: 0,
            proposed_conformance: 0,
            proposed_max_pdu_size: 1024,
        };

        self.buffer.clear();
        aarq.to_bytes(&mut self.buffer)?;

        let mut vec: Vec<u8> = Vec::new();
        let bytes = self.buffer.get_bytes(self.buffer.len())?;
        vec.extend_from_slice(&bytes);
        self.transport.send(&vec)?;

        let mut response_buffer = self.transport.receive()?;
        let aare = Aare::from_bytes(&mut response_buffer)?;

        if aare.association_result == AssociationResult::Accepted {
            Ok(())
        } else {
            Err(Error::InvalidData)
        }
    }

    pub fn read(&mut self, object: &dyn CosemObject, attribute_id: u8) -> Result<Variant, Error> {
        let attribute_descriptor = CosemAttributeDescriptor {
            class_id: object.object_type(),
            instance_id: object.logical_name().try_into().map_err(|_| Error::InvalidData)?,
            attribute_id: attribute_id as i8,
        };

        let req = GetRequest::Normal {
            invoke_id_and_priority: 0x41,
            attribute_descriptor,
        };

        self.buffer.clear();
        req.to_bytes(&mut self.buffer)?;

        let mut vec: Vec<u8> = Vec::new();
        let bytes = self.buffer.get_bytes(self.buffer.len())?;
        vec.extend_from_slice(&bytes);
        self.transport.send(&vec)?;

        let mut response_buffer = self.transport.receive()?;
        let response = GetResponse::from_bytes(&mut response_buffer)?;

        match response {
            GetResponse::Normal { result, .. } => match result {
                DataAccessResult::Success(variant) => Ok(variant),
                DataAccessResult::Error(_) => Err(Error::InvalidData),
            },
            // _ => Err(Error::InvalidData),
        }
    }

    pub fn write(
        &mut self,
        _object: &mut dyn CosemObject,
        _attribute_id: u8,
        _value: Variant,
    ) -> Result<(), Error> {
        // Placeholder for SetRequest
        Err(Error::InvalidData)
    }
}
