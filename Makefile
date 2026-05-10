CXX = g++
CXXFLAGS = -std=c++23 -Wall -Werror -Wextra -DUNICODE -D_UNICODE 
LDFLAGS = -static -municode -mwindows  
LDLIBS = -ld2d1 -ldwrite

SRCDIR := src
BUILDDIR := build
TARGET := bin/snake_win32
SRCEXT := cpp

SRCS := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SRCS:.$(SRCEXT)=.o))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	@./$(TARGET)

clean:
	@$(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: run clean