OBJS = hvkvp.o hvkvpfile.o hvkvpreader.o hvkvpwriter.o hvkvpremover.o main.o
CC = c++
DEBUG = -g0
CXXFLAGS = -Wall -fomit-frame-pointer -std=c++11 -c -fexceptions -O3
LFLAGS = -Wall
OUT = hvkvp
INSTALLDIR = /bin/

BUILDSTAT = stat $(OUT) 2>/dev/null | grep Modify
INSTALLEDSTAT = stat $(INSTALLDIR)$(OUT) 2>/dev/null | grep Modify

TARBALL = $(OUT).tar.gz

all : clean $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(OUT)
	
hvkvp.o : hvkvp.cpp
	$(CC) $(CXXFLAGS) hvkvp.cpp

hvkvpfile.o : hvkvpfile.cpp
	$(CC) $(CXXFLAGS) hvkvpfile.cpp

hvkvpreader.o : hvkvpreader.cpp
	$(CC) $(CXXFLAGS) hvkvpreader.cpp

hvkvpwriter.o : hvkvpwriter.cpp
	$(CC) $(CXXFLAGS) hvkvpwriter.cpp
	
hvkvpremover.o : hvkvpremover.cpp
	$(CC) $(CXXFLAGS) hvkvpremover.cpp
	
main.o : main.cpp
	$(CC) $(CXXFLAGS) main.cpp
	
clean :
	\rm -f *.o $(OUT) *.tar.gz

install : $(OUT)
	@if [ "$(BUILDSTAT)" != "$(INSTALLEDSTAT)" ];\
	then\
		cp -p $(OUT) $(INSTALLDIR);\
	fi

uninstall : $(INSTALLDIR)$(OUT)
	\rm $(INSTALLDIR)$(OUT)

tar :
	\rm -f $(TARBALL)
	tar zcvf $(TARBALL) *.cpp *.h makefile README LICENSE