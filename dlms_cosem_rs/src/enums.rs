// Auto-generated from enums.h

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Priority {
    Normal = 0,
    High = 1,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ServiceClass {
    Unconfirmed = 0,
    Confirmed = 1,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum InterfaceType {
    Hdlc = 0,
    Wrapper = 1,
    Pdu,
    WirelessMbus,
    HdlcWithModeE,
    Plc,
    PlcHdlc,
    Lpwan,
    WiSun,
    PlcPrime,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Authentication {
    None = 0,
    Low = 1,
    High = 2,
    HighMd5 = 3,
    HighSha1 = 4,
    HighGmac = 5,
    HighSha256 = 6,
    HighEcdsa = 7,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ConnectionState {
    None = 0,
    Hdlc = 1,
    Dlms = 2,
    Iec = 4,
}

// TODO: This is a large enum. I will add more variants as needed.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Command {
    None = 0,
    InitiateRequest = 0x1,
    InitiateResponse = 0x8,
    ReadRequest = 0x5,
    ReadResponse = 0xC,
    WriteRequest = 0x6,
    WriteResponse = 0xD,
    GetRequest = 0xC0,
    GetResponse = 0xC4,
    SetRequest = 0xC1,
    SetResponse = 0xC5,
    MethodRequest = 0xC3,
    MethodResponse = 0xC7,
    DisconnectMode = 0x1f,
    Rejected = 0x97,
    Snrm = 0x93,
    Ua = 0x73,
    Aarq = 0x60,
    Aare = 0x61,
    Disc = 0x53,
    ReleaseRequest = 0x62,
    ReleaseResponse = 0x63,
    ConfirmedServiceError = 0x0E,
    ExceptionResponse = 0xD8,
    GeneralBlockTransfer = 0xE0,
    AccessRequest = 0xD9,
    AccessResponse = 0xDA,
    DataNotification = 0x0F,
    EventNotification = 0xC2,
    InformationReport = 0x18,
}

bitflags::bitflags! {
    #[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
    pub struct Conformance: u32 {
        const RESERVED_ZERO = 0x1;
        const GENERAL_PROTECTION = 0x2;
        const GENERAL_BLOCK_TRANSFER = 0x4;
        const READ = 0x8;
        const WRITE = 0x10;
        const UN_CONFIRMED_WRITE = 0x20;
        const DELTA_VALUE_ENCODING = 0x40;
        const RESERVED_SEVEN = 0x80;
        const ATTRIBUTE_0_SUPPORTED_WITH_SET = 0x100;
        const PRIORITY_MGMT_SUPPORTED = 0x200;
        const ATTRIBUTE_0_SUPPORTED_WITH_GET = 0x400;
        const BLOCK_TRANSFER_WITH_GET_OR_READ = 0x800;
        const BLOCK_TRANSFER_WITH_SET_OR_WRITE = 0x1000;
        const BLOCK_TRANSFER_WITH_ACTION = 0x2000;
        const MULTIPLE_REFERENCES = 0x4000;
        const INFORMATION_REPORT = 0x8000;
        const DATA_NOTIFICATION = 0x10000;
        const ACCESS = 0x20000;
        const PARAMETERIZED_ACCESS = 0x40000;
        const GET = 0x80000;
        const SET = 0x100000;
        const SELECTIVE_ACCESS = 0x200000;
        const EVENT_NOTIFICATION = 0x400000;
        const ACTION = 0x800000;
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(u8)]
pub enum DataType {
    None = 0,
    Array = 1,
    Structure = 2,
    Boolean = 3,
    BitString = 4,
    Int32 = 5,
    UInt32 = 6,
    OctetString = 9,
    String = 10,
    StringUtf8 = 12,
    BinaryCodedDesimal = 13,
    Int8 = 15,
    Int16 = 16,
    UInt8 = 17,
    UInt16 = 18,
    CompactArray = 19,
    Int64 = 20,
    UInt64 = 21,
    Enum = 22,
    Float32 = 23,
    Float64 = 24,
    Datetime = 25,
    Date = 26,
    Time = 27,
    DeltaInt8 = 28,
    DeltaInt16 = 29,
    DeltaInt32 = 30,
    DeltaUInt8 = 31,
    DeltaUInt16 = 32,
    DeltaUInt32 = 33,
    ByRef = 0x80,
}

impl TryFrom<u8> for DataType {
    type Error = ();

    fn try_from(value: u8) -> Result<Self, Self::Error> {
        match value {
            0 => Ok(DataType::None),
            1 => Ok(DataType::Array),
            2 => Ok(DataType::Structure),
            3 => Ok(DataType::Boolean),
            4 => Ok(DataType::BitString),
            5 => Ok(DataType::Int32),
            6 => Ok(DataType::UInt32),
            9 => Ok(DataType::OctetString),
            10 => Ok(DataType::String),
            12 => Ok(DataType::StringUtf8),
            13 => Ok(DataType::BinaryCodedDesimal),
            15 => Ok(DataType::Int8),
            16 => Ok(DataType::Int16),
            17 => Ok(DataType::UInt8),
            18 => Ok(DataType::UInt16),
            19 => Ok(DataType::CompactArray),
            20 => Ok(DataType::Int64),
            21 => Ok(DataType::UInt64),
            22 => Ok(DataType::Enum),
            23 => Ok(DataType::Float32),
            24 => Ok(DataType::Float64),
            25 => Ok(DataType::Datetime),
            26 => Ok(DataType::Date),
            27 => Ok(DataType::Time),
            28 => Ok(DataType::DeltaInt8),
            29 => Ok(DataType::DeltaInt16),
            30 => Ok(DataType::DeltaInt32),
            31 => Ok(DataType::DeltaUInt8),
            32 => Ok(DataType::DeltaUInt16),
            33 => Ok(DataType::DeltaUInt32),
            0x80 => Ok(DataType::ByRef),
            _ => Err(()),
        }
    }
}
