use crate::byte_buffer::ByteBuffer;

pub enum Asn1Tag {
    Boolean,
    Integer,
    BitString,
    OctetString,
    Null,
    ObjectIdentifier,
    ObjectDescriptor,
    External,
    Real,
    Enumerated,
    EmbeddedPdv,
    Utf8String,
    RelativeOid,
    Sequence,
    Set,
    NumericString,
    PrintableString,
    T61String,
    VideotexString,
    Ia5String,
    UtcTime,
    GeneralizedTime,
    GraphicString,
    VisibleString,
    GeneralString,
    UniversalString,
    CharacterString,
    BmpString,
    ContextSpecific(u8),
}

pub struct Asn1Object {
    pub tag: Asn1Tag,
    pub data: ByteBuffer,
}

pub fn parse(_buffer: &mut ByteBuffer) -> Result<Asn1Object, std::io::Error> {
    // Implementation of the ASN.1 parser will go here
    unimplemented!();
}
