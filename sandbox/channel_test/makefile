
F_OBJS = ./q_val/cqvalue.o ./q_val/dqvalu.o ./q_val/dmass.o

all: test.o hf_qvalue
	g++ -o test test.o $(F_OBJS)

test.o: test.cc
	g++ -c test.cc

hf_qvalue:
	make -C ./q_val/

clean:
	rm test *.o
