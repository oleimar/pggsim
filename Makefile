#  The EvoProg program runs actor-critic learning simulations
#  Copyright (C) 2019  Olof Leimar
#  See Readme.md for copyright notice

# ----------- assumptions about environment ------------

# This make file should work on both Linux and OS X

# It is assumed that g++ is the command to run the C++ compiler, but as an
# alternative the environment variable GPP_COMP can contain the command for the
# C++ compiler to use (GPP_COMP can, e.g., be set in the user's .profile file).
# This allows the flexibility of using a different compiler than the default
# supplied by the system.

# The makefile should work both on Linux and on Mac, but note that OpenMP is
# not safe on Mac (shown by experience of problems running this program
# multithreaded on Mac), so this makefile does not use OpenMP on Mac.

# ---------------- variable assignments ----------------

PROGNAME = EvoProg
PROGEXT = .exe
DEBUG_PROG = $(PROGNAME:%=%Debug$(PROGEXT))
RELEASE_PROG = $(PROGNAME:%=%$(PROGEXT))

SOURCES = Evo.cpp EvoCode.cpp InpFile.cpp Utils.cpp

PLATFORM = $(shell uname)

# The location of include files not found by default can be given here
# INCL_DIR = /usr/local/include
INCL_DIRS = . $(INCL_DIR)

# The location of lib files not found by default can be given here
# LIB_DIR = /usr/local/lib
# LIB_DIR = /usr/local/opt/libomp/lib/
LIB_DIRS = $(LIB_DIR)
OBJ_DIR = obj
# lib files can be given here
# DEBUG_LIBS = xyz
# RELEASE_LIBS = xyz

INCL_DIR_FLAGS = $(INCL_DIRS:%=-I%)
WARNING_FLAGS = -Wall -Wno-sign-compare
ifeq ($(PLATFORM),Darwin)
CXXFLAGS_COMMON = $(INCL_DIR_FLAGS) $(WARNING_FLAGS) -std=c++14
# CXXFLAGS_DEBUG = $(CXXFLAGS_COMMON) -Xpreprocessor -fno-inline -O0 -fopenmp -g
# CXXFLAGS_RELEASE = $(CXXFLAGS_COMMON) -Xpreprocessor -fopenmp -O3
CXXFLAGS_DEBUG = $(CXXFLAGS_COMMON) -fno-inline -O0 -g
CXXFLAGS_RELEASE = $(CXXFLAGS_COMMON) -O3
else ifeq ($(PLATFORM),Linux)
CXXFLAGS_COMMON = $(INCL_DIR_FLAGS) $(WARNING_FLAGS) -std=c++14
CXXFLAGS_DEBUG = $(CXXFLAGS_COMMON) -fno-inline -O0 -fopenmp -g
CXXFLAGS_RELEASE = $(CXXFLAGS_COMMON) -fopenmp -O3
endif

LIB_DIR_FLAGS = $(LIB_DIRS:%=-L%)
ifeq ($(PLATFORM),Darwin)
# LDFLAGS_RELEASE = $(LIB_DIR_FLAGS) -lomp
# LDFLAGS_DEBUG = $(LIB_DIR_FLAGS) -lomp
LDFLAGS_RELEASE = $(LIB_DIR_FLAGS)
LDFLAGS_DEBUG = $(LIB_DIR_FLAGS)
else ifeq ($(PLATFORM),Linux)
LDFLAGS_RELEASE = $(LIB_DIR_FLAGS) -fopenmp
LDFLAGS_DEBUG = $(LIB_DIR_FLAGS) -fopenmp
endif
DEBUG_LIB_FLAGS = $(DEBUG_LIBS:%=-l%)
RELEASE_LIB_FLAGS = $(RELEASE_LIBS:%=-l%)

DEBUG_OBJECTS = $(SOURCES:%.cpp=$(OBJ_DIR)/%Debug.o)
RELEASE_OBJECTS = $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)

# CXX = $(GPP_COMP)
CXX = g++
# LINK = $(GPP_COMP)
LINK = g++
RM = rm -rf


# -------------- targets and rules ---------------------

all: debug release

debug: $(DEBUG_PROG)

release: $(RELEASE_PROG)

clean:
	-$(RM) $(DEBUG_OBJECTS) $(RELEASE_OBJECTS)

clobber: clean
	-$(RM) $(DEBUG_PROG) $(RELEASE_PROG)

.SUFFIXES: .cpp .o

$(OBJ_DIR)/%Debug.o: %.cpp
	$(CXX) $(CXXFLAGS_DEBUG) -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS_RELEASE) -c $< -o $@

$(DEBUG_PROG): $(DEBUG_OBJECTS)
	$(LINK) $(LDFLAGS_DEBUG) $(DEBUG_OBJECTS) $(DEBUG_LIB_FLAGS) -o $@

$(RELEASE_PROG): $(RELEASE_OBJECTS)
	$(LINK) $(LDFLAGS_RELEASE) $(RELEASE_OBJECTS) $(RELEASE_LIB_FLAGS) -o $@

# ----------------------- dependencies -----------------------

$(PROFILE_OBJECTS) $(DEBUG_OBJECTS) $(RELEASE_OBJECTS) : \
./EvoCode.hpp ./ACgroup.hpp ./Genotype.hpp ./Individual.hpp ./InpFile.hpp \
./MetaPopState.hpp ./Phenotype.hpp ./Utils.hpp
