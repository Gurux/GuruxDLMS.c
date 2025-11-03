use dlms_cosem_rs::client::Client;

fn main() {
    let mut client = Client::new();
    println!("Connecting to meter...");
    client.connect().unwrap();
    println!("Connected.");

    // Replace with actual object and attribute ID
    // let object = ...;
    // let attribute_id = 1;
    // let value = client.read(&object, attribute_id).unwrap();
    // println!("Read value: {:?}", value);
}
