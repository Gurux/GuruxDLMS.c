use crate::cosem::CosemObject;
use crate::variant::Variant;

pub struct Clock {
    logical_name: [u8; 6],
    time: Variant,
    time_zone: Variant,
    status: Variant,
    daylight_saving_begin: Variant,
    daylight_saving_end: Variant,
    daylight_saving_deviation: Variant,
    enabled: Variant,
}

impl CosemObject for Clock {
    fn logical_name(&self) -> &[u8] {
        &self.logical_name
    }

    fn object_type(&self) -> u16 {
        8 // Clock
    }

    fn get_attribute(&self, attribute_id: u8) -> Option<Variant> {
        match attribute_id {
            2 => Some(self.time.clone()),
            3 => Some(self.time_zone.clone()),
            4 => Some(self.status.clone()),
            5 => Some(self.daylight_saving_begin.clone()),
            6 => Some(self.daylight_saving_end.clone()),
            7 => Some(self.daylight_saving_deviation.clone()),
            8 => Some(self.enabled.clone()),
            _ => None,
        }
    }

    fn set_attribute(&mut self, attribute_id: u8, value: Variant) -> Result<(), ()> {
        match attribute_id {
            2 => {
                self.time = value;
                Ok(())
            }
            3 => {
                self.time_zone = value;
                Ok(())
            }
            4 => {
                self.status = value;
                Ok(())
            }
            5 => {
                self.daylight_saving_begin = value;
                Ok(())
            }
            6 => {
                self.daylight_saving_end = value;
                Ok(())
            }
            7 => {
                self.daylight_saving_deviation = value;
                Ok(())
            }
            8 => {
                self.enabled = value;
                Ok(())
            }
            _ => Err(()),
        }
    }
}
