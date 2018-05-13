all: mypipeline

mypipeline: mypipeline.o line_parser.o
	gcc -Wall -o mypipeline mypipeline.o line_parser.o

mypipeline.o: mypipeline.c
	gcc -g -Wall -c -o mypipeline.o mypipeline.c

line_parser.o: line_parser.c line_parser.h
	gcc -g -Wall -c -o line_parser.o line_parser.c

.PHONY: clean

clean: 
	rm -f *.o mypipeline