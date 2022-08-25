CC=gcc
FLAGS=-O3 -DNDEBUG -g -Wall
PROJNAME=yallic

LIB=lib/
OBJ=obj/
BIN=bin/

SLIB=$(LIB)/lib$(PROJNAME).a
DLIB=$(LIB)/lib$(PROJNAME).so

.PHONY: all clean slib dlib


clean:
    if [ -d $(LIB) ]; rm -r $(LIB); fi
    if [ -d $(OBJ) ]; rm -r $(OBJ); fi
    if [ -d $(BIN) ]; rm -r $(BIN); fi


all: slib dlib
slib: $(SLIB)
dlib: $(DLIB)
