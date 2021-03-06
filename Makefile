CC = g++
CFLAGS = -std=c++11 -c -g -Og -Wall -Werror -pedantic  
OBJ = main.o
DEPS = main.cpp
LIBS = -lboost_regex -lboost_date_time
EXE = ps6

all: $(OBJ)
	$(CC) $(OBJ) -o $(EXE) $(LIBS)

%.o: %.cpp %.h
	$(CC) $(CFLAGS)  -c $< -o $@

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm $(OBJ) $(EXE)
