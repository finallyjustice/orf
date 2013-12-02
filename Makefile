CC = gcc

FLAGS = -g -02

all: orf count partition reverse

orf: orf.o
	${CC} -o orf orf.o
orf.o: orf.c
	${CC} -c orf.c

count: count.o
	${CC} -o count count.o
count.o: count.c
	${CC} -c count.c

partition: partition.o
	${CC} -o partition partition.o
partition.o: partition.c
	${CC} -c partition.c

reverse: reverse.o
	${CC} -o reverse reverse.o
reverse.o: reverse.c
	${CC} -c reverse.c

clean:
	rm orf.o orf count.o count partition partition.o reverse reverse.o
