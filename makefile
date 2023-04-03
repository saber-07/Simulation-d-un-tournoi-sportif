# all : MatchGenerator doxygen
OBJ = list.o ipcTools.o MatchGenerator.o


MatchGenerator : $(OBJ)
MatchGenerator.o : list.h ipcTools.h

# doxygen :
# 	doxygen ./generate-doxygen


# clean : 
# 	rm -f $(OBJ) MatchGenerator
# 	rm -rf ./doxygen