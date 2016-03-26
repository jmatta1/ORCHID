#This variable stores the name/path of the directory with all source and header files is (relative to the make file)
SRC_DIR=src
#This variable stores the name/path of the directory with all the object files will be (relative to the make file)
OBJ_DIR=build
#This variable stores the names/paths of all the header files relative to the SRC_DIR
# a command like "find . -name \*.h | sed 's/^..//'" is good for this
HEADERS=DAQLib/DigiController.h DAQLib/DigiReader.h HVLib/MPODController.h HVLib/MPODReader.h\
Threads/DigitizerThread.h Threads/EventThread.h Threads/EventThreadPool.h Threads/MPODThread.h\
Threads/UserThread.h InputLib\InputParser.h
#This variable stores the names/paths of all the source files relative to the SRC_DIR 
# a command like "find . -name \*.cpp | sed 's/^..//'" is good for this
SOURCES=main.cpp DAQLib/DigiController.cpp DAQLib/DigiReader.cpp HVLib/MPODController.cpp\
HVLib/MPODReader.cpp Threads/DigitizerThread.cpp Threads/EventThread.cpp Threads/EventThreadPool.cpp\
Threads/MPODThread.cpp Threads/UserThread.cpp InputLib\InputParser.cpp
#This variable stores the name/path of the executable file this executable will appear in the same folder as the make file
EXECUTABLE=orchid

#This flag contains special include directories in case the user needs to include headers from non standard directories
#The user need not modify this for their own header files as the paths are calculated and included elsewhere in this program
#here I also use this to include a compiler option needed to make the blue libraries compatible with a c++ program
#CLANG_INCLUDE_LOCATIONS=-I/usr/include/c++/5.1.1/x86_64-redhat-linux/
USER_INCLUDE_LOCATIONS=$(CLANG_INCLUDE_LOCATIONS)
#This flag contains special library location directories and library link flags
#CLANG_LIBRARY_FLAGS=-L/usr/lib/gcc/x86_64-redhat-linux/5.1.1 -L/usr/lib64 -L/usr/lib/
USER_LIBRARY_FLAGS=$(CLANG_LIBRARY_FLAGS)
#This variable contains the compiler command set it to g++ for C++ programs and gcc for C programs
COMP=g++
#COMP=clang++
#This variable contains the extentions for the source files set it to .cpp or .C for C++ and .c for C files
EXT=.cpp


#here are the variables that users do not need to modify unless they want to modify compiler flags and the like
#This variable stores the names of all the object files
TEMP_DIR=$(MAKECMDGOALS)
OBJECTS=$(patsubst %$(EXT),$(OBJ_DIR)/$(TEMP_DIR)/%.o,$(SOURCES))

# here we generate and include the list of make files that contain prerequisites
# but we only want to include them if we are not doing a cleaning
MAKE_INCLUDES=$(patsubst %$(EXT),$(SRC_DIR)/%.mk,$(SOURCES))
ifeq (,$(findstring clean,$(MAKECMDGOALS)))
include $(MAKE_INCLUDES)
endif

# compiler flags moved to outside this file for readability
include mk-compflags.mk

# list of targets defined for the use in a seperate file for readability
include mk-usertargets.mk

#This is the section of the file that is the boiler room it constructs the prerequisites file and the object files
#this constructs the executable
$(EXECUTABLE):$(OBJECTS)
	$(COMP) $(FLAGS) $(LIB_FLAG) $(OBJECTS) $(OUTPUT_FLAG) $(EXECUTABLE)

#all object files are built here, some moderate magic in making the prerequisites work right
.SECONDEXPANSION:
$(OBJECTS): $$($$(*F))
	-@mkdir -p $(@D)
	$(COMP) $(FLAGS) $(NO_LINK_FLAG) $(OUTPUT_FLAG) $@ $(patsubst $(OBJ_DIR)/$(TEMP_DIR)/%.o,$(SRC_DIR)/%$(EXT),$@) #| gSTLFilt.pl
	@echo ""

#The prereqs.mk target, here the dependencies for the various object files 
#	are calculated and placed in a seperate file for each object
%.mk: %$(EXT)
	$(COMP) $(DIR_FLAGS) $(MAKE_FLAG) $(INCLUDE_LOC_FLAGS) $< | sed 's,\([abcdefghijklmnopqrstuvwxyz_-]*\).o:,\1= \\\n,' > $@
	@echo ""

