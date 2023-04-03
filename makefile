OBJ = list.o ipcTools.o MatchGenerator.o


MatchGenerator : $(OBJ)
MatchGenerator.o : list.h ipcTools.h