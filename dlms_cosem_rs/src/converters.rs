pub enum DlmsObjectType {
    None,
    Data,
    Register,
    Clock,
    // Add other object types here...
}

pub fn object_type_to_string(object_type: DlmsObjectType) -> &'static str {
    match object_type {
        DlmsObjectType::None => "",
        DlmsObjectType::Data => "Data",
        DlmsObjectType::Register => "Register",
        DlmsObjectType::Clock => "Clock",
        // Add other object types here...
    }
}
