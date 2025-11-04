use crate::variant::Variant;

pub trait CosemObject {
    fn logical_name(&self) -> &[u8];
    fn object_type(&self) -> u16;
    fn get_attribute(&self, attribute_id: u8) -> Option<Variant>;
    fn set_attribute(&mut self, attribute_id: u8, value: Variant) -> Result<(), ()>;
}

pub mod clock;
pub mod data;
pub mod register;
