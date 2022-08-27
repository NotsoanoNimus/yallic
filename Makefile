CC=gcc
CFLAGS=-O3 -DNDEBUG -g -Wall
PROJNAME=yallic

DOCS=html

SRC=src
SRCS=$(SRC)/$(PROJNAME).c

OBJ=obj
OBJS=$(OBJ)/$(PROJNAME).o

LIB=lib
SLIB=$(LIB)/lib$(PROJNAME).a

TEST=test
TESTS=$(TEST)/$(PROJNAME)_test


.PHONY: all clean slib docs tests $(TESTS)
.DEFAULT_GOAL = all


clean:
	if [ -d $(LIB) ]; then rm -r $(LIB); fi
	if [ -d $(OBJ) ]; then rm -r $(OBJ); fi
	if [ -f $(TESTS) ]; then rm -f $(TESTS)*; fi
	if [ -d $(DOCS) ]; then rm -r $(DOCS); fi

docs:
	if [ -d $(DOCS) ]; then rm -r $(DOCS); fi
	command -v doxygen &>/dev/null && doxygen Doxyfile


all: $(SLIB) docs
release: clean all

install: slib
	if [ ! -x $(SLIB) ]; then chmod +x $(SLIB); fi
	cp -a $(SLIB) /usr/local/lib/
	cp -a $(SRC)/$(PROJNAME).h /usr/local/include/


dirs:
	if [ ! -d $(LIB) ]; then mkdir $(LIB); fi
	if [ ! -d $(OBJ) ]; then mkdir $(OBJ); fi


$(OBJS): dirs
	$(CC) $(CFLAGS) -c -fPIC $(SRCS) -o $(OBJS)

$(SLIB): $(OBJS)
	ar rcs $(SLIB) $(OBJS)


tests: CFLAGS=-g -Wall -O3 -DEBUG -L./lib/ -L/usr/local/lib64 -Wl,-rpath,/usr/local/lib64
tests: clean
tests: $(TESTS)

$(TESTS): $(SLIB) $(TEST)
	$(CC) $(CFLAGS) -c $(TEST)/lists_test.c -o $(TESTS).o
	$(CC) $(CFLAGS) $(TESTS).o -o $(TESTS) -lcriterion -l$(PROJNAME)
	if [ ! -x $(TESTS) ]; then chmod +x $(TESTS); fi
	./$(TESTS)
