#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "sha-256.h"

void print_arr_512(unsigned char *arr) {
    for (int i = 0; i < 64 * 2; i++) {
        for (int k = 0; k < 8; k++) {
            printf("%08b ", arr[i + k]);
        }
        printf("\n");
    }
}

uint32_t ch(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (~x & z);
}

uint32_t maj(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

uint32_t rotR(uint32_t num, uint32_t shiftBy) {
    return (num >> shiftBy) | (num << (32 - shiftBy));
}

uint32_t Sigma_256_0(uint32_t word) {
    return rotR(word, 2) ^ rotR(word, 13) ^ rotR(word, 22);
}

uint32_t Sigma_256_1(uint32_t word) {
    return rotR(word, 6) ^ rotR(word, 11) ^ rotR(word, 25);
}

uint32_t sig_256_0(uint32_t word) {
    return rotR(word, 7) ^ rotR(word, 18) ^ (word >> 3);
}

uint32_t sig_256_1(uint32_t word) {
    return rotR(word, 17) ^ rotR(word, 19) ^ (word >> 10);
}

uint32_t *generate_primes(int n) {
    if (n < 2)
        return NULL;
    uint32_t *primes = malloc(n * sizeof(int));
    if (!primes)
        return NULL;
    int found_primes = 0;
    int flag = 1;
    int check = 2;

    while (found_primes <= n) {
        flag = 1;
        for (int j = 2; j < check; j++) {
            if (check % j == 0 && check != j) {
                flag = 0;
                break;
            }
        }
        if (flag) {
            primes[found_primes] = check;
            found_primes++;
        }
        check++;
    }
    return primes;
}

unsigned long convert_frac_to_bin(long double num, int firstBits) {
    if (num < 0 || num >= 1) {
        printf("Error: Input must be in the range [0, 1).\n");
        return 0; // Handle invalid input
    }
    unsigned long bits = 0;
    for (int i = 0; i < firstBits; i++) {
        num *= 2.0;
        int bit = (int)num; // get first bit
        // printf("frac=%Lf bits=%b | %b\n", num,bits<<1,bit);
        bits = (bits << 1) | bit; // shift left and add bit
        num -= bit;
    }
    return bits;
}

uint32_t *create_hash_constants(int amount, int bitsOfFractional) {
    printf("[FUNC]%s\n", __func__);
    uint32_t *constants = malloc(sizeof(int) * amount);

    uint32_t *primes = generate_primes(amount);
    for (int i = 0; i < amount; i++) {
        long double cube = cbrtl(primes[i]);
        long double intpart;
        long double frac = modfl(cube, &intpart);
        unsigned int result = convert_frac_to_bin(frac, bitsOfFractional);
        constants[i] = result;
    }

    return constants;
}

uint32_t *create_hash_inital_values(int amount, int bitsOfFractional) {
    printf("[FUNC]%s\n", __func__);
    uint32_t *constants = malloc(sizeof(int) * amount);

    uint32_t *primes = generate_primes(amount);
    for (int i = 0; i < amount; i++) {
        long double square = sqrtl(primes[i]);
        long double intpart;
        long double frac = modfl(square, &intpart);
        unsigned int result = convert_frac_to_bin(frac, bitsOfFractional);
        constants[i] = result;
    }

    return constants;
}
