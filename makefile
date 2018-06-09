# make P=nome

P = ring

all:
	mpicc $(P).c -o $(P).o
	mpiexec -np 4 ./$(P).o
install:
	sudo apt install openmpi-bin libopenmpi-dev
