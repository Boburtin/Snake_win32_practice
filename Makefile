CXX = g++
CXXFLAGS = -std=c++23 -Wall -Werror -DUNICODE -D_UNICODE
LDFLAGS = -static -municode -mwindows

SRCS = snake.cpp
OBJS = ${SRCS:.cpp=.o}
TARGET = snake.exe

.PHONY: all clean install test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cpp	
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)