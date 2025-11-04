use crate::byte_buffer::{ByteBuffer, Error};
use alloc::vec::Vec;

#[derive(Debug, PartialEq, Eq)]
pub enum TagClass {
    Universal,
    Application,
    ContextSpecific,
    Private,
}

#[derive(Debug, PartialEq, Eq)]
pub struct Asn1Tag {
    pub class: TagClass,
    pub constructed: bool,
    pub number: u32,
}

impl Asn1Tag {
    pub fn from_byte(byte: u8) -> (Self, bool) {
        let class = match (byte >> 6) & 0b11 {
            0b00 => TagClass::Universal,
            0b01 => TagClass::Application,
            0b10 => TagClass::ContextSpecific,
            0b11 => TagClass::Private,
            _ => unreachable!(),
        };
        let constructed = (byte & 0b0010_0000) != 0;
        let number = (byte & 0b0001_1111) as u32;
        let is_long_tag = number == 0b0001_1111;

        (
            Self {
                class,
                constructed,
                number,
            },
            is_long_tag,
        )
    }
}

#[derive(Debug)]
pub struct Asn1Object {
    pub tag: Asn1Tag,
    pub value: Vec<u8>,
}

pub fn parse(buffer: &mut ByteBuffer) -> Result<Asn1Object, Error> {
    // 1. Parse Tag
    let tag_byte = buffer.get_u8()?;
    let (mut tag, is_long_tag) = Asn1Tag::from_byte(tag_byte);

    if is_long_tag {
        // A simple implementation for multi-byte tags.
        let mut number: u32 = 0;
        loop {
            let byte = buffer.get_u8()?;
            number = (number << 7) | (byte & 0x7F) as u32;
            if (byte & 0x80) == 0 {
                break;
            }
        }
        tag.number = number;
    }

    // 2. Parse Length
    let len_byte = buffer.get_u8()?;
    let len: usize;

    if (len_byte & 0x80) == 0 {
        // Short form length
        len = len_byte as usize;
    } else {
        // Long form length
        let num_len_bytes = (len_byte & 0x7F) as usize;
        if num_len_bytes > 4 {
            // Prevent excessively large lengths
            return Err(Error::UnexpectedEof);
        }
        let len_bytes = buffer.get_bytes(num_len_bytes)?;
        len = len_bytes
            .iter()
            .fold(0, |acc, &byte| (acc << 8) | byte as usize);
    }

    // 3. Parse Value
    let value = buffer.get_bytes(len)?;

    Ok(Asn1Object { tag, value })
}


#[cfg(test)]
mod tests {
    use super::*;
    use crate::byte_buffer::ByteBuffer;

    #[test]
    fn test_parse_simple_integer() {
        // INTEGER 1
        let data = vec![0x02, 0x01, 0x01];
        let mut bb = ByteBuffer::from_vec(data);
        let obj = parse(&mut bb).unwrap();
        assert_eq!(obj.tag.class, TagClass::Universal);
        assert_eq!(obj.tag.constructed, false);
        assert_eq!(obj.tag.number, 2); // INTEGER tag number
        assert_eq!(obj.value, vec![0x01]);
    }

    #[test]
    fn test_parse_octet_string() {
        // OCTET STRING "hello"
        let data = vec![0x04, 0x05, 0x68, 0x65, 0x6c, 0x6c, 0x6f];
        let mut bb = ByteBuffer::from_vec(data);
        let obj = parse(&mut bb).unwrap();
        assert_eq!(obj.tag.class, TagClass::Universal);
        assert_eq!(obj.tag.number, 4); // OCTET STRING
        assert_eq!(obj.value, b"hello");
    }

    #[test]
    fn test_parse_long_length() {
        // SEQUENCE with a 256-byte value
        let mut data = vec![0x30, 0x82, 0x01, 0x00];
        let value = vec![0xAA; 256];
        data.extend_from_slice(&value);
        let mut bb = ByteBuffer::from_vec(data);
        let obj = parse(&mut bb).unwrap();
        assert_eq!(obj.tag.class, TagClass::Universal);
        assert_eq!(obj.tag.constructed, true);
        assert_eq!(obj.tag.number, 16); // SEQUENCE
        assert_eq!(obj.value, value);
    }

    #[test]
    fn test_parse_nested_structure() {
        // SEQUENCE { INTEGER 1, OCTET STRING "hi" }
        let data = vec![0x30, 0x07, 0x02, 0x01, 0x01, 0x04, 0x02, 0x68, 0x69];
        let mut bb = ByteBuffer::from_vec(data);
        let obj = parse(&mut bb).unwrap();
        assert_eq!(obj.tag.number, 16); // SEQUENCE
        let mut inner_bb = ByteBuffer::from_vec(obj.value);

        let int_obj = parse(&mut inner_bb).unwrap();
        assert_eq!(int_obj.tag.number, 2); // INTEGER
        assert_eq!(int_obj.value, vec![0x01]);

        let octet_obj = parse(&mut inner_bb).unwrap();
        assert_eq!(octet_obj.tag.number, 4); // OCTET STRING
        assert_eq!(octet_obj.value, b"hi");
    }
}
