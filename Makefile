build:
	gcc main.c -o main
	mpicc main_mpi.c -o main_mpi

run:
	time ./main img/image-06.pnm img/out.pnm width 500

run_mpi:
	time mpirun -np 4 ./main_mpi img/image-06.pnm img/out.pnm width 500

clean:
	rm main main_mpi
