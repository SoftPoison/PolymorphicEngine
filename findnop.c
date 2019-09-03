#include <stdio.h>
#include <stdint.h>

/*
 * This code calculates how many bytes into the file the
 * second to last nop in the nop slide occurs
 */
int main(int argc, char** argv) {
    if (argc != 2) {
	fprintf(stderr, "Usage: %s <file>\n", argv[0]);
	return 1;
    }

    FILE* fp = fopen(argv[1], "rb");
    uint8_t buf[10] = { 0 };
 
    fseek(fp, 0l, SEEK_END);
    size_t fsize = ftell(fp);
    rewind(fp);

    size_t nopcount = 0;
    for (size_t i = 0; i < fsize; i++) {
	fread(buf, 1, 1, fp);
	if (*buf == 0x90)
	    nopcount++;
	else
	    nopcount = 0;

	if (nopcount == 3) {
	    printf("%lu", i - 1);
	    fclose(fp);
	    return 0;
	}
    }

    fclose(fp);

    // no nop slide found :(
    return 2;
}
