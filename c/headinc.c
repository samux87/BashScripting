/* file: es76.c */
/* comando head */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	char c, *filename;
	int lines = 0, fdin = 0, fdout = 1, i = 0, n = 10;

	for (i = 1; i < argc - 1; i++) {
		/* controllo -n X */
		if (strcmp(argv[i], "-n") == 0) {
			n = atoi(argv[i + 1]);
			i++; /* mangia l'argomento */
			continue; /* prossimo */
		}
		if (strcmp(argv[i], "-f") == 0) {
			filename = (argv[i + 1]);
			fdin = open(filename, O_RDONLY);
			i++; /* mangia l'argomento */
			if (fdin < 0) {
				printf("error: %s non found.\n", filename);
				return(1);
			}
			continue; /* prossimo */
		}
		printf("error invalid option: %s\n", argv[i]);
		return(2);
	}

	while(read(fdin, &c, 1) > 0) {
		if (lines < n) {
			write(fdout, &c, 1);
		}
		if (c == '\n') {
			lines++;
		}
	}     
	return(0);
}
