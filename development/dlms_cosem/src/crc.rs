//! CRC calculation.

/// Calculates the CRC-16 CCITT-FALSE for the given data.
pub fn count_crc(data: &[u8]) -> u16 {
    crc16::State::<crc16::CCITT_FALSE>::calculate(data)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_crc() {
        let data = b"123456789";
        let crc = count_crc(data);
        assert_eq!(crc, 0x29B1);
    }
}
