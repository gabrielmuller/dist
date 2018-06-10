# make P=nome

P = ring
#DEBUG=-DDEBUG

all:
	mpicc $(P).c -o $(P).o
	mpiexec -np 4 ./$(P).o
install:
	sudo apt install openmpi-bin libopenmpi-dev
oversubscribe:
	# Quando não houver slots suficientes
	mpicc $(DEBUG) $(P).c -o $(P).o
	mpirun --map-by socket:OVERSUBSCRIBE -np 5 ./$(P).o 


