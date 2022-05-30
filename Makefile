# Compiler settings
CC = clang
MPICC = mpicc
PERFFLAGS = -std=gnu11 -O0 -Werror -lprofiler -pthread
TESTFLAGS = -std=gnu11 -O0 -Werror -pthread
MPIFLAG = -pthread
LDFLAGS =

# Makefile settings
APPNAME = distributed efficient
EXT = .c
SRCDIR = src
OBJDIR = obj

SRC = src/distributed.c src/efficient.c
OBJ = obj/distributed.o obj/efficient.o

RM = rm

# Targets
all: CFLAG=PERFFLAGS

# Test
tests: CFLAG=TESTFLAGS

all tests: distributed efficient

distributed: obj/distributed.o

efficient: obj/efficient.o

obj/distributed.o: src/distributed.c
	$(MPICC) $(MPIFLAG) -o $@ $<

obj/efficient.o: src/efficient.c
	$(CC) $(TESTFLAGS) -o $@ $<

# Building rule for .o files and its .c/.cpp in combination with all .h
#$(OBJDIR)/%.o: $(SRCDIR)/%$(EXT)
#	$(CC) -D$(CFLAG) -o $@ $<

# Cleaning rules
.PHONY: clean
clean:
	$(RM) $(OBJ)


