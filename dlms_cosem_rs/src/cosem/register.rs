use crate::cosem::CosemObject;
use crate::variant::Variant;

pub struct Register {
    logical_name: [u8; 6],
    value: Variant,
    scaler_unit: Variant,
}

impl CosemObject for Register {
    fn logical_name(&self) -> &[u8] {
        &self.logical_name
    }

    fn object_type(&self) -> u16 {
        3 // Register
    }

    fn get_attribute(&self, attribute_id: u8) -> Option<Variant> {
        match attribute_id {
            2 => Some(self.value.clone()),
            3 => Some(self.scaler_unit.clone()),
            _ => None,
        }
    }

    fn set_attribute(&mut self, attribute_id: u8, value: Variant) -> Result<(), ()> {
        match attribute_id {
            2 => {
                self.value = value;
                Ok(())
            }
            3 => {
                self.scaler_unit = value;
                Ok(())
            }
            _ => Err(()),
        }
    }
}
