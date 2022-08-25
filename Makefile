CC=gcc
CFLAGS=-O3 -DNDEBUG -g -Wall
PROJNAME=yallic

SRC=src
SRCS=$(SRC)/$(PROJNAME).c

OBJ=obj
OBJS=$(OBJ)/$(PROJNAME).o

LIB=lib
SLIB=$(LIB)/lib$(PROJNAME).a
DLIB=$(LIB)/lib$(PROJNAME).so

.PHONY: all clean slib dlib
.DEFAULT_GOAL = all


clean:
	if [ -d $(LIB) ]; then rm -r $(LIB); fi
	if [ -d $(OBJ) ]; then rm -r $(OBJ); fi


all: slib dlib
slib: $(SLIB)
dlib: $(DLIB)

dirs:
	if [ ! -d $(LIB) ]; then mkdir $(LIB); fi
	if [ ! -d $(OBJ) ]; then mkdir $(OBJ); fi


$(OBJS): dirs
	$(CC) $(CFLAGS) -c -fPIC $(SRCS) -o $(OBJS)

$(SLIB): $(OBJS)
	ar rcs $(SLIB) $(OBJS)

$(DLIB): $(OBJS)
	$(CC) -shared $(OBJS) -o $(DLIB)
