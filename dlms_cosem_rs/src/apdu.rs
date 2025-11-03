pub enum Command {
    GetRequest,
    SetRequest,
    ActionRequest,
    // Add other commands here
}

pub enum GetRequest {
    Normal {
        invoke_id_and_priority: u8,
        attribute_descriptor: (), // Replace with actual type
    },
    Next {
        invoke_id_and_priority: u8,
        block_number: u32,
    },
    WithList {
        invoke_id_and_priority: u8,
        attribute_descriptors: Vec<()>, // Replace with actual type
    },
}

// Define other request/response structs here
