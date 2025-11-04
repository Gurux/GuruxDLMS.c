use alloc::vec::Vec;

#[derive(Clone, Debug)]
pub struct BitArray {
    data: Vec<u8>,
    bit_count: usize,
}

impl BitArray {
    pub fn new() -> Self {
        Self {
            data: Vec::new(),
            bit_count: 0,
        }
    }

    pub fn with_capacity(bit_capacity: usize) -> Self {
        let byte_capacity = (bit_capacity + 7) / 8;
        Self {
            data: Vec::with_capacity(byte_capacity),
            bit_count: 0,
        }
    }

    pub fn len(&self) -> usize {
        self.bit_count
    }

    pub fn is_empty(&self) -> bool {
        self.bit_count == 0
    }

    pub fn clear(&mut self) {
        self.data.clear();
        self.bit_count = 0;
    }

    pub fn get(&self, index: usize) -> Option<bool> {
        if index >= self.bit_count {
            return None;
        }
        let byte_index = index / 8;
        let bit_index = 7 - (index % 8);
        self.data
            .get(byte_index)
            .map(|&byte| (byte & (1 << bit_index)) != 0)
    }

    pub fn set(&mut self, index: usize, value: bool) {
        if index >= self.bit_count {
            self.bit_count = index + 1;
        }
        let byte_index = index / 8;
        let bit_index = 7 - (index % 8);
        if byte_index >= self.data.len() {
            self.data.resize(byte_index + 1, 0);
        }
        if value {
            self.data[byte_index] |= 1 << bit_index;
        } else {
            self.data[byte_index] &= !(1 << bit_index);
        }
    }

    pub fn push(&mut self, value: bool) {
        self.set(self.bit_count, value);
    }
}
