#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "kyber/utils/byte_conversion.h"  // Assuming bytes_to_bits, bits_to_bytes defined here
#include "kyber/entities/polring.h"      // Assuming PolynomialRing defined here

// Helper function to convert an integer to bits (replaces np.empty)
void int_to_bits(uint8_t *bits, int value, int num_bits) {
    for (int i = num_bits - 1; i >= 0; i--) {
        bits[i] = (value & (1 << i)) ? 1 : 0;
    }
}

// Helper function to convert bits to an integer (replaces np.sum)
int bits_to_int(const uint8_t *bits, int num_bits) {
    int value = 0;
    for (int i = 0; i < num_bits; i++) {
        value += bits[i] * (1 << i);
    }
    return value;
}

uint8_t *encode(PolynomialRing **pols, size_t pol_count, int l) {
    uint8_t *result = malloc(32 * l * pol_count);
    if (!result) {
        return NULL;  // Handle memory allocation failure
    }

    size_t offset = 0;
    for (size_t i = 0; i < pol_count; i++) {
        uint8_t *bits = malloc(256 * l);
        if (!bits) {
            free(result);
            return NULL;  // Handle memory allocation failure
        }

        for (size_t j = 0; j < 256; j++) {
            int f_item = pols[i]->coefficients[j];
            if (f_item < 0 || f_item > (1 << l) - 1) {
                free(bits);
                free(result);
                return NULL;  // Raise appropriate error
            }
            int_to_bits(bits + j * l, f_item, l);
        }

        memcpy(result + offset, bits_to_bytes(bits, 256 * l), 32 * l);
        offset += 32 * l;
        free(bits);
    }

    assert(offset == 32 * l * pol_count);
    return result;
}

PolynomialRing **decode(const uint8_t *b, size_t b_len, int l) {
    if (b_len % (32 * l) != 0) {
        return NULL;  // Raise appropriate error
    }

    size_t pol_count = b_len / (32 * l);
    PolynomialRing **result = malloc(pol_count * sizeof(PolynomialRing *));
    if (!result) {
        return NULL;  // Handle memory allocation failure
    }

    for (size_t i = 0; i < pol_count; i++) {
        uint8_t *bits = bytes_to_bits(b + 32 * l * i, 32 * l);
        int *f = malloc(256 * sizeof(int));
        if (!f) {
            for (size_t j = 0; j < i; j++) {
                polynomial_ring_destroy(result[j]);
            }
            free(result);
            return NULL;  // Handle memory allocation failure
        }

        for (size_t j = 0; j < 256; j++) {
            f[j] = bits_to_int(bits + j * l, l);
            assert(0 <= f[j] && f[j] <= (1 << l) - 1);
        }

        result[i] = polynomial_ring_create(f, 256, false);
        free(f);
        free(bits);
    }

    return result;
}
