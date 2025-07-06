#include <time.h>
#include <openssl/sha.h>
#include <string.h>
#include <stdio.h>

typedef struct Block {
    int index;
    time_t timestamp;
    char data[256];
    char prev_hash[65];
    char hash[65]; // using sha-256
    int nonce;  
    struct Block* next;
} Block;

void sha256(char *str, char outputBuffer[65]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)str, strlen(str), hash);
    // convert to hex and write to output buffer
    for (int i=0; i<SHA256_DIGEST_LENGTH; i++) {
        // move write pointer forward by 2 each iteration to 
        // place each hex pair in the right position
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
}

int main() {
    char *input = "hello world";
    char hash_output[65];

    sha256(input, hash_output);
    printf("Input: %s\n", input);
    printf("SHA-256: %s\n", hash_output);

    return 0;
}