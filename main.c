#include "sha-256.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void padding(unsigned char **message, uint64_t size) {
    printf("[FUNC]%s\n", __func__);

    int sizeInBytes = size / 8;
    // add 1 to seperate message from padding
    (*message)[sizeInBytes] = 0x80; // 0x80 (instead of 0x01) is important for 1000_000b represention
    sizeInBytes++;

    int zerosToAdd = MSG_LENGTH - (sizeInBytes % 64);

    if (zerosToAdd < 0) {
        // add extra 512bit block
        zerosToAdd += BLOCK_LENGTH;
        *message = (unsigned char *)realloc(*message, size + BLOCK_LENGTH); // extend size of pointer if an extra block is needed
        if (!(*message)) {
            exit(1);
        }
        printf("[INFO]BLOCK OVERFLOW\n");
    }

    // padding zeros
    for (int i = 0; i < zerosToAdd; i++) {
        (*message)[sizeInBytes + i] = 0x00;
    }

    // todo lastbyte is incorrect for 2 blocks
    int lastByte = sizeInBytes + zerosToAdd + 8;

    // add 64bit message length
    for (int i = 0; i < 8; i++) {
        (*message)[lastByte - 1 - i] = (unsigned char)(size >> (i * 8)) & 0xFF;
    }
}

void process_block_512(unsigned char *block, uint32_t *hashConstants, uint32_t *initalValues) {
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

    uint32_t words[BLOCK_LENGTH];

    // calculate 16 starter words
    for (int i = 0; i < 16; i++) {
        words[i] = (uint32_t)block[4 * i] << 24 | (uint32_t)block[4 * i + 1] << 16 |
            (uint32_t)block[4 * i + 2] << 8 | (uint32_t)block[4 * i + 3];
        printf("W[%2d] = %08x\n", i, words[i]);
    }

    // initalizing words
    for (int i = 16; i < 64; i++) {
        words[i] = sig_256_1(words[i - 2]) + words[i - 7] +
            sig_256_0(words[i - 15]) + words[i - 16];
    }

    // calculating the hash
    for (int t = 0; t < 64; t++) {
        uint32_t temp_1 =
            h + Sigma_256_1(e) + ch(e, f, g) + hashConstants[t] + words[t];
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
        // %08x\n",t,a,b,c,d,e,f,g,h);
    }
    initalValues[0] += a;
    initalValues[1] += b;
    initalValues[2] += c;
    initalValues[3] += d;
    initalValues[4] += e;
    initalValues[5] += f;
    initalValues[6] += g;
    initalValues[7] += h;
}

int hash_data(char *str) {
    // calculate constants
    uint32_t *constants = create_hash_constants(64, 32);
    uint32_t *initals = create_hash_inital_values(8, 32);

    uint64_t length = strlen(str) * 8;
    int a = ((length / 8) + 1 + 8) / 64;
    int b = ((length / 8) + 1 + 8) % 64;
    if (b > 0)
        a++;
    int amountOfBlocks = a;

    // alloc message
    unsigned char *msg =
        (unsigned char *)malloc(sizeof(unsigned char) * amountOfBlocks * 64);
    if (!msg) {
        return EXIT_FAILURE;
    }

    strcpy((char *)msg, str);

    // start hash computation
    padding(&msg, length);

    unsigned char *buf = (unsigned char *)malloc(64);

    // process each 512 bit block
    for (int i = 0; i < amountOfBlocks; i++) {
        memcpy(buf, msg + (i * 64), 64);
        process_block_512(buf, constants, initals);
        printf("sha256: %08x %08x %08x %08x %08x %08x %08x %08x\n", initals[0],
               initals[1], initals[2], initals[3], initals[4], initals[5],
               initals[6], initals[7]);
    }

    free(msg);
    free(constants);
    free(initals);
    return 0;
}

int main(int argc, char **argv) {

    if (2 == argc) {
        hash_data(argv[1]);
    }
    hash_data("abc");

    return EXIT_SUCCESS;
}
