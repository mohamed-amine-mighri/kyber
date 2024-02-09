#include <stdint.h>
#include <math.h>
#include <assert.h>

#include "kyber/constants.h"  // Assuming q is defined here
#include "kyber/utils/round.h"  // Assuming normal_round is defined here
#include "kyber/entities/polring.h"  // Assuming PolynomialRing is defined here

PolynomialRing **compress(PolynomialRing **pols, size_t pol_count, int d) {
    assert(d < ceil(log2(q)));

    PolynomialRing **result = malloc(pol_count * sizeof(PolynomialRing *));
    if (!result) {
        return NULL;  // Handle memory allocation failure
    }

    for (size_t i = 0; i < pol_count; i++) {
        int *compressed_coefs = malloc(pols[i]->coefficient_count * sizeof(int));
        if (!compressed_coefs) {
            free(result);
            return NULL;  // Handle memory allocation failure
        }

        for (size_t j = 0; j < pols[i]->coefficient_count; j++) {
            compressed_coefs[j] = compress_int(pols[i]->coefficients[j], d);
        }

        result[i] = polynomial_ring_create(compressed_coefs, pols[i]->coefficient_count, false);
        free(compressed_coefs);
    }

    return result;
}

PolynomialRing *decompress(PolynomialRing *pol, int d) {
    assert(d < ceil(log2(q)));

    int *decompressed_coefs = malloc(pol->coefficient_count * sizeof(int));
    if (!decompressed_coefs) {
        return NULL;  // Handle memory allocation failure
    }

    for (size_t i = 0; i < pol->coefficient_count; i++) {
        decompressed_coefs[i] = decompress_int(pol->coefficients[i], d);
    }

    PolynomialRing *result = polynomial_ring_create(decompressed_coefs, pol->coefficient_count, false);
    free(decompressed_coefs);
    return result;
}

int compress_int(int x, int d) {
    assert(d < ceil(log2(q)));
    int result = normal_round((1LL << d) * x / q) % (1 << d);  // Use 1LL for 64-bit literal
    assert(0 <= result && result <= (1 << d) - 1);
    return result;
}

int decompress_int(int x, int d) {
    assert(d < ceil(log2(q)));
    if (x < 0 || x > (1 << d) - 1) {
        // Handle invalid input (e.g., raise an error or return a default value)
    }
    int result = normal_round(q * x / (1LL << d));  // Use 1LL for 64-bit literal
    return result;
}
