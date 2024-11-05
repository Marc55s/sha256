#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

void hash_computation(unsigned char *block, uint32_t *hashConstants,
                      uint32_t *initalValues);

uint32_t hash_constants[] = {
    // data from paper
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

void print_arr_512(unsigned char *arr) {
    for (int i = 0; i < 64 * 2; i++) {
        for (int k = 0; k < 8; k++) {
            printf("%08b ", arr[i + k]);
        }
        printf("\n");
    }
}

uint32_t ch(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (~x & z); }

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
    printf("%s\n", __func__);
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
    printf("%s\n", __func__);
    uint32_t *constants = malloc(sizeof(int) * amount);

    uint32_t *primes = generate_primes(amount);
    for (int i = 0; i < amount; i++) {
        long double cube = sqrtl(primes[i]);
        long double intpart;
        long double frac = modfl(cube, &intpart);
        unsigned int result = convert_frac_to_bin(frac, bitsOfFractional);
        constants[i] = result;
    }

    return constants;
}

void padding(unsigned char **message, uint64_t size) {
    printf("[FUNC]%s\n", __func__);

    int sizeInBytes = size / 8;
    // add 1 to seperate message from padding
    (*message)[sizeInBytes] = 0x80; // 0x80 (instead of 0x01) is important for 1000_000b represention
    sizeInBytes++;


    int zerosToAdd = 56 - (sizeInBytes % 64);
    if (zerosToAdd < 0) {
        zerosToAdd += 64; 
        *message = (unsigned char *)realloc(*message, size + 64);
        if(!(*message)){
            exit(1);
        }
        printf("BLOCK OVERFLOW\n");
    }
    // padding zeros
    for (int i = 0; i < zerosToAdd; i++) {
        (*message)[sizeInBytes + i] = 0x00;
    }

    //todo lastbyte is incorrect for 2 blocks
    int lastByte = sizeInBytes + zerosToAdd + 8;

    printf("lastByte %d sizeInBytes %d, zerosToAdd %d\n",lastByte, sizeInBytes, zerosToAdd);

    // add 64bit message length
    for (int i = 0; i < 8; i++) {
        (*message)[lastByte - 1 - i] = (unsigned char)(size >> (i * 8)) & 0xFF;
    }
}

void hash_block_512(unsigned char *block, uint32_t *hashConstants, uint32_t *initalValues) {
    printf("[FUNC]%s\n", __func__);
    // init hashvalues
    uint32_t a, b, c, d, e, f, g, h;
    a = initalValues[0];
    b = initalValues[1];
    c = initalValues[2];
    d = initalValues[3];
    e = initalValues[4];
    f = initalValues[5];
    g = initalValues[6];
    h = initalValues[7];

    // 1. MESSAGE SCHEDULE
    uint32_t words[64];

    // calculate 16 starter words
    for (int i = 0; i < 16; i++) {
        words[i] = (uint32_t)block[4 * i] << 24 | (uint32_t)block[4 * i + 1] << 16 |
            (uint32_t)block[4 * i + 2] << 8 | (uint32_t)block[4 * i + 3];
        printf("W[%2d] = %08x\n", i, words[i]);
    }

    for (int i = 16; i < 64; i++) {
        words[i] = sig_256_1(words[i - 2]) + words[i - 7] +
            sig_256_0(words[i - 15]) + words[i - 16];
    }

    for (int t = 0; t < 64; t++) {
        uint32_t temp_1 =
            h + Sigma_256_1(e) + ch(e, f, g) + hash_constants[t] + words[t];
        uint32_t temp_2 = Sigma_256_0(a) + maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + temp_1;
        d = c;
        c = b;
        b = a;
        a = temp_1 + temp_2;

        // printf("t = %2d: %08x %08x %08x %08x %08x %08x %08x
        // %08x\n",t,a,b,c,d,e,f,g,h); printf("t = : %08x %08x %08x %08x %08x %08x
        // %08x %08x\n",a,b,c,d,e,f,g,h);
    }
    //printf("first hash part %x + %x = %x\n", initalValues[0], a,initalValues[0] + a);
    initalValues[0] += a;
    initalValues[1] += b;
    initalValues[2] += c;
    initalValues[3] += d;
    initalValues[4] += e;
    initalValues[5] += f;
    initalValues[6] += g;
    initalValues[7] += h;
}

void hash_data(){

}

int main() {
    // calculate constants
    uint32_t *constants = create_hash_constants(64, 32);
    uint32_t *initals = create_hash_inital_values(8, 32);


    //char *str = "abc";
    //char *str = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    char str[1000000];
    for (int i = 0; i < 1000000; i++) {
       str[i] = 'a';
    }

    uint64_t length = strlen(str) * 8;
    int a = ((length/8) + 1 + 8) / 64;
    int b = ((length/8) + 1 + 8) % 64;
    if(b > 0) a++;
    int amountOfBlocks = a;

    // alloc message
    unsigned char *msg = (unsigned char *)malloc(sizeof(unsigned char) * amountOfBlocks * 64);
    if (!msg) {
        return EXIT_FAILURE;
    }

    strcpy((char *)msg, str);

    printf("bytes of input string: %ld\n", length / 8);


    // start hash computation
    padding(&msg, length);

    unsigned char* buf = (unsigned char*) malloc(64);

    for (int i = 0; i < amountOfBlocks; i++) {
        memcpy(buf,msg+(i*64),64);
        hash_block_512(buf, constants, initals);
        printf("Hex values: %08x %08x %08x %08x %08x %08x %08x %08x\n", initals[0],
               initals[1], initals[2], initals[3], initals[4], initals[5], initals[6],
               initals[7]);
    }

    free(msg);
    free(constants);
    free(initals);
    return EXIT_SUCCESS;
}
