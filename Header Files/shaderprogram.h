


#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "GL/glew.h"
#include "stdio.h"

class ShaderProgram {
private:
	GLuint shaderProgram; 
	GLuint vertexShader; 
	GLuint geometryShader; 
	GLuint fragmentShader; 
	char* readFile(const char* fileName); 
	GLuint loadShader(GLenum shaderType,const char* fileName);
public:
	ShaderProgram(const char* vertexShaderFile,const char* geometryShaderFile,const char* fragmentShaderFile);
	~ShaderProgram();
	void use(); 
	GLuint getUniformLocation(const char* variableName); 
	GLuint getAttribLocation(const char* variableName); 
};



#endif
