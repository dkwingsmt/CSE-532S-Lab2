MAINDIR=lab2
MAINFILES=main.cpp Director.cpp Play.cpp Player.cpp
EXTRADIR=lab2_generator
EXTRAFILES=main.cpp
COMMONDIR=common
COMMONFILES=common.cpp
TGTDIR=build

CC=g++
CFLAGS=-I$(COMMONDIR) --std=c++0x -g
CFLAGS_MAIN=-I$(MAINDIR)

OBJ_MAIN=$(patsubst %,$(MAINDIR)/%,$(MAINFILES))
OBJ_COMMON=$(patsubst %,$(COMMONDIR)/%,$(COMMONFILES))
TGT_MAIN=lab2

main: $(OBJ_MAIN) $(OBJ_COMMON)
	$(CC) $(CFLAGS) $(OBJ_MAIN) $(OBJ_COMMON) -o $(TGTDIR)/$(TGT_MAIN)

