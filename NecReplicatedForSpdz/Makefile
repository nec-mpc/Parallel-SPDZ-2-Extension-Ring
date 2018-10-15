# Makefile

#Cxxprof includes and linkage: 
INCLUDE =  -I./include #-I$(HOME)/CxxProf/thirdparty/Boost -I$(HOME)/CxxProf/install/cxxprof_static/include
# to prevent linking with cxxprof, insert '#' after the '=' in next line:
LINKER_OPTIONS = -lssl -lcrypto #$(HOME)/CxxProf/install/cxxprof_static/lib/libcxxprof_static.a $(LIBS) -lboost_system -lboost_thread -lzmq -lpluma
LIBSSL = #-l:libssl.a -l:libcrypto.a -ldl

# Choose g++ or icpc for compiler, and GXX_FLAGS or ICPC_FLAGS relatively in COMPILER_FLAGS
COMPILER = g++  
CFLAGS   = -O3 -pthread # -g -MMD -MP 

ICPC_FLAGS = -march=core-avx2 # -qopt-report3
GXX_FLAGS = -mavx -mavx2 -maes
# use '-D USECXXPROF' flag to turn on the cxxprof measurments.
COMPILER_FLAGS = -std=c++11 $(INCLUDE) $(GXX_FLAGS) $(CFLAGS) -Wno-unused-result -Wno-format  # $(GXX_FLAGS) #-D USECXXPROF # $(GXX_FLAGS) # $(ICPC_FLAGS)
LDFLAGS = 

SRC_DIR      = ./src
OBJ_DIR      = ./obj
BIN_DIR      = ./bin
INC_DIR      = ./include
LIB_DIR      = ./lib
MAIN_DIR     = ./main
MAIN_OBJ     = ./main/main_obj
SOURCES      = $(wildcard $(SRC_DIR)/*.cpp)
OBJS         = $(addprefix $(OBJ_DIR)/, $(notdir $(SOURCES:.cpp=.o)))
DEPENDS      = $(OBJS:.o=.d)
MAIN         = test 
LIBRARIES = #-L./$(LIB) #-L$(HOME)/CxxProf/install/cxxprof_dyn_network/lib -L$(HOME)/CxxProf/thirdparty/pluma/lib -L$(HOME)/CxxProf/thirdparty/Boost/lib -L$(HOME)/CxxProf/thirdparty/zmq/lib

.PHONY: clean

all: $(MAIN)

$(MAIN): %: $(MAIN_OBJ)/%.o $(OBJS)
	@mkdir -p ./bin
	$(COMPILER) $(COMPILER_FLAGS) $(INCLUDE) -o $@ $^ $(LINKER_OPTIONS)
	mv $@ $(BIN_DIR)

$(MAIN_OBJ)/%.o: $(MAIN_DIR)/%.cpp
	@mkdir -p ./main/main_obj
	$(COMPILER) $(COMPILER_FLAGS) $(INCLUDE) -o $@ -c $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p ./obj
	$(COMPILER) $(COMPILER_FLAGS) $(INCLUDE) -o $@ -c $^

clean:
	rm -f $(BIN_DIR)/* $(OBJ_DIR)/* $(MAIN_OBJ)/*

-include $(DEPENDS)

