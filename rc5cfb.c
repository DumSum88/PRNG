#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// RC5-32/12/16
#define W 32
#define R 12
#define KEY_LENGTH_BYTES 16
#define C (KEY_LENGTH_BYTES / 4)
#define T (2 * (R + 1))

uint32_t S[T];

#define ROTL(x, y) (((x) << (y & (W - 1))) | ((x) >> (W - (y & (W - 1)))))
#define ROTR(x, y) (((x) >> (y & (W - 1))) | ((x) << (W - (y & (W - 1)))))

void RC5_key_schedule(uint8_t *K) {
    uint32_t L[C];
    memset(L, 0, sizeof(L)); // Initialize L to zero
    for (int i = KEY_LENGTH_BYTES - 1; i >= 0; i--) {
        L[i / 4] = (L[i / 4] << 8) + K[i];
    }

    S[0] = 0xB7E15163; // число e
    for (int i = 1; i < T; i++) {
        S[i] = S[i - 1] + 0x9E3779B9; // число phi^2
    }

    uint32_t A = 0, B = 0;
    int i = 0, j = 0;
    int v = 3 * ((C > T) ? C : T);
    for (int s = 0; s < v; s++) {
        A = S[i] = ROTL(S[i] + A + B, 3);
        B = L[j] = ROTL(L[j] + A + B, A + B);
        i = (i + 1) % T;
        j = (j + 1) % C;
    }
}

void RC5_encrypt(uint32_t *pt, uint32_t *ct) {
    uint32_t A = pt[0] + S[0];
    uint32_t B = pt[1] + S[1];
    for (int i = 1; i <= R; i++) {
        A = ROTL(A ^ B, B) + S[2 * i];
        B = ROTL(B ^ A, A) + S[2 * i + 1];
 }
    ct[0] = A;
    ct[1] = B;
}

void RC5_CFB_encrypt(uint8_t *iv, uint8_t *data, size_t data_len) {
    uint32_t iv_block[2];
    memcpy(iv_block, iv, 8);

    for (size_t i = 0; i < data_len; i += 8) {
        uint32_t ct[2];
        RC5_encrypt(iv_block, ct);

        size_t block_len = (data_len - i < 8) ? data_len - i : 8;
        for (size_t j = 0; j < block_len; j++) {
            data[i + j] ^= ((uint8_t*)ct)[j];
        }

        memcpy(iv_block, &data[i], block_len);
    }
}

void generate_random_bytes(uint8_t *key, uint8_t *iv, FILE *file, size_t count) {
    RC5_key_schedule(key);

    uint32_t buffer[2];
    size_t generated = 0;

    while (generated < count) {
        RC5_encrypt((uint32_t*)iv, buffer);

        for (size_t i = 0; i < 2 && generated < count; i++) {
            fprintf(file, "%u\n", buffer[i]);
            generated++;
        }

        memcpy(iv, buffer, 8);
    }
}

int main() {
    uint8_t key[KEY_LENGTH_BYTES] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    uint8_t iv[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    size_t count = 10000;

    FILE *file = fopen("random_numbers.txt", "w");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    generate_random_bytes(key, iv, file, count);

    fclose(file);
    return 0;
}