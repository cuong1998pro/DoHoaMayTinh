#include <iostream>
#include <cstdlib>
#include <ctime>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include"glut.h"

#include "Shader.h"
#include"SOIL2/SOIL2.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Camera.h"
#include"Mesh.h"
#include"Model.h"
#include <ft2build.h>
#include FT_FREETYPE_H  


using namespace std;

int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
bool keys[1024];
bool FirstMouse = true;

void drawBall(Model ball, Shader shader);
void keyCallBack(GLFWwindow * window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void DoMovement();

Camera camera(glm::vec3(0, 8, 25));
GLfloat lastX = SCREEN_WIDTH / 2;
GLfloat lastY = SCREEN_WIDTH / 2;

GLfloat deltaTime = 0;
GLfloat lastTime = 0;

glm::mat4 view(1), projection(1), model(1);
glm::mat4 x(1);
float moveX = 0, moveZ = 0, spin = 0, moveJkt = 0, hitSpin = 0, ballMoveX = 0, ballMoveZ = 0, ballMoveY = 0, h = 2;
int i = 0;
int point = 0, live = 6,level=0;
bool isUp = true;
float xOffset = 0, zOffset = 16;
bool firstMouse = true;
void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  

	lastX = xPos;
	lastY = yPos;


	camera.ProcessMouseMovement(xOffset, yOffset,moveX,moveZ);
}


struct Character {
	GLuint TextureID;   
	glm::ivec2 Size;   
	glm::ivec2 Bearing;  
	GLuint Advance;    
};

std::map<GLchar, Character> Characters;
GLuint VAO, VBO;

void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

float temp = 0;
bool run, hit = false;

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_WIDTH, "LearnOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
	glfwSetKeyCallback(window, keyCallBack);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	glewInit();

	Shader shader("modelLoading.vs", "modelLoading.frag");
	Model map4("res/map/map1.obj");
	Model map2("res/map/map2.obj");
	Model map3("res/map/map3.obj");
	Model map1("res/map/map4.obj");
	Model hit("res/player/hit.obj");
	Model run("res/player/run.obj");
	Model df("res/player/...obj");
	Model tennis_jacket("res/10540_Tennis_racket_V2_L3.obj");
	Model ball("res/10539_tennis_ball_L3.obj");
	
	

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Shader shader1("text.vs", "text.frag");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(SCREEN_WIDTH), 0.0f, static_cast<GLfloat>(SCREEN_HEIGHT));
	shader1.Use();
	glUniformMatrix4fv(glGetUniformLocation(shader1.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	FT_Library ft;
	FT_Init_FreeType(&ft);


	FT_Face face;
	FT_New_Face(ft, "fonts/Base 02.ttf", 0, &face);
		

	FT_Set_Pixel_Sizes(face, 0, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (GLubyte c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			continue;
		}
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	projection = glm::perspective(45.0f, (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);


	while (!glfwWindowShouldClose(window))
	{

		if (live == 0)
		{
			glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glfwPollEvents();
			shader1.Use();

			
			RenderText(shader1,"THUA ROI", 300, 400.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
			
			glfwSwapBuffers(window);
		}
		else
		{
			GLfloat currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastTime;
			lastTime = currentFrame;

			glfwPollEvents();
			DoMovement();

			glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			shader1.Use();

			string str = "Score : " + std::to_string(point) + "   Level : " + std::to_string(level);
			string life = "   Live : " + std::to_string(live);
			RenderText(shader1, life, 500.0f, 750.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
			RenderText(shader1, str, 30, 750, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

			shader.Use();
			view = camera.getViewMatrix();
			glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));


			glm::mat4 model(1);
			glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			if (level == 1)
				map1.Draw_Model(shader);
			if (level == 2)
				map2.Draw_Model(shader);
			if (level == 3)
				map3.Draw_Model(shader);
			if (level >= 4)
				map4.Draw_Model(shader);

			glm::mat4 x(1);
			x = glm::translate(x, glm::vec3(moveX, 0, moveZ + 12));
			x = glm::scale(x, glm::vec3(0.15));
			glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(x));

			if (::run == true)
				run.Draw_Model(shader);
			if (::hit == true)
				hit.Draw_Model(shader);
			else
				if (::run == false)
					df.Draw_Model(shader);

			glm::mat4 y(1);

			y = glm::translate(y, glm::vec3(moveX + temp + (::hit == true ? 0.2 : 0.35), 2 - (fabs)(temp), moveZ + (::hit == true ? 11 : 11.6)));
			y = glm::rotate(y, glm::radians(::hit == true ? -60.0f : 0), glm::vec3(1, 0, 0));
			y = glm::scale(y, glm::vec3(0.035));

			y = glm::rotate(y, glm::radians(hitSpin), glm::vec3(0, 0, 1));
			glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(y));
			tennis_jacket.Draw_Model(shader);


			drawBall(ball, shader);
			if (point < 5)level = 1;
			if (point == 5)level = 2;
			if (point == 10)level = 3;
			if (point == 15)level = 4;
			//if (live == 0) glfwSetWindowShouldClose(window, GL_TRUE);
			spin = 0;
			glfwSwapBuffers(window);

		}
	}
}
float rand_FloatRange(float a, float b)
{
	return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}
float yOffset = 2.5;
void drawBall(Model ball, Shader shader)
{
	
	if (isUp)
	{
		ballMoveX += xOffset *deltaTime;
		ballMoveZ += zOffset*deltaTime;
		ballMoveY -= yOffset*deltaTime;
	}
	if (!isUp)
	{
		ballMoveX += xOffset * deltaTime;
		ballMoveZ += zOffset * deltaTime;
		ballMoveY +=  yOffset*deltaTime;
	}
	if (ballMoveY <= 0)
	{
		isUp = false;
	}

	if (ballMoveY >= 1)
	{
		isUp = true;
	}

	glm::mat4 z(1);
	
	z = glm::translate(z, glm::vec3(ballMoveX,1+ballMoveY, -15+ballMoveZ));
	z = glm::scale(z, glm::vec3(0.1));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(z));
	cout << moveX << " ";
	cout << ballMoveX << " " << ballMoveY << " " << ballMoveZ << " " << moveX << endl;
	
	//reset ball
	
	if (ballMoveZ > 40 )
	{
		ballMoveX = ballMoveY = ballMoveZ = 0;
		zOffset+=2;
		//yOffset+=0.4;
		isUp = true;
		//h = 20;
		live--;
		
		xOffset = rand_FloatRange(-5.2, 5.2);
		
	}
	if (ballMoveZ < -14.1)
	{
		ballMoveX = ballMoveY = ballMoveZ = 0;
		zOffset -= 2;
		//yOffset += 0.4;
		isUp = true;
		h = 20;
		point++;
		if(zOffset<0)
		zOffset = (fabs)(zOffset);

		xOffset = rand_FloatRange(-5.2, 5.2);

	}
	ball.Draw_Model(shader);


}

void keyCallBack(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024)
	{
		if (GLFW_PRESS == action)
		{
			keys[key] = true;
		}
		else
			if (GLFW_RELEASE == action)
			{
				keys[key] = false;
			}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if ((action == GLFW_PRESS || action == GLFW_REPEAT) )
		{
			//hitSpin = 45;
			//moveJkt = -0.3;
			//zOffset += 1;
			if(moveX<=ballMoveX+2 && moveX>=ballMoveX-2   && ballMoveZ <=   27+3 && ballMoveZ >= 18)
				if(zOffset >0)
			zOffset = -zOffset;
			hit = true;
			
		}
		if (action == GLFW_RELEASE)
		{
			hitSpin = 0;
			moveJkt = 0;
			hit = false;
			temp = 0;
		}
	}
}
void DoMovement()
{
	GLfloat velocity = 6 * deltaTime;

	
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.HandleKeyBoard(CameraMove::forward, deltaTime);
		moveZ -= velocity;
		hitSpin = 0; temp = -0.05;
		moveJkt = 0; run = true;
	}
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.HandleKeyBoard(CameraMove::backward, deltaTime);
		moveZ += velocity;
		hitSpin = 0; temp = -0.05;
		moveJkt = 0; run = true;
	}
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.HandleKeyBoard(CameraMove::left, deltaTime);
		moveX -= velocity;

		hitSpin = 45;
		temp = -0.3;
		moveJkt = -0.3; run = true;
	}
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.HandleKeyBoard(CameraMove::right, deltaTime);
		moveX += velocity;  
		hitSpin = -45;
		temp = 0.5;
		moveJkt = 0.03; run = true;
	}
	if (keys[GLFW_KEY_A] == false && keys[GLFW_KEY_S] == false && keys[GLFW_KEY_D] == false && keys[GLFW_KEY_W] == false) {
		run = false;
		//temp = 0;
	}
	


}
void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// Activate corresponding render state	
	shader.Use();
	glUniform3f(glGetUniformLocation(shader.Program, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}