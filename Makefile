IDIR =include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=src/obj
LDIR =lib

LIBS=-lm

_DEPS = config_params.h options.h suntime.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = nightwatch.o config_params.o options.o suntime.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

VPATH=src

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

nightwatch: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
