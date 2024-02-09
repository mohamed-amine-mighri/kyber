#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#include "kyber/entities/polring.h"  // Assuming PolynomialRing is defined here

// Function prototype for clarity
PolynomialRing *cbd(const uint8_t *b, size_t byte_count, int eta);

PolynomialRing *cbd(const uint8_t *b, size_t byte_count, int eta) {
    // Validate input: length should be 64*eta
    if (byte_count != 64 * eta) {
        return NULL;
    }

    // Convert bytes to bits (assuming `bytes_to_bits` is implemented elsewhere)
    size_t bit_count = byte_count * 8;
    uint8_t *bits = bytes_to_bits(b, byte_count);
    if (!bits) {
        return NULL;
    }

    // Ensure correct bit count after conversion
    assert(bit_count == 512 * eta);

    // Create PolynomialRing object (assuming constructor exists)
    PolynomialRing *f = allocate_polynomial_ring(256);
    if (!f) {
        free(bits);
        return NULL;
    }

    // Calculate coefficients
    for (int i = 0; i < 256; i++) {
        int a = 0, b = 0;
        for (int j = 0; j < eta; j++) {
            a += bits[2 * i * eta + j];
            b += bits[2 * i * eta + eta + j];
        }
        f->coefficients[i] = a - b;
    }

    // Free allocated memory
    free(bits);

    return f;
}
