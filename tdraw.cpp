#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <math.h>
#include <glew.h>
#include <glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader_utils.h"
#include "tdraw.h"
#include "readObj.h"
#include "readBMP.h"

GLuint program;
GLint attribute_coord3d;
GLint attribute_texcoord;
GLint uniform_proj_matrix;  // pointer to uniform variable - total matrix
GLint uniform_texture;
GLint uniform_color;
GLint uniform_tex_enabler;
GLint uniform_norm_mat;

GLint attribute_normals;

GLuint vbo_vertices, vbo_normals, ibo_elements;
GLuint vbo_floor, vbo_floor_texcoords, ibo_floor_elements;
GLuint vbo_cube, vbo_cube_texcoords, ibo_cube_elements, vbo_cube_normals;
Mesh cow;
Mesh cube;

glm::mat4 projMatrix; // Current total transform

glm::vec4 cameraPosition = glm::vec4(0.0, 0.0, -5.0, 1.0);

Image texImage;
Image texImage2;
GLuint textureId;
GLuint textureId2;

int size = 4; // number of triangles to draw in scene

GLfloat vertices[] = {
//  -4, -4, 0.0, 
//  4, -4, 0.0, 
//  4, 4, 0.0, 
//  -4, 4, 0.0, 

    -20, -4, -25,
    20, -4, -25, 
    20, -4, 25, 
    -20, -4, 25
};

GLfloat tex_vertices[] = {
//	-2.0, -2.0, 
//	2.0, -2.0, 
//	2.0, 2.0, 
//	-2.0, 2.0,

    -10.0, -12.5,
     10.0, -12.5,
     10.0, 12.5, 
    -10.0, 12.5
};


GLubyte elements[] = {
    0, 1, 2,
    2, 3, 0,
    4, 5, 6,
    6, 7, 4
  };

// global matricies
// specify by columns

  // Current total transformation
glm::mat4 totalMatrix;
glm::mat4 cubeTotalMatrix;

glm::mat4 initialView;
// Current turtle part of the transformation
glm::mat4 turtleMatrix;
glm::mat4 cubeMatrix;

// move cube into box centered at (0,0,-5)
glm::mat4 view  = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
			     glm::vec4(0.0, 1.0, 0.0, 0.0),
			     glm::vec4(0.0, 0.0, 1.0, 0.0),
			     glm::vec4(0.0, 0.0, -40.0, 1.0));

// projection
// n = near plane = -3
// f = far plane = 21*(-3) = -63
glm::mat4 proj = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
			   glm::vec4(0.0, 1.0, 0.0, 0.0),
			   glm::vec4(0.0, 0.0, -22.0/(20*3), -0.33),
			   glm::vec4(0.0, 0.0, -2.0*21/20, 0.0));


// print out matrix by rows
void printMat(glm::mat4  mat){
  int i,j;
  for (j=0; j<4; j++){
    for (i=0; i<4; i++){
    printf("%f ",mat[i][j]);
  }
  printf("\n");
 }
}

void moveCamera(float move) {
  //cameraPosition[2] += move;
  //printMat(view);
  //view[3] = cameraPosition;
	glm::mat4 moveZ  = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
								glm::vec4(0.0, 1.0, 0.0, 0.0),
								glm::vec4(0.0, 0.0, 1.0, 0.0),
								glm::vec4(0.0, 0.0, move, 1.0));
	view = moveZ * view;
}

void moveCameraLR(float move) {
  //cameraPosition[0] += move;
  //view[3] = cameraPosition;
	glm::mat4 moveX  = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
								glm::vec4(0.0, 1.0, 0.0, 0.0),
								glm::vec4(0.0, 0.0, 1.0, 0.0),
								glm::vec4(move, 0.0, 0.0, 1.0));
	view = moveX * view;

}

void rotateCamera(float angle) {
	glm::mat4 rot_y = glm::mat4(
                glm::vec4(cos(angle), 0.0, -sin(angle), 0.0),
                glm::vec4(0.0, 1, 0.0, 0.0),
                glm::vec4(sin(angle), 0.0, cos(angle), 0.0),
                glm::vec4(0.0, 0.0, 0.0, 1.0));
	view = rot_y * view;
}

void tiltCamera(float angle) {
	glm::mat4 rot_x = glm::mat4(
                glm::vec4(1.0, 0.0, 0.0, 0.0),
                glm::vec4(0.0, cos(angle), sin(angle), 0.0),
                glm::vec4(0.0, -sin(angle), cos(angle), 0.0),
                glm::vec4(0.0, 0.0, 0.0, 1.0));
	view = rot_x * view;
}

void resetCamera(){
	view  = initialView;
}


// Set up the shader programs, compile and link them, get pointers to 
// where the shader variables are in GPU memory. 
int init_resources()
{
  // Projection matrix
  projMatrix = proj * view;

  initialView = view;

  glm::mat4 scaleMatrix = glm::mat4(glm::vec4(5.0, 0.0, 0.0, 0.0),
									glm::vec4(0.0, 5.0, 0.0, 0.0),
									glm::vec4(0.0, 0.0, 5.0, 0.0),
									glm::vec4(0.0, 0.0, 0.0, 1.0));

  glm::mat4 translateMatrix = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
										glm::vec4(0.0, 1.0, 0.0, 0.0),
										glm::vec4(0.0, 0.0, 1.0, 0.0),
										glm::vec4(1.0, -0.2, 1.0, 1.0));

  glm::mat4 translateMatrix2 = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
										glm::vec4(0.0, 1.0, 0.0, 0.0),
										glm::vec4(0.0, 0.0, 1.0, 0.0),
										glm::vec4(-1.0, -0.2, 1.0, 1.0));

  //make the cow bigger
  turtleMatrix = scaleMatrix * translateMatrix * turtleMatrix;
  cubeMatrix = scaleMatrix * translateMatrix2 * cubeMatrix;

  // total matrix for the cow is formed by multiplying projection by turtle
  totalMatrix = proj * view * turtleMatrix;
  cubeTotalMatrix = proj * view * cubeMatrix;

  glGenBuffers(1, &vbo_floor);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_floor);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &vbo_floor_texcoords);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_floor_texcoords);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tex_vertices), tex_vertices, GL_STATIC_DRAW);
  
  glGenBuffers(1, &ibo_floor_elements);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_floor_elements);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);



  // Read in texture image
  int flag = ImageLoad("grass.bmp", &texImage);
  if (flag != 1) {
    printf("Trouble reading image\n");
  }

  
  if (ImageLoad("cubeTex.bmp", &texImage2) != 1) {
    printf("Trouble reading image\n");
  }

  glActiveTexture(GL_TEXTURE0); // Load texture into GPU texture unit 0
  glGenTextures(1,&textureId); // Make a texture object
  glBindTexture(GL_TEXTURE_2D, textureId); // Use this object as the 
  // current 2D texture
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // Load the image into GPU texture memory
  glTexImage2D(GL_TEXTURE_2D, // the current 2D texture
	       0, // Mipmap level
	       GL_RGB, //internal format for texture
	       128, // size in s coord
	       128, // size in t coord
	       0, // should always be 0
	       GL_RGB, // incoming data format; should match internal
	       GL_UNSIGNED_BYTE, // type of incoming data
	       texImage.data // pointer to the data
	       ); 

  //texture for cube
  glActiveTexture(GL_TEXTURE1); // Load texture into GPU texture unit 0
  glGenTextures(1,&textureId2); // Make a texture object
  glBindTexture(GL_TEXTURE_2D, textureId2); // Use this object as the 
  // current 2D texture
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // Load the image into GPU texture memory
  glTexImage2D(GL_TEXTURE_2D, // the current 2D texture
	       0, // Mipmap level
	       GL_RGB, //internal format for texture
		   texImage2.sizeX, // size in s coord
		   texImage2.sizeY, // size in t coord
	       0, // should always be 0
	       GL_RGB, // incoming data format; should match internal
	       GL_UNSIGNED_BYTE, // type of incoming data
	       texImage2.data // pointer to the data
	       ); 



  // Error flag is initially false
  GLint link_ok = GL_FALSE;
  // Indices for vertex and fragment shaders
  GLuint vs, fs;

  // create_shader is a function in shader_utils that reads in 
  // a vertex or fragment program from a file, creates a shader 
  // object, puts the program into the object, and compiles it.
  // If all goes well, returns 1. 
  vs = create_shader("maze.v.glsl", GL_VERTEX_SHADER);
  if (vs == 0) return 0;

  fs = create_shader("maze.f.glsl", GL_FRAGMENT_SHADER);
  if (fs == 0) return 0;


  // The GPU program contains both the vertex and shader programs. 
  program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);

  // Link them together.
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    fprintf(stderr, "glLinkProgram: failed\n");
    return 0;
  }
  
  // Now figure out where the linker put everything
  // We don't know where the vertex program is going to store all its
  // input data until after linking. 
  // Results are indicies into some table on the GPU. 

  // Ask for the index of shader variable coord3d. 
  attribute_coord3d = glGetAttribLocation(program, "coord3d");
  if (attribute_coord3d == -1) {
    fprintf(stderr, "Could not bind attribute coord3d\n");
    return 0;
  }


  // Ask for the index of shader variable texcoord
  attribute_texcoord = glGetAttribLocation(program, "texcoord");
  if (attribute_texcoord == -1) {
    fprintf(stderr, "Could not bind attribute texcoord\n");
    return 0;
  }

  // Ask for the normal
  attribute_normals = glGetAttribLocation(program, "normals");
  if (attribute_normals == -1) {
    fprintf(stderr, "Could not bind attribute normals\n");
    return 0;
  }

  uniform_texture = glGetUniformLocation(program, "u_texture");
  if (uniform_texture == -1) {
    fprintf(stderr, "Could not bind uniform u_texture \n");
    return 0;
  }
  
  // Get linker's index to the uniform variable
  uniform_proj_matrix = glGetUniformLocation(program, "m_proj");
  if (uniform_proj_matrix == -1) {
    fprintf(stderr, "Could not bind uniform variable m_proj \n");
    return 0;
  }

  uniform_tex_enabler = glGetUniformLocation(program, "tex_enabler");
  if (uniform_tex_enabler == -1) {
    fprintf(stderr, "Could not bind uniform variable v_tex_enabler \n");
    return 0;
  }

  uniform_norm_mat = glGetUniformLocation(program, "normal_mat");
  if (uniform_norm_mat == -1) {
    fprintf(stderr, "Could not bind uniform variable normal_mat \n");
    return 0;
  }

  //Get color
  uniform_color = glGetUniformLocation(program, "v_color");
  if (uniform_color == -1){
    fprintf(stderr, "Could not bind uniform variable v_color\n");
    return 0;
  }

  load_obj("cowScaled.obj", &cow);
  load_obj("cube.obj", &cube);

  glGenBuffers(1, &vbo_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
  glBufferData(GL_ARRAY_BUFFER, cow.vertices.size() * sizeof(cow.vertices[0]), cow.vertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &ibo_elements);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, cow.elements.size() * sizeof(cow.elements[0]), cow.elements.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &vbo_normals);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
  glBufferData(GL_ARRAY_BUFFER, cow.normals.size() * sizeof(cow.normals[0]), cow.normals.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &vbo_cube);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube);
  glBufferData(GL_ARRAY_BUFFER, cube.vertices.size() * sizeof(cube.vertices[0]), cube.vertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &ibo_cube_elements);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.elements.size() * sizeof(cube.elements[0]), cube.elements.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &vbo_cube_normals);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_normals);
  glBufferData(GL_ARRAY_BUFFER, cube.normals.size() * sizeof(cube.normals[0]), cube.normals.data(), GL_STATIC_DRAW);
  
  GLfloat cube_texcoords[2*4*6] = {  
     // front  
     0.0, 0.0,  
     1.0, 0.0,  
     1.0, 1.0,  
     0.0, 1.0,  
   };  

   for (int i = 1; i < 6; i++)  {
     memcpy(&cube_texcoords[i*4*2], &cube_texcoords[0], 2*4*sizeof(GLfloat));  
   }
   glGenBuffers(1, &vbo_cube_texcoords);  
   glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoords);  
   glBufferData(GL_ARRAY_BUFFER, sizeof(cube_texcoords), cube_texcoords, GL_STATIC_DRAW); 
  
  // If all went well....
  return 1;
}

// Draw the floor and wall
void drawScene(void) {

  // Send the program to the GPU
  glUseProgram(program);

  // Projection matrix
  projMatrix = proj * view;

  glActiveTexture(GL_TEXTURE0); // Load texture into GPU texture unit 0
  glBindTexture(GL_TEXTURE_2D, textureId);
  
  // Tell GPU to use Texture Unit 0
  glUniform1i(uniform_texture, 0);  
  //enable texture
  glUniform1i(uniform_tex_enabler, 1);

  // Now hook up input data to program.

  // Two attributes for the vertex, position and texture coordinate
  // Let OpenGL know we'll use both of them. 
  glEnableVertexAttribArray(attribute_coord3d);
  glEnableVertexAttribArray(attribute_texcoord);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_floor);
  //   Describe the position attribute and where the data is in the array
  glVertexAttribPointer(
    attribute_coord3d, // attribute ID
    3,                 // number of elements per vertex, here (x,y,z)
    GL_FLOAT,          // the type of each element
    GL_FALSE,          // take our values as-is, don't normalize
    0,  // stride between one position and the next
    0  // pointer to first position in the C array
  );

  // Describe the position attribute and where the data is in the array
  //glVertexAttribPointer(
  //  attribute_coord3d, // attribute ID
  //  3,                 // number of elements per vertex, here (x,y,z)
  //  GL_FLOAT,          // the type of each element
  //  GL_FALSE,          // take our values as-is, don't normalize
  //  3*sizeof(float),  // stride between one position and the next
  //  vertices  // pointer to first position in the C array
  //);

  //glVertexAttribPointer(
  //  attribute_texcoord, // attribute
  //  2,                  // number of elements per vertex, (s,t)
  //  GL_FLOAT,           // the type of each element
  //  GL_FALSE,           // take our values as-is
  //  2*sizeof(float),    // stride to next texture element
  //  tex_vertices              // offset of first element
  //);

  glBindBuffer(GL_ARRAY_BUFFER, vbo_floor_texcoords);
  //   Describe the position attribute and where the data is in the array
  glVertexAttribPointer(
    attribute_texcoord, // attribute ID
    2,                 // number of elements per vertex, here (s,t)
    GL_FLOAT,          // the type of each element
    GL_FALSE,          // take our values as-is, don't normalize
    0,  // stride between one position and the next
    0  // pointer to first position in the C array
  );


  // Send GPU projection matrix
  glUniformMatrix4fv(uniform_proj_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix));


  // draw the wall....
  //glDrawElements(GL_TRIANGLES, size*3, GL_UNSIGNED_BYTE, elements);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_floor_elements);
  int size;
  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  glDrawElements(GL_TRIANGLES, size/sizeof(GLubyte), GL_UNSIGNED_BYTE, 0);

  glDisableVertexAttribArray(attribute_coord3d);
  glDisableVertexAttribArray(attribute_texcoord);
}

void drawCow(void) {

  // Send the program to the GPU
  glUseProgram(program);
  glUniform1i(uniform_tex_enabler, 0);

  
  totalMatrix = proj * view * turtleMatrix;

  // Now hook up input data to program.
  // Only attribute for the vertex is position. 
  glEnableVertexAttribArray(attribute_coord3d);
  glEnableVertexAttribArray(attribute_normals);

  //glEnableVertexAttribArray(attribute_color);
  //glGenBuffers(1, &vbo_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
  //glBufferData(GL_ARRAY_BUFFER, cow.vertices.size() * sizeof(cow.vertices[0]), cow.vertices.data(), GL_STATIC_DRAW);
  
  // Describe the position attribute and where the data is in the array
  glVertexAttribPointer(
    attribute_coord3d, // attribute ID
    4,                 // number of elements per vertex, here (x,y,z)
    GL_FLOAT,          // the type of each element
    GL_FALSE,          // take our values as-is, don't normalize
    0,  // stride between one position and the next
    0  // pointer to first position in the C array
  );


  glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
  glVertexAttribPointer(
    attribute_normals, //attribute
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    0
  );


  // give the matrix a value
  glUniformMatrix4fv(uniform_proj_matrix, 1, GL_FALSE, glm::value_ptr(totalMatrix));
  
  glUniformMatrix4fv(uniform_norm_mat, 1, GL_FALSE, glm::value_ptr(turtleMatrix));
  //glUniform3f(uniform_color, 1, 1, 1);
  glUniform3f(uniform_color, 0.45,0.29,0.07);


//  glGenBuffers(1, &ibo_elements);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
//  glBufferData(GL_ELEMENT_ARRAY_BUFFER, cow.elements.size() * sizeof(cow.elements[0]), cow.elements.data(), GL_STATIC_DRAW);
  int size;
  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  glDrawElements(GL_TRIANGLES, size/sizeof(GLuint), GL_UNSIGNED_INT, 0);

  // Done with the attribute
  glDisableVertexAttribArray(attribute_coord3d);
  glDisableVertexAttribArray(attribute_normals);

}

void moveObject(float x, float y,  float z){
	glm::mat4 scaleMatrix = glm::mat4(glm::vec4(5.0, 0.0, 0.0, 0.0),
									glm::vec4(0.0, 5.0, 0.0, 0.0),
									glm::vec4(0.0, 0.0, 5.0, 0.0),
									glm::vec4(0.0, 0.0, 0.0, 1.0));

	glm::mat4 translateMatrix = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
										glm::vec4(0.0, 1.0, 0.0, 0.0),
										glm::vec4(0.0, 0.0, 1.0, 0.0),
										glm::vec4(x, y, z, 1.0));

	turtleMatrix = translateMatrix * turtleMatrix;
}

void drawCube(void) {

  // Send the program to the GPU
  glUseProgram(program);
  glUniform1i(uniform_tex_enabler, 1);

  cubeTotalMatrix = proj * view * cubeMatrix;

  // Now hook up input data to program.
  // Only attribute for the vertex is position. 
  glEnableVertexAttribArray(attribute_coord3d);
  glEnableVertexAttribArray(attribute_normals);
  glEnableVertexAttribArray(attribute_texcoord);

  glActiveTexture(GL_TEXTURE1); // Load texture into GPU texture unit 0
  glBindTexture(GL_TEXTURE_2D, textureId2); 
  // Tell GPU to use Texture Unit 0
  glUniform1i(uniform_texture, 1);  

  //glEnableVertexAttribArray(attribute_color);
  //glGenBuffers(1, &vbo_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube);
  //glBufferData(GL_ARRAY_BUFFER, cow.vertices.size() * sizeof(cow.vertices[0]), cow.vertices.data(), GL_STATIC_DRAW);
  
  // Describe the position attribute and where the data is in the array
  glVertexAttribPointer(
    attribute_coord3d, // attribute ID
    4,                 // number of elements per vertex, here (x,y,z)
    GL_FLOAT,          // the type of each element
    GL_FALSE,          // take our values as-is, don't normalize
    0,  // stride between one position and the next
    0  // pointer to first position in the C array
  );


  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_normals);
  glVertexAttribPointer(
    attribute_normals, //attribute
    3,
    GL_FLOAT,
    GL_FALSE,
    0,
    0
  );

  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texcoords);
  glVertexAttribPointer(
    attribute_texcoord, // attribute
    2,                  // number of elements per vertex, here (s,t)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
    0,                  // no extra data between each position
    0                   // offset of first element
  );


  // give the matrix a value
  glUniformMatrix4fv(uniform_proj_matrix, 1, GL_FALSE, glm::value_ptr(cubeTotalMatrix));
  
  glUniformMatrix4fv(uniform_norm_mat, 1, GL_FALSE, glm::value_ptr(cubeMatrix));
  //glUniform3f(uniform_color, 1, 1, 1);
  glUniform3f(uniform_color, 0.45,0.29,0.07);


//  glGenBuffers(1, &ibo_elements);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
//  glBufferData(GL_ELEMENT_ARRAY_BUFFER, cow.elements.size() * sizeof(cow.elements[0]), cow.elements.data(), GL_STATIC_DRAW);
  int size;
  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  glDrawElements(GL_TRIANGLES, size/sizeof(GLuint), GL_UNSIGNED_INT, 0);

  // Done with the attribute
  glDisableVertexAttribArray(attribute_coord3d);
  glDisableVertexAttribArray(attribute_normals);
  glDisableVertexAttribArray(attribute_texcoord);
}


void free_resources()
{
  glDeleteProgram(program);
  glDeleteTextures(1,&textureId);
  glDeleteTextures(1,&textureId2);
}
