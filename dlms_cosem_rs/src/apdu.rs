use crate::asn1;
use crate::byte_buffer::{ByteBuffer, Error};
use crate::variant::Variant;

#[derive(Debug, PartialEq, Eq)]
pub enum Command {
    GetRequest,
    SetRequest,
    ActionRequest,
    AssociationRequest,
}

// OID for LN without Ciphering: 2.16.756.5.8.1.1
pub const LN_NO_CIPHERING: [u8; 6] = [0x60, 0x85, 0x74, 0x05, 0x08, 0x01];

#[derive(Debug, PartialEq, Eq, Clone, Copy)]
pub enum ApplicationContextName {
    LogicalName,
    ShortName,
    LogicalNameWithCiphering,
    ShortNameWithCiphering,
}

#[derive(Debug, PartialEq, Eq)]
pub struct Aarq {
    pub application_context_name: ApplicationContextName,
    pub sender_acse_requirements: u8,
    pub proposed_conformance: u32,
    pub proposed_max_pdu_size: u16,
}

#[derive(Debug, PartialEq, Eq, Clone, Copy)]
#[repr(u8)]
pub enum AssociationResult {
    Accepted = 0,
    RejectedPermanent = 1,
    RejectedTransient = 2,
}

#[derive(Debug, PartialEq, Eq)]
pub struct Aare {
    pub application_context_name: ApplicationContextName,
    pub association_result: AssociationResult,
    pub negotiated_conformance: u32,
    pub negotiated_max_pdu_size: u16,
}

impl Aarq {
    pub fn to_bytes(&self, buffer: &mut ByteBuffer) -> Result<(), Error> {
        // AARQ APDU Tag
        buffer.put_u8(0x60)?;
        // Length placeholder
        let len_pos = buffer.position();
        buffer.put_u8(0x00)?;
        let content_pos = buffer.position();

        // Application Context Name
        buffer.put_u8(0xA1)?; // Tag [1]
        buffer.put_u8(0x09)?; // Length
        buffer.put_u8(0x06)?; // OBJECT IDENTIFIER
        buffer.put_u8(0x07)?; // Length
        buffer.put_bytes(&LN_NO_CIPHERING)?;
        buffer.put_u8(0x01)?;

        // User Information
        buffer.put_u8(0xBE)?; // Tag [30]
        buffer.put_u8(0x10)?; // Length
        buffer.put_u8(0x04)?; // OCTET STRING
        buffer.put_u8(0x0E)?; // Length
        buffer.put_u8(0x01)?; // Conformance Negotiation
        buffer.put_u8(0x00)?;
        buffer.put_u8(0x00)?;
        buffer.put_u8(0x00)?;
        buffer.put_u8(0x06)?; // PDU Size
        buffer.put_u16(self.proposed_max_pdu_size)?;

        // Update length
        let len = buffer.position() - content_pos;
        buffer.set_u8(len_pos, len as u8)?;

        Ok(())
    }
}

impl AssociationResult {
    pub fn from_u8(value: u8) -> Result<Self, Error> {
        match value {
            0 => Ok(AssociationResult::Accepted),
            1 => Ok(AssociationResult::RejectedPermanent),
            2 => Ok(AssociationResult::RejectedTransient),
            _ => Err(Error::InvalidData),
        }
    }
}

impl Aare {
    pub fn from_bytes(buffer: &mut ByteBuffer) -> Result<Self, Error> {
        let aare_apdu = asn1::parse(buffer)?;
        if aare_apdu.tag.class != asn1::TagClass::Application || aare_apdu.tag.number != 1 {
            return Err(Error::InvalidData);
        }

        let mut inner_buffer = ByteBuffer::from_vec(aare_apdu.value);
        let mut application_context_name = None;
        let mut association_result = None;
        let mut negotiated_max_pdu_size = None;

        while inner_buffer.remaining() > 0 {
            let obj = asn1::parse(&mut inner_buffer)?;
            match obj.tag.number {
                1 => {
                    application_context_name = Some(ApplicationContextName::LogicalName);
                },
                2 => {
                    let mut result_buffer = ByteBuffer::from_vec(obj.value);
                    let result_obj = asn1::parse(&mut result_buffer)?;
                    association_result = Some(AssociationResult::from_u8(result_obj.value[0])?);
                },
                3 => {
                    // Result Source Diagnostic, just ignore it for now
                },
                30 => {
                    let mut user_info_buffer = ByteBuffer::from_vec(obj.value);
                    let user_info_octet_string = asn1::parse(&mut user_info_buffer)?;
                    let mut initiate_response_buffer = ByteBuffer::from_vec(user_info_octet_string.value);
                    let initiate_response_seq = asn1::parse(&mut initiate_response_buffer)?;

                    let mut seq_content_buffer = ByteBuffer::from_vec(initiate_response_seq.value);

                    let _conformance_obj = asn1::parse(&mut seq_content_buffer)?;
                    let pdu_size_obj = asn1::parse(&mut seq_content_buffer)?;
                    negotiated_max_pdu_size = Some(u16::from_be_bytes(
                        pdu_size_obj.value.as_slice().try_into().map_err(|_| Error::InvalidData)?
                    ));
                }
                _ => return Err(Error::InvalidData),
            }
        }

        Ok(Aare {
            application_context_name: application_context_name.ok_or(Error::InvalidData)?,
            association_result: association_result.ok_or(Error::InvalidData)?,
            negotiated_conformance: 0,
            negotiated_max_pdu_size: negotiated_max_pdu_size.ok_or(Error::InvalidData)?,
        })
    }
}

#[derive(Debug, PartialEq, Eq)]
pub struct CosemAttributeDescriptor {
    pub class_id: u16,
    pub instance_id: [u8; 6],
    pub attribute_id: i8,
}

impl CosemAttributeDescriptor {
    pub fn to_bytes(&self, buffer: &mut ByteBuffer) -> Result<(), Error> {
        buffer.put_u16(self.class_id)?;
        buffer.put_bytes(&self.instance_id)?;
        buffer.put_u8(self.attribute_id as u8)?;
        Ok(())
    }

    pub fn from_bytes(buffer: &mut ByteBuffer) -> Result<Self, Error> {
        let class_id = buffer.get_u16()?;
        let mut instance_id = [0u8; 6];
        instance_id.copy_from_slice(&buffer.get_bytes(6)?);
        let attribute_id = buffer.get_u8()? as i8;
        Ok(Self {
            class_id,
            instance_id,
            attribute_id,
        })
    }
}


#[derive(Debug, PartialEq, Eq)]
pub enum GetRequest {
    Normal {
        invoke_id_and_priority: u8,
        attribute_descriptor: CosemAttributeDescriptor,
    },
    // Other GetRequest variants...
}

impl GetRequest {
    pub fn to_bytes(&self, buffer: &mut ByteBuffer) -> Result<(), Error> {
        match self {
            GetRequest::Normal {
                invoke_id_and_priority,
                attribute_descriptor,
            } => {
                buffer.put_u8(1)?; // Tag for GetRequestNormal
                buffer.put_u8(*invoke_id_and_priority)?;
                attribute_descriptor.to_bytes(buffer)?;
                buffer.put_u8(0)?; // Access selector: no
            } // Other variants...
        }
        Ok(())
    }

    pub fn from_bytes(buffer: &mut ByteBuffer) -> Result<Self, Error> {
        let request_type = buffer.get_u8()?;
        match request_type {
            1 => {
                // GetRequestNormal
                let invoke_id_and_priority = buffer.get_u8()?;
                let attribute_descriptor = CosemAttributeDescriptor::from_bytes(buffer)?;
                let _access_selector = buffer.get_u8()?;
                Ok(GetRequest::Normal {
                    invoke_id_and_priority,
                    attribute_descriptor,
                })
            }
            _ => Err(Error::UnexpectedEof), // Placeholder
        }
    }
}


#[derive(Debug, Clone)]
pub enum DataAccessResult {
    Success(Variant),
    Error(u8),
}

impl DataAccessResult {
    pub fn from_bytes(buffer: &mut ByteBuffer) -> Result<Self, Error> {
        let tag = buffer.get_u8()?;
        match tag {
            0 => { // Success
                let data = Variant::from_bytes(buffer)?;
                Ok(DataAccessResult::Success(data))
            },
            _ => Err(Error::InvalidData)
        }
    }
}

#[derive(Debug, Clone)]
pub enum GetResponse {
    Normal {
        invoke_id_and_priority: u8,
        result: DataAccessResult,
    },
    // Other GetResponse variants...
}

impl GetResponse {
    pub fn from_bytes(buffer: &mut ByteBuffer) -> Result<Self, Error> {
        let response_type = buffer.get_u8()?;
        match response_type {
            1 => { // GetResponseNormal
                let invoke_id_and_priority = buffer.get_u8()?;
                let result = DataAccessResult::from_bytes(buffer)?;
                Ok(GetResponse::Normal {
                    invoke_id_and_priority,
                    result,
                })
            }
            _ => Err(Error::InvalidData),
        }
    }
}


#[cfg(test)]
mod tests {
    use super::*;
    use crate::byte_buffer::ByteBuffer;

    #[test]
    fn test_aare_deserialization() {
        let aare_pdu = vec![
            0x61, 0x2D, 0x80, 0x02, 0x02, 0x84, 0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01, 0xA2, 0x03, 0x02, 0x01, 0x00, 0xA3, 0x05, 0xA1, 0x03, 0x02, 0x01, 0x00, 0xBE, 0x10, 0x04, 0x0E, 0x08, 0x00, 0x06, 0x5F, 0x1F, 0x04, 0x00, 0x00, 0x10, 0x15, 0x08, 0x00, 0x00, 0x07
        ];
        let mut buffer = ByteBuffer::from_vec(aare_pdu);
        let aare = Aare::from_bytes(&mut buffer).unwrap();

        assert_eq!(aare.application_context_name, ApplicationContextName::LogicalName);
        assert_eq!(aare.association_result, AssociationResult::Accepted);
        assert_eq!(aare.negotiated_max_pdu_size, 1815);
    }

    #[test]
    fn test_get_request_normal_serialization() {
        let attribute_descriptor = CosemAttributeDescriptor {
            class_id: 1,
            instance_id: [0, 0, 1, 0, 0, 255],
            attribute_id: 2,
        };
        let req = GetRequest::Normal {
            invoke_id_and_priority: 0x41,
            attribute_descriptor,
        };

        let mut bb = ByteBuffer::new();
        req.to_bytes(&mut bb).unwrap();

        let mut expected = ByteBuffer::new();
        expected.put_u8(1).unwrap();
        expected.put_u8(0x41).unwrap();
        expected.put_u16(1).unwrap();
        expected.put_bytes(&[0, 0, 1, 0, 0, 255]).unwrap();
        expected.put_u8(2).unwrap();
        expected.put_u8(0).unwrap();

        assert_eq!(bb.len(), expected.len());
        bb.set_position(0);
        expected.set_position(0);

        assert_eq!(bb.get_bytes(bb.len()).unwrap(), expected.get_bytes(expected.len()).unwrap());
    }
}
