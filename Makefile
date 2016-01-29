CC=gcc
CCFLAGS=-w -c -Wall
LIBDIR=lib
INCLUDE=-I lib
SRC=src
BINDER=ar
BINDERFLAGS=rcs
MAINOBJS=queue.o semaphore.o thread.o mythread.o
EXTRAOBJS=queue.o semaphore.o thread.o mythreadextra.o
TARGET=mythread.a
TARGETEXTRA=mythreadextra.a
BUILDDIR=build
TESTMAIN=tests/main
TESTEXTRA=tests/extra

all: dir main extra clean

dir: 
	mkdir $(BUILDDIR)

main: $(MAINOBJS)
	$(BINDER) $(BINDERFLAGS) $(TARGET) $(BUILDDIR)/queue.o $(BUILDDIR)/semaphore.o $(BUILDDIR)/thread.o $(BUILDDIR)/mythread.o

extra: $(EXTRAOBJS)
	$(BINDER) $(BINDERFLAGS) $(TARGETEXTRA) $(BUILDDIR)/queue.o $(BUILDDIR)/semaphore.o $(BUILDDIR)/thread.o $(BUILDDIR)/mythreadextra.o 

queue.o: $(SRC)/queue.c $(LIBDIR)/myqueue.h
	$(CC) $(CCFLAGS) $(SRC)/queue.c -o $(BUILDDIR)/queue.o $(INCLUDE)

semaphore.o: $(SRC)/semaphore.c $(LIBDIR)/mysemaphore.h
	$(CC) $(CCFLAGS) $(SRC)/semaphore.c -o $(BUILDDIR)/semaphore.o $(INCLUDE)

thread.o: $(SRC)/thread.c $(LIBDIR)/thread.h
	$(CC) $(CCFLAGS) $(SRC)/thread.c -o $(BUILDDIR)/thread.o $(INCLUDE)

mythread.o: $(SRC)/mythread.c $(LIBDIR)/mythread.h
	$(CC) $(CCFLAGS) $(SRC)/mythread.c -o $(BUILDDIR)/mythread.o $(INCLUDE)

mythreadextra.o: $(SRC)/mythreadextra.c $(LIBDIR)/mythreadextra.h
	$(CC) $(CCFLAGS) $(SRC)/mythreadextra.c -o $(BUILDDIR)/mythreadextra.o $(INCLUDE)

clean:
	rm -rf $(BUILDDIR)



