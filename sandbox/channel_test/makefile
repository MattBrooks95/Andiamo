

all: test.o dqv.o
	g++ -o test test.o dqv.o

dqv.o: dqv.f
	gfortran -c -ffree-form dqv.f

clean:
	rm test *.o
