#uso: comando make nella cartella dove è contenuto questo file
CC=gcc
CFLAGS=-O2 -w
BINS= parteC

all: $(BINS)
	

clean:
	rm -rf *.o $(BINS)


%: %.c
	$(CC) $< $(CFLAGS) -o $@
