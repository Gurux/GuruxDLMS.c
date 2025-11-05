use crate::enums::{Authentication, Conformance, InterfaceType, DataType};
use crate::byte_buffer::{ByteBuffer, Error};
use crate::variant::Variant;
use alloc::vec::Vec;
use core::convert::TryFrom;

#[derive(Debug, Clone)]
pub struct DlmsSettings {
    pub use_logical_name_referencing: bool,
    pub interface_type: InterfaceType,
    pub client_address: u16,
    pub server_address: u32,
    pub authentication: Authentication,
    pub password: Option<Vec<u8>>,
    pub kek: Option<Vec<u8>>,
    pub dlms_version: u8,
    pub max_pdu_size: u16,
    pub max_server_pdu_size: u16,
    pub proposed_conformance: Conformance,
    pub negotiated_conformance: Conformance,
    pub quality_of_service: u8,
}

impl DlmsSettings {
    pub fn new() -> Self {
        Self {
            use_logical_name_referencing: true,
            interface_type: InterfaceType::Hdlc,
            client_address: 0,
            server_address: 0,
            authentication: Authentication::None,
            password: None,
            kek: None,
            dlms_version: 6,
            max_pdu_size: 1024,
            max_server_pdu_size: 1024,
            proposed_conformance: Conformance::all(),
            negotiated_conformance: Conformance::empty(),
            quality_of_service: 0,
        }
    }

    pub fn use_logical_name_referencing(mut self, value: bool) -> Self {
        self.use_logical_name_referencing = value;
        self
    }

    pub fn interface_type(mut self, value: InterfaceType) -> Self {
        self.interface_type = value;
        self
    }

    pub fn client_address(mut self, value: u16) -> Self {
        self.client_address = value;
        self
    }

    pub fn server_address(mut self, value: u32) -> Self {
        self.server_address = value;
        self
    }

    pub fn authentication(mut self, value: Authentication) -> Self {
        self.authentication = value;
        self
    }

    pub fn password(mut self, value: Vec<u8>) -> Self {
        self.password = Some(value);
        self
    }

    pub fn kek(mut self, value: Vec<u8>) -> Self {
        self.kek = Some(value);
        self
    }

    pub fn dlms_version(mut self, value: u8) -> Self {
        self.dlms_version = value;
        self
    }

    pub fn max_pdu_size(mut self, value: u16) -> Self {
        self.max_pdu_size = value;
        self
    }

    pub fn max_server_pdu_size(mut self, value: u16) -> Self {
        self.max_server_pdu_size = value;
        self
    }

    pub fn proposed_conformance(mut self, value: Conformance) -> Self {
        self.proposed_conformance = value;
        self
    }

    pub fn negotiated_conformance(mut self, value: Conformance) -> Self {
        self.negotiated_conformance = value;
        self
    }

    pub fn quality_of_service(mut self, value: u8) -> Self {
        self.quality_of_service = value;
        self
    }
}

#[derive(Debug, Clone, Copy)]
pub struct DataInfo {
    pub complete: bool,
    pub data_type: Option<DataType>,
    pub count: u32,
    pub index: u32,
}

impl DataInfo {
    pub fn new() -> Self {
        Self {
            complete: false,
            data_type: None,
            count: 0,
            index: 0,
        }
    }
}

fn get_uint32(buff: &mut ByteBuffer, info: &mut DataInfo) -> Result<u32, Error> {
    if buff.remaining() < 4 {
        info.complete = false;
        return Err(Error::UnexpectedEof);
    }
    buff.get_u32()
}

fn get_int32(buff: &mut ByteBuffer, info: &mut DataInfo) -> Result<i32, Error> {
    if buff.remaining() < 4 {
        info.complete = false;
        return Err(Error::UnexpectedEof);
    }
    Ok(i32::from_be_bytes(buff.get_bytes(4)?.try_into().unwrap()))
}

fn get_bool(buff: &mut ByteBuffer, info: &mut DataInfo) -> Result<bool, Error> {
    if buff.remaining() < 1 {
        info.complete = false;
        return Err(Error::UnexpectedEof);
    }
    Ok(buff.get_u8()? != 0)
}

fn get_octet_string(buff: &mut ByteBuffer, info: &mut DataInfo) -> Result<ByteBuffer, Error> {
    let len = buff.get_u8()? as usize;
    if buff.remaining() < len {
        info.complete = false;
        return Err(Error::UnexpectedEof);
    }
    let data = buff.get_bytes(len)?;
    Ok(ByteBuffer::from_vec(data))
}

fn get_data(
    buff: &mut ByteBuffer,
    info: &mut DataInfo,
) -> Result<Variant, Error> {
    if buff.position() == buff.len() {
        info.complete = false;
        return Err(Error::UnexpectedEof);
    }
    info.complete = true;
    let data_type = if let Some(dt) = info.data_type {
        dt
    } else {
        let dt = buff.get_u8()?;
        DataType::try_from(dt).map_err(|_| Error::InvalidData)?
    };
    info.data_type = Some(data_type);

    match data_type {
        DataType::Boolean => Ok(Variant::Boolean(get_bool(buff, info)?)),
        DataType::Int32 => Ok(Variant::Int32(get_int32(buff, info)?)),
        DataType::OctetString => Ok(Variant::OctetString(get_octet_string(buff, info)?)),
        _ => Err(Error::InvalidData),
    }
}
