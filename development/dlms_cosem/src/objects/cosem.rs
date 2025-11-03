//! COSEM object model.

/// A COSEM object.
pub trait CosemObject {
    /// Returns the object type.
    fn object_type() -> u16;
    /// Returns the logical name of the object.
    fn logical_name(&self) -> &[u8; 6];
}
