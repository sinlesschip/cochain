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
    }

 
int is_valid_block(Block *block, Block *prev_block) {
    char expected_hash[65];
    char input[1024];
    sprintf(input, "%d%ld%s%s%d", block->index, block->timestamp,
            block->data, block->prev_hash, block->nonce);
    sha256(input, expected_hash);
    
    return (strcmp(block->hash, expected_hash) == 0) &&
           (strcmp(block->prev_hash, prev_block->hash) == 0);
}

void add_block(Block **head, Block *new_block) {
    if (*head == NULL) {
        *head = new_block;
    } else {
        Block *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_block;
    }
    new_block->next = NULL;
}


int main() {
    Block *head = NULL;
    
    Block block;
    block.index = 1;
    block.timestamp = time(NULL);
    strcpy(block.data, "first block.");
    strcpy(block.prev_hash, "0000000000000000000000000000000000000000000000000000000000000000");
    block.nonce = 0;
    block.next = NULL;

    int difficulty = 5;
    
    // mine and add a couple blocks
    mine_block(&block, difficulty);
    add_block(&head, &block);
    printf("Found block: %s\nNonce: %d\n", block.hash, block.nonce);

    Block block2;
    block2.index = 2;
    block2.timestamp = time(NULL);
    strcpy(block2.data, "second block.");
    strcpy(block2.prev_hash, block.hash);
    block2.nonce = 0;
    mine_block(&block2, difficulty);  
    add_block(&head, &block2);
    printf("Found block: %s\nNonce: %d\n", block2.hash, block2.nonce);

    // print the blockchain
    Block *current = head;
    while (current != NULL) {
        printf("Block %d: %s\n", current->index, current->hash);
        current = current->next;
    }
    return 0;
}