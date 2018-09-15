/*
Niniejszy program jest wolnym oprogramowaniem; mo�esz go
rozprowadza� dalej i / lub modyfikowa� na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundacj� Wolnego
Oprogramowania - wed�ug wersji 2 tej Licencji lub(wed�ug twojego
wyboru) kt�rej� z p�niejszych wersji.

Niniejszy program rozpowszechniany jest z nadziej�, i� b�dzie on
u�yteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domy�lnej
gwarancji PRZYDATNO�CI HANDLOWEJ albo PRZYDATNO�CI DO OKRE�LONYCH
ZASTOSOWA�.W celu uzyskania bli�szych informacji si�gnij do
Powszechnej Licencji Publicznej GNU.

Z pewno�ci� wraz z niniejszym programem otrzyma�e� te� egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
je�li nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"

using namespace glm;

GLuint diffTex; //Zmienna reprezentujaca tekstur�
GLuint normalTex; //Zmienna reprezentujaca tekstur�
GLuint heightTex; //Zmienna reprezentuj�ca tekstur�

float speed_x = 0; // [radiany/s]
float speed_y = 0; // [radiany/s]

float lastX = 250, lastY = 250;
bool firstMouse = true;
float yaw2 = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch2 = 0.0f;
float fov = 45.0f;

float aspect = 1; //Stosunek szeroko�ci do wysoko�ci okna


				  //Uchwyty na VAO i bufory wierzcho�k�w
GLuint vao;
GLuint bufVertices; //Uchwyt na bufor VBO przechowuj�cy tablic� wsp�rz�dnych wierzcho�k�w
GLuint bufNormals; //Uchwyt na bufor VBO przechowuj�cy tablic� wektor�w normalnych
GLuint bufTexCoords; //Uchwyt na bufor VBO przechowuj�cy tablic� wsp�rz�dnych teksturowania
GLuint bufC1; //Uchwyt na bufor VBO przechowuj�cy pierwsz� kolumn� moacierzy TBN^-1
GLuint bufC2; //Uchwyt na bufor VBO przechowuj�cy drug� kolumn� moacierzy TBN^-1
GLuint bufC3; //Uchwyt na bufor VBO przechowuj�cy trzeci� kolumn� moacierzy TBN^-1

			  //Uchwyty na shadery
ShaderProgram *shaderProgram; //Wska�nik na obiekt reprezentuj�cy program cieniuj�cy.


							  //Kostka
float* vertices = Models::CubeInternal::vertices;
float* normals = Models::CubeInternal::normals;
float* texCoords = Models::CubeInternal::texCoords;
float* c1 = Models::CubeInternal::c1;
float* c2 = Models::CubeInternal::c2;
float* c3 = Models::CubeInternal::c3;
int vertexCount = Models::CubeInternal::vertexCount;

glm::vec3 observer = glm::vec3(0.0f, 0.0f, -5.0f);
glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 NoseVector = glm::vec3(0.0f, 1.0f, 0.0f);

//Czajnik
/*float* vertices=Models::TeapotInternal::vertices;
float* normals=Models::TeapotInternal::vertexNormals;
float* texCoords=Models::TeapotInternal::texCoords;
float* c1=Models::TeapotInternal::c1;
float* c2=Models::TeapotInternal::c2;
float* c3=Models::TeapotInternal::c3;
int vertexCount=Models::TeapotInternal::vertexCount;
*/

//Procedura obs�ugi b��d�w
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void idz_do_przodu(int speed, glm::vec3 *observer, glm::vec3 *center) {
	*observer = *observer + glm::vec3(0.0f, 0.0f, 0.2f)**center;
	*center = *center + glm::vec3(0.0f, 0.0f, 0.2f)*pitch2;
}

void idz_do_tylu(int speed, glm::vec3 *observer, glm::vec3 *center) {
	*observer = *observer - glm::vec3(0.0f, 0.0f, 0.2f);
	*center = *center - glm::vec3(0.0f, 0.0f, 0.2f);
}

void obrot_prawo(int speed,glm::vec3 *observer, glm::vec3 *center) {
	*observer = *observer - glm::vec3(0.2f, 0.0f, 0.0f);
	*center = *center - glm::vec3(0.2f, 0.0f, 0.0f);
}

void obrot_lewo(int speed, glm::vec3 *observer, glm::vec3 *center) {
	*observer = *observer + glm::vec3(0.2f, 0.0f, 0.0f);
	*center = *center + glm::vec3(0.2f, 0.0f, 0.0f);
}


//Procedura obs�ugi klawiatury
void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) speed_y = -3.14;
		if (key == GLFW_KEY_RIGHT) speed_y = 3.14;
		if (key == GLFW_KEY_UP) speed_x = -3.14;
		if (key == GLFW_KEY_DOWN) speed_x = 3.14;
		if (key == GLFW_KEY_W) idz_do_przodu(0, &observer, &center);
		if (key == GLFW_KEY_S) idz_do_tylu(0, &observer, &center);
		if (key == GLFW_KEY_D) obrot_prawo(0, &observer, &center);
		if (key == GLFW_KEY_A) obrot_lewo(0, &observer, &center);
	}

	if (action == GLFW_REPEAT) {
		if (key == GLFW_KEY_W) idz_do_przodu(0, &observer, &center);
		if (key == GLFW_KEY_S) idz_do_tylu(0, &observer, &center);
		if (key == GLFW_KEY_D) obrot_prawo(0, &observer, &center);
		if (key == GLFW_KEY_A) obrot_lewo(0, &observer, &center);
	}

	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) speed_y = 0;
		if (key == GLFW_KEY_RIGHT) speed_y = 0;
		if (key == GLFW_KEY_UP) speed_x = 0;
		if (key == GLFW_KEY_DOWN) speed_x = 0;
	}
}

//tutaj nowy kod, do poprawki n shit
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

	float sensitivity = 0.1;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw2 += xoffset;
	pitch2 += yoffset;

	if (pitch2 > 89.0f)
		pitch2 = 89.0f;
	if (pitch2 < -89.0f)
		pitch2 = -89.0f;
	
	glm::vec3 front;
	front.x = cos(glm::radians(yaw2)) * cos(glm::radians(pitch2));
	front.y = sin(glm::radians(pitch2));
	front.z = sin(glm::radians(yaw2)) * cos(glm::radians(pitch2));
	center = glm::normalize(front);
}

//Procedura ob�ugi zmiany rozmiaru bufora ramki
void windowResize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height); //Obraz ma by� generowany w oknie o tej rozdzielczo�ci
	if (height != 0) {
		aspect = (float)width / (float)height; //Stosunek szeroko�ci do wysoko�ci okna
	}
	else {
		aspect = 1;
	}
}


GLuint readTexture(char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pami�ci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do kt�rych wczytamy wymiary obrazka
							  //Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pami�ci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
									   //Wczytaj obrazek do pami�ci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}


//Tworzy bufor VBO z tablicy
GLuint makeBuffer(void *data, int vertexCount, int vertexSize) {
	GLuint handle;

	glGenBuffers(1, &handle);//Wygeneruj uchwyt na Vertex Buffer Object (VBO), kt�ry b�dzie zawiera� tablic� danych
	glBindBuffer(GL_ARRAY_BUFFER, handle);  //Uaktywnij wygenerowany uchwyt VBO
	glBufferData(GL_ARRAY_BUFFER, vertexCount*vertexSize, data, GL_STATIC_DRAW);//Wgraj tablic� do VBO

	return handle;
}

//Przypisuje bufor VBO do atrybutu
void assignVBOtoAttribute(ShaderProgram *shaderProgram, const char* attributeName, GLuint bufVBO, int vertexSize) {
	GLuint location = shaderProgram->getAttribLocation(attributeName); //Pobierz numer slotu dla atrybutu
	glBindBuffer(GL_ARRAY_BUFFER, bufVBO);  //Uaktywnij uchwyt VBO
	glEnableVertexAttribArray(location); //W��cz u�ywanie atrybutu o numerze slotu zapisanym w zmiennej location
	glVertexAttribPointer(location, vertexSize, GL_FLOAT, GL_FALSE, 0, NULL); //Dane do slotu location maj� by� brane z aktywnego VBO
}

//Przygotowanie do rysowania pojedynczego obiektu
void prepareObject(ShaderProgram *shaderProgram) {
	//Zbuduj VBO z danymi obiektu do narysowania
	bufVertices = makeBuffer(vertices, vertexCount, sizeof(float) * 4); //VBO ze wsp�rz�dnymi wierzcho�k�w
	bufNormals = makeBuffer(normals, vertexCount, sizeof(float) * 4);//VBO z wektorami normalnymi wierzcho�k�w
	bufTexCoords = makeBuffer(texCoords, vertexCount, sizeof(float) * 2);//VBO ze wsp�rz�dnymi teksturowania
	bufC1 = makeBuffer(c1, vertexCount, sizeof(float) * 4);//VBO z pierwsz� kolumn� macierzy TBN^-1 dla ka�dego wierzcho�a
	bufC2 = makeBuffer(c2, vertexCount, sizeof(float) * 4);//VBO z drug� kolumn� macierzy TBN^-1 dla ka�dego wierzcho�a
	bufC3 = makeBuffer(c3, vertexCount, sizeof(float) * 4);//VBO z trzeci� kolumn� macierzy TBN^-1 dla ka�dego wierzcho�a

														   //Zbuduj VAO wi���cy atrybuty z konkretnymi VBO
	glGenVertexArrays(1, &vao); //Wygeneruj uchwyt na VAO i zapisz go do zmiennej globalnej

	glBindVertexArray(vao); //Uaktywnij nowo utworzony VAO

	assignVBOtoAttribute(shaderProgram, "vertex", bufVertices, 4); //"vertex" odnosi si� do deklaracji "in vec4 vertex;" w vertex shaderze
	assignVBOtoAttribute(shaderProgram, "normal", bufNormals, 4); //"normal" odnosi si� do deklaracji "in vec4 normal;" w vertex shaderze
	assignVBOtoAttribute(shaderProgram, "texCoord0", bufTexCoords, 2); //"texCoord0" odnosi si� do deklaracji "in vec2 texCoord0;" w vertex shaderze
	assignVBOtoAttribute(shaderProgram, "c1", bufC1, 4); //"c1" odnosi si� do deklaracji "in vec4 c1;" w vertex shaderze
	assignVBOtoAttribute(shaderProgram, "c2", bufC2, 4); //"c2" odnosi si� do deklaracji "in vec4 c2;" w vertex shaderze
	assignVBOtoAttribute(shaderProgram, "c3", bufC3, 4); //"c3" odnosi si� do deklaracji "in vec4 c3;" w vertex shaderze

	glBindVertexArray(0); //Dezaktywuj VAO
}


//Procedura inicjuj�ca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, kt�ry nale�y wykona� raz, na pocz�tku programu************
	glClearColor(0, 0, 0, 1); //Czy�� ekran na czarno
	glEnable(GL_DEPTH_TEST); //W��cz u�ywanie Z-Bufora
	glfwSetKeyCallback(window, key_callback); //Zarejestruj procedur� obs�ugi klawiatury
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //wywal kursor 
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetFramebufferSizeCallback(window, windowResize); //Zarejestruj procedur� obs�ugi zmiany rozmiaru bufora ramki

	shaderProgram = new ShaderProgram("Shaders/vshader.glsl", NULL, "Shaders/fshader.glsl"); //Wczytaj program cieniuj�cy

	prepareObject(shaderProgram);

	diffTex = readTexture("Textures/planeta.png");
	normalTex = readTexture("Textures/bricks2_normal.png");
	heightTex = readTexture("Textures/bricks2_height.png");
}

//Zwolnienie zasob�w zaj�tych przez program
void freeOpenGLProgram() {
	delete shaderProgram; //Usuni�cie programu cieniuj�cego

	glDeleteVertexArrays(1, &vao); //Wykasuj VAO

								   //Wykasuj bufory  VBO
	glDeleteBuffers(1, &bufVertices);
	glDeleteBuffers(1, &bufNormals);
	glDeleteBuffers(1, &bufTexCoords);
	glDeleteBuffers(1, &bufC1);
	glDeleteBuffers(1, &bufC2);
	glDeleteBuffers(1, &bufC3);

	//Wykasuj tekstury
	glDeleteTextures(1, &diffTex);
	glDeleteTextures(1, &normalTex);
	glDeleteTextures(1, &heightTex);

}

void drawObject(ShaderProgram *shaderProgram, mat4 mP, mat4 mV, mat4 mM) {
	//W��czenie programu cieniuj�cego, kt�ry ma zosta� u�yty do rysowania
	//W tym programie wystarczy�oby wywo�a� to raz, w setupShaders, ale chodzi o pokazanie,
	//�e mozna zmienia� program cieniuj�cy podczas rysowania jednej sceny
	shaderProgram->use();

	//Przeka� do shadera macierze P,V i M.
	//W linijkach poni�ej, polecenie:
	//  shaderProgram->getUniformLocation("P")
	//pobiera numer przypisany zmiennej jednorodnej o podanej nazwie
	//UWAGA! "P" w powy�szym poleceniu odpowiada deklaracji "uniform mat4 P;" w vertex shaderze,
	//a mP w glm::value_ptr(mP) odpowiada argumentowi  "mat4 mP;" TYM pliku.
	//Ca�a poni�sza linijka przekazuje do zmiennej jednorodnej P w vertex shaderze dane z argumentu mP niniejszej funkcji
	//Pozosta�e polecenia dzia�aj� podobnie.
	//Poni�sze polecenia s� z grubsza odpowiednikami glLoadMatrixf ze starego opengla
	glUniformMatrix4fv(shaderProgram->getUniformLocation("P"), 1, false, glm::value_ptr(mP));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("V"), 1, false, glm::value_ptr(mV));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("M"), 1, false, glm::value_ptr(mM));

	//Powi�� zmienne typu sampler2D z jednostkami teksturuj�cymi
	glUniform1i(shaderProgram->getUniformLocation("diffuseMap"), 0);
	glUniform1i(shaderProgram->getUniformLocation("normalMap"), 1);
	glUniform1i(shaderProgram->getUniformLocation("heightMap"), 2);


	//Przypisz tekstury do jednostek teksturuj�cych
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, heightTex);

	//Aktywuj VAO  (powi�zania pomi�dzy atrybutami i VBO)
	glBindVertexArray(vao);

	//Narysowanie obiektu
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);


	//Dezaktywuj VAO
	glBindVertexArray(0);
}

//Procedura rysuj�ca zawarto�� sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y, glm::vec3 observer, glm::vec3 center, glm::vec3 NoseVector) {
	//************Tutaj umieszczaj kod rysuj�cy obraz******************l

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wykonaj czyszczenie bufora kolor�w i g��boko�ci

	glm::mat4 P = glm::perspective(50 * PI / 180, aspect, 1.0f, 50.0f); //Wylicz macierz rzutowania

	glm::mat4 V = glm::lookAt( //Wylicz macierz widoku
		observer,
		center,
		NoseVector);


	//Wylicz macierz modelu rysowanego obiektu
	glm::mat4 M = glm::mat4(1.0f);
	M = glm::rotate(M, angle_x, glm::vec3(1, 0, 0));
	M = glm::rotate(M, angle_y, glm::vec3(0, 1, 0));

	//Narysuj obiekt
	drawObject(shaderProgram, P, V, M);

	//Przerzu� tylny bufor na przedni
	glfwSwapBuffers(window);

}



int main(void)
{
	GLFWwindow* window; //Wska�nik na obiekt reprezentuj�cy okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedur� obs�ugi b��d�w
	

	if (!glfwInit()) { //Zainicjuj bibliotek� GLFW
		fprintf(stderr, "Nie mo�na zainicjowa� GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utw�rz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Je�eli okna nie uda�o si� utworzy�, to zamknij program
	{
		fprintf(stderr, "Nie mo�na utworzy� okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje si� aktywny i polecenia OpenGL b�d� dotyczy� w�a�nie jego.
	glfwSwapInterval(1); //Czekaj na 1 powr�t plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotek� GLEW
		fprintf(stderr, "Nie mo�na zainicjowa� GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjuj�ce

	float angle_x = 0; //K�t obrotu obiektu
	float angle_y = 0; //K�t obrotu obiektu

	glfwSetTime(0); //Wyzeruj licznik czasu

					//G��wna p�tla
	while (!glfwWindowShouldClose(window)) //Tak d�ugo jak okno nie powinno zosta� zamkni�te
	{
		angle_x += speed_x*glfwGetTime(); //Zwi�ksz k�t o pr�dko�� k�tow� razy czas jaki up�yn�� od poprzedniej klatki
		angle_y += speed_y*glfwGetTime(); //Zwi�ksz k�t o pr�dko�� k�tow� razy czas jaki up�yn�� od poprzedniej klatki
		glfwSetTime(0); //Wyzeruj licznik czasu
		drawScene(window, angle_x, angle_y, observer, center, NoseVector); //Wykonaj procedur� rysuj�c�
		glfwPollEvents(); //Wykonaj procedury callback w zalezno�ci od zdarze� jakie zasz�y.
	}

	freeOpenGLProgram();

	glfwDestroyWindow(window); //Usu� kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zaj�te przez GLFW
	exit(EXIT_SUCCESS);
}
