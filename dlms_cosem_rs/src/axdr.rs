use crate::byte_buffer::{ByteBuffer, Error};

#[derive(Debug, PartialEq, Eq)]
pub struct InitiateResponse {
    pub negotiated_dlms_version_number: u8,
    pub negotiated_conformance: u32,
    pub negotiated_max_pdu_size: u16,
    pub vaa_name: u16,
}

impl InitiateResponse {
    pub fn from_bytes(buffer: &mut ByteBuffer) -> Result<Self, Error> {
        // DLMS Version Number
        let negotiated_dlms_version_number = buffer.get_u8()?;

        // Negotiated Conformance
        let conformance_len = buffer.get_u8()?;
        let conformance_bytes = buffer.get_bytes(conformance_len as usize)?;
        let mut negotiated_conformance = 0;
        for (i, byte) in conformance_bytes.iter().enumerate() {
            negotiated_conformance |= (*byte as u32) << (8 * (conformance_bytes.len() - 1 - i));
        }

        // Negotiated Max PDU Size
        let negotiated_max_pdu_size = buffer.get_u16()?;

        // VAA Name
        let vaa_name = buffer.get_u16()?;

        Ok(Self {
            negotiated_dlms_version_number,
            negotiated_conformance,
            negotiated_max_pdu_size,
            vaa_name,
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::byte_buffer::ByteBuffer;

    #[test]
    fn test_initiate_response_deserialization() {
        let user_information = vec![0x06, 0x03, 0x5f, 0x1f, 0x04, 0x07, 0x80, 0x00, 0x00];
        let mut buffer = ByteBuffer::from_vec(user_information);
        let initiate_response = InitiateResponse::from_bytes(&mut buffer).unwrap();

        assert_eq!(initiate_response.negotiated_dlms_version_number, 6);
        assert_eq!(initiate_response.negotiated_conformance, 0x5f1f04);
        assert_eq!(initiate_response.negotiated_max_pdu_size, 1920);
        assert_eq!(initiate_response.vaa_name, 0);
    }
}
