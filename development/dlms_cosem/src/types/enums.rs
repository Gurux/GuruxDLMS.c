use core::convert::TryFrom;
use core::fmt;
use core::str::FromStr;

use bitflags::bitflags;

#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};

/// Result of the association process as described in Green Book 8.1.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
#[repr(u8)]
pub enum AssociationResult {
    /// Association request was accepted.
    Accepted = 0,
    /// Association request was permanently rejected.
    PermanentRejected = 1,
    /// Association request was temporarily rejected.
    TransientRejected = 2,
}

impl TryFrom<u8> for AssociationResult {
    type Error = ();

    fn try_from(value: u8) -> Result<Self, Self::Error> {
        match value {
            0 => Ok(Self::Accepted),
            1 => Ok(Self::PermanentRejected),
            2 => Ok(Self::TransientRejected),
            _ => Err(()),
        }
    }
}

impl From<AssociationResult> for u8 {
    fn from(value: AssociationResult) -> Self {
        value as u8
    }
}

/// Diagnostics for failed application associations.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
#[repr(u8)]
pub enum SourceDiagnostic {
    /// Association succeeded.
    None = 0,
    /// No additional reason provided.
    NoReasonGiven = 1,
    /// Application context name not supported.
    ApplicationContextNameNotSupported = 2,
    /// Calling AP title not recognised.
    CallingApTitleNotRecognized = 3,
    /// Calling AP invocation identifier not recognised.
    CallingApInvocationIdNotRecognized = 4,
    /// Calling AE qualifier not recognised.
    CallingAeQualifierNotRecognized = 5,
    /// Calling AE invocation identifier not recognised.
    CallingAeInvocationIdNotRecognized = 6,
    /// Called AP title not recognised.
    CalledApTitleNotRecognized = 7,
    /// Called AP invocation identifier not recognised.
    CalledApInvocationIdNotRecognized = 8,
    /// Called AE qualifier not recognised.
    CalledAeQualifierNotRecognized = 9,
    /// Called AE invocation identifier not recognised.
    CalledAeInvocationIdNotRecognized = 10,
    /// Authentication mechanism name not recognised.
    AuthenticationMechanismNameNotRecognized = 11,
    /// Authentication mechanism name required.
    AuthenticationMechanismNameRequired = 12,
    /// Authentication failure.
    AuthenticationFailure = 13,
    /// Authentication required.
    AuthenticationRequired = 14,
}

impl TryFrom<u8> for SourceDiagnostic {
    type Error = ();

    fn try_from(value: u8) -> Result<Self, Self::Error> {
        Ok(match value {
            0 => Self::None,
            1 => Self::NoReasonGiven,
            2 => Self::ApplicationContextNameNotSupported,
            3 => Self::CallingApTitleNotRecognized,
            4 => Self::CallingApInvocationIdNotRecognized,
            5 => Self::CallingAeQualifierNotRecognized,
            6 => Self::CallingAeInvocationIdNotRecognized,
            7 => Self::CalledApTitleNotRecognized,
            8 => Self::CalledApInvocationIdNotRecognized,
            9 => Self::CalledAeQualifierNotRecognized,
            10 => Self::CalledAeInvocationIdNotRecognized,
            11 => Self::AuthenticationMechanismNameNotRecognized,
            12 => Self::AuthenticationMechanismNameRequired,
            13 => Self::AuthenticationFailure,
            14 => Self::AuthenticationRequired,
            _ => return Err(()),
        })
    }
}

impl From<SourceDiagnostic> for u8 {
    fn from(value: SourceDiagnostic) -> Self {
        value as u8
    }
}

/// Priority of DLMS/COSEM service invocations.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
#[repr(u8)]
pub enum Priority {
    /// Normal priority service.
    Normal = 0,
    /// High priority service.
    High = 1,
}

impl TryFrom<u8> for Priority {
    type Error = ();

    fn try_from(value: u8) -> Result<Self, Self::Error> {
        match value {
            0 => Ok(Self::Normal),
            1 => Ok(Self::High),
            _ => Err(()),
        }
    }
}

impl From<Priority> for u8 {
    fn from(value: Priority) -> Self {
        value as u8
    }
}

/// Requested service class for confirmed messaging.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
#[repr(u8)]
pub enum ServiceClass {
    /// Unconfirmed service class.
    Unconfirmed = 0,
    /// Confirmed service class.
    Confirmed = 1,
}

impl TryFrom<u8> for ServiceClass {
    type Error = ();

    fn try_from(value: u8) -> Result<Self, Self::Error> {
        match value {
            0 => Ok(Self::Unconfirmed),
            1 => Ok(Self::Confirmed),
            _ => Err(()),
        }
    }
}

impl From<ServiceClass> for u8 {
    fn from(value: ServiceClass) -> Self {
        value as u8
    }
}

/// Supported DLMS communication profiles.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
#[repr(u8)]
pub enum ServiceType {
    /// TCP/IP based transport.
    Tcp = 0,
    /// UDP/IP based transport.
    Udp = 1,
    /// FTP based transport.
    Ftp = 2,
    /// SMTP based messaging.
    Smtp = 3,
    /// SMS transport channel.
    Sms = 4,
    /// HDLC based transport.
    Hdlc = 5,
    /// Wired M-Bus transport.
    MBus = 6,
    /// ZigBee transport.
    ZigBee = 7,
    /// DLMS gateway service.
    DlmsGateway = 8,
    /// Reliable CoAP transport.
    ReliableCoap = 9,
    /// Unreliable CoAP transport.
    UnreliableCoap = 10,
}

impl TryFrom<u8> for ServiceType {
    type Error = ();

    fn try_from(value: u8) -> Result<Self, Self::Error> {
        Ok(match value {
            0 => Self::Tcp,
            1 => Self::Udp,
            2 => Self::Ftp,
            3 => Self::Smtp,
            4 => Self::Sms,
            5 => Self::Hdlc,
            6 => Self::MBus,
            7 => Self::ZigBee,
            8 => Self::DlmsGateway,
            9 => Self::ReliableCoap,
            10 => Self::UnreliableCoap,
            _ => return Err(()),
        })
    }
}

impl From<ServiceType> for u8 {
    fn from(value: ServiceType) -> Self {
        value as u8
    }
}

/// DLMS message payload format.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
#[repr(u8)]
pub enum MessageType {
    /// Raw COSEM APDU binary message.
    CosemApdu = 0,
    /// COSEM APDU represented in XML.
    CosemApduXml = 1,
    /// Manufacturer specific extension message.
    ManufacturerSpecific = 128,
}

impl TryFrom<u8> for MessageType {
    type Error = ();

    fn try_from(value: u8) -> Result<Self, Self::Error> {
        match value {
            0 => Ok(Self::CosemApdu),
            1 => Ok(Self::CosemApduXml),
            128 => Ok(Self::ManufacturerSpecific),
            _ => Err(()),
        }
    }
}

impl From<MessageType> for u8 {
    fn from(value: MessageType) -> Self {
        value as u8
    }
}

bitflags! {
    /// Security policy mask describing authentication/encryption requirements.
    #[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
    #[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
    pub struct SecurityPolicy: u8 {
        /// Security is not applied to any message.
        const NOTHING = 0x00;
        /// Every message is authenticated.
        const AUTHENTICATED = 0x01;
        /// Every message is encrypted.
        const ENCRYPTED = 0x02;
        /// Every message is authenticated and encrypted.
        const AUTHENTICATED_ENCRYPTED = 0x03;
        /// Requests must be authenticated.
        const AUTHENTICATED_REQUEST = 0x04;
        /// Requests must be encrypted.
        const ENCRYPTED_REQUEST = 0x08;
        /// Requests must be digitally signed.
        const DIGITALLY_SIGNED_REQUEST = 0x10;
        /// Responses must be authenticated.
        const AUTHENTICATED_RESPONSE = 0x20;
        /// Responses must be encrypted.
        const ENCRYPTED_RESPONSE = 0x40;
        /// Responses must be digitally signed.
        const DIGITALLY_SIGNED_RESPONSE = 0x80;
    }
}

bitflags! {
    /// Security level in use for a given message.
    #[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
    #[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
    pub struct Security: u8 {
        /// Security services are disabled.
        const NONE = 0x00;
        /// Only authentication is requested.
        const AUTHENTICATION = 0x10;
        /// Only encryption is requested.
        const ENCRYPTION = 0x20;
        /// Both authentication and encryption are requested.
        const AUTHENTICATION_ENCRYPTION = 0x30;
    }
}

/// Security suite describing algorithm selections.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
#[repr(u8)]
pub enum SecuritySuite {
    /// AES-GCM-128 authenticated encryption with AES-128 key wrapping.
    V0 = 0,
    /// AES-GCM-128 authenticated encryption with elliptic curve signatures and key agreement.
    V1 = 1,
    /// AES-GCM-256 authenticated encryption with P-384 elliptic curve primitives.
    V2 = 2,
}

impl TryFrom<u8> for SecuritySuite {
    type Error = ();

    fn try_from(value: u8) -> Result<Self, Self::Error> {
        match value {
            0 => Ok(Self::V0),
            1 => Ok(Self::V1),
            2 => Ok(Self::V2),
            _ => Err(()),
        }
    }
}

impl From<SecuritySuite> for u8 {
    fn from(value: SecuritySuite) -> Self {
        value as u8
    }
}

/// DLMS interface wrapper to use when forming connections.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
#[repr(u8)]
pub enum InterfaceType {
    /// High-Level Data Link Control framing.
    Hdlc = 0,
    /// DLMS wrapper over TCP/UDP transports.
    Wrapper = 0x1,
    /// Raw APDU exchange without additional framing.
    Pdu = 0x2,
    /// Wireless M-Bus transport profile.
    WirelessMbus = 0x3,
    /// HDLC framing using Mode E optical signalling.
    HdlcWithModeE = 0x4,
    /// Narrowband PLC G3 transport.
    Plc = 0x5,
    /// Hybrid PLC/HDLC transport profile.
    PlcHdlc = 0x6,
    /// Low-Power Wide-Area Network profile.
    Lpwan = 0x7,
    /// Wi-SUN FAN profile.
    WiSun = 0x8,
    /// PRIME PLC transport profile.
    PlcPrime = 0x9,
}

impl TryFrom<u8> for InterfaceType {
    type Error = ();

    fn try_from(value: u8) -> Result<Self, Self::Error> {
        Ok(match value {
            0 => Self::Hdlc,
            1 => Self::Wrapper,
            2 => Self::Pdu,
            3 => Self::WirelessMbus,
            4 => Self::HdlcWithModeE,
            5 => Self::Plc,
            6 => Self::PlcHdlc,
            7 => Self::Lpwan,
            8 => Self::WiSun,
            9 => Self::PlcPrime,
            _ => return Err(()),
        })
    }
}

impl From<InterfaceType> for u8 {
    fn from(value: InterfaceType) -> Self {
        value as u8
    }
}

/// DLMS authentication mechanisms.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
#[repr(u8)]
pub enum Authentication {
    /// No authentication.
    None = 0,
    /// Low level (password) authentication.
    Low = 1,
    /// High level challenge-response without a specific algorithm.
    High = 2,
    /// High level authentication using MD5.
    HighMd5 = 3,
    /// High level authentication using SHA-1.
    HighSha1 = 4,
    /// High level authentication using GMAC.
    HighGmac = 5,
    /// High level authentication using SHA-256.
    HighSha256 = 6,
    /// High level authentication using ECDSA.
    HighEcdsa = 7,
}

impl TryFrom<u8> for Authentication {
    type Error = ();

    fn try_from(value: u8) -> Result<Self, Self::Error> {
        Ok(match value {
            0 => Self::None,
            1 => Self::Low,
            2 => Self::High,
            3 => Self::HighMd5,
            4 => Self::HighSha1,
            5 => Self::HighGmac,
            6 => Self::HighSha256,
            7 => Self::HighEcdsa,
            _ => return Err(()),
        })
    }
}

impl From<Authentication> for u8 {
    fn from(value: Authentication) -> Self {
        value as u8
    }
}

bitflags! {
    /// Conformance block negotiated between client and server.
    #[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
    #[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
    pub struct Conformance: u32 {
        /// No services enabled.
        const NONE = 0x0;
        /// Reserved bit used in the original specification.
        const RESERVED_ZERO = 0x1;
        /// General protection (security) services are supported.
        const GENERAL_PROTECTION = 0x2;
        /// General block transfer is supported.
        const GENERAL_BLOCK_TRANSFER = 0x4;
        /// Read services are supported.
        const READ = 0x8;
        /// Write services are supported.
        const WRITE = 0x10;
        /// Unconfirmed write services are supported.
        const UN_CONFIRMED_WRITE = 0x20;
        /// Delta value encoding is supported.
        const DELTA_VALUE_ENCODING = 0x40;
        /// Reserved bit defined in the specification.
        const RESERVED_SEVEN = 0x80;
        /// Attribute 0 can be written via the set service.
        const ATTRIBUTE_0_SUPPORTED_WITH_SET = 0x100;
        /// Priority management services are supported.
        const PRIORITY_MGMT_SUPPORTED = 0x200;
        /// Attribute 0 can be read via the get service.
        const ATTRIBUTE_0_SUPPORTED_WITH_GET = 0x400;
        /// Block transfer is supported for get and read requests.
        const BLOCK_TRANSFER_WITH_GET_OR_READ = 0x800;
        /// Block transfer is supported for set and write requests.
        const BLOCK_TRANSFER_WITH_SET_OR_WRITE = 0x1000;
        /// Block transfer is supported for action requests.
        const BLOCK_TRANSFER_WITH_ACTION = 0x2000;
        /// Multiple references are supported.
        const MULTIPLE_REFERENCES = 0x4000;
        /// Information report service is supported.
        const INFORMATION_REPORT = 0x8000;
        /// Data notification service is supported.
        const DATA_NOTIFICATION = 0x10000;
        /// Access service is supported.
        const ACCESS = 0x20000;
        /// Parameterised access is supported.
        const PARAMETERIZED_ACCESS = 0x40000;
        /// Get service is supported.
        const GET = 0x80000;
        /// Set service is supported.
        const SET = 0x100000;
        /// Selective access is supported.
        const SELECTIVE_ACCESS = 0x200000;
        /// Event notification is supported.
        const EVENT_NOTIFICATION = 0x400000;
        /// Action (method) service is supported.
        const ACTION = 0x800000;
    }
}

bitflags! {
    /// Connection state flags describing active transports.
    #[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
    #[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
    pub struct ConnectionState: u8 {
        /// No connection has been established.
        const NONE = 0x00;
        /// HDLC link layer is active.
        const HDLC = 0x01;
        /// DLMS application layer association is active.
        const DLMS = 0x02;
        /// IEC optical mode handshake is active.
        const IEC = 0x04;
    }
}

/// DLMS command identifiers.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
#[repr(u16)]
pub enum Command {
    /// No command pending.
    None = 0x00,
    /// Initiate request APDU.
    InitiateRequest = 0x01,
    /// Read request (SN referencing).
    ReadRequest = 0x05,
    /// Write request (SN referencing).
    WriteRequest = 0x06,
    /// HDLC disconnect mode frame.
    DisconnectMode = 0x1F,
    /// PLC ping request.
    PingRequest = 0x19,
    /// PLC ping response.
    PingResponse = 0x1A,
    /// PLC register request.
    RegisterRequest = 0x1C,
    /// PLC discover request.
    DiscoverRequest = 0x1D,
    /// PLC discover report.
    DiscoverReport = 0x1E,
    /// SNRM frame for HDLC link establishment.
    Snrm = 0x93,
    /// UA frame acknowledging HDLC setup.
    Ua = 0x73,
    /// Association request (AARQ).
    Aarq = 0x60,
    /// Association response (AARE).
    Aare = 0x61,
    /// HDLC disconnect (DISC).
    Disc = 0x53,
    /// Association release request.
    ReleaseRequest = 0x62,
    /// Association release response.
    ReleaseResponse = 0x63,
    /// Confirmed service error notification.
    ConfirmedServiceError = 0x0E,
    /// Push data notification.
    DataNotification = 0x0F,
    /// Information report request.
    InformationReport = 0x18,
    /// Initiate response APDU.
    InitiateResponse = 0x08,
    /// Read response (SN referencing).
    ReadResponse = 0x0C,
    /// Write response (SN referencing).
    WriteResponse = 0x0D,
    /// Get request (LN referencing).
    GetRequest = 0xC0,
    /// Get response (LN referencing).
    GetResponse = 0xC4,
    /// Set request (LN referencing).
    SetRequest = 0xC1,
    /// Set response (LN referencing).
    SetResponse = 0xC5,
    /// Action (method) request.
    MethodRequest = 0xC3,
    /// Action (method) response.
    MethodResponse = 0xC7,
    /// Exception response to a request.
    ExceptionResponse = 0xD8,
    /// General block transfer frame.
    GeneralBlockTransfer = 0xE0,
    /// Access request service.
    AccessRequest = 0xD9,
    /// Access response service.
    AccessResponse = 0xDA,
    /// Event notification message.
    EventNotification = 0xC2,
    /// HDLC reject frame.
    Rejected = 0x97,
    /// Gateway specific request.
    GatewayRequest = 0xE6,
    /// Gateway specific response.
    GatewayResponse = 0xE7,
}

impl From<Command> for u16 {
    fn from(value: Command) -> Self {
        value as u16
    }
}

impl TryFrom<u16> for Command {
    type Error = ();

    fn try_from(value: u16) -> Result<Self, Self::Error> {
        Ok(match value {
            0x00 => Self::None,
            0x01 => Self::InitiateRequest,
            0x05 => Self::ReadRequest,
            0x06 => Self::WriteRequest,
            0x1F => Self::DisconnectMode,
            0x19 => Self::PingRequest,
            0x1A => Self::PingResponse,
            0x1C => Self::RegisterRequest,
            0x1D => Self::DiscoverRequest,
            0x1E => Self::DiscoverReport,
            0x93 => Self::Snrm,
            0x73 => Self::Ua,
            0x60 => Self::Aarq,
            0x61 => Self::Aare,
            0x53 => Self::Disc,
            0x62 => Self::ReleaseRequest,
            0x63 => Self::ReleaseResponse,
            0x0E => Self::ConfirmedServiceError,
            0x0F => Self::DataNotification,
            0x18 => Self::InformationReport,
            0x08 => Self::InitiateResponse,
            0x0C => Self::ReadResponse,
            0x0D => Self::WriteResponse,
            0xC0 => Self::GetRequest,
            0xC4 => Self::GetResponse,
            0xC1 => Self::SetRequest,
            0xC5 => Self::SetResponse,
            0xC3 => Self::MethodRequest,
            0xC7 => Self::MethodResponse,
            0xD8 => Self::ExceptionResponse,
            0xE0 => Self::GeneralBlockTransfer,
            0xD9 => Self::AccessRequest,
            0xDA => Self::AccessResponse,
            0xC2 => Self::EventNotification,
            0x97 => Self::Rejected,
            0xE6 => Self::GatewayRequest,
            0xE7 => Self::GatewayResponse,
            _ => return Err(()),
        })
    }
}

/// Primitive DLMS data types as defined by the xDLMS specification.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
#[repr(u8)]
pub enum DataType {
    /// No value.
    None = 0,
    /// Array container.
    Array = 1,
    /// Structure container.
    Structure = 2,
    /// Boolean value.
    Boolean = 3,
    /// Bit-string value.
    BitString = 4,
    /// 32-bit signed integer.
    Int32 = 5,
    /// 32-bit unsigned integer.
    Uint32 = 6,
    /// Octet string.
    OctetString = 9,
    /// ASCII string.
    String = 10,
    /// UTF-8 encoded string.
    StringUtf8 = 12,
    /// Binary coded decimal.
    BinaryCodedDecimal = 13,
    /// 8-bit signed integer.
    Int8 = 15,
    /// 16-bit signed integer.
    Int16 = 16,
    /// 8-bit unsigned integer.
    Uint8 = 17,
    /// 16-bit unsigned integer.
    Uint16 = 18,
    /// Compact array container.
    CompactArray = 19,
    /// 64-bit signed integer.
    Int64 = 20,
    /// 64-bit unsigned integer.
    Uint64 = 21,
    /// Enumerated value.
    Enum = 22,
    /// 32-bit floating point.
    Float32 = 23,
    /// 64-bit floating point.
    Float64 = 24,
    /// COSEM date-time.
    DateTime = 25,
    /// COSEM date.
    Date = 26,
    /// COSEM time.
    Time = 27,
    /// Delta encoded 8-bit signed integer.
    DeltaInt8 = 28,
    /// Delta encoded 16-bit signed integer.
    DeltaInt16 = 29,
    /// Delta encoded 32-bit signed integer.
    DeltaInt32 = 30,
    /// Delta encoded 8-bit unsigned integer.
    DeltaUint8 = 31,
    /// Delta encoded 16-bit unsigned integer.
    DeltaUint16 = 32,
    /// Delta encoded 32-bit unsigned integer.
    DeltaUint32 = 33,
    /// By-reference indicator bit.
    ByRef = 0x80,
}

impl TryFrom<u8> for DataType {
    type Error = ();

    fn try_from(value: u8) -> Result<Self, Self::Error> {
        Ok(match value {
            0 => Self::None,
            1 => Self::Array,
            2 => Self::Structure,
            3 => Self::Boolean,
            4 => Self::BitString,
            5 => Self::Int32,
            6 => Self::Uint32,
            9 => Self::OctetString,
            10 => Self::String,
            12 => Self::StringUtf8,
            13 => Self::BinaryCodedDecimal,
            15 => Self::Int8,
            16 => Self::Int16,
            17 => Self::Uint8,
            18 => Self::Uint16,
            19 => Self::CompactArray,
            20 => Self::Int64,
            21 => Self::Uint64,
            22 => Self::Enum,
            23 => Self::Float32,
            24 => Self::Float64,
            25 => Self::DateTime,
            26 => Self::Date,
            27 => Self::Time,
            28 => Self::DeltaInt8,
            29 => Self::DeltaInt16,
            30 => Self::DeltaInt32,
            31 => Self::DeltaUint8,
            32 => Self::DeltaUint16,
            33 => Self::DeltaUint32,
            0x80 => Self::ByRef,
            _ => return Err(()),
        })
    }
}

impl From<DataType> for u8 {
    fn from(value: DataType) -> Self {
        value as u8
    }
}

impl fmt::Display for DataType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{:?}", self)
    }
}

impl FromStr for DataType {
    type Err = ();

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        match s {
            "None" => Ok(Self::None),
            "Array" => Ok(Self::Array),
            "Structure" => Ok(Self::Structure),
            "Boolean" => Ok(Self::Boolean),
            "BitString" => Ok(Self::BitString),
            "Int32" => Ok(Self::Int32),
            "Uint32" => Ok(Self::Uint32),
            "OctetString" => Ok(Self::OctetString),
            "String" => Ok(Self::String),
            "StringUtf8" => Ok(Self::StringUtf8),
            "BinaryCodedDecimal" => Ok(Self::BinaryCodedDecimal),
            "Int8" => Ok(Self::Int8),
            "Int16" => Ok(Self::Int16),
            "Uint8" => Ok(Self::Uint8),
            "Uint16" => Ok(Self::Uint16),
            "CompactArray" => Ok(Self::CompactArray),
            "Int64" => Ok(Self::Int64),
            "Uint64" => Ok(Self::Uint64),
            "Enum" => Ok(Self::Enum),
            "Float32" => Ok(Self::Float32),
            "Float64" => Ok(Self::Float64),
            "DateTime" => Ok(Self::DateTime),
            "Date" => Ok(Self::Date),
            "Time" => Ok(Self::Time),
            "DeltaInt8" => Ok(Self::DeltaInt8),
            "DeltaInt16" => Ok(Self::DeltaInt16),
            "DeltaInt32" => Ok(Self::DeltaInt32),
            "DeltaUint8" => Ok(Self::DeltaUint8),
            "DeltaUint16" => Ok(Self::DeltaUint16),
            "DeltaUint32" => Ok(Self::DeltaUint32),
            "ByRef" => Ok(Self::ByRef),
            _ => Err(()),
        }
    }
}
