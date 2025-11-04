use crate::cosem::CosemObject;
use crate::variant::Variant;

pub struct Data {
    logical_name: [u8; 6],
    value: Variant,
}

impl Data {
    pub fn new(logical_name: [u8; 6], value: Variant) -> Self {
        Self {
            logical_name,
            value,
        }
    }
}

impl CosemObject for Data {
    fn logical_name(&self) -> &[u8] {
        &self.logical_name
    }

    fn object_type(&self) -> u16 {
        1 // Data
    }

    fn get_attribute(&self, attribute_id: u8) -> Option<Variant> {
        match attribute_id {
            2 => Some(self.value.clone()),
            _ => None,
        }
    }

    fn set_attribute(&mut self, attribute_id: u8, value: Variant) -> Result<(), ()> {
        match attribute_id {
            2 => {
                self.value = value;
                Ok(())
            }
            _ => Err(()),
        }
    }
}
