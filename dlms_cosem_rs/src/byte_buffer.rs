use alloc::vec::Vec;
use core::fmt;

#[derive(Debug)]
pub enum Error {
    Io(#[cfg(feature = "std")] std::io::Error),
    UnexpectedEof,
    WriteZero,
}

#[cfg(feature = "std")]
impl From<std::io::Error> for Error {
    fn from(err: std::io::Error) -> Error {
        Error::Io(err)
    }
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            #[cfg(feature = "std")]
            Error::Io(err) => write!(f, "I/O error: {}", err),
            Error::UnexpectedEof => write!(f, "Unexpected end of file"),
            Error::WriteZero => write!(f, "Write zero"),
        }
    }
}

pub struct ByteBuffer {
    inner: Vec<u8>,
    pos: usize,
}

impl ByteBuffer {
    pub fn new() -> Self {
        Self {
            inner: Vec::new(),
            pos: 0,
        }
    }

    pub fn with_capacity(capacity: usize) -> Self {
        Self {
            inner: Vec::with_capacity(capacity),
            pos: 0,
        }
    }

    pub fn from_vec(vec: Vec<u8>) -> Self {
        Self {
            inner: vec,
            pos: 0,
        }
    }

    pub fn len(&self) -> usize {
        self.inner.len()
    }

    pub fn capacity(&self) -> usize {
        self.inner.capacity()
    }

    pub fn position(&self) -> usize {
        self.pos
    }

    pub fn set_position(&mut self, pos: usize) {
        self.pos = pos;
    }

    pub fn remaining(&self) -> usize {
        self.len() - self.pos
    }

    pub fn clear(&mut self) {
        self.inner.clear();
        self.pos = 0;
    }

    pub fn is_empty(&self) -> bool {
        self.inner.is_empty()
    }

    pub fn get_u8(&mut self) -> Result<u8, Error> {
        if self.remaining() < 1 {
            return Err(Error::UnexpectedEof);
        }
        let val = self.inner[self.pos];
        self.pos += 1;
        Ok(val)
    }

    pub fn get_u16(&mut self) -> Result<u16, Error> {
        if self.remaining() < 2 {
            return Err(Error::UnexpectedEof);
        }
        let mut buf = [0; 2];
        buf.copy_from_slice(&self.inner[self.pos..self.pos + 2]);
        self.pos += 2;
        Ok(u16::from_be_bytes(buf))
    }

    pub fn get_u32(&mut self) -> Result<u32, Error> {
        if self.remaining() < 4 {
            return Err(Error::UnexpectedEof);
        }
        let mut buf = [0; 4];
        buf.copy_from_slice(&self.inner[self.pos..self.pos + 4]);
        self.pos += 4;
        Ok(u32::from_be_bytes(buf))
    }

    pub fn get_u64(&mut self) -> Result<u64, Error> {
        if self.remaining() < 8 {
            return Err(Error::UnexpectedEof);
        }
        let mut buf = [0; 8];
        buf.copy_from_slice(&self.inner[self.pos..self.pos + 8]);
        self.pos += 8;
        Ok(u64::from_be_bytes(buf))
    }

    pub fn get_bytes(&mut self, len: usize) -> Result<Vec<u8>, Error> {
        if self.remaining() < len {
            return Err(Error::UnexpectedEof);
        }
        let mut buf = Vec::with_capacity(len);
        buf.extend_from_slice(&self.inner[self.pos..self.pos + len]);
        self.pos += len;
        Ok(buf)
    }

    pub fn put_u8(&mut self, value: u8) -> Result<(), Error> {
        self.inner.push(value);
        Ok(())
    }

    pub fn put_u16(&mut self, value: u16) -> Result<(), Error> {
        self.inner.extend_from_slice(&value.to_be_bytes());
        Ok(())
    }

    pub fn put_u32(&mut self, value: u32) -> Result<(), Error> {
        self.inner.extend_from_slice(&value.to_be_bytes());
        Ok(())
    }

    pub fn put_u64(&mut self, value: u64) -> Result<(), Error> {
        self.inner.extend_from_slice(&value.to_be_bytes());
        Ok(())
    }

    pub fn put_bytes(&mut self, value: &[u8]) -> Result<(), Error> {
        self.inner.extend_from_slice(value);
        Ok(())
    }
}
