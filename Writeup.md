# SHA-256 Implementation
[source](https://csrc.nist.gov/files/pubs/fips/180-2/final/docs/fips180-2.pdf)
## Constants
These words were obtained by taking the first thirty-two bits of the fractional parts of the cube
roots of the first sixty-four prime numbers.
### Initial hash value
These words were obtained by taking the first thirty-two bits of the fractional parts of the square
roots of the first eight prime numbers.
## Preprocessing
### Padding
Padding is achieved by putting 1 after the messagege after that padding zeros until a multiple of 512 is achieved.
However the last 64 bits are there to store length of the entire message.
### Parsing
The message is parsed into N 512 bit blocks to process further
## Calculating
1. Message schedule of 32bit words
2. 8 working variables of each 32bit 
3. An array of 64 is precalculated
4. Next up a cycle of 64 is calculating the working varbles for each cycle
5. 8 32bit words are then the final result of 256bit

