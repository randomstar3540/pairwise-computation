# Compiler settings
CC = gcc
PERFFLAGS = -std=gnu11 -O0 -Werror -fsanitize=address -pthread
TESTFLAGS = -std=gnu11 -g -O0 -Werror --coverage -fsanitize=address -pthread
LDFLAGS =

# Makefile settings
APPNAME = baseline efficient
EXT = .c
SRCDIR = src
OBJDIR = obj

SRC = $(wildcard $(SRCDIR)/*$(EXT))
OBJ = $(SRC:$(SRCDIR)/%$(EXT)=$(OBJDIR)/%.o)

RM = rm
DELOBJ = $(OBJ)

# Targets
all: CFLAG=PERFFLAGS

# Test
tests: CFLAG=TESTFLAGS

all tests: $(OBJ)

# Building rule for .o files and its .c/.cpp in combination with all .h
$(OBJDIR)/%.o: $(SRCDIR)/%$(EXT)
	$(CC) -D$(CFLAG) -o $@ $<

# Cleaning rules
.PHONY: clean
clean:
	$(RM) $(DELOBJ)


