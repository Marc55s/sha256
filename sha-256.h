#include <stdint.h>

#define MSG_LENGTH 56
#define BLOCK_LENGTH 512

void print_arr_512(unsigned char *arr);
uint32_t ch(uint32_t x, uint32_t y, uint32_t z);
uint32_t maj(uint32_t x, uint32_t y, uint32_t z);
uint32_t rotR(uint32_t num, uint32_t shiftBy);
uint32_t Sigma_256_0(uint32_t word);
uint32_t Sigma_256_1(uint32_t word);
uint32_t sig_256_0(uint32_t word);
uint32_t sig_256_1(uint32_t word);
uint32_t *generate_primes(int n);
uint32_t *create_hash_constants(int amount, int bitsOfFractional);
uint32_t *create_hash_inital_values(int amount, int bitsOfFractional);
unsigned long convert_frac_to_bin(long double num, int firstBits);
