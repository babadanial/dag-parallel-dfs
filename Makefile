CXX = g++
# -	MMD causes g++ to create .d files (make dependencies, which are
#		technically makefiles of their own) whenever it compiles a .cc file
# 	- for example, "cat main.d" outputs "main.o: main.cc graph.h"
#	- .d file automatically updates itself
CXXFLAGS = -std=c++17 -g -O0 -Wall -MMD -Werror=vla -Wpedantic
OBJECTS = graph.o main.o
DEPENDS = ${OBJECTS:.o=.d}
EXEC = dfs

${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}

# making our own default, general recipe:
#	- this one means to build .o files from every .cc file
#	- $< is a special variable representing the filestem before the .cc
#		we 
%.o : %.cc
	${CXX} ${CXXFLAGS} -c $< 

.PHONY: clean
clean: 
	rm ${DEPENDS} ${OBJECTS} ${EXEC}

# - "include" directive tells make to suspend reading the current makefile
#		and read one or more other makefiles before continuing
# - reads the content of the .d files above and uses them in generating 
#		default recipes (which use CXX, CXXFLAGS vars) to create the .o
#		files needed in default, general recipe
# - the dash (-) before "include" means "if these files don't exist, 
#		don't worry about it"
-include ${DEPENDS} 