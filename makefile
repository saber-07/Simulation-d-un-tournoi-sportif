# all : main doxygen
OBJ = list.o ipcTools.o main.o


main : $(OBJ)
main.o : list.h ipcTools.h

# doxygen :
# 	doxygen ./generate-doxygen


# clean : 
# 	rm -f $(OBJ) main
# 	rm -rf ./doxygen