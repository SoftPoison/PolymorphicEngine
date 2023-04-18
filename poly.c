#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>

// a function that will get encrypted that we will later define
void encrypted();

/*
 * All code after the first nop is encrypted, including functions defined after <main>
 */
int main() {
    // generate random encryption keys and initialization vectors
    unsigned char key1[32], iv1[16], key2[32], iv2[16];
    if (RAND_bytes(key1, sizeof(key1)) != 1 || RAND_bytes(iv1, sizeof(iv1)) != 1
        || RAND_bytes(key2, sizeof(key2)) != 1 || RAND_bytes(iv2, sizeof(iv2)) != 1)
        return 1;

    // get pointer to first byte of main function
    uint8_t* code = (uint8_t*) &main;

    // calculate page size and mask for page alignment
    long pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize <= 0)
        return 1;
    size_t mask = pagesize - 1;

    // align code to page boundary
    void* alignedcode = (void*) ((size_t) code & ~mask);

    // make code writable
    if (mprotect(alignedcode, (size_t) code - (size_t) alignedcode + END, PROT_READ | PROT_WRITE | PROT_EXEC))
        return 1;

    // encrypt first code section using key1 and iv1
    AES_KEY aes_key1;
    if (AES_set_encrypt_key(key1, 256, &aes_key1) < 0)
        return 1;
    for (unsigned long i = OFFSET; i < END/2; i += AES_BLOCK_SIZE) {
        unsigned char ctr[AES_BLOCK_SIZE];
        AES_encrypt(iv1, ctr, &aes_key1);
        for (unsigned int j = 0; j < AES_BLOCK_SIZE && i + j < END/2; j++) {
            *(code + i + j) ^= ctr[j];
        }
        iv1[0]++;
    }

    // insert junk code between first and second code sections
    asm("push %rax; pop %rax; push %rax; pop %rax; push %rax; pop %rax; push %rax; pop %rax");

    // encrypt second code section using key2 and iv2
    AES_KEY aes_key2;
    if (AES_set_encrypt_key(key2, 256, &aes_key2) < 0)
        return 1;
    for (unsigned long i = END/2; i < END; i += AES_BLOCK_SIZE) {
        unsigned char ctr[AES_BLOCK_SIZE];
        AES_encrypt(iv2, ctr, &aes_key2);
        for (unsigned int j = 0; j < AES_BLOCK_SIZE && i + j < END; j++) {
            *(code + i + j) ^= ctr[j];
        }
        iv2[0]++;
    }

    encrypted();

    // make code read-only and executable
    if (mprotect(alignedcode, (size_t) code - (size_t) alignedcode + END, PROT_READ | PROT_EXEC))
        return 1;

    return 0;
}

void encrypted() {
    printf("This code is encrypted\n");
}

