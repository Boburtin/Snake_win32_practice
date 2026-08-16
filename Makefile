SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
TARGET := win32_snake.exe

ifeq ($(origin COMSPEC), undefined)
	DETECTED_SHELL := CMD
else
	DETECTED_SHELL := BASH
endif

ifeq ($(DETECTED_SHELL), BASH)
	MD := mkdir -p
	RM := rm -f
	CXX := g++
	CXXFLAGS := -std=c++23 -Werror -Wall -Wextra -DUNICODE -D_UNICODE -DWIN32_LEAN_AND_MEAN
	LDFLAGS := -municode -mwindows
	LDLIBS := -ld2d1 -ldwrite
	FIX_PATH = $1
	OBJ_EXT := o
else
	RM := del /q /f
	MD := mkdir
	CXX := cl
	CXXFLAGS := /std:c++latest /W4 /WX /EHsc /DUNICODE /D_UNICODE /DWIN32_LEAN_AND_MEAN
	LDFLAGS := /SUBSYSTEM:WINDOWS
	LDLIBS := d2d1.lib dwrite.lib user32.lib ole32.lib
	FIX_PATH = $(subst /,\,$1)
	OBJ_EXT := obj
endif

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.$(OBJ_EXT), $(SRCS))

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJS) | $(BIN_DIR)
ifeq ($(DETECTED_SHELL),BASH)
		$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)
else
		$(CXX) $^ /Fe:$@ /link $(LDFLAGS) $(LDLIBS)
endif

$(OBJ_DIR)/%.$(OBJ_EXT): $(SRC_DIR)/%.cpp | $(OBJ_DIR)
ifeq ($(DETECTED_SHELL),BASH)
		$(CXX) $(CXXFLAGS) -c $< -o $@
else
		$(CXX) /c $(CXXFLAGS) $^ /Fo"$(OBJ_DIR)\\"
endif

$(BIN_DIR) $(OBJ_DIR):
	$(MD) $(call FIX_PATH,$@)

info:
	@echo "Detected Shell: $(DETECTED_SHELL)"
	@echo "Executable Output: $(BIN_DIR)/$(TARGET)"
	@echo $(SHELL)

clean:
	@$(RM) $(call FIX_PATH,$(OBJ_DIR)/*.$(OBJ_EXT))
	@$(RM) $(call FIX_PATH,$(BIN_DIR)/$(TARGET))

.PHONY: run test clean all


