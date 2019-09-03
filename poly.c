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
    // we want a pointer to the first byte of the main method such that we can iterate over it
    uint8_t* code = (uint8_t*) &main;
    long pagesize = sysconf(_SC_PAGESIZE);

    if (pagesize <= 0)
	return 1;

    size_t mask = pagesize - 1;
    // the code needs to be aligned to the start of a page in order for mprotect to work
    void* alignedcode = (void*) ((size_t) code & ~mask);

    // luckily instead of realigning code, we just reach back a little and it generally turns out to be fine
    if (mprotect(alignedcode, (size_t) code - (size_t) alignedcode + END, PROT_READ | PROT_WRITE | PROT_EXEC))
        return 1;

    // simple xor cipher
    // this block can be replaced with a more complicated cipher if desired, as long as the nop slide stays intact
    unsigned char key[] = { 0x69, 0x13, 0x37, 0xd1, 0x9a, 0x33, 0x42 };
    size_t keylen = 7;
    for (unsigned long i = OFFSET; i < END; i++) {
	*(code + i) ^= key[i % keylen];
    }

    asm("nop;nop;nop");

    // your code here

    // this just prints out the first 30 Fibonacci numbers
    unsigned long a = 0, b = 1, temp;
    for (int i = 0; i < 30; i++) {
	temp = b;
	b += a;
	a = temp;
	printf("%lu\n", b);
    }

    encrypted();

    // always return properly
    return 0;
}

void encrypted() {
    printf("This code is encrypted\n");
}

