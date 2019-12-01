build:
	gcc main.c -o main
	mpicc main_mpi.c -o main_mpi

run:
	time ./main img/image-05.pnm img/out.pnm width 150

run_mpi:
	time mpirun -np 4 ./main_mpi img/image-05.pnm img/out.pnm width 150

clean:
	rm main 
