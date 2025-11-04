use crate::axdr::InitiateResponse;
use crate::byte_buffer::{ByteBuffer, Error};
use asn1_rs::{
    nom::sequence::tuple, Any, Class, FromBer, Integer, OctetString, Oid, OptTaggedImplicit, Tag,
    Tagged, TaggedExplicit,
};

// ... (structs remain the same)

#[derive(Debug, PartialEq, Eq)]
pub enum Command {
    GetRequest,
    SetRequest,
    ActionRequest,
    AssociationRequest,
}

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

#[derive(Debug)]
struct AareApdu<'a> {
    application_context_name: Oid<'a>,
    result: AssociationResult,
    result_source_diagnostic: AssociateSourceDiagnostic<'a>,
    user_information: Option<OctetString<'a>>,
}

#[derive(Debug)]
enum AssociateSourceDiagnostic<'a> {
    AcseServiceUser(Integer<'a>),
    AcseServiceProvider(Integer<'a>),
}

const TAG_ACSE_SERVICE_USER: Tag = Tag { class: Class::ContextSpecific, constructed: true, number: 1 };
const TAG_ACSE_SERVICE_PROVIDER: Tag = Tag { class: Class::ContextSpecific, constructed: true, number: 2 };

impl<'a> TryFrom<Any<'a>> for AssociateSourceDiagnostic<'a> {
    type Error = asn1_rs::Error;

    fn try_from(any: Any<'a>) -> Result<Self, Self::Error> {
        println!("Parsing AssociateSourceDiagnostic from: {:?}", any);
        match any.tag() {
            TAG_ACSE_SERVICE_USER => {
                let (_, integer) = Integer::from_ber(any.data)?;
                Ok(Self::AcseServiceUser(integer))
            }
            TAG_ACSE_SERVICE_PROVIDER => {
                let (_, integer) = Integer::from_ber(any.data)?;
                Ok(Self::AcseServiceProvider(integer))
            }
            _ => Err(asn1_rs::Error::BerTypeError),
        }
    }
}


impl<'a> TryFrom<Any<'a>> for AareApdu<'a> {
    type Error = asn1_rs::Error;

    fn try_from(any: Any<'a>) -> Result<Self, Self::Error> {
        println!("Parsing AareApdu from: {:?}", any);
        any.tag().assert_eq(Tag { class: Class::Universal, constructed: true, number: 16 })?; // SEQUENCE
        let (rem, (application_context_name, result, result_source_diagnostic, user_information)) =
            tuple((
                TaggedExplicit::<Oid, _, 1>::from_ber,
                TaggedExplicit::<Integer, _, 2>::from_ber,
                TaggedExplicit::<Any, _, 3>::from_ber, // Parse as Any first for CHOICE
                OptTaggedImplicit::<OctetString, _, 30>::from_ber,
            ))(any.data)?;

        if !rem.is_empty() {
            println!("Remaining bytes in AareApdu parser: {:?}", rem);
            return Err(asn1_rs::Error::BerTypeError);
        }

        let result_source_diagnostic =
            AssociateSourceDiagnostic::try_from(result_source_diagnostic.into_inner())?;

        Ok(Self {
            application_context_name: application_context_name.into_inner(),
            result: AssociationResult::from_u8(result.into_inner().as_u8().unwrap()).unwrap(),
            result_source_diagnostic,
            user_information: user_information.map(|t| t.into_inner()),
        })
    }
}


impl Aare {
    pub fn from_bytes(data: &[u8]) -> Result<Self, Error> {
        println!("AARE PDU: {:?}", data);
        let (rem, outer_any) = Any::from_ber(data).map_err(|e| {
            println!("BER Error: {:?}", e);
            Error::InvalidData
        })?;

        if !rem.is_empty() {
            return Err(Error::InvalidData);
        }

        if outer_any.tag() != (Tag { class: Class::Application, constructed: true, number: 1 }) {
            return Err(Error::InvalidData);
        }

        let (_, inner_any) = Any::from_ber(outer_any.data).map_err(|_| Error::InvalidData)?;

        let aare_apdu = AareApdu::try_from(inner_any).map_err(|e| {
            println!("AareApdu parse error: {:?}", e);
            Error::InvalidData
        })?;

        let user_info = aare_apdu.user_information.ok_or(Error::InvalidData)?;
        let mut user_info_buffer = ByteBuffer::from_vec(user_info.as_ref().to_vec());
        let initiate_response = InitiateResponse::from_bytes(&mut user_info_buffer)?;

        Ok(Aare {
            application_context_name: ApplicationContextName::LogicalName,
            association_result: aare_apdu.result,
            negotiated_conformance: initiate_response.negotiated_conformance,
            negotiated_max_pdu_size: initiate_response.negotiated_max_pdu_size,
        })
    }
}

// ... (rest of the file is the same)
impl Aarq {
    pub fn to_bytes(&self, _buffer: &mut ByteBuffer) -> Result<(), Error> {
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
}

impl GetRequest {
    pub fn to_bytes(&self, _buffer: &mut ByteBuffer) -> Result<(), Error> {
        Ok(())
    }

    pub fn from_bytes(_buffer: &mut ByteBuffer) -> Result<Self, Error> {
        Ok(GetRequest::Normal {
            invoke_id_and_priority: 0,
            attribute_descriptor: CosemAttributeDescriptor {
                class_id: 0,
                instance_id: [0; 6],
                attribute_id: 0,
            },
        })
    }
}

#[derive(Debug, Clone)]
pub enum DataAccessResult {
    Success(crate::variant::Variant),
    Error(u8),
}

impl DataAccessResult {
    pub fn from_bytes(_buffer: &mut ByteBuffer) -> Result<Self, Error> {
        Ok(DataAccessResult::Error(0))
    }
}

#[derive(Debug, Clone)]
pub enum GetResponse {
    Normal {
        invoke_id_and_priority: u8,
        result: DataAccessResult,
    },
}

impl GetResponse {
    pub fn from_bytes(_buffer: &mut ByteBuffer) -> Result<Self, Error> {
        Ok(GetResponse::Normal {
            invoke_id_and_priority: 0,
            result: DataAccessResult::Error(0),
        })
    }
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_aare_deserialization() {
        let aare_pdu = vec![
            0x61, 0x29,
            0xA1, 0x09,
                0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01,
            0xA2, 0x03,
                0x02, 0x01, 0x00,
            0xA3, 0x05,
                0xA1, 0x03,
                    0x02, 0x01, 0x00,
            0xBE, 0x0A,
                0x04, 0x08,
                    0x06, 0x03, 0x5F, 0x1F, 0x04, 0x00, 0x07, 0x80,
        ];
        let aare = Aare::from_bytes(&aare_pdu).unwrap();

        assert_eq!(aare.association_result, AssociationResult::Accepted);
        assert_eq!(aare.negotiated_conformance, 0x005F1F04);
        assert_eq!(aare.negotiated_max_pdu_size, 1920);
    }
}
