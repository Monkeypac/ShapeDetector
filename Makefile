CC = g++
INCLUDES = -I./include -I./
CXXFLAGS = -std=c++11 -g -O3 $(INCLUDES)
LFLAGS =
LIBS = -lsfml-window -lsfml-graphics -lsfml-system -pthread

SRC = src
OBJ = obj

SRCS_EXTERN = $(wildcard external/pugixml/pugixml.cpp)
SRCS_CODE = $(wildcard $(SRC)/*.cpp)
SRCS = $(SRCS_CODE) $(SRCS_EXTERN)

OBJS_EXTERN = $(OBJ)/pugixml.o

OBJS_CODE = $(SRCS_CODE:$(SRC)/%.cpp=$(OBJ)/%.o)

OBJS = $(OBJS_CODE) $(OBJS_EXTERN)

TARGET = bin/ShapeDetector

#$(info $(CFLAGS))
#$(info $(INCLUDES))
#$(info $(SRCS))
#$(info $(OBJS))

.PHONY: depend clean

all: $(TARGET)
	@echo Make Complete.

$(TARGET): $(OBJS)
	@echo "Linking ... "
	$(CC) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS)
	@echo "Linking Complete."

$(OBJ)/%.o : $(SRC)/%.cpp
	@echo "Compiling "$<" ..."
	$(CC) $(CXXFLAGS) -c $<  -o $@
	@echo "Compiled "$<" successfully!"

$(OBJ)/%.o : $(SRCS_EXTERN)
	@echo "Compiling "$<" ..."
	$(CC) $(CXXFLAGS) -c $<  -o $@
	@echo "Compiled "$<" successfully!"	

RT=
ARGS=input/sale_01.png -ooutput/map.xml -p -T

run: $(TARGET) run-time-plus run-clean

run-simple:
	./$(TARGET) $(ARGS) $(RT)
run-time:
	time -f "\nreal\t%E\nuser\t%U\nsys\t%S\n" ./$(TARGET) $(ARGS) $(RT)


RTP=\n\n\n\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\#\nSTATS\n\n
RTP+=time\t%E\t(U:%U + S%S)\n
RTP+=%PCPU (%Xtext+%Ddata %Mmax)k\n
RTP+=%Iinputs+%Ooutputs (%Fmajor+%Rminor)pagefaults %Wswaps\n
RTP+=Exit status:\t%x\n

run-time-plus:
	@echo $(TARGET) $(ARGS)
	@time -f '$(RTP)' ./$(TARGET) $(ARGS) $(RT)

run-clean:
	@mv log.txt ./output
	@echo Output cleaned

clean:
	$(RM) $(OBJ)/*.o $(TARGET)
	@echo Clean compiled: Done.

clean-output:
	$(RM) *simplified.png map_explored.xml log.txt debug.txt map.xml
	@echo Clean output: Done.

clean-all: clean clean-output
	@echo Clean: Done.

re:
	@make clean
	@make all -j 8
	@make doc
	@echo Clean / Make / Doc: Done.

doc:
	doxygen Documentation/doxy.conf

infos:
	@echo "Usage:"
	@echo "make all\tto compile"
	@echo "make clean\tto clean"
	@echo "make doc\tto generate documentation
	@echo "make re\t\tto clean and recompile and regenerate doc"
	@echo "if running on linux, make run should work"

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

