#ifndef GNUPG_H_
#define GNUPG_H_

#define gnupg_path "/home/vinayakt/Desktop/7thSem/SecureMemorySystems/assgn1/Task-1a/gnupg-1.4.13/g10/gpg"

#define base_add 0x00000000

#define square_add 0x0000c58c4		//mpih-mul.c:270 (First cache line in mpih_sqr_n())

#define remainder_add 0x0000c5122	//mpih-div.c:329 (Loop in default case in mpihelp_divrem())

#define multiply_add 0x0000c5245	//mpih-mul.c:121 (First cache line of mul_n())

#endif
