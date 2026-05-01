CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -Werror -DUNICODE -D_UNICODE -masm=intel
LDFLAGS = -static -municode -mwindows

SRCDIR := src
BUILDDIR := build
TARGET := bin/snake_win32
SRCEXT := cpp

SRCS := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SRCS:.$(SRCEXT)=.o))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@$(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean