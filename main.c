// This source file is part of the hibiscuit open source project
// Date created: Friday, 22 March, 2024
// Copyright (c) 2024 nicklaus yap ken yik (nicklausyap)

#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#define CHUNK_SIZE 512 // Size of each chunk in bits
#define NUM_WORDS 16   // Number of 32-bit words in each chunk

// Function to perform right rotation
uint32_t rightRotate(uint32_t value, int shift) {
    return (value >> shift) | (value << (32 - shift));
}

void doHash(uint32_t words[], uint32_t* h0, uint32_t* h1, uint32_t* h2, uint32_t* h3, uint32_t* h4, uint32_t* h5) {
    uint32_t a = *h0;
    uint32_t b = *h1;
    uint32_t c = *h2;
    uint32_t d = *h3;
    uint32_t e = *h4;
    uint32_t f = *h5;
    uint32_t temp = 0;
    int i; // used in for loop
    uint32_t p;
    uint32_t k;
    for (i = 0; i < 120; i++) {
        if (i >= 0 && i <= 19) {
            p = (b & c) | (~b & d);
            k = 0x5A82799A; // (2^30)*sqrt(2). Is rounded to the closest
        }
        else if (i >= 20 && i <= 39) {
            p = b ^ c ^ d;
            k = 0x6ED9EBA1; // (2^30)*sqrt(4)
        }
        else if (i >= 40 && i <= 59) {
            p = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC; // (2^30)*sqrt(6)
        }
        else if (i >= 60 && i <= 79) {
            p = b ^ c ^ d;
            k = 0xCA62C1D6; // (2^30)*sqrt(8)
        }
        else if (i >= 80 && i <= 99) {
            p = (a & c) | (a & d) | (c & d);
            k = 0xCA62C1D6; // (2^30)*sqrt(10)
        }else if (i >= 100 && i <= 119) {
            p = c ^ d ^ e;
            k = 0xDDB3D743; // (2^30)*sqrt(12). Is rounded to the closest
        }

        temp = rightRotate(a, 5) + p + e + k + words[i % 16];
        e = d;
        d = c;
        c = rightRotate(b, 30);
        b = f;
        a = temp;
    }

    // Add this chunk's hash to result so far
    *h0 += a;
    *h1 += b;
    *h2 += c;
    *h3 += d;
    *h4 += e;
    *h5 += f;
}

void breakIntoWords(const char* chunk, uint32_t* words) {
    for (int i = 0; i < NUM_WORDS; i++) {
        // Extract each 32-bit word from the chunk (big-endian)
        words[i] = ((uint32_t)chunk[i * 4] << 24) |
            ((uint32_t)chunk[i * 4 + 1] << 16) |
            ((uint32_t)chunk[i * 4 + 2] << 8) |
            (uint32_t)chunk[i * 4 + 3];
    }
}

char** breakInto512BitChunks(const char* inputString, size_t* numChunks) {
    // initialize the starting variable
    uint32_t h0 = 0x67452301;
    uint32_t h1 = 0xEFCDAB89;
    uint32_t h2 = 0x98BADCFE;
    uint32_t h3 = 0xC3D2A1F0;
    uint32_t h4 = 0x10325476;
    uint32_t h5 = 0xC3D2A1F0;

    // Get the length of the input string
    size_t inputLength = strlen(inputString);

    // Calculate the number of bits needed for padding
    size_t paddingBits = CHUNK_SIZE - (inputLength % CHUNK_SIZE);

    // Calculate the number of bytes needed for padding
    size_t paddingBytes = paddingBits / 8;

    // Calculate the total length of the padded string
    size_t paddedLength = inputLength + paddingBytes;

    // Allocate memory for the padded string
    char* paddedString = (char*)malloc(paddedLength + 1); // +1 for the null terminator
    if (paddedString == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    // Copy the input string to the padded string
    strcpy(paddedString, inputString);

    // Add padding to the padded string
    for (size_t i = inputLength; i < paddedLength; i++) {
        paddedString[i] = (char)(paddingBytes % 256); // Use the padding bytes as characters
    }
    paddedString[paddedLength] = '\0'; // Null terminator

    // Calculate the number of chunks
    *numChunks = paddedLength / (CHUNK_SIZE / 8); // Convert bits to bytes

    // Allocate memory for the array of chunk strings
    char** chunks = (char**)malloc(*numChunks * sizeof(char*));
    if (chunks == NULL) {
        printf("Memory allocation failed.\n");
        free(paddedString);
        return NULL;
    }

    uint32_t words[NUM_WORDS];
    // Extract each chunk and store it in the array
    for (size_t i = 0; i < *numChunks; i++) {
        chunks[i] = (char*)malloc(CHUNK_SIZE / 8 + 1); // +1 for the null terminator
        if (chunks[i] == NULL) {
            printf("Memory allocation failed.\n");
            // Free previously allocated memory
            for (size_t j = 0; j < i; j++) {
                free(chunks[j]);
            }
            free(chunks);
            free(paddedString);
            return NULL;
        }
        strncpy(chunks[i], paddedString + i * (CHUNK_SIZE / 8), CHUNK_SIZE / 8);
        chunks[i][CHUNK_SIZE / 8] = '\0'; // Null terminator

        // Break each chunk into 32-bit words
        breakIntoWords(chunks[i], words);

        // Print the words
        printf("Words in Chunk %zu:\n", i + 1);
        for (size_t j = 0; j < NUM_WORDS; j++) {
            printf("w[%zu]: %08x\n", j, words[j]);
        }
        printf("\n");

        doHash(words, &h0, &h1, &h2, &h3, &h4, &h5);
    }

    // print final hash value
    printf("h0 in decimal: %u\n", h0);
    printf("h1 in decimal: %u\n", h1);
    printf("h2 in decimal: %u\n", h2);
    printf("h3 in decimal: %u\n", h3);
    printf("h4 in decimal: %u\n", h4);
    printf("h5 in decimal: %u\n", h5);
    uint64_t hh;
    hh = h0 + h1 + h2 + h3 + h4 + h5;
    printf("hash(combined) in decimal: ");
    printf("%" PRIu64 "\n", hh); // PRIu64 is used to print uint64_t type in printf(). Is included with #include <inttypes.h>
    printf("final hash(192 bits): %08x%08x%08x%08x%08x%08x\n", h0,h1,h2,h3,h4,h5);

    // end of printing final hash value

    // Free the allocated memory for the padded string
    free(paddedString);

    // Return the array of chunk strings
    return chunks;
}

int main() {
    // Below is some random message
    const char* inputString = "fishhello hello hello world my love 3029 fishhello hello hello world my love 3029 fishhello hello hello world my love 3029";
    size_t numChunks;
    char** chunks = breakInto512BitChunks(inputString, &numChunks);
    if (chunks != NULL) {
        // Print the chunks
        for (size_t i = 0; i < numChunks; i++) {
            printf("Chunk %zu:\n%s\n", i + 1, chunks[i]);
        }

        // Free the memory allocated for each chunk string
        for (size_t i = 0; i < numChunks; i++) {
            free(chunks[i]);
        }

        // Free the memory allocated for the array of chunk strings
        free(chunks);
    }
    return 0;
}
