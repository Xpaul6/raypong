CC := gcc
ECHO := echo

ifeq ($(OS), Windows_NT)
	CC ?= x86_64-w64-mingw32-gcc
	EXE := .exe
	RM := del /Q
	TXTEDIT := notepad
else
	EXE := 
	RM := rm -f
	TXTEDIT := less
endif

TARGET := main$(EXE)
SOURCE := main.c
ASM := main.s
FLAGS := -Wall -Wextra -std=c11 -lraylib  -o

ifeq ($(OS), Windows_NT)
	RUN := $(TARGET)
else
	RUN := ./$(TARGET)
endif

$(TARGET): $(SOURCE)
	$(CC) $(FLAGS) $(TARGET) $(SOURCE)

run: $(TARGET)
	$(RUN)

clean:
	$(RM) $(TARGET) $(ASM)

source: $(SOURCE)
	$(CC) -S $(SOURCE)
	$(TXTEDIT) $(ASM)

