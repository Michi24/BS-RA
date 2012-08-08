CC = gcc

BUILDDIR = .
LIBDIR = lib
LDFLAGS = -L$(LIBDIR)

OBJDIR = obj
SRCDIR = src
BINDIR = bin
INCLUDEDIR = include
INCDIR = -I$(INCLUDEDIR)

CFLAGS = -Wall -Wextra -pedantic -std=c99 -g

SRCS = $(wildcard $(BUILDDIR)/$(SRCDIR)/*.c)
OBJS += $(addprefix $(BUILDDIR)/$(OBJDIR)/, $(notdir $(SRCS:.c=.o)))


$(OBJDIR)/%.o:  $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $< $(INCDIR) -D_GNU_SOURCE 

all: $(OBJS)
	$(CC) -o $(BINDIR)/simulator $(OBJDIR)/simulator.o

simulator: $(OBJDIR)/simulator.o
	$(CC) -o $(BINDIR)/simulator $(OBJDIR)/simulator.o

clean:
	rm -f $(OBJDIR)/*.o
	#rm -f $(LIBDIR)/*
	rm -f $(BINDIR)/*