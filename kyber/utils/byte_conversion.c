#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define BYTE_SIZE 8

// Function prototypes for clarity
uint8_t *bytes_to_bits(const uint8_t *byte_array, size_t byte_count, size_t *bit_count);
uint8_t *bits_to_bytes(const uint8_t *bits, size_t bit_count);
uint8_t *int_to_bytes(uint32_t n);

uint8_t *bytes_to_bits(const uint8_t *byte_array, size_t byte_count, size_t *bit_count) {
    // Ensure input is valid
    if (!byte_array || byte_count == 0) {
        return NULL;
    }

    // Calculate expected bit count and allocate memory
    size_t expected_bit_count = byte_count * BYTE_SIZE;
    uint8_t *bits = malloc(expected_bit_count);
    if (!bits) {
        return NULL;
    }

    // Convert each byte to its bit representation
    size_t bit_index = 0;
    for (size_t i = 0; i < byte_count; i++) {
        uint8_t byte = byte_array[i];
        for (int j = 7; j >= 0; j--) {
            bits[bit_index++] = (byte >> j) & 1;
        }
    }

    // Set the actual bit count and return the result
    *bit_count = expected_bit_count;
    return bits;
}

uint8_t *bits_to_bytes(const uint8_t *bits, size_t bit_count) {
    // Ensure input is valid
    if (!bits || bit_count % BYTE_SIZE != 0) {
        return NULL;
    }

    // Calculate expected byte count and allocate memory
    size_t byte_count = bit_count / BYTE_SIZE;
    uint8_t *bytes = malloc(byte_count);
    if (!bytes) {
        return NULL;
    }

    // Convert each group of 8 bits to a byte
    size_t byte_index = 0;
    for (size_t i = 0; i < bit_count; i += BYTE_SIZE) {
        uint8_t byte = 0;
        for (int j = 0; j < BYTE_SIZE; j++) {
            byte |= bits[i + j] << (7 - j);
        }
        bytes[byte_index++] = byte;
    }

    return bytes;
}

uint8_t *int_to_bytes(uint32_t n) {
    // Ensure input is non-negative
    if (n < 0) {
        return NULL;
    }

    // Calculate minimum necessary byte count (considering endianness)
    int byte_count = sizeof(n);
    int system_endian = 1;

    // Check endianness using a union trick
    union {
        uint32_t i;
        uint8_t b[4];
    } u;
    u.i = 0x12345678;
    system_endian = u.b[0] == 0x78;

    // Ensure big-endian representation for consistent handling
    if (system_endian == 0) {
        for (int i = 0; i < byte_count / 2; i++) {
            uint8_t temp = n >> (8 * i);
            n <<= 8 * i;
            n |= temp << (8 * (byte_count - 1 - i));
        }
    }

    // Allocate memory and cast integer to bytes
    uint8_t *bytes = malloc(byte_count);
    if (!bytes) {
        return NULL;
    }
    memcpy(bytes, &n, byte_count);

    return bytes;
}
