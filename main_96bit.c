// This source file is part of the hibiscuit open source project
// Date created: Friday, 22 March, 2024
// Copyright (c) 2024 nicklaus yap ken yik (nicklausyap)
// This is the shorten-version which only output 96 bit hash
// version 1.1

#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For dynamic memory allocation

#define CHUNK_SIZE 512 // Size of each chunk in bits
#define NUM_WORDS 16   // Number of 32-bit words in each chunk

unsigned int rightRotate(unsigned int value, int shift) {
    return ((value >> shift) | (value << (32 - shift))) & 0xFFFFFFFF;
}

void doHash(unsigned int words[], unsigned int *h0, unsigned int *h1, unsigned int *h2, unsigned int *h3, unsigned int *h4, unsigned int *h5) {
    unsigned int a = *h0;
    unsigned int b = *h1;
    unsigned int c = *h2;
    unsigned int d = *h3;
    unsigned int e = *h4;
    unsigned int f = *h5;
    unsigned int temp = 0;
    unsigned int p = 0;
    unsigned int k = 0;
    for (int i = 0; i < 60; i++) {
        if (i >= 0 && i <= 19) {
            p = (b & c) | (~b & d);
            k = 0x5A82799A;
        } else if (i >= 20 && i <= 39) {
            p = b ^ c ^ d;
            k = 0x6ED9EBA1;
        } else if (i >= 40 && i <= 59) {
            p = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
        }
        temp = rightRotate(a, 5) + p + e + k + words[i % 16];
        e = d;
        d = c;
        c = rightRotate(b, 30);
        b = f;
        a = temp & 0xFFFFFFFF; // Ensure 32-bit arithmetic
    }
    *h0 = (*h0 + a) & 0xFFFFFFFF;
    *h1 = (*h1 + b) & 0xFFFFFFFF;
    *h2 = (*h2 + c) & 0xFFFFFFFF;
    *h3 = (*h3 + d) & 0xFFFFFFFF;
    *h4 = (*h4 + e) & 0xFFFFFFFF;
    *h5 = (*h5 + f) & 0xFFFFFFFF;
}

void breakIntoWords(unsigned char chunk[], unsigned int words[]) {
    for (int i = 0; i < NUM_WORDS; i++) {
        words[i] = (chunk[i * 4] << 24) |
                   (chunk[i * 4 + 1] << 16) |
                   (chunk[i * 4 + 2] << 8) |
                   chunk[i * 4 + 3];
        words[i] = words[i] & 0xFFFFFFFF; // Ensure 32-bit arithmetic
    }
}

void breakInto512BitChunks(unsigned char inputString[]) {
    unsigned int h0 = 0x67452301;
    unsigned int h1 = 0xEFCDAB89;
    unsigned int h2 = 0x98BADCFE;
    unsigned int h3 = 0xC3D2A1F0;
    unsigned int h4 = 0x10325476;
    unsigned int h5 = 0x1645931C;
    int inputLength = strlen(inputString);
    int totalLength = inputLength * 8; // Total length in bits
    int paddingBits = (CHUNK_SIZE - ((totalLength + 64) % CHUNK_SIZE)) % CHUNK_SIZE;
    int paddingBytes = (paddingBits + 64) / 8; // 64 bits for the original length
    int paddedLength = inputLength + paddingBytes;
    unsigned char* paddedString = (unsigned char*)malloc(paddedLength * sizeof(unsigned char)); // Dynamic allocation

    if (paddedString == NULL) {
        printf("Memory allocation failed\n");
        return;
    }

    memcpy(paddedString, inputString, inputLength);
    paddedString[inputLength] = 0x80; // Start padding
    memset(paddedString + inputLength + 1, 0, paddingBytes - 9); // Zero padding
    unsigned long long bitLength = (unsigned long long)inputLength * 8; // Original length in bits
    // Assuming big-endian for the length encoding as per standard
    for (int i = 0; i < 8; i++) {
        paddedString[paddedLength - 1 - i] = (unsigned char)((bitLength >> (i * 8)) & 0xFF);
    }

    int numChunks = paddedLength / (CHUNK_SIZE / 8);
    for (int i = 0; i < numChunks; i++) {
        unsigned char chunk[CHUNK_SIZE / 8];
        memcpy(chunk, paddedString + i * (CHUNK_SIZE / 8), CHUNK_SIZE / 8);
        unsigned int words[NUM_WORDS];
        breakIntoWords(chunk, words);
        doHash(words, &h0, &h1, &h2, &h3, &h4, &h5);
    }
    // Print the final hash in hexadecimal format
    printf("Final hash: %08x%08x%08x\n", h0, h4, h5);

    free(paddedString); // Free the dynamically allocated memory
}


int main() {
    unsigned char inputString[] = "He knew it was going to be a bad day when he saw mountain lions roaming the streets.Italy is my favorite country; in fact, I plan to spend two weeks there next year.";
    breakInto512BitChunks(inputString);
    return 0;
}

