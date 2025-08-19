#include <time.h>
#include <openssl/sha.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
    
    if (strcmp(block->hash, expected_hash) != 0) {
        printf("Invalid block %d: Hash mismatch\n", block->index);
        printf("Expected: %s\n", expected_hash);
        printf("Got: %s\n", block->hash);
        return 0;
    }
    
    if (strcmp(block->prev_hash, prev_block->hash) != 0) {
        printf("Invalid block %d: Previous hash mismatch\n", block->index);
        printf("Expected: %s\n", prev_block->hash);
        printf("Got: %s\n", block->prev_hash);
        return 0;
    }
    
    return 1;
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

// mine n blocks and add them to the blockchain
void mine_and_add_blocks(Block **head, int n, int difficulty) {
    for (int i = 0; i < n; i++) {
        Block *new_block = malloc(sizeof(Block));
        new_block->index = i + 1;
        new_block->timestamp = time(NULL);
        sprintf(new_block->data, "Block %d data", i + 1);
        
        if (*head == NULL) {
            strcpy(new_block->prev_hash, "0000000000000000000000000000000000000000000000000000000000000000");
        } else {
            // Find the last block to get its hash
            Block *current = *head;
            while (current->next != NULL) {
                current = current->next;
            }
            strcpy(new_block->prev_hash, current->hash);
        }
        
        new_block->nonce = 0;
        mine_block(new_block, difficulty);
        printf("Found block: %s\nNonce: %d\n", new_block->hash, new_block->nonce);
        add_block(head, new_block);
    }
}

int is_valid_chain(Block *head) {
    if (head == NULL) return 1;
    
    // Validate genesis block's previous hash
    if (strcmp(head->prev_hash, "0000000000000000000000000000000000000000000000000000000000000000") != 0) {
        printf("Invalid genesis block: Incorrect previous hash\n");
        return 0;
    }

    Block *current = head;
    while (current->next != NULL) {
        if (!is_valid_block(current->next, current)) {
            return 0;
        }
        current = current->next;
    }
    return 1;
}


int main() {
    Block *head = NULL;
    
    int difficulty = 5;
    int n = 5;
    mine_and_add_blocks(&head, n, difficulty);
    printf("Blockchain after mining %d blocks:\n", n);
    
    Block *current = head;
    while (current != NULL) {
        printf("Block %d: %s\n", current->index, current->hash);
        current = current->next;
    }
    
    if (is_valid_chain(head)) {
        printf("Blockchain is valid.\n");
    } else {
        printf("Blockchain is invalid.\n");
    }

    // free the blockchain
    current = head;
    while (current != NULL) {
        Block *temp = current;
        current = current->next;
        free(temp);
    }
    return 0;
}