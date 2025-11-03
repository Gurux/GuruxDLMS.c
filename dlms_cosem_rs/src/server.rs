use crate::cosem::CosemObject;
use crate::byte_buffer::ByteBuffer;

pub struct Server {
    objects: Vec<Box<dyn CosemObject>>,
}

impl Server {
    pub fn new() -> Self {
        Self {
            objects: Vec::new(),
        }
    }

    pub fn add_object(&mut self, object: Box<dyn CosemObject>) {
        self.objects.push(object);
    }

    pub fn handle_request(&mut self, _request: &mut ByteBuffer) -> Result<ByteBuffer, std::io::Error> {
        // Implementation of the request handling logic
        unimplemented!()
    }
}
