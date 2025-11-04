use crate::asn1;
use crate::byte_buffer::{ByteBuffer, Error};
use crate::bit_array::BitArray;
use alloc::string::{String, FromUtf8Error};
use alloc::vec::Vec;


#[cfg(feature = "std")]
use std::time::SystemTime;

#[cfg(not(feature = "std"))]
#[derive(Clone, Copy, Debug, PartialEq, Eq, PartialOrd, Ord)]
pub struct SystemTime;


#[derive(Clone, Debug)]
pub enum Variant {
    None,
    Boolean(bool),
    Int8(i8),
    Int16(i16),
    Int32(i32),
    Int64(i64),
    UInt8(u8),
    UInt16(u16),
    UInt32(u32),
    UInt64(u64),
    Float32(f32),
    Float64(f64),
    Enum(u8),
    OctetString(ByteBuffer),
    String(String),
    BitString(BitArray),
    DateTime(SystemTime),
    Date,
    Time,
    Array(Vec<Variant>),
    Struct(Vec<Variant>),
}

impl Variant {
    pub fn from_bytes(buffer: &mut ByteBuffer) -> Result<Self, Error> {
        let obj = asn1::parse(buffer)?;

        match obj.tag.number {
            // Boolean
            3 => Ok(Variant::Boolean(obj.value[0] != 0)),
            // Integer
            5 => Ok(Variant::Int8(obj.value[0] as i8)),
            6 => Ok(Variant::Int16(i16::from_be_bytes(
                obj.value.as_slice().try_into().map_err(|_| Error::InvalidData)?,
            ))),
            // Octet String
            9 => Ok(Variant::OctetString(ByteBuffer::from_vec(obj.value))),
            // UTF8String
            12 => Ok(Variant::String(String::from_utf8(obj.value)?)),
            // Sequence for Array/Struct
            16 => {
                let mut items = Vec::new();
                let mut inner_buffer = ByteBuffer::from_vec(obj.value);
                while inner_buffer.remaining() > 0 {
                    items.push(Variant::from_bytes(&mut inner_buffer)?);
                }
                // Differentiate based on context, for now assume Array
                Ok(Variant::Array(items))
            }
            _ => Err(Error::InvalidData),
        }
    }
}

impl From<FromUtf8Error> for Error {
    fn from(_: FromUtf8Error) -> Self {
        Error::InvalidData
    }
}
