//! Serialization utilities that mirror the legacy `dlms_setData`, `dlms_getData`
//! and `gxserializer` helpers.

use crate::error::{Error, Result};
use crate::types::buffer::ByteBuffer;
use crate::types::enums::{Command, DataType};
use crate::types::value::{DlmsDateTime, Variant, VariantCollection};

/// Encodes a DLMS object count using the BER style length rules.
pub fn write_object_count(buffer: &mut ByteBuffer, count: usize) -> Result<()> {
    if count < 0x80 {
        buffer.push(count as u8);
    } else if count <= u8::MAX as usize {
        buffer.push(0x81);
        buffer.push(count as u8);
    } else if count <= u16::MAX as usize {
        buffer.push(0x82);
        buffer.push_u16(count as u16);
    } else {
        buffer.push(0x84);
        buffer.push_u32(count as u32);
    }
    Ok(())
}

/// Decodes a DLMS object count returning the value and number of bytes consumed.
pub fn read_object_count(data: &[u8]) -> Result<(usize, usize)> {
    if data.is_empty() {
        return Err(Error::UnexpectedEof);
    }
    match data[0] {
        0x81 => {
            if data.len() < 2 {
                return Err(Error::UnexpectedEof);
            }
            Ok((data[1] as usize, 2))
        }
        0x82 => {
            if data.len() < 3 {
                return Err(Error::UnexpectedEof);
            }
            Ok((u16::from_be_bytes([data[1], data[2]]) as usize, 3))
        }
        0x84 => {
            if data.len() < 5 {
                return Err(Error::UnexpectedEof);
            }
            Ok((
                u32::from_be_bytes([data[1], data[2], data[3], data[4]]) as usize,
                5,
            ))
        }
        value => Ok((value as usize, 1)),
    }
}

/// Serializes a `Variant` into the provided buffer.
pub fn serialize_variant(buffer: &mut ByteBuffer, value: &Variant) -> Result<()> {
    match value {
        Variant::Null => buffer.push(DataType::None as u8),
        Variant::Boolean(v) => {
            buffer.push(DataType::Boolean as u8);
            buffer.push(if *v { 1 } else { 0 });
        }
        Variant::Int8(v) => {
            buffer.push(DataType::Int8 as u8);
            buffer.push(*v as u8);
        }
        Variant::Int16(v) => {
            buffer.push(DataType::Int16 as u8);
            buffer.push_u16(*v as u16);
        }
        Variant::Int32(v) => {
            buffer.push(DataType::Int32 as u8);
            buffer.push_u32(*v as u32);
        }
        Variant::Int64(v) => {
            buffer.push(DataType::Int64 as u8);
            buffer.push_u64(*v as u64);
        }
        Variant::UInt8(v) => {
            buffer.push(DataType::Uint8 as u8);
            buffer.push(*v);
        }
        Variant::UInt16(v) => {
            buffer.push(DataType::Uint16 as u8);
            buffer.push_u16(*v);
        }
        Variant::UInt32(v) => {
            buffer.push(DataType::Uint32 as u8);
            buffer.push_u32(*v);
        }
        Variant::UInt64(v) => {
            buffer.push(DataType::Uint64 as u8);
            buffer.push_u64(*v);
        }
        Variant::Enum(v) => {
            buffer.push(DataType::Enum as u8);
            buffer.push(*v);
        }
        Variant::Float32(v) => {
            buffer.push(DataType::Float32 as u8);
            buffer.extend_from_slice(&v.to_be_bytes());
        }
        Variant::Float64(v) => {
            buffer.push(DataType::Float64 as u8);
            buffer.extend_from_slice(&v.to_be_bytes());
        }
        Variant::String(v) => {
            buffer.push(DataType::String as u8);
            write_object_count(buffer, v.len())?;
            buffer.extend_from_slice(v.as_bytes());
        }
        Variant::Utf8String(v) => {
            buffer.push(DataType::StringUtf8 as u8);
            write_object_count(buffer, v.len())?;
            buffer.extend_from_slice(v.as_bytes());
        }
        Variant::OctetString(bytes) => {
            buffer.push(DataType::OctetString as u8);
            write_object_count(buffer, bytes.len())?;
            buffer.extend_from_slice(bytes.as_slice());
        }
        Variant::BitString(bytes) => {
            buffer.push(DataType::BitString as u8);
            write_object_count(buffer, bytes.len() * 8)?;
            buffer.extend_from_slice(bytes.as_slice());
        }
        Variant::CompactArray(bytes) => {
            buffer.push(DataType::CompactArray as u8);
            write_object_count(buffer, bytes.len())?;
            buffer.extend_from_slice(bytes.as_slice());
        }
        Variant::DateTime(dt) => serialize_datetime(buffer, DataType::DateTime, dt)?,
        Variant::Date(dt) => serialize_datetime(buffer, DataType::Date, dt)?,
        Variant::Time(dt) => serialize_datetime(buffer, DataType::Time, dt)?,
        Variant::Array(items) => serialize_collection(buffer, DataType::Array, items)?,
        Variant::Structure(items) => serialize_collection(buffer, DataType::Structure, items)?,
    }
    Ok(())
}

fn serialize_collection(
    buffer: &mut ByteBuffer,
    ty: DataType,
    collection: &VariantCollection,
) -> Result<()> {
    buffer.push(ty as u8);
    write_object_count(buffer, collection.len())?;
    for item in collection.iter() {
        serialize_variant(buffer, item)?;
    }
    Ok(())
}

fn serialize_datetime(buffer: &mut ByteBuffer, ty: DataType, dt: &DlmsDateTime) -> Result<()> {
    buffer.push(ty as u8);
    let bytes = dt.as_bytes();
    write_object_count(buffer, bytes.len())?;
    buffer.extend_from_slice(bytes);
    Ok(())
}

/// Deserializes a `Variant` from a slice returning the parsed value and the
/// number of bytes consumed.
pub fn deserialize_variant(data: &[u8]) -> Result<(Variant, usize)> {
    if data.is_empty() {
        return Err(Error::UnexpectedEof);
    }
    let tag = DataType::try_from(data[0]).map_err(|_| Error::UnknownDataType(data[0]))?;
    let mut offset = 1;
    let variant = match tag {
        DataType::None => Variant::Null,
        DataType::Boolean => {
            if data.len() < offset + 1 {
                return Err(Error::UnexpectedEof);
            }
            let value = data[offset] != 0;
            offset += 1;
            Variant::Boolean(value)
        }
        DataType::Int8 => {
            if data.len() < offset + 1 {
                return Err(Error::UnexpectedEof);
            }
            let value = data[offset] as i8;
            offset += 1;
            Variant::Int8(value)
        }
        DataType::Int16 => {
            if data.len() < offset + 2 {
                return Err(Error::UnexpectedEof);
            }
            let value = i16::from_be_bytes([data[offset], data[offset + 1]]);
            offset += 2;
            Variant::Int16(value)
        }
        DataType::Int32 => {
            if data.len() < offset + 4 {
                return Err(Error::UnexpectedEof);
            }
            let value = i32::from_be_bytes([
                data[offset],
                data[offset + 1],
                data[offset + 2],
                data[offset + 3],
            ]);
            offset += 4;
            Variant::Int32(value)
        }
        DataType::Int64 => {
            if data.len() < offset + 8 {
                return Err(Error::UnexpectedEof);
            }
            let value = i64::from_be_bytes([
                data[offset],
                data[offset + 1],
                data[offset + 2],
                data[offset + 3],
                data[offset + 4],
                data[offset + 5],
                data[offset + 6],
                data[offset + 7],
            ]);
            offset += 8;
            Variant::Int64(value)
        }
        DataType::Uint8 => {
            if data.len() < offset + 1 {
                return Err(Error::UnexpectedEof);
            }
            let value = data[offset];
            offset += 1;
            Variant::UInt8(value)
        }
        DataType::Uint16 => {
            if data.len() < offset + 2 {
                return Err(Error::UnexpectedEof);
            }
            let value = u16::from_be_bytes([data[offset], data[offset + 1]]);
            offset += 2;
            Variant::UInt16(value)
        }
        DataType::Uint32 => {
            if data.len() < offset + 4 {
                return Err(Error::UnexpectedEof);
            }
            let value = u32::from_be_bytes([
                data[offset],
                data[offset + 1],
                data[offset + 2],
                data[offset + 3],
            ]);
            offset += 4;
            Variant::UInt32(value)
        }
        DataType::Uint64 => {
            if data.len() < offset + 8 {
                return Err(Error::UnexpectedEof);
            }
            let value = u64::from_be_bytes([
                data[offset],
                data[offset + 1],
                data[offset + 2],
                data[offset + 3],
                data[offset + 4],
                data[offset + 5],
                data[offset + 6],
                data[offset + 7],
            ]);
            offset += 8;
            Variant::UInt64(value)
        }
        DataType::Enum => {
            if data.len() < offset + 1 {
                return Err(Error::UnexpectedEof);
            }
            let value = data[offset];
            offset += 1;
            Variant::Enum(value)
        }
        DataType::Float32 => {
            if data.len() < offset + 4 {
                return Err(Error::UnexpectedEof);
            }
            let value = f32::from_be_bytes([
                data[offset],
                data[offset + 1],
                data[offset + 2],
                data[offset + 3],
            ]);
            offset += 4;
            Variant::Float32(value)
        }
        DataType::Float64 => {
            if data.len() < offset + 8 {
                return Err(Error::UnexpectedEof);
            }
            let value = f64::from_be_bytes([
                data[offset],
                data[offset + 1],
                data[offset + 2],
                data[offset + 3],
                data[offset + 4],
                data[offset + 5],
                data[offset + 6],
                data[offset + 7],
            ]);
            offset += 8;
            Variant::Float64(value)
        }
        DataType::String
        | DataType::StringUtf8
        | DataType::OctetString
        | DataType::BitString
        | DataType::CompactArray
        | DataType::Date
        | DataType::DateTime
        | DataType::Time => {
            let (count, used) = read_object_count(&data[offset..])?;
            offset += used;
            if data.len() < offset + count {
                return Err(Error::UnexpectedEof);
            }
            let bytes = &data[offset..offset + count];
            offset += count;
            match tag {
                DataType::String | DataType::StringUtf8 => {
                    let value = String::from_utf8(bytes.to_vec())
                        .map_err(|_| Error::InvalidFrame("invalid UTF-8"))?;
                    if tag == DataType::StringUtf8 {
                        Variant::Utf8String(value)
                    } else {
                        Variant::String(value)
                    }
                }
                DataType::OctetString => Variant::OctetString(ByteBuffer::from(bytes)),
                DataType::BitString => Variant::BitString(ByteBuffer::from(bytes)),
                DataType::CompactArray => Variant::CompactArray(ByteBuffer::from(bytes)),
                DataType::Date | DataType::DateTime | DataType::Time => {
                    let dt = DlmsDateTime::new(bytes.to_vec());
                    match tag {
                        DataType::Date => Variant::Date(dt),
                        DataType::Time => Variant::Time(dt),
                        _ => Variant::DateTime(dt),
                    }
                }
                _ => unreachable!(),
            }
        }
        DataType::Array | DataType::Structure => {
            let (count, used) = read_object_count(&data[offset..])?;
            offset += used;
            let mut items = VariantCollection::new();
            for _ in 0..count {
                let (item, consumed) = deserialize_variant(&data[offset..])?;
                offset += consumed;
                items.push(item);
            }
            if tag == DataType::Array {
                Variant::Array(items)
            } else {
                Variant::Structure(items)
            }
        }
        other => return Err(Error::UnknownDataType(other as u8)),
    };
    Ok((variant, offset))
}

/// Representation of a simple APDU combining a command and payload.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Apdu {
    /// DLMS command identifier.
    pub command: Command,
    /// Raw payload bytes.
    pub payload: Vec<u8>,
}

impl Apdu {
    /// Creates a new APDU instance.
    pub fn new(command: Command, payload: Vec<u8>) -> Self {
        Self { command, payload }
    }

    /// Serializes the APDU into a `ByteBuffer`.
    pub fn serialize(&self, buffer: &mut ByteBuffer) {
        buffer.push(self.command as u16 as u8);
        buffer.extend_from_slice(&self.payload);
    }

    /// Returns the serialized APDU as a `Vec<u8>`.
    pub fn to_bytes(&self) -> Vec<u8> {
        let mut buffer = ByteBuffer::with_capacity(1 + self.payload.len());
        self.serialize(&mut buffer);
        buffer.into_vec()
    }

    /// Parses an APDU from the provided slice.
    pub fn parse(data: &[u8]) -> Result<Self> {
        if data.is_empty() {
            return Err(Error::UnexpectedEof);
        }
        let command =
            Command::try_from(data[0] as u16).map_err(|_| Error::UnknownDataType(data[0]))?;
        Ok(Self {
            command,
            payload: data[1..].to_vec(),
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn variant_roundtrip() {
        let mut buffer = ByteBuffer::new();
        let value = Variant::UInt16(0x1234);
        serialize_variant(&mut buffer, &value).unwrap();
        assert_eq!(buffer.as_slice(), &[DataType::Uint16 as u8, 0x12, 0x34]);
        let (decoded, used) = deserialize_variant(buffer.as_slice()).unwrap();
        assert_eq!(used, buffer.len());
        assert_eq!(decoded, value);
    }

    #[test]
    fn string_serialization_matches_length_encoding() {
        let mut buffer = ByteBuffer::new();
        let value = Variant::String("DLMS".into());
        serialize_variant(&mut buffer, &value).unwrap();
        assert_eq!(buffer.as_slice()[0], DataType::String as u8);
        assert_eq!(buffer.as_slice()[1], 4);
        let (decoded, _) = deserialize_variant(buffer.as_slice()).unwrap();
        assert_eq!(decoded, value);
    }

    #[test]
    fn apdu_serialization_roundtrip() {
        let apdu = Apdu::new(Command::GetRequest, vec![0x01, 0x02, 0x03]);
        let bytes = apdu.to_bytes();
        let parsed = Apdu::parse(&bytes).unwrap();
        assert_eq!(parsed, apdu);
    }
}
