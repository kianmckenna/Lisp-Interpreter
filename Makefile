CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -g

TARGET = S_Expr_2
OBJS = main.o sexpression.o parser.o interpreter.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

main.o: main.cpp sexpression.h token.h interpreter.h parser.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

interpreter.o: interpreter.cpp interpreter.h  sexpression.h
	$(CXX) $(CXXFLAGS) -c interpreter.cpp -o interpreter.o

parser.o: parser.cpp parser.h token.h
	$(CXX) $(CXXFLAGS) -c parser.cpp -o parser.o

sexpression.o: sexpression.cpp sexpression.h
	$(CXX) $(CXXFLAGS) -c sexpression.cpp -o sexpression.o

clean:
	$(RM) $(OBJS) $(TARGET)