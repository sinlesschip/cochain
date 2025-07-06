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

void mine_block(Block *block, int difficulty) {
    char prefix[65];
    memset(prefix, '0', difficulty);
    prefix[difficulty] = '\0';

    do {
        block->nonce++;
        char input[1024];
        sprintf(input, "%d%ld%s%s%d", block->index, block->timestamp,
                block->data, block->prev_hash, block->nonce);
        sha256(input, block->hash);
        } 
        while (strncmp(block->hash, prefix, difficulty) != 0);
        printf("Found block: %s\n", block->hash);
    }

    

int main() {
    Block block;

    block.index = 1;
    block.timestamp = time(NULL);
    strcpy(block.data, "first block.");
    strcpy(block.prev_hash, "0000000000000000000000000000000000000000000000000000000000000000");
    block.nonce = 0;
    block.next = NULL;

    int difficulty = 5;

    mine_block(&block, difficulty);

    return 0;
}