//! DLMS/COSEM Variant data type.

use crate::types::DataType;

/// DLMS/COSEM Variant data type.
pub enum Variant {
    /// None.
    None,
    /// Boolean.
    Boolean(bool),
    /// BitString.
    BitString(Vec<u8>),
    /// Int32.
    Int32(i32),
    /// UInt32.
    UInt32(u32),
    /// String.
    String(Vec<u8>),
    /// OctetString.
    OctetString(Vec<u8>),
    /// Int8.
    Int8(i8),
    /// Int16.
    Int16(i16),
    /// UInt8.
    UInt8(u8),
    /// UInt16.
    UInt16(u16),
    /// Int64.
    Int64(i64),
    /// UInt64.
    UInt64(u64),
    /// Enum.
    Enum(u8),
    /// Float32.
    Float32(f32),
    /// Float64.
    Float64(f64),
    /// DateTime.
    DateTime(Vec<u8>),
    /// Date.
    Date(Vec<u8>),
    /// Time.
    Time(Vec<u8>),
    /// Array.
    Array(Vec<Variant>),
    /// Structure.
    Structure(Vec<Variant>),
    /// CompactArray.
    CompactArray(Vec<Variant>),
}

impl Variant {
    /// Creates a new `Variant`.
    pub fn new() -> Self {
        Variant::None
    }

    /// Clears the `Variant`.
    pub fn clear(&mut self) {
        *self = Variant::None;
    }

    /// Returns the `DataType` of the `Variant`.
    pub fn data_type(&self) -> DataType {
        match self {
            Variant::None => DataType::None,
            Variant::Boolean(_) => DataType::Boolean,
            Variant::BitString(_) => DataType::BitString,
            Variant::Int32(_) => DataType::Int32,
            Variant::UInt32(_) => DataType::Uint32,
            Variant::String(_) => DataType::String,
            Variant::OctetString(_) => DataType::OctetString,
            Variant::Int8(_) => DataType::Int8,
            Variant::Int16(_) => DataType::Int16,
            Variant::UInt8(_) => DataType::Uint8,
            Variant::UInt16(_) => DataType::Uint16,
            Variant::Int64(_) => DataType::Int64,
            Variant::UInt64(_) => DataType::Uint64,
            Variant::Enum(_) => DataType::Enum,
            Variant::Float32(_) => DataType::Float32,
            Variant::Float64(_) => DataType::Float64,
            Variant::DateTime(_) => DataType::DateTime,
            Variant::Date(_) => DataType::Date,
            Variant::Time(_) => DataType::Time,
            Variant::Array(_) => DataType::Array,
            Variant::Structure(_) => DataType::Structure,
            Variant::CompactArray(_) => DataType::CompactArray,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_variant() {
        let mut variant = Variant::UInt32(123);
        assert_eq!(variant.data_type(), DataType::Uint32);
        variant.clear();
        assert_eq!(variant.data_type(), DataType::None);
    }
}
