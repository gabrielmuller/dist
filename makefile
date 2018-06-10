# make P=nome

P = ring
#DEBUG=-DDEBUG

all:
	# Se não houver slots suficientes, use 'make oversubscribe'.
	mpicc $(P).c -o $(P).o
	mpiexec $(DEBUG) -np 4 ./$(P).o
install:
	sudo apt install openmpi-bin libopenmpi-dev
oversubscribe:
	mpicc $(DEBUG) $(P).c -o $(P).o
	mpiexec --map-by socket:OVERSUBSCRIBE -np 4 ./$(P).o 


