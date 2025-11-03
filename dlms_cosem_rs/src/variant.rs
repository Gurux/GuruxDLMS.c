use crate::byte_buffer::ByteBuffer;
use crate::bit_array::BitArray;
use alloc::string::String;
use alloc::vec::Vec;


#[cfg(feature = "std")]
use std::time::SystemTime;

#[cfg(not(feature = "std"))]
#[derive(Clone, Copy, Debug, PartialEq, Eq, PartialOrd, Ord)]
pub struct SystemTime;


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
