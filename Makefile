CC = g++
CFLAGS = -g -DGL_GLEXT_PROTOTYPES 
INCLUDES = -I/usr/include/GL -I./common/include/glm-0.9.3.2/
LDFLAGS = -lglut -lGLU -lGL -lX11 -lGLEW

RM = /bin/rm -f 

maze:  maze.o tdraw.o shader_utils.o readObj.o readBMP.o
	$(CC) $(CFLAGS) -o maze maze.o tdraw.o shader_utils.o readObj.o readBMP.o $(LDFLAGS) 
%.o : %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
clean: 
	$(RM) *.o 
