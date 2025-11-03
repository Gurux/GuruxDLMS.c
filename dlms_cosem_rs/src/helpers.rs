use crate::byte_buffer::ByteBuffer;

pub fn get_object_count_size_in_bytes(count: u32) -> u8 {
    if count < 0x80 {
        1
    } else if count < 0x100 {
        2
    } else if count < 0x10000 {
        3
    } else {
        5
    }
}

pub fn get_object_count(buff: &mut ByteBuffer) -> Result<u32, crate::byte_buffer::Error> {
    let ch = buff.get_u8()?;
    if ch > 0x80 {
        match ch {
            0x81 => Ok(buff.get_u8()? as u32),
            0x82 => Ok(buff.get_u16()? as u32),
            0x83 => {
                let mut bytes = [0; 4];
                let slice = buff.get_bytes(3)?;
                bytes[1..].copy_from_slice(&slice);
                Ok(u32::from_be_bytes(bytes))
            }
            0x84 => Ok(buff.get_u32()?),
            _ => Err(crate::byte_buffer::Error::UnexpectedEof),
        }
    } else {
        Ok(ch as u32)
    }
}

pub fn set_object_count(count: u32, buff: &mut ByteBuffer) -> Result<(), crate::byte_buffer::Error> {
    if count < 0x80 {
        buff.put_u8(count as u8)?;
    } else if count < 0x100 {
        buff.put_u8(0x81)?;
        buff.put_u8(count as u8)?;
    } else if count < 0x10000 {
        buff.put_u8(0x82)?;
        buff.put_u16(count as u16)?;
    } else {
        buff.put_u8(0x84)?;
        buff.put_u32(count)?;
    }
    Ok(())
}
