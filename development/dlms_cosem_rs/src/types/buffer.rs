use bytes::{BufMut, Bytes, BytesMut};

use crate::error::{Error, Result};

#[cfg(feature = "serde")]
use serde::{Deserialize, Deserializer, Serialize, Serializer};
#[cfg(feature = "serde")]
use serde_bytes::{ByteBuf, Bytes as SerdeBytes};

/// Growable binary buffer mirroring the behaviour of the C `gxByteBuffer` helper.
#[derive(Clone, Debug, Default, PartialEq, Eq)]
pub struct ByteBuffer {
    inner: BytesMut,
}

impl ByteBuffer {
    /// Creates an empty buffer.
    pub fn new() -> Self {
        Self::default()
    }

    /// Creates an empty buffer with pre-allocated capacity.
    pub fn with_capacity(capacity: usize) -> Self {
        Self {
            inner: BytesMut::with_capacity(capacity),
        }
    }

    /// Creates a buffer from raw bytes without additional allocations.
    pub fn from_bytes(bytes: Bytes) -> Self {
        Self {
            inner: BytesMut::from(bytes.as_ref()),
        }
    }

    /// Returns the number of bytes currently stored in the buffer.
    pub fn len(&self) -> usize {
        self.inner.len()
    }

    /// Returns `true` when the buffer is empty.
    pub fn is_empty(&self) -> bool {
        self.inner.is_empty()
    }

    /// Clears the content of the buffer without modifying the allocated capacity.
    pub fn clear(&mut self) {
        self.inner.clear();
    }

    /// Reserves additional capacity ensuring that at least `additional` bytes can be appended
    /// without a reallocation.
    pub fn reserve(&mut self, additional: usize) {
        self.inner.reserve(additional);
    }

    /// Appends a single byte to the end of the buffer.
    pub fn push(&mut self, value: u8) {
        self.inner.put_u8(value);
    }

    /// Appends a 16-bit unsigned integer in big endian order.
    pub fn push_u16(&mut self, value: u16) {
        self.inner.put_u16(value);
    }

    /// Appends a 32-bit unsigned integer in big endian order.
    pub fn push_u32(&mut self, value: u32) {
        self.inner.put_u32(value);
    }

    /// Appends a 64-bit unsigned integer in big endian order.
    pub fn push_u64(&mut self, value: u64) {
        self.inner.put_u64(value);
    }

    /// Extends the buffer from the provided slice.
    pub fn extend_from_slice(&mut self, data: &[u8]) {
        self.inner.extend_from_slice(data);
    }

    /// Writes a byte at the given index if it is within bounds.
    pub fn set(&mut self, index: usize, value: u8) -> Result<()> {
        if index < self.inner.len() {
            self.inner[index] = value;
            Ok(())
        } else {
            Err(Error::BufferTooSmall {
                requested: index + 1,
                available: self.inner.len(),
            })
        }
    }

    /// Provides an immutable view of the underlying bytes.
    pub fn as_slice(&self) -> &[u8] {
        &self.inner
    }

    /// Provides a mutable slice of the buffer.
    pub fn as_mut_slice(&mut self) -> &mut [u8] {
        &mut self.inner
    }

    /// Consumes the buffer and returns the underlying bytes in a `Bytes` instance.
    pub fn into_bytes(self) -> Bytes {
        self.inner.freeze()
    }

    /// Consumes the buffer and returns the owned `Vec<u8>`.
    pub fn into_vec(self) -> Vec<u8> {
        self.into_bytes().to_vec()
    }

    /// Returns a cloned `Vec<u8>` representation of the buffer.
    pub fn to_vec(&self) -> Vec<u8> {
        self.inner.to_vec()
    }
}

impl From<Vec<u8>> for ByteBuffer {
    fn from(value: Vec<u8>) -> Self {
        Self {
            inner: BytesMut::from(&value[..]),
        }
    }
}

impl From<&[u8]> for ByteBuffer {
    fn from(value: &[u8]) -> Self {
        Self {
            inner: BytesMut::from(value),
        }
    }
}

impl From<ByteBuffer> for Vec<u8> {
    fn from(value: ByteBuffer) -> Self {
        value.into_vec()
    }
}

impl AsRef<[u8]> for ByteBuffer {
    fn as_ref(&self) -> &[u8] {
        self.as_slice()
    }
}

impl AsMut<[u8]> for ByteBuffer {
    fn as_mut(&mut self) -> &mut [u8] {
        self.as_mut_slice()
    }
}

#[cfg(feature = "serde")]
impl Serialize for ByteBuffer {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        SerdeBytes::new(self.as_slice()).serialize(serializer)
    }
}

#[cfg(feature = "serde")]
impl<'de> Deserialize<'de> for ByteBuffer {
    fn deserialize<D>(deserializer: D) -> Result<Self, D::Error>
    where
        D: Deserializer<'de>,
    {
        let bytes = ByteBuf::deserialize(deserializer)?;
        Ok(ByteBuffer::from(bytes.into_vec()))
    }
}
