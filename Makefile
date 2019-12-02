build:
	gcc main.c -o main
	mpicc main_mpi.c -o main_mpi
	gcc main_pthread.c -o main_pthread -lpthread  
	gcc main_omp.c -o main_omp -fopenmp
	gcc main_hibrid.c -o main_hibrid -lpthread -fopenmp

run:
	time ./main img/image-06.pnm img/out.pnm width 500

run_mpi:
	time mpirun -np 4 ./main_mpi img/image-06.pnm img/out1.pnm width 500

run_pthread:
	time ./main_pthread img/image-06.pnm img/out2.pnm width 500

run_omp:
	time ./main_omp img/image-06.pnm img/out3.pnm width 500

run_hibrid:
	time ./main_hibrid img/image06.pnm img/out4.pnm width 500

clean:
	rm main main_mpi main_pthread main_omp main_hibrid
