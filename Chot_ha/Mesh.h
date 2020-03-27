#pragma once
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<string>

#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include<assimp/material.h>

#include"Shader.h"

using namespace std;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};
struct Texture
{
	GLuint id;
	string type;
	aiString path;
};

class Mesh
{
private:
	GLuint VAO, VBO, EBO;
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	Mesh(vector<Vertex> vertices,
		vector<GLuint> indices,
		vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->SetUpMesh();
	}

	void Draw(Shader shader) 
	{
		GLuint diffuseNr = 1;
		GLuint specularNr = 1;
		for (int i = 0; i < this->textures.size(); i++) 
		{
			glActiveTexture(GL_TEXTURE0 + i);
			stringstream ss;
			string number;
			string name = this->textures[i].type;
			if ("texture_diffuse" == name) 
			{
				ss << diffuseNr++;
			}
			else if ("texture_specular" == name)
			{
				ss << specularNr++;
			}
			number = ss.str();
			glUniform1i(glGetUniformLocation(shader.Program, (name + number).c_str()),i);
			glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
		}
		glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 32);
		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		for (int i = 0; i < this->textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	void SetUpMesh() 
	{
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size()*sizeof(GLuint),&this->indices[0],GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoord));

		glBindVertexArray(0);
	}
};
