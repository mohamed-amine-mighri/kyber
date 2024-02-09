#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "kyber/constants.h"  // Assuming n and q are defined here
#include "kyber/entities/polring.h"  // Assuming PolynomialRing is defined here

// Function to read 3 bytes from the stream
uint8_t *get_3bytes(uint8_t **stream) {
    uint8_t *bytes = malloc(3);
    if (!bytes) {
        return NULL;  // Handle memory allocation failure
    }
    memcpy(bytes, *stream, 3);
    *stream += 3;  // Advance the stream pointer
    return bytes;
}

// Function to convert a byte array to an integer (replaces int.from_bytes)
uint32_t byte_to_int(const uint8_t *bytes, size_t len) {
    uint32_t result = 0;
    for (size_t i = 0; i < len; i++) {
        result = (result << 8) | bytes[i];
    }
    return result;
}

PolynomialRing *parse(uint8_t **stream) {
    int i = 0, j = 0;
    int *a = malloc(n * sizeof(int));
    if (!a) {
        return NULL;  // Handle memory allocation failure
    }

    while (j < n) {
        uint8_t *bytes = get_3bytes(stream);
        if (!bytes) {
            free(a);
            return NULL;  // Handle memory allocation failure
        }

        uint32_t b1 = byte_to_int(bytes, 1);
        uint32_t b2 = byte_to_int(bytes + 1, 1);
        uint32_t b3 = byte_to_int(bytes + 2, 1);
        free(bytes);

        uint32_t d1 = b1 + 256 * (b2 % 16);
        uint32_t d2 = floor(b2 / 16.0) + 16 * b3;  // Use floor for floating-point division

        if (d1 < q) {
            a[j++] = d1;
        }
        if (d2 < q && j < n) {
            a[j++] = d2;
        }
        i += 3;
    }

    return polynomial_ring_create(a, n, false);
}
