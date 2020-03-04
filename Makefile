TARGET := dzxc

CC := arm-linux-gcc

INCLUDE := -Iinclude
INCLUDE += -I/home/gec/arm-libs/include

LIBDIR := -Llib/

LIB := -ljpeg
LIB += -lpthread

CSRCS:=$(wildcard ./*.c)
CSRCS+=$(wildcard ./display/*.c)
CSRCS+=$(wildcard ./input/*.c)
CSRCS+=$(wildcard ./dir/*.c)
CSRCS+=$(wildcard ./list/*.c)
CSRCS+=$(wildcard ./show_pic/*.c)
CSRCS+=$(wildcard ./music/*.c)
CSRCS+=$(wildcard ./runing/*.c)

DEST:=$(patsubst %.c,%.o,$(CSRCS))

$(TARGET):$(DEST)
	$(CC) $^ -o $@ $(LIBDIR) $(LIB)
	
%.o:%.c
	$(CC) -c -o $@ $< $(INCLUDE)

clean:
	rm $(DEST) $(TARGET) 
