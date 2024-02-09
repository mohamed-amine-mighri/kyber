#include <stdint.h>

// Assuming these values are accurate and consistent with the official specification

// Plaintext size in bits
const uint32_t n = 256;

// Divider of polynomial coefficient modulos
const uint32_t q = 3329;

// Dimension of polynomial matrices
const uint8_t k = 4;

// Noise level for some polynomials (Kyber1024 specific)
const uint8_t eta1 = 2;
const uint8_t eta2 = 2;

// Compression and encoding level for some polynomials (Kyber1024 specific)
const uint8_t du = 11;
const uint8_t dv = 5;
