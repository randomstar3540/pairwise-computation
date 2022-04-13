# Compiler settings
CC = clang
PERFFLAGS = -std=gnu11 -O0 -Werror -lprofiler -pthread
TESTFLAGS = -std=gnu11 -g -O0 -Werror --coverage -lprofiler -fsanitize=address -pthread
LDFLAGS =

# Makefile settings
APPNAME = baseline efficient
EXT = .c
SRCDIR = src
OBJDIR = obj

SRC = src/baseline.c src/efficient.c
OBJ = obj/baseline.o obj/efficient.o

RM = rm

# Targets
all: CFLAG=PERFFLAGS

# Test
tests: CFLAG=TESTFLAGS

all tests: baseline efficient

baseline: obj/baseline.o

efficient: obj/efficient.o

obj/baseline.o: src/baseline.c
	$(CC) $(PERFFLAGS) -o $@ $<

obj/efficient.o: src/efficient.c
	$(CC) $(PERFFLAGS) -o $@ $<

# Building rule for .o files and its .c/.cpp in combination with all .h
#$(OBJDIR)/%.o: $(SRCDIR)/%$(EXT)
#	$(CC) -D$(CFLAG) -o $@ $<

# Cleaning rules
.PHONY: clean
clean:
	$(RM) $(OBJ)


