all : main doxygen
OBJ = ipcTools.o main.o


main : $(OBJ)
main.o : ipcTools.h

doxygen :
	doxygen Doxyfile


clean : 
	rm -f $(OBJ) main ./html/*