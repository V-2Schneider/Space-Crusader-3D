

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "test.h"

using namespace glm;

float horizontalAngle = 3.14f;
float verticalAngle = 0.0f;

GLuint diffTex[5]; 


float speed_x = 0; 
float speed_y = 0.02f; 

float speed = 0.1f;

glm::mat4 M[5];
bool istnieje[5];
glm::vec3 gdzie[5];

float lastX = 500, lastY = 500;
bool firstMouse = true;
float yaw2 = -90.0f;	
float pitch2 = 0.0f;
float fov = 45.0f;

float aspect = 1; 



			
GLuint vao;
GLuint bufVertices; 
GLuint bufNormals; 
GLuint bufTexCoords; 

			
ShaderProgram *shaderProgram; 


					  
float* vertices = testPositions;
float* normals = testNormals;
float* texCoords = testTexels;
int vertexCount = testVertices;

glm::vec3 observer = glm::vec3(0.0f, 2.0f, -1.0f);
glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 NoseVector = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 Right;



void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void tworzenie_macierzy_M() { //a tak¿e innych wa¿nych rzeczy dla przeciwników
	M[0] = glm::mat4(1.0f);
	gdzie[0] = vec3(1.0f, 1.0f, 1.0f);
	M[1] = glm::mat4(vec4(1.0f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 1.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 1.0f, 0.0f),
		vec4(3.0f, 3.0f, 0.0f, 1.0f));
	M[2] = glm::mat4(vec4(1.0f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 1.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 1.0f, 0.0f),
		vec4(1.0f, -6.0f, 10.0f, 1.0f));
	M[3] = glm::mat4(vec4(1.0f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 1.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 1.0f, 0.0f),
		vec4(7.0f, -9.0f, -9.0f, 1.0f));
	M[4] = M[2]*glm::mat4(vec4(1.0f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 1.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 1.0f, 0.0f),
		vec4(0.0f, 5.0f, 0.0f, 1.0f));
	gdzie[1] = glm::vec3(3.0f, 3.0f, 0.0f);
	for (int i = 0; i < 5; i++)
		istnieje[i] = true;
}

void idz(float speed) {
	observer.x = observer.x + center.x * speed;
	observer.z = observer.z + center.z * speed;


}

void krok_w_bok(float speed) {

	observer.x = observer.x + Right.x * speed;
	observer.z = observer.z + Right.z * speed;

}

void hitbox() {
	for (int i = 0; i < 5; i++)
	{
		glm::vec3 vVector = gdzie[i] - center;
		if ((((vVector.x < 1) && (vVector.x > 0)) && ((vVector.y < 1) && (vVector.y > 0))) && ((vVector.z < 1) && (vVector.z > 0))) {
			istnieje[i] = false;
		}
	}
}



void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_W) idz(speed);
		if (key == GLFW_KEY_S) idz(-speed);
		if (key == GLFW_KEY_D) krok_w_bok(speed);
		if (key == GLFW_KEY_A) krok_w_bok(-speed);
		if (key == GLFW_KEY_T) hitbox();
	}

	if (action == GLFW_REPEAT) {
		if (key == GLFW_KEY_W) idz(speed);
		if (key == GLFW_KEY_S) idz(-speed);
		if (key == GLFW_KEY_D) krok_w_bok(speed);
		if (key == GLFW_KEY_A) krok_w_bok(-speed);
	}

	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) speed_y = 0;
		if (key == GLFW_KEY_RIGHT) speed_y = 0;
		if (key == GLFW_KEY_UP) speed_x = 0;
		if (key == GLFW_KEY_DOWN) speed_x = 0;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.5;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw2 += xoffset;
	pitch2 += yoffset;

	
	if (pitch2 > 89.0f)
		pitch2 = 89.0f;
	if (pitch2 < -89.0f)
		pitch2 = -89.0f;
	
	if (yaw2 <= -180.0f)
	{
		yaw2 += 360.0f;
	}

	if (yaw2 >= 180.0f)
	{
		yaw2 -= 360.0f;
	}
	
	glm::vec3 front;
	front.x = cos(glm::radians(yaw2)) * cos(glm::radians(pitch2));
	front.y = sin(glm::radians(pitch2));
	front.z = sin(glm::radians(yaw2)) * cos(glm::radians(pitch2));

	center = glm::normalize(front);
	Right = glm::normalize(glm::cross(front, NoseVector));
}





void windowResize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height); 
	if (height != 0) {
		aspect = (float)width / (float)height; 
	}
	else {
		aspect = 1;
	}
}


GLuint readTexture(char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);


	std::vector<unsigned char> image;   
	unsigned width, height;   
							  
	unsigned error = lodepng::decode(image, width, height, filename);


	glGenTextures(1, &tex); 
	glBindTexture(GL_TEXTURE_2D, tex); 
									  
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

void animacja_obrót(int speed, glm::mat4 M) {

}



GLuint makeBuffer(void *data, int vertexCount, int vertexSize) {
	GLuint handle;

	glGenBuffers(1, &handle);
	glBindBuffer(GL_ARRAY_BUFFER, handle); 
	glBufferData(GL_ARRAY_BUFFER, vertexCount*vertexSize, data, GL_STATIC_DRAW);

	return handle;
}


void assignVBOtoAttribute(ShaderProgram *shaderProgram, const char* attributeName, GLuint bufVBO, int vertexSize) {
	GLuint location = shaderProgram->getAttribLocation(attributeName); 
	glBindBuffer(GL_ARRAY_BUFFER, bufVBO);  
	glEnableVertexAttribArray(location); 
	glVertexAttribPointer(location, vertexSize, GL_FLOAT, GL_FALSE, 0, NULL); 
}


void prepareObject(ShaderProgram *shaderProgram, float *vertices, float *normals, float *texCoords,
	GLuint *bufVertices, GLuint *bufNormals, GLuint * bufTexCoords) {
	*bufVertices = makeBuffer(vertices, vertexCount, sizeof(float) * 4); 
	*bufNormals = makeBuffer(normals, vertexCount, sizeof(float) * 4);
	*bufTexCoords = makeBuffer(texCoords, vertexCount, sizeof(float) * 2);

														 
	glGenVertexArrays(1, &vao); 

	glBindVertexArray(vao); 

	assignVBOtoAttribute(shaderProgram, "vertex", *bufVertices, 4); 
	assignVBOtoAttribute(shaderProgram, "normal", *bufNormals, 4); 
	assignVBOtoAttribute(shaderProgram, "texCoord0", *bufTexCoords, 2);


	glBindVertexArray(0); 
}

void prepareObjects(ShaderProgram *shaderProgram, int numberOfObj) {
	for (int i = 0; i < numberOfObj; i++) {

	}
}



void initOpenGLProgram(GLFWwindow* window) {
	tworzenie_macierzy_M();
	glClearColor(0, 0, 1, 1);
	glEnable(GL_DEPTH_TEST); 
	glfwSetKeyCallback(window, key_callback); 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetFramebufferSizeCallback(window, windowResize);

	shaderProgram = new ShaderProgram("Shaders/vshader.glsl", NULL, "Shaders/fshader.glsl"); 

	
	

	diffTex[0] = readTexture("Textures/planeta.png");
	diffTex[1] = readTexture("Textures/planeta2.png");
	diffTex[2] = readTexture("Textures/planeta2.png");
	diffTex[3] = readTexture("Textures/planeta.png");
	diffTex[4] = readTexture("Textures/planeta.png");
}


void freeOpenGLProgram() {
	delete shaderProgram; 

	glDeleteVertexArrays(1, &vao); 

								 
	glDeleteBuffers(1, &bufVertices);
	glDeleteBuffers(1, &bufNormals);
	glDeleteBuffers(1, &bufTexCoords);


	glDeleteTextures(1, &diffTex[0]);
	glDeleteTextures(1, &diffTex[1]);
	glDeleteTextures(1, &diffTex[2]);
	glDeleteTextures(1, &diffTex[3]);
	glDeleteTextures(1, &diffTex[4]);

}

void drawObject(ShaderProgram *shaderProgram, mat4 mP, mat4 mV, mat4 mM, GLuint diffTex) {
	shaderProgram->use();

	glUniformMatrix4fv(shaderProgram->getUniformLocation("P"), 1, false, glm::value_ptr(mP));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("V"), 1, false, glm::value_ptr(mV));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("M"), 1, false, glm::value_ptr(mM));

	glUniform1i(shaderProgram->getUniformLocation("diffuseMap"), 0);



	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffTex);


	glBindVertexArray(vao);


	glDrawArrays(GL_TRIANGLES, 0, vertexCount);



	glBindVertexArray(0);
}


void drawScene(GLFWwindow* window, float angle_x, float angle_y, glm::vec3 observer, glm::vec3 center, glm::vec3 NoseVector) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	glm::mat4 P = glm::perspective(50 * PI / 180, aspect, 1.0f, 50.0f); 

	glm::mat4 V = glm::lookAt( 
		observer,
		observer + center,
		NoseVector);


	M[1] = glm::rotate(M[1], angle_y*0.01f, vec3(1.0f, 0.0f, 0.0f));

	M[2] = glm::rotate(M[2], angle_y*0.03f, vec3(1.0f, 1.0f, 0.0f));

	M[3] = glm::rotate(M[1], angle_y, vec3(0.0f, 1.0f, 0.0f));
	M[3] = glm::translate(M[3], vec3(5.0f, 0.0f, 0.0f));

	for (int i = 0; i < 5; i++) {
		if (istnieje[i]) {
			prepareObject(shaderProgram, vertices, normals, texCoords, &bufVertices, &bufNormals, &bufTexCoords);
			drawObject(shaderProgram, P, V, M[i], diffTex[i]);
		}
	}
	
	glfwSwapBuffers(window);

}



int main(void)
{
	GLFWwindow* window; 

	glfwSetErrorCallback(error_callback);
	

	if (!glfwInit()) { 
		fprintf(stderr, "Nie mo¿na zainicjowaæ GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1000, 1000, "Space Crusader 3D", NULL, NULL);  

	if (!window) 
	{
		fprintf(stderr, "Nie mo¿na utworzyæ okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); 
	glfwSwapInterval(1); 

	if (glewInit() != GLEW_OK) { 
		fprintf(stderr, "Nie mo¿na zainicjowaæ GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); 

	float angle_x = 0; 
	float angle_y = 0.5; 


	glfwSetTime(0);
					
	while (!glfwWindowShouldClose(window)) 
	{
		angle_y += speed_y*glfwGetTime();
		glfwSetTime(0);
		drawScene(window, angle_x, angle_y, observer, center, NoseVector); 
		glfwPollEvents(); 
	}

	freeOpenGLProgram();

	glfwDestroyWindow(window); 
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
