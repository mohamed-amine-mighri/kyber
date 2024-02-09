#include <stdint.h>
#include <string.h>

#include "crypto/sha256.h"  // Assuming SHA256 implementation
#include "crypto/sha3.h"    // Assuming SHA3 implementation

// Custom SHAKE family implementation (can be replaced with actual library calls)
typedef struct shake_state {
    uint8_t state[64];
    uint64_t rate;
    uint64_t capacity;
    uint64_t count;
    uint8_t output[16];  // Adjust size based on shake output length
} shake_state;

void shake_init(shake_state *state, const uint8_t *digest, int rate, int capacity) {
    memcpy(state->state, digest, 64);
    state->rate = rate;
    state->capacity = capacity;
    state->count = 0;
    memset(state->output, 0, 16);
}

void shake_update(shake_state *state, const uint8_t *data, size_t data_len) {
    while (data_len > 0) {
        size_t bytes_to_absorb = state->capacity - state->count;
        size_t to_copy = data_len < bytes_to_absorb ? data_len : bytes_to_absorb;
        memcpy(state->state + state->count, data, to_copy);
        state->count += to_copy;
        data += to_copy;
        data_len -= to_copy;

        if (state->count == state->capacity) {
            // Permute state and reset count
            // ... (actual permutation implementation needed)
            state->count = 0;
        }
    }
}

uint8_t shake_read(shake_state *state, uint8_t *output, size_t output_len) {
    while (output_len > 0) {
        if (state->count == 0) {
            // Permute state and reset count
            // ... (actual permutation implementation needed)
            state->count = 0;
        }

        size_t bytes_to_squeeze = state->rate - state->count;
        size_t to_copy = output_len < bytes_to_squeeze ? output_len : bytes_to_squeeze;
        memcpy(output, state->state + state->count, to_copy);
        state->count += to_copy;
        output += to_copy;
        output_len -= to_copy;
    }

    return 1;  // Indicate success (can be adapted based on library convention)
}

// Function definitions
uint8_t *prf(const uint8_t *s, const uint8_t *b, uint8_t *output) {
    shake_state state;
    shake_init(&state, SHA256_ DIGEST, SHA256_ RATE, SHA256_ CAPACITY);
    shake_update(&state, s, strlen((const char *)s));
    shake_update(&state, b, strlen((const char *)b));
    shake_read(&state, output, 128);
    return output;
}

uint8_t *kdf(const uint8_t *b, int l, uint8_t *output) {
    shake_state state;
    shake_init(&state, SHA256_ DIGEST, SHA256_ RATE, SHA256_ CAPACITY);
    shake_update(&state, b, strlen((const char *)b));
    shake_read(&state, output, l);
    return output;
}

uint8_t *G(const uint8_t *b, uint8_t *output) {
    SHA3_CTX ctx;
    sha3_Init(&ctx, SHA3_512);
    sha3_Update(&ctx, b, strlen((const char *)b));
    sha3_Final(&ctx, output);
    return output;
}

typedef struct xof_state {
    shake_state state;
    uint8_t i[16];
    uint8_t j[16];
} xof_state;

xof_state *xof_init(const uint8_t *p, const uint8_t *i, const uint8_t *j) {
    xof_state *state = malloc(sizeof(xof_state));
    if (!state) {
        return NULL;  // Handle memory allocation failure
    }

    // Initialize SHAKE state
    shake_init(&state->state, SHAKE128_ DIGEST, SHAKE128_ RATE, SHAKE128_ CAPACITY);

    // Update state with p, i, and j
    shake_update(&state->state, p, strlen((const char *)p));
    shake_update(&state->state, i, 16);  // Assuming i has fixed length of 16 bytes
    shake_update(&state->state, j, 16);  // Assuming j has fixed length of 16 bytes

    // Copy i and j values for future reference
    memcpy(state->i, i, 16);
    memcpy(state->j, j, 16);

    return state;
}