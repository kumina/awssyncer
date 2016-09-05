CXXFLAGS=-O3 -Wall -Werror -std=c++11

all: awssyncer

OBJS=filesystem_dirt.o

awssyncer: ${OBJS}

clean:
	rm -f *.o
