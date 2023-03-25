OBJ = ipcTools.o list.o MatchSimul.o


MatchSimul : $(OBJ)
MatchSimul.o : ipcTools.h list.h