build:
	gcc main.c -o main

run:
	./main img/image-02.pgm out.pgm

clean:
	rm main *.pnm *.pgm
