//! HIGH_GMAC security suite.

use aes::{Aes128, Aes256};
use cipher::generic_array::GenericArray;
use cipher::{BlockEncrypt, KeyInit, KeyIvInit, StreamCipher};
use ctr::Ctr128BE;

/// AES key size.
#[derive(Copy, Clone)]
pub enum Aes {
    /// AES 128.
    Aes128,
    /// AES 256.
    Aes256,
}

/// Make Xor for 128 bits.
pub fn gxgcm_xor(block: &mut [u8; 16], value: &[u8; 16]) {
    for i in 0..16 {
        block[i] ^= value[i];
    }
}

/// Shift block to right.
pub fn gxgcm_shift_right(block: &mut [u8; 16]) {
    let mut val = u32::from_be_bytes(block[12..16].try_into().unwrap());
    val >>= 1;
    if (block[11] & 0x01) != 0 {
        val |= 0x80000000;
    }
    block[12..16].copy_from_slice(&val.to_be_bytes());

    val = u32::from_be_bytes(block[8..12].try_into().unwrap());
    val >>= 1;
    if (block[7] & 0x01) != 0 {
        val |= 0x80000000;
    }
    block[8..12].copy_from_slice(&val.to_be_bytes());

    val = u32::from_be_bytes(block[4..8].try_into().unwrap());
    val >>= 1;
    if (block[3] & 0x01) != 0 {
        val |= 0x80000000;
    }
    block[4..8].copy_from_slice(&val.to_be_bytes());

    val = u32::from_be_bytes(block[0..4].try_into().unwrap());
    val >>= 1;
    block[0..4].copy_from_slice(&val.to_be_bytes());
}

/// Multiply by H.
pub fn gxgcm_multiply_h(y: &mut [u8; 16], h: &[u8; 16]) {
    let mut z = [0u8; 16];
    let mut tmp = *h;

    for i in 0..16 {
        for j in 0..8 {
            if (y[i] & (1 << (7 - j))) != 0 {
                gxgcm_xor(&mut z, &tmp);
            }
            if (tmp[15] & 0x01) != 0 {
                gxgcm_shift_right(&mut tmp);
                tmp[0] ^= 0xe1;
            } else {
                gxgcm_shift_right(&mut tmp);
            }
        }
    }
    *y = z;
}

/// Get GHASH.
#[allow(clippy::too_many_arguments)]
pub fn gxgcm_get_ghash(
    security: u8,
    aes: Aes,
    tag: u8,
    authentication_key: &[u8],
    value: &[u8],
    len_a: u32,
    len_c: u32,
    y: &mut [u8; 16],
    key: &[u8],
) {
    let mut h = GenericArray::from([0u8; 16]);

    match aes {
        Aes::Aes128 => {
            let cipher = Aes128::new(key.into());
            cipher.encrypt_block(&mut h);
        }
        Aes::Aes256 => {
            let cipher = Aes256::new(key.into());
            cipher.encrypt_block(&mut h);
        }
    }

    let mut x = [0u8; 16];
    y.fill(0);
    x[0] = tag;
    x[1..16].copy_from_slice(&authentication_key[0..15]);
    gxgcm_xor(y, &x);
    gxgcm_multiply_h(y, &h.into());

    let mut available = if let Aes::Aes128 = aes { 16 } else { 32 } - 15;
    if available > 16 {
        available = 16;
        x[0..available].copy_from_slice(&authentication_key[15..15 + available]);
        gxgcm_xor(y, &x);
        gxgcm_multiply_h(y, &h.into());
        available = if let Aes::Aes128 = aes { 16 } else { 32 } - 31;
        x[0..available].copy_from_slice(&authentication_key[31..31 + available]);
    } else {
        x[0..available].copy_from_slice(&authentication_key[15..15 + available]);
    }

    if security != 0x10 {
        x[available..].fill(0);
        available = 0;
        gxgcm_xor(y, &x);
        gxgcm_multiply_h(y, &h.into());
    }

    let mut pos = 0;
    while pos < value.len() {
        x[available..].fill(0);
        let mut count = value.len() - pos;
        if count > 16 - available {
            count = 16 - available;
        }
        x[available..available + count].copy_from_slice(&value[pos..pos + count]);
        gxgcm_xor(y, &x);
        gxgcm_multiply_h(y, &h.into());
        pos += count;
        available = 0;
    }

    if value.len() + available > pos {
        x[available..].fill(0);
        x[0..available].copy_from_slice(&value[pos - available..]);
        gxgcm_xor(y, &x);
        gxgcm_multiply_h(y, &h.into());
    }

    x[0..4].copy_from_slice(&0u32.to_be_bytes());
    x[4..8].copy_from_slice(&len_a.to_be_bytes());
    x[8..12].copy_from_slice(&0u32.to_be_bytes());
    x[12..16].copy_from_slice(&len_c.to_be_bytes());
    gxgcm_xor(y, &x);
    gxgcm_multiply_h(y, &h.into());
}

/// Get tag.
#[allow(clippy::too_many_arguments)]
pub fn gxgcm_get_tag(
    security: u8,
    aes: Aes,
    system_title: &[u8],
    frame_counter: u32,
    authentication_key: &[u8],
    tag: u8,
    value: &[u8],
    a_tag: &mut [u8],
    key: &[u8],
) {
    let len_a = (1 + if let Aes::Aes128 = aes { 16 } else { 32 }) * 8;
    let len_c = if security == 0x10 {
        0
    } else {
        value.len() as u32 * 8
    };

    let mut hash = [0u8; 16];
    gxgcm_get_ghash(
        security,
        aes,
        tag,
        authentication_key,
        value,
        len_a,
        len_c,
        &mut hash,
        key,
    );

    gxgcm_transform_block(aes, system_title, frame_counter, &mut hash, 1, key);
    a_tag[..12].copy_from_slice(&hash[..12]);
}

/// Transform block.
pub fn gxgcm_transform_block(
    aes: Aes,
    system_title: &[u8],
    counter: u32,
    data: &mut [u8],
    algorithm_initial_block_counter: u32,
    key: &[u8],
) {
    let mut iv = [0u8; 16];
    iv[0..8].copy_from_slice(system_title);
    iv[8..12].copy_from_slice(&counter.to_be_bytes());
    iv[12..16].copy_from_slice(&algorithm_initial_block_counter.to_be_bytes());

    match aes {
        Aes::Aes128 => {
            let mut cipher = Ctr128BE::<Aes128>::new(key.into(), &iv.into());
            cipher.apply_keystream(data);
        }
        Aes::Aes256 => {
            let mut cipher = Ctr128BE::<Aes256>::new(key.into(), &iv.into());
            cipher.apply_keystream(data);
        }
    }
}
