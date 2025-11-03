use std::fmt;
use std::ops::{Deref, DerefMut};
use std::vec::Vec;

use super::buffer::ByteBuffer;
use super::enums::DataType;

#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};

/// Raw DLMS timestamp value stored in the binary representation defined by the standard.
#[derive(Clone, Debug, PartialEq, Eq, Hash)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
pub struct DlmsDateTime {
    #[cfg_attr(feature = "serde", serde(with = "serde_bytes"))]
    raw: Vec<u8>,
}

impl DlmsDateTime {
    /// Creates a new timestamp from the raw octet representation.
    pub fn new(raw: Vec<u8>) -> Self {
        Self { raw }
    }

    /// Returns the underlying raw representation.
    pub fn as_bytes(&self) -> &[u8] {
        &self.raw
    }
}

impl From<Vec<u8>> for DlmsDateTime {
    fn from(value: Vec<u8>) -> Self {
        Self::new(value)
    }
}

impl AsRef<[u8]> for DlmsDateTime {
    fn as_ref(&self) -> &[u8] {
        self.as_bytes()
    }
}

/// High level representation of the legacy `dlmsVARIANT` container.
#[derive(Clone, Debug, PartialEq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
pub enum Variant {
    /// Represents the absence of a value.
    Null,
    /// Boolean value.
    Boolean(bool),
    /// 8-bit signed integer value.
    Int8(i8),
    /// 16-bit signed integer value.
    Int16(i16),
    /// 32-bit signed integer value.
    Int32(i32),
    /// 64-bit signed integer value.
    Int64(i64),
    /// 8-bit unsigned integer value.
    UInt8(u8),
    /// 16-bit unsigned integer value.
    UInt16(u16),
    /// 32-bit unsigned integer value.
    UInt32(u32),
    /// 64-bit unsigned integer value.
    UInt64(u64),
    /// 32-bit floating point value.
    Float32(f32),
    /// 64-bit floating point value.
    Float64(f64),
    /// Enumerated value encoded as an unsigned byte.
    Enum(u8),
    /// Octet string buffer.
    OctetString(ByteBuffer),
    /// Bit-string buffer.
    BitString(ByteBuffer),
    /// ASCII string value.
    String(String),
    /// UTF-8 string value.
    Utf8String(String),
    /// Compact array encoded as bytes.
    CompactArray(ByteBuffer),
    /// DLMS date-time value.
    DateTime(DlmsDateTime),
    /// DLMS date value.
    Date(DlmsDateTime),
    /// DLMS time value.
    Time(DlmsDateTime),
    /// Array of nested variants.
    Array(VariantCollection),
    /// Structure of nested variants.
    Structure(VariantCollection),
}

impl Variant {
    /// Returns the DLMS data type tag of the variant.
    pub fn data_type(&self) -> DataType {
        match self {
            Variant::Null => DataType::None,
            Variant::Boolean(_) => DataType::Boolean,
            Variant::Int8(_) => DataType::Int8,
            Variant::Int16(_) => DataType::Int16,
            Variant::Int32(_) => DataType::Int32,
            Variant::Int64(_) => DataType::Int64,
            Variant::UInt8(_) => DataType::Uint8,
            Variant::UInt16(_) => DataType::Uint16,
            Variant::UInt32(_) => DataType::Uint32,
            Variant::UInt64(_) => DataType::Uint64,
            Variant::Float32(_) => DataType::Float32,
            Variant::Float64(_) => DataType::Float64,
            Variant::Enum(_) => DataType::Enum,
            Variant::OctetString(_) => DataType::OctetString,
            Variant::BitString(_) => DataType::BitString,
            Variant::String(_) => DataType::String,
            Variant::Utf8String(_) => DataType::StringUtf8,
            Variant::CompactArray(_) => DataType::CompactArray,
            Variant::DateTime(_) => DataType::DateTime,
            Variant::Date(_) => DataType::Date,
            Variant::Time(_) => DataType::Time,
            Variant::Array(_) => DataType::Array,
            Variant::Structure(_) => DataType::Structure,
        }
    }
}

impl From<bool> for Variant {
    fn from(value: bool) -> Self {
        Variant::Boolean(value)
    }
}

impl From<i8> for Variant {
    fn from(value: i8) -> Self {
        Variant::Int8(value)
    }
}

impl From<i16> for Variant {
    fn from(value: i16) -> Self {
        Variant::Int16(value)
    }
}

impl From<i32> for Variant {
    fn from(value: i32) -> Self {
        Variant::Int32(value)
    }
}

impl From<i64> for Variant {
    fn from(value: i64) -> Self {
        Variant::Int64(value)
    }
}

impl From<u8> for Variant {
    fn from(value: u8) -> Self {
        Variant::UInt8(value)
    }
}

impl From<u16> for Variant {
    fn from(value: u16) -> Self {
        Variant::UInt16(value)
    }
}

impl From<u32> for Variant {
    fn from(value: u32) -> Self {
        Variant::UInt32(value)
    }
}

impl From<u64> for Variant {
    fn from(value: u64) -> Self {
        Variant::UInt64(value)
    }
}

impl From<f32> for Variant {
    fn from(value: f32) -> Self {
        Variant::Float32(value)
    }
}

impl From<f64> for Variant {
    fn from(value: f64) -> Self {
        Variant::Float64(value)
    }
}

impl From<String> for Variant {
    fn from(value: String) -> Self {
        Variant::String(value)
    }
}

impl<'a> From<&'a str> for Variant {
    fn from(value: &'a str) -> Self {
        Variant::String(value.to_string())
    }
}

impl From<ByteBuffer> for Variant {
    fn from(value: ByteBuffer) -> Self {
        Variant::OctetString(value)
    }
}

impl From<Vec<Variant>> for Variant {
    fn from(value: Vec<Variant>) -> Self {
        Variant::Array(VariantCollection::from(value))
    }
}

/// Convenience wrapper around a vector of `Variant` values.
#[derive(Clone, Debug, Default, PartialEq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
pub struct VariantCollection {
    items: Vec<Variant>,
}

impl VariantCollection {
    /// Creates an empty collection.
    pub fn new() -> Self {
        Self { items: Vec::new() }
    }

    /// Returns the number of elements contained in the collection.
    pub fn len(&self) -> usize {
        self.items.len()
    }

    /// Returns `true` when the collection does not contain any values.
    pub fn is_empty(&self) -> bool {
        self.items.is_empty()
    }

    /// Appends a new element to the collection.
    pub fn push(&mut self, value: Variant) {
        self.items.push(value);
    }

    /// Extends the collection with the elements produced by the iterator.
    pub fn extend<I>(&mut self, iter: I)
    where
        I: IntoIterator<Item = Variant>,
    {
        self.items.extend(iter);
    }

    /// Clears the collection leaving the allocated capacity untouched.
    pub fn clear(&mut self) {
        self.items.clear();
    }
}

impl From<Vec<Variant>> for VariantCollection {
    fn from(items: Vec<Variant>) -> Self {
        Self { items }
    }
}

impl Deref for VariantCollection {
    type Target = [Variant];

    fn deref(&self) -> &Self::Target {
        &self.items
    }
}

impl DerefMut for VariantCollection {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.items
    }
}

impl IntoIterator for VariantCollection {
    type Item = Variant;
    type IntoIter = std::vec::IntoIter<Variant>;

    fn into_iter(self) -> Self::IntoIter {
        self.items.into_iter()
    }
}

impl<'a> IntoIterator for &'a VariantCollection {
    type Item = &'a Variant;
    type IntoIter = core::slice::Iter<'a, Variant>;

    fn into_iter(self) -> Self::IntoIter {
        self.items.iter()
    }
}

impl<'a> IntoIterator for &'a mut VariantCollection {
    type Item = &'a mut Variant;
    type IntoIter = core::slice::IterMut<'a, Variant>;

    fn into_iter(self) -> Self::IntoIter {
        self.items.iter_mut()
    }
}

impl fmt::Display for Variant {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Variant::Null => write!(f, "null"),
            Variant::Boolean(v) => write!(f, "{}", v),
            Variant::Int8(v) => write!(f, "{}", v),
            Variant::Int16(v) => write!(f, "{}", v),
            Variant::Int32(v) => write!(f, "{}", v),
            Variant::Int64(v) => write!(f, "{}", v),
            Variant::UInt8(v) => write!(f, "{}", v),
            Variant::UInt16(v) => write!(f, "{}", v),
            Variant::UInt32(v) => write!(f, "{}", v),
            Variant::UInt64(v) => write!(f, "{}", v),
            Variant::Float32(v) => write!(f, "{}", v),
            Variant::Float64(v) => write!(f, "{}", v),
            Variant::Enum(v) => write!(f, "enum({})", v),
            Variant::OctetString(_) => write!(f, "octet-string(...)"),
            Variant::BitString(_) => write!(f, "bit-string(...)"),
            Variant::String(v) | Variant::Utf8String(v) => write!(f, "\"{}\"", v),
            Variant::CompactArray(_) => write!(f, "compact-array(...)"),
            Variant::DateTime(_) => write!(f, "datetime(...)"),
            Variant::Date(_) => write!(f, "date(...)"),
            Variant::Time(_) => write!(f, "time(...)"),
            Variant::Array(values) => write!(
                f,
                "[{}]",
                values
                    .iter()
                    .map(|v| v.to_string())
                    .collect::<Vec<_>>()
                    .join(", ")
            ),
            Variant::Structure(values) => write!(
                f,
                "{{{}}}",
                values
                    .iter()
                    .map(|v| v.to_string())
                    .collect::<Vec<_>>()
                    .join(", ")
            ),
        }
    }
}
