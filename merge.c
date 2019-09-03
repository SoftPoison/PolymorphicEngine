#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
 * Here we merge a memory dump with a file
 * Pretty self explanatory
 */
int main(int argc, char** argv) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <bin_file> <mem_file> <offset> <out_file>\n", argv[0]);
        return 1;
    }

    FILE* bin_file = fopen(argv[1], "rb");
    FILE* mem_file = fopen(argv[2], "rb");

    fseek(bin_file, 0l, SEEK_END);
    size_t bin_size = ftell(bin_file);
    rewind(bin_file);

    fseek(mem_file, 0l, SEEK_END);
    size_t mem_size = ftell(mem_file);
    rewind(mem_file);

    unsigned long offset = strtoul(argv[3], NULL, 0);

    uint8_t* bin_data = (uint8_t*) calloc(bin_size, sizeof(uint8_t));
    fread(bin_data, sizeof(uint8_t), bin_size, bin_file);

    // overwrite the file with the memory
    fread(bin_data + offset, sizeof(uint8_t), mem_size, mem_file);

    fclose(bin_file);
    fclose(mem_file);

    FILE* out_file = fopen(argv[4], "wb");
    fwrite(bin_data, sizeof(uint8_t), bin_size, out_file);

    fclose(out_file);
    free(bin_data);

    return 0;
}
