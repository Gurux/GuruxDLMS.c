use crate::byte_buffer::{ByteBuffer, Error};
use crate::variant::Variant;
use crate::axdr::InitiateResponse;
use asn1_rs::{
    ApplicationImplicit, FromBer, Integer, OctetString, Oid, Tagged,
    Tag, TaggedImplicit,
};

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

struct AareApdu<'a> {
    application_context_name: Oid<'a>,
    association_result: AssociationResultAsn<'a>,
    result_source_diagnostic: ResultSourceDiagnostic<'a>,
    user_information: OctetString<'a>,
}

struct AssociationResultAsn<'a> {
    result: Integer<'a>,
}

struct ResultSourceDiagnostic<'a> {
    acse_service_user: Integer<'a>,
}

impl<'a> TryFrom<asn1_rs::Any<'a>> for AareApdu<'a> {
    type Error = asn1_rs::Error;

    fn try_from(any: asn1_rs::Any<'a>) -> Result<Self, Self::Error> {
        any.tag().assert_eq(Self::TAG)?;
        let (rem, (application_context_name, association_result, result_source_diagnostic, user_information)) =
            asn1_rs::nom::sequence::tuple((
                TaggedImplicit::<_, _, 1>::from_ber,
                TaggedImplicit::<_, _, 2>::from_ber,
                TaggedImplicit::<_, _, 3>::from_ber,
                TaggedImplicit::<_, _, 30>::from_ber,
            ))(any.data)?;
        if !rem.is_empty() {
            return Err(asn1_rs::Error::BerTypeError);
        }
        Ok(Self {
            application_context_name: application_context_name.into_inner(),
            association_result: association_result.into_inner(),
            result_source_diagnostic: result_source_diagnostic.into_inner(),
            user_information: user_information.into_inner(),
        })
    }
}

impl<'a> Tagged for AareApdu<'a> {
    const TAG: Tag = Tag::Sequence;
}

impl<'a> TryFrom<asn1_rs::Any<'a>> for AssociationResultAsn<'a> {
    type Error = asn1_rs::Error;

    fn try_from(any: asn1_rs::Any<'a>) -> Result<Self, Self::Error> {
        any.tag().assert_eq(Self::TAG)?;
        let (rem, result) = Integer::from_ber(any.data)?;
        if !rem.is_empty() {
            return Err(asn1_rs::Error::BerTypeError);
        }
        Ok(Self { result })
    }
}

impl<'a> Tagged for AssociationResultAsn<'a> {
    const TAG: Tag = Tag::Sequence;
}

impl<'a> TryFrom<asn1_rs::Any<'a>> for ResultSourceDiagnostic<'a> {
    type Error = asn1_rs::Error;

    fn try_from(any: asn1_rs::Any<'a>) -> Result<Self, Self::Error> {
        any.tag().assert_eq(Self::TAG)?;
        let (rem, acse_service_user) =
            TaggedImplicit::<_, _, 1>::from_ber(any.data)?;
        if !rem.is_empty() {
            return Err(asn1_rs::Error::BerTypeError);
        }
        Ok(Self { acse_service_user: acse_service_user.into_inner() })
    }
}

impl<'a> Tagged for ResultSourceDiagnostic<'a> {
    const TAG: Tag = Tag::Sequence;
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

type AarePdu<'a> = ApplicationImplicit<AareApdu<'a>, asn1_rs::Error, 1>;

impl Aare {
    pub fn from_bytes(data: &[u8]) -> Result<Self, Error> {
        println!("AARE PDU: {:?}", data);
        let (_, pdu) = AarePdu::from_ber(data).map_err(|e| {
            println!("BER Error: {:?}", e);
            Error::InvalidData
        })?;
        let aare_apdu = pdu.into_inner();

        println!("User Information: {:?}", aare_apdu.user_information.as_ref());

        let result = AssociationResult::from_u8(
            aare_apdu
                .association_result
                .result
                .as_u8()
                .map_err(|_| Error::InvalidData)?,
        )?;

        let mut user_info_buffer = ByteBuffer::from_vec(aare_apdu.user_information.as_ref().to_vec());
        let initiate_response = InitiateResponse::from_bytes(&mut user_info_buffer)?;

        Ok(Aare {
            application_context_name: ApplicationContextName::LogicalName, // Placeholder
            association_result: result,
            negotiated_conformance: initiate_response.negotiated_conformance,
            negotiated_max_pdu_size: initiate_response.negotiated_max_pdu_size,
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
                let attribute_descriptor =
                    CosemAttributeDescriptor::from_bytes(buffer)?;
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
            0 => {
                // Success
                let data = Variant::from_bytes(buffer)?;
                Ok(DataAccessResult::Success(data))
            }
            _ => Err(Error::InvalidData),
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
            1 => {
                // GetResponseNormal
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
            0x61, 0x29, 0xa1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08,
            0x01, 0x01, 0xa2, 0x03, 0x02, 0x01, 0x00, 0xa3, 0x05, 0xa1, 0x03,
            0x02, 0x01, 0x00, 0xbe, 0x0a, 0x04, 0x08, 0x06, 0x03, 0x5f, 0x1f,
            0x04, 0x00, 0x07, 0x80,
        ];
        let aare = Aare::from_bytes(&aare_pdu).unwrap();

        assert_eq!(
            aare.application_context_name,
            ApplicationContextName::LogicalName
        );
        assert_eq!(aare.association_result, AssociationResult::Accepted);
        assert_eq!(aare.negotiated_conformance, 0x5f1f04);
        assert_eq!(aare.negotiated_max_pdu_size, 1920);
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
        expected
            .put_bytes(&[0, 0, 1, 0, 0, 255])
            .unwrap();
        expected.put_u8(2).unwrap();
        expected.put_u8(0).unwrap();

        assert_eq!(bb.len(), expected.len());
        bb.set_position(0);
        expected.set_position(0);

        assert_eq!(
            bb.get_bytes(bb.len()).unwrap(),
            expected.get_bytes(expected.len()).unwrap()
        );
    }
}
