#ifndef GNUPG_H_
#define GNUPG_H_

#define gnupg_path "/home/msharma/build/gnupg-1.4.13/g10/gpg"

#define base_add 0x00000000

#define square_add 0x0000c600e		//mpih-mul.c:270 (First cache line in mpih_sqr_n())

#define remainder_add 0x0000c510c	//mpih-div.c:329 (Loop in default case in mpihelp_divrem())

#define multiply_add 0x0000c5589	//mpih-mul.c:121 (First cache line of mul_n())

#endif
