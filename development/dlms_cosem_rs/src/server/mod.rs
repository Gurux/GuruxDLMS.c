//! Server side helpers for exposing DLMS/COSEM objects.

use core::convert::TryInto;
use core::fmt;

use std::sync::Arc;

use crate::error::{Error, Result};
use crate::objects::ObisCode;
use crate::transport::framing::FrameCodec;
use crate::types::{
    serialize_variant, AssociationResult, Command, DlmsSettings, ObjectList, SourceDiagnostic,
    Variant,
};

type FrameCallback = Arc<dyn Fn(&[u8]) + Send + Sync>;
type AttributeReadCallback =
    Arc<dyn Fn(&CosemObject, &CosemAttribute) -> Result<Variant> + Send + Sync>;
type MethodInvokeCallback = Arc<
    dyn Fn(&CosemObject, &CosemMethod, Option<&Variant>) -> Result<Option<Variant>> + Send + Sync,
>;

/// Collection of hooks that allow applications to observe the server lifecycle.
#[derive(Default, Clone)]
pub struct ServerCallbacks {
    on_request: Option<FrameCallback>,
    on_before_send: Option<FrameCallback>,
    on_after_send: Option<FrameCallback>,
    on_attribute_read: Option<AttributeReadCallback>,
    on_method_invoke: Option<MethodInvokeCallback>,
}

impl fmt::Debug for ServerCallbacks {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("ServerCallbacks")
            .field("on_request", &self.on_request.is_some())
            .field("on_before_send", &self.on_before_send.is_some())
            .field("on_after_send", &self.on_after_send.is_some())
            .field("on_attribute_read", &self.on_attribute_read.is_some())
            .field("on_method_invoke", &self.on_method_invoke.is_some())
            .finish()
    }
}

impl ServerCallbacks {
    /// Registers a callback executed whenever a raw APDU is received from the transport.
    pub fn set_on_request<F>(&mut self, callback: F)
    where
        F: Fn(&[u8]) + Send + Sync + 'static,
    {
        self.on_request = Some(Arc::new(callback));
    }

    /// Removes the callback that observes inbound APDUs.
    pub fn clear_on_request(&mut self) {
        self.on_request = None;
    }

    /// Registers a callback triggered right before a response is handed to the transport layer.
    pub fn set_on_before_send<F>(&mut self, callback: F)
    where
        F: Fn(&[u8]) + Send + Sync + 'static,
    {
        self.on_before_send = Some(Arc::new(callback));
    }

    /// Removes the callback that runs before responses are emitted.
    pub fn clear_on_before_send(&mut self) {
        self.on_before_send = None;
    }

    /// Registers a callback triggered after a response has been prepared.
    pub fn set_on_after_send<F>(&mut self, callback: F)
    where
        F: Fn(&[u8]) + Send + Sync + 'static,
    {
        self.on_after_send = Some(Arc::new(callback));
    }

    /// Removes the callback that runs after responses are emitted.
    pub fn clear_on_after_send(&mut self) {
        self.on_after_send = None;
    }

    /// Registers a callback that produces dynamic attribute values.
    pub fn set_on_attribute_read<F>(&mut self, callback: F)
    where
        F: Fn(&CosemObject, &CosemAttribute) -> Result<Variant> + Send + Sync + 'static,
    {
        self.on_attribute_read = Some(Arc::new(callback));
    }

    /// Removes the attribute read callback.
    pub fn clear_on_attribute_read(&mut self) {
        self.on_attribute_read = None;
    }

    /// Registers a callback that executes COSEM methods on behalf of the server.
    pub fn set_on_method_invoke<F>(&mut self, callback: F)
    where
        F: Fn(&CosemObject, &CosemMethod, Option<&Variant>) -> Result<Option<Variant>>
            + Send
            + Sync
            + 'static,
    {
        self.on_method_invoke = Some(Arc::new(callback));
    }

    /// Removes the method invocation callback.
    pub fn clear_on_method_invoke(&mut self) {
        self.on_method_invoke = None;
    }

    fn on_request(&self) -> Option<&FrameCallback> {
        self.on_request.as_ref()
    }

    fn on_before_send(&self) -> Option<&FrameCallback> {
        self.on_before_send.as_ref()
    }

    fn on_after_send(&self) -> Option<&FrameCallback> {
        self.on_after_send.as_ref()
    }

    fn on_attribute_read(&self) -> Option<&AttributeReadCallback> {
        self.on_attribute_read.as_ref()
    }

    fn on_method_invoke(&self) -> Option<&MethodInvokeCallback> {
        self.on_method_invoke.as_ref()
    }
}

/// Minimal representation of a COSEM attribute exposed by the server.
#[derive(Clone, Debug, PartialEq)]
pub struct CosemAttribute {
    index: u8,
    value: Variant,
    writable: bool,
}

impl CosemAttribute {
    /// Creates a read-only attribute with the provided static value.
    pub fn new(index: u8, value: Variant) -> Self {
        Self {
            index,
            value,
            writable: false,
        }
    }

    /// Marks the attribute as writable.
    pub fn with_writable(mut self, writable: bool) -> Self {
        self.writable = writable;
        self
    }

    /// Returns the attribute index as used by the DLMS/COSEM protocol.
    pub fn index(&self) -> u8 {
        self.index
    }

    /// Returns the static value associated with the attribute.
    pub fn value(&self) -> &Variant {
        &self.value
    }

    /// Indicates whether the attribute can be modified via `set` services.
    pub fn is_writable(&self) -> bool {
        self.writable
    }
}

/// Representation of a COSEM method exposed by the server.
#[derive(Clone, Debug, PartialEq)]
pub struct CosemMethod {
    index: u8,
    default_response: Option<Variant>,
}

impl CosemMethod {
    /// Creates a method definition without a default response payload.
    pub fn new(index: u8) -> Self {
        Self {
            index,
            default_response: None,
        }
    }

    /// Configures the default response returned when no callback overrides the value.
    pub fn with_default_response(mut self, response: Variant) -> Self {
        self.default_response = Some(response);
        self
    }

    /// Returns the method index.
    pub fn index(&self) -> u8 {
        self.index
    }

    fn default_response(&self) -> Option<Variant> {
        self.default_response.clone()
    }
}

/// Metadata describing a COSEM object handled by the server.
#[derive(Clone, Debug, PartialEq)]
pub struct CosemObject {
    class_id: u16,
    version: u8,
    logical_name: ObisCode,
    attributes: Vec<CosemAttribute>,
    methods: Vec<CosemMethod>,
    include_in_association_view: bool,
}

impl CosemObject {
    /// Creates a new COSEM object description.
    pub fn new(class_id: u16, logical_name: ObisCode, version: u8) -> Self {
        Self {
            class_id,
            version,
            logical_name,
            attributes: Vec::new(),
            methods: Vec::new(),
            include_in_association_view: true,
        }
    }

    /// Marks whether the object should be reported in the association view.
    pub fn with_association_visibility(mut self, visible: bool) -> Self {
        self.include_in_association_view = visible;
        self
    }

    /// Adds an attribute definition to the object.
    pub fn with_attribute(mut self, attribute: CosemAttribute) -> Self {
        self.attributes.push(attribute);
        self
    }

    /// Adds a method definition to the object.
    pub fn with_method(mut self, method: CosemMethod) -> Self {
        self.methods.push(method);
        self
    }

    /// Returns the class identifier of the object.
    pub fn class_id(&self) -> u16 {
        self.class_id
    }

    /// Returns the logical name associated with the object.
    pub fn logical_name(&self) -> ObisCode {
        self.logical_name
    }

    /// Returns the version number of the object.
    pub fn version(&self) -> u8 {
        self.version
    }

    /// Provides read-only access to the attribute definitions.
    pub fn attributes(&self) -> &[CosemAttribute] {
        &self.attributes
    }

    /// Provides read-only access to the method definitions.
    pub fn methods(&self) -> &[CosemMethod] {
        &self.methods
    }

    fn attribute(&self, index: u8) -> Option<&CosemAttribute> {
        self.attributes.iter().find(|attr| attr.index == index)
    }

    fn method(&self, index: u8) -> Option<&CosemMethod> {
        self.methods.iter().find(|method| method.index == index)
    }
}

/// Entry in the server side association view mirroring the legacy implementation.
#[derive(Clone, Debug, PartialEq, Eq)]
pub struct AssociationViewEntry {
    /// Class identifier of the exposed object.
    pub class_id: u16,
    /// Logical name of the exposed object.
    pub logical_name: ObisCode,
    /// Implemented version of the object.
    pub version: u8,
    /// List of attribute identifiers accessible through the association.
    pub attributes: Vec<u8>,
    /// List of method identifiers accessible through the association.
    pub methods: Vec<u8>,
}

/// High level DLMS/COSEM server facade used in integration tests and examples.
#[derive(Debug)]
pub struct DlmsServer {
    settings: DlmsSettings<()>,
    objects: ObjectList<CosemObject>,
    association_view: Vec<AssociationViewEntry>,
    callbacks: ServerCallbacks,
    associated: bool,
}

impl DlmsServer {
    /// Creates a new server facade from the provided settings and object list.
    pub fn new(mut settings: DlmsSettings<()>, objects: ObjectList<CosemObject>) -> Self {
        settings.server = true;
        Self {
            settings,
            objects,
            association_view: Vec::new(),
            callbacks: ServerCallbacks::default(),
            associated: false,
        }
    }

    /// Returns a shared reference to the underlying DLMS settings.
    pub fn settings(&self) -> &DlmsSettings<()> {
        &self.settings
    }

    /// Returns a mutable reference to the underlying DLMS settings.
    pub fn settings_mut(&mut self) -> &mut DlmsSettings<()> {
        &mut self.settings
    }

    /// Provides access to the callback registry allowing applications to install hooks.
    pub fn callbacks_mut(&mut self) -> &mut ServerCallbacks {
        &mut self.callbacks
    }

    /// Returns the association view calculated during initialization.
    pub fn association_view(&self) -> &[AssociationViewEntry] {
        &self.association_view
    }

    /// Indicates whether the server has accepted an association request from a client.
    pub fn is_associated(&self) -> bool {
        self.associated
    }

    /// Performs validation of the configuration and builds the association view.
    pub fn initialize(&mut self) -> Result<()> {
        if self.settings.max_pdu_size < 64 {
            return Err(Error::InvalidConfiguration(
                "max_pdu_size must be at least 64 octets",
            ));
        }
        let mut association_view = Vec::new();
        for object in self.objects.iter() {
            if object.logical_name().is_zero() {
                return Err(Error::InvalidConfiguration(
                    "logical name cannot be all zeros",
                ));
            }
            if object.include_in_association_view {
                association_view.push(AssociationViewEntry {
                    class_id: object.class_id(),
                    logical_name: object.logical_name(),
                    version: object.version(),
                    attributes: object
                        .attributes()
                        .iter()
                        .map(CosemAttribute::index)
                        .collect(),
                    methods: object.methods().iter().map(CosemMethod::index).collect(),
                });
            }
        }
        self.association_view = association_view;
        Ok(())
    }

    /// Handles an inbound APDU returning the serialized response.
    pub fn handle_apdu_bytes(&mut self, request: &[u8]) -> Result<Vec<u8>> {
        if let Some(callback) = self.callbacks.on_request() {
            callback(request);
        }
        let apdu = crate::types::Apdu::parse(request)?;
        let response = self.dispatch_apdu(apdu)?;
        let bytes = response.to_bytes();
        if let Some(callback) = self.callbacks.on_before_send() {
            callback(&bytes);
        }
        if let Some(callback) = self.callbacks.on_after_send() {
            callback(&bytes);
        }
        Ok(bytes)
    }

    /// Handles a wrapper frame and returns the encoded response frame.
    pub fn handle_wrapper_frame(&mut self, frame: &[u8]) -> Result<Vec<u8>> {
        let parsed = FrameCodec::parse_wrapper(frame)?;
        let response = self.handle_apdu_bytes(&parsed.payload)?;
        FrameCodec::wrapper(&self.settings, &response)
    }

    /// Handles a HDLC frame returning the response ready to be transmitted to the client.
    pub fn handle_hdlc_frame(&mut self, frame: &[u8]) -> Result<Vec<u8>> {
        let parsed = FrameCodec::parse_hdlc(&mut self.settings, frame)?;
        let response = self.handle_apdu_bytes(&parsed.payload)?;
        let apdu = crate::types::Apdu::parse(&response)?;
        FrameCodec::hdlc(&mut self.settings, apdu.command, &response)
    }

    fn dispatch_apdu(&mut self, apdu: crate::types::Apdu) -> Result<crate::types::Apdu> {
        match apdu.command {
            Command::Aarq => self.handle_aarq(apdu.payload),
            Command::GetRequest => self.handle_get_request(apdu.payload),
            Command::MethodRequest => self.handle_method_request(apdu.payload),
            other => Err(Error::UnsupportedCommand(other)),
        }
    }

    fn handle_aarq(&mut self, _payload: Vec<u8>) -> Result<crate::types::Apdu> {
        self.associated = true;
        let payload = vec![
            AssociationResult::Accepted as u8,
            SourceDiagnostic::None as u8,
        ];
        Ok(crate::types::Apdu::new(Command::Aare, payload))
    }

    fn handle_get_request(&mut self, payload: Vec<u8>) -> Result<crate::types::Apdu> {
        let request = GetRequest::parse(&payload)?;
        let object = self
            .objects
            .iter()
            .find(|object| {
                object.class_id() == request.class_id
                    && object.logical_name() == request.logical_name
            })
            .ok_or(Error::InvalidFrame("unknown logical name"))?;
        let attribute = object
            .attribute(request.attribute_id)
            .ok_or(Error::InvalidFrame("unknown attribute index"))?;
        let value = if let Some(callback) = self.callbacks.on_attribute_read() {
            callback(object, attribute)?
        } else {
            attribute.value().clone()
        };
        let mut buffer = crate::types::ByteBuffer::new();
        buffer.push(0x01); // get-response-normal
        buffer.push(request.invoke_id);
        buffer.push(0x00); // data
        serialize_variant(&mut buffer, &value)?;
        Ok(crate::types::Apdu::new(
            Command::GetResponse,
            buffer.into_vec(),
        ))
    }

    fn handle_method_request(&mut self, payload: Vec<u8>) -> Result<crate::types::Apdu> {
        let request = MethodRequest::parse(&payload)?;
        let object = self
            .objects
            .iter()
            .find(|object| {
                object.class_id() == request.class_id
                    && object.logical_name() == request.logical_name
            })
            .ok_or(Error::InvalidFrame("unknown logical name"))?;
        let method = object
            .method(request.method_id)
            .ok_or(Error::InvalidFrame("unknown method index"))?;
        let result = if let Some(callback) = self.callbacks.on_method_invoke() {
            callback(object, method, request.parameter.as_ref())?
        } else {
            method.default_response()
        };
        let mut buffer = crate::types::ByteBuffer::new();
        buffer.push(0x01); // method-response-normal
        buffer.push(request.invoke_id);
        match result {
            Some(value) => {
                buffer.push(0x00);
                serialize_variant(&mut buffer, &value)?;
            }
            None => {
                buffer.push(0x01);
            }
        }
        Ok(crate::types::Apdu::new(
            Command::MethodResponse,
            buffer.into_vec(),
        ))
    }
}

struct GetRequest {
    invoke_id: u8,
    class_id: u16,
    logical_name: ObisCode,
    attribute_id: u8,
}

impl GetRequest {
    fn parse(payload: &[u8]) -> Result<Self> {
        if payload.len() < 13 {
            return Err(Error::InvalidFrame("get-request too short"));
        }
        if payload[0] != 0x01 {
            return Err(Error::InvalidFrame("unsupported get-request variant"));
        }
        if payload[2] != 0x00 {
            return Err(Error::InvalidFrame("unsupported attribute descriptor"));
        }
        let class_id = u16::from_be_bytes([payload[3], payload[4]]);
        let ln: [u8; 6] = payload[5..11]
            .try_into()
            .map_err(|_| Error::InvalidFrame("invalid logical name length"))?;
        let attribute_id = payload[11];
        Ok(Self {
            invoke_id: payload[1],
            class_id,
            logical_name: ObisCode::from(ln),
            attribute_id,
        })
    }
}

struct MethodRequest {
    invoke_id: u8,
    class_id: u16,
    logical_name: ObisCode,
    method_id: u8,
    parameter: Option<Variant>,
}

impl MethodRequest {
    fn parse(payload: &[u8]) -> Result<Self> {
        if payload.len() < 13 {
            return Err(Error::InvalidFrame("method-request too short"));
        }
        if payload[0] != 0x01 {
            return Err(Error::InvalidFrame("unsupported method-request variant"));
        }
        if payload[2] != 0x00 {
            return Err(Error::InvalidFrame("unsupported method descriptor"));
        }
        let class_id = u16::from_be_bytes([payload[3], payload[4]]);
        let ln: [u8; 6] = payload[5..11]
            .try_into()
            .map_err(|_| Error::InvalidFrame("invalid logical name length"))?;
        let method_id = payload[11];
        let offset = 12;
        let parameter = if payload.len() > offset {
            match payload[offset] {
                0x00 => None,
                0x01 => {
                    let (value, consumed) =
                        crate::types::deserialize_variant(&payload[offset + 1..])?;
                    if consumed == 0 {
                        return Err(Error::InvalidFrame("invalid method parameter"));
                    }
                    Some(value)
                }
                _ => return Err(Error::InvalidFrame("invalid method parameter tag")),
            }
        } else {
            None
        };
        Ok(Self {
            invoke_id: payload[1],
            class_id,
            logical_name: ObisCode::from(ln),
            method_id,
            parameter,
        })
    }
}

/// Placeholder description of a DLMS/COSEM logical device served by Gurux stacks.
#[derive(Debug, Default)]
pub struct LogicalDevice {
    /// Optional name for documentation and diagnostics.
    pub name: Option<&'static str>,
    /// Indicates that persistent storage is required by the device.
    pub requires_persistence: bool,
}

impl LogicalDevice {
    /// Checks whether the logical device should be compiled into the binary.
    pub const fn is_available() -> bool {
        !cfg!(feature = "dlms_ignore_server")
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::transport::framing::FrameCodec;
    use crate::types::{Apdu, InterfaceType};
    use std::sync::atomic::{AtomicUsize, Ordering};

    fn sample_settings() -> DlmsSettings<()> {
        let mut settings = DlmsSettings::default().with_addresses(0x10, 1);
        settings.interface_type = InterfaceType::Wrapper;
        settings.max_pdu_size = 1024;
        settings
    }

    fn sample_object() -> CosemObject {
        CosemObject::new(1, ObisCode(1, 0, 0, 2, 0, 255), 0)
            .with_attribute(CosemAttribute::new(2, Variant::UInt16(42)))
            .with_method(
                CosemMethod::new(1)
                    .with_default_response(Variant::Utf8String("executed".to_string())),
            )
    }

    fn build_get_request(object: &CosemObject, attribute: u8) -> Apdu {
        let mut buffer = crate::types::ByteBuffer::new();
        buffer.push(0x01);
        buffer.push(0x01);
        buffer.push(0x00);
        buffer.push_u16(object.class_id());
        buffer.extend_from_slice(&object.logical_name().to_bytes());
        buffer.push(attribute);
        buffer.push(0x00);
        Apdu::new(Command::GetRequest, buffer.into_vec())
    }

    fn build_method_request(object: &CosemObject, method: u8) -> Apdu {
        let mut buffer = crate::types::ByteBuffer::new();
        buffer.push(0x01);
        buffer.push(0x02);
        buffer.push(0x00);
        buffer.push_u16(object.class_id());
        buffer.extend_from_slice(&object.logical_name().to_bytes());
        buffer.push(method);
        buffer.push(0x00);
        Apdu::new(Command::MethodRequest, buffer.into_vec())
    }

    #[test]
    fn initialize_populates_association_view() {
        let settings = sample_settings();
        let objects = ObjectList::from_vec(vec![sample_object()]);
        let mut server = DlmsServer::new(settings, objects);
        server.initialize().unwrap();
        assert_eq!(server.association_view().len(), 1);
        let entry = &server.association_view()[0];
        assert_eq!(entry.class_id, 1);
        assert_eq!(entry.attributes, vec![2]);
        assert_eq!(entry.methods, vec![1]);
    }

    #[test]
    fn association_request_is_acknowledged() {
        let settings = sample_settings();
        let objects = ObjectList::from_vec(vec![sample_object()]);
        let mut server = DlmsServer::new(settings, objects);
        server.initialize().unwrap();
        let aarq = Apdu::new(Command::Aarq, vec![]);
        let request_frame = FrameCodec::wrapper(server.settings(), &aarq.to_bytes()).unwrap();
        let response_frame = server.handle_wrapper_frame(&request_frame).unwrap();
        let parsed = FrameCodec::parse_wrapper(&response_frame).unwrap();
        let response_apdu = Apdu::parse(&parsed.payload).unwrap();
        assert_eq!(response_apdu.command, Command::Aare);
        assert_eq!(response_apdu.payload[0], AssociationResult::Accepted as u8);
        assert!(server.is_associated());
    }

    #[test]
    fn get_request_reads_attribute_through_wrapper() {
        let settings = sample_settings();
        let object = sample_object();
        let objects = ObjectList::from_vec(vec![object.clone()]);
        let mut server = DlmsServer::new(settings, objects);
        server.initialize().unwrap();
        let apdu = build_get_request(&object, 2);
        let request = FrameCodec::wrapper(server.settings(), &apdu.to_bytes()).unwrap();
        let response = server.handle_wrapper_frame(&request).unwrap();
        let parsed = FrameCodec::parse_wrapper(&response).unwrap();
        let response_apdu = Apdu::parse(&parsed.payload).unwrap();
        assert_eq!(response_apdu.command, Command::GetResponse);
        assert_eq!(response_apdu.payload[0], 0x01);
        assert_eq!(response_apdu.payload[1], 0x01);
        assert_eq!(response_apdu.payload[2], 0x00);
        let (value, _) = crate::types::deserialize_variant(&response_apdu.payload[3..]).unwrap();
        assert_eq!(value, Variant::UInt16(42));
    }

    #[test]
    fn method_request_uses_callbacks() {
        let settings = sample_settings();
        let object = sample_object();
        let objects = ObjectList::from_vec(vec![object.clone()]);
        let mut server = DlmsServer::new(settings, objects);
        server.initialize().unwrap();
        let invocations = Arc::new(AtomicUsize::new(0));
        server.callbacks_mut().set_on_method_invoke({
            let count = Arc::clone(&invocations);
            move |_, _, _| {
                count.fetch_add(1, Ordering::SeqCst);
                Ok(Some(Variant::Int16(7)))
            }
        });
        let apdu = build_method_request(&object, 1);
        let response = server.handle_apdu_bytes(&apdu.to_bytes()).unwrap();
        assert_eq!(invocations.load(Ordering::SeqCst), 1);
        let parsed = Apdu::parse(&response).unwrap();
        assert_eq!(parsed.command, Command::MethodResponse);
        assert_eq!(parsed.payload[0], 0x01);
        assert_eq!(parsed.payload[1], 0x02);
        assert_eq!(parsed.payload[2], 0x00);
        let (value, _) = crate::types::deserialize_variant(&parsed.payload[3..]).unwrap();
        assert_eq!(value, Variant::Int16(7));
    }

    #[test]
    fn callbacks_observe_request_and_response() {
        let settings = sample_settings();
        let object = sample_object();
        let objects = ObjectList::from_vec(vec![object.clone()]);
        let mut server = DlmsServer::new(settings, objects);
        server.initialize().unwrap();
        let seen_request = Arc::new(AtomicUsize::new(0));
        let seen_response = Arc::new(AtomicUsize::new(0));
        server.callbacks_mut().set_on_request({
            let counter = Arc::clone(&seen_request);
            move |_| {
                counter.fetch_add(1, Ordering::SeqCst);
            }
        });
        server.callbacks_mut().set_on_before_send({
            let counter = Arc::clone(&seen_response);
            move |_| {
                counter.fetch_add(1, Ordering::SeqCst);
            }
        });
        let apdu = build_get_request(&object, 2);
        let _ = server.handle_apdu_bytes(&apdu.to_bytes()).unwrap();
        assert_eq!(seen_request.load(Ordering::SeqCst), 1);
        assert_eq!(seen_response.load(Ordering::SeqCst), 1);
    }
}
