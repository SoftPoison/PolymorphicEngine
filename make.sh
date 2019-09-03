#!/bin/sh

# dummy values so the compiler doesn't optimise the loops
gcc -o poly_raw poly.c -DOFFSET=100 -DEND=200 -Wall

# calculation of positions in the file
hexstart=$(nm -S poly_raw | grep 'T main' | gawk '{printf "0x%s", $1}') # the start of main
hexoffset=$(objdump --disassemble=main poly_raw | grep 'nop' | head -2 | tail -1 | gawk '{printf "%s", "0x" substr($1, 1, length($1)-1)}') # start of the nop slide
hexend=$(nm -S poly_raw | grep 'T __libc_csu_init' | gawk '{printf "0x%s", $1}') # the libc_csu_init method that is placed right after all user define methods

# conversion of hex to dec, and then turned into relative offsets
startpos=$(printf "%d" $hexstart)
offset=$(($(printf "%d" $hexoffset)-$startpos))
breakpoint=$(($offset-1))
end=$(($(printf "%d" $hexend)-$startpos))

binnopoffset=$(./findnop poly_raw)

# compile the program with the correct offsets
gcc -o poly_raw poly.c -DOFFSET=$offset -DEND=$end -Wall

# write gdb config
printf "set confirm off\nbreak *main+${breakpoint}\nr\ndump binary memory mem.bin *main+${offset} *main+${end}\nq\n" > gdb.cfg

# use gdb to run the program until the program's memory is encrypted, dumping the memory to mem.bin when done
gdb -x gdb.cfg poly_raw

# merge the memory blob with the program
./merge poly_raw mem.bin $binnopoffset poly
chmod 755 poly

#clean up
rm -f poly_raw mem.bin gdb.cfg

exit 0
