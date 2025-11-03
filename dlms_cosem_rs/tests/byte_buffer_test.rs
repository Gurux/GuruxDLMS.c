use dlms_cosem_rs::byte_buffer::ByteBuffer;

#[test]
fn test_new() {
    let bb = ByteBuffer::new();
    assert_eq!(bb.len(), 0);
    assert_eq!(bb.position(), 0);
}

#[test]
fn test_with_capacity() {
    let bb = ByteBuffer::with_capacity(10);
    assert_eq!(bb.len(), 0);
    assert!(bb.capacity() >= 10);
}

#[test]
fn test_put_get_u8() {
    let mut bb = ByteBuffer::new();
    bb.put_u8(42).unwrap();
    bb.set_position(0);
    assert_eq!(bb.get_u8().unwrap(), 42);
}
