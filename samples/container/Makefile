CC=g++
CFLAGS=-Wall -I/opt/boost_1_47_0/ -I../..
LFLAGS=-L/opt/boost_1_47_0/installation/
LIBS=-lboost_thread
DEPS = token.h 
OBJ = componenta.o  componentb.o  console.o  interpreter.o  main.o
EXE=container_sample

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LFLAGS) $(LIBS)

clean:
	rm -f *.o *~ core $(EXE)
