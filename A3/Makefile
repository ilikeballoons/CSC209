# Makefile for programs to index and search an index.

FLAGS = -Wall -g -std=gnu99
SRC = freq_list.c punc.c
HDR = freq_list.h worker.h
OBJ = freq_list.o punc.o

all : indexer queryone printindex

indexer : indexer.o ${OBJ}
	gcc ${FLAGS} -o $@ indexer.o ${OBJ}

printindex : printindex.o ${OBJ}
	gcc ${FLAGS} -o $@ printindex.o ${OBJ}

queryone : queryone.o worker.o ${OBJ}
	gcc ${FLAGS} -o $@ queryone.o worker.o ${OBJ}

testworker : test_worker.o worker.o ${OBJ}
	gcc ${FLAGS} -o test_worker test_worker.o worker.o ${OBJ}

run_test : testworker
	./test_worker -i ./index -n ./filenames -q ${Q}

# Separately compile each C file
%.o : %.c ${HDR}
	gcc ${FLAGS} -c $<

clean :
	-rm *.o indexer queryone printindex test_worker
