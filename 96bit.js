// This source file is part of the hibiscuit open source project
// Date created: Monday, 1 April, 2024
// Copyright (c) 2024 nicklaus yap ken yik (nicklausyap)
// This is the javascript implementation of hibiscuit for 96-bit hash
// version 1.1

function rightRotate(value, shift) {
    return ((value >>> shift) | (value << (32 - shift))) >>> 0;
}

function doHash(words, h) {
    let a = h[0],
        b = h[1],
        c = h[2],
        d = h[3],
        e = h[4],
        f = h[5];
    let temp = 0, p = 0, k = 0;

    for (let i = 0; i < 60; i++) {
        if (i < 20) {
            p = (b & c) | (~b & d);
            k = 0x5A82799A;
        } else if (i < 40) {
            p = b ^ c ^ d;
            k = 0x6ED9EBA1;
        } else if (i < 60) {
            p = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
        }
        temp = (rightRotate(a, 5) + p + e + k + words[i % 16]) >>> 0;
        e = d;
        d = c;
        c = rightRotate(b, 30);
        b = f;
        a = temp;
    }

    h[0] = (h[0] + a) >>> 0;
    h[1] = (h[1] + b) >>> 0;
    h[2] = (h[2] + c) >>> 0;
    h[3] = (h[3] + d) >>> 0;
    h[4] = (h[4] + e) >>> 0;
    h[5] = (h[5] + f) >>> 0;
}

function breakIntoWords(chunk) {
    let words = [];
    for (let i = 0; i < 16; i++) {
        words[i] = ((chunk[i * 4] << 24) | (chunk[i * 4 + 1] << 16) | (chunk[i * 4 + 2] << 8) | chunk[i * 4 + 3]) >>> 0;
    }
    return words;
}

function breakInto512BitChunks(inputString) {
    let h = [0x67452301, 0xEFCDAB89, 0x98BADCFE, 0xC3D2A1F0, 0x10325476, 0x1645931C];
    let bytes = new TextEncoder().encode(inputString);
    let bitLength = bytes.length * 8;
    let paddingLength = (448 - bitLength % 512 + 512) % 512; // Padding to 448 mod 512
    let totalLength = bitLength + paddingLength + 64; // +64 for the bit length
    let paddedArray = new Uint8Array((totalLength + 7) >> 3);

    paddedArray.set(bytes, 0);
    paddedArray[bytes.length] = 0x80;
    let view = new DataView(paddedArray.buffer);
    view.setUint32(paddedArray.length - 4, bitLength >>> 0, false); // Big endian

    for (let i = 0; i < paddedArray.length; i += 64) {
        let chunk = paddedArray.slice(i, i + 64);
        let words = breakIntoWords(chunk);
        doHash(words, h);
    }

    console.log(`Final hash: ${h[0].toString(16)}${h[4].toString(16)}${h[5].toString(16)}`);
}

// Below is an example input
const inputString = "He knew it was going to be a bad day when he saw mountain lions roaming the streets.Italy is my favorite country; in fact, I plan to spend two weeks there next year.";
breakInto512BitChunks(inputString);
