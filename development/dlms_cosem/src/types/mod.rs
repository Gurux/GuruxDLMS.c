//! Strongly typed representations of DLMS/COSEM primitives shared across the crate.

mod buffer;
mod codec;
mod enums;
mod object;
mod settings;
mod value;

pub use buffer::ByteBuffer;
pub use codec::{
    deserialize_variant, read_object_count, serialize_variant, write_object_count, Apdu,
};
pub use enums::{
    AssociationResult, Authentication, Command, Conformance, ConnectionState, DataType,
    InterfaceType, MessageType, Priority, Security, SecurityPolicy, SecuritySuite, ServiceClass,
    ServiceType, SourceDiagnostic,
};
pub use object::{ObjectArray, ObjectList};
pub use settings::{
    DlmsSettings, InvocationCounter, PlcSettings, DEFAULT_MAX_INFO_RX, DEFAULT_MAX_INFO_TX,
    DEFAULT_MAX_WINDOW_SIZE_RX, DEFAULT_MAX_WINDOW_SIZE_TX,
};
pub use value::{DlmsDateTime, Variant, VariantCollection};
