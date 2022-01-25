#pragma once

#include <glew.h>
#include <freeglut.h>
#include "my_utilis.h"
#include "myShader.h"

struct Vertex {
	vec3 pos;
	vec2 tex;

	Vertex() {}

	Vertex(const vec3& _pos, const vec2& _tex)
	{
		pos = _pos;

		tex = _tex;
	}
};

//the basic unit of assembly line
class basicUnit
{
public:
	basicUnit(shader* _pShader)
	{
		pShader = _pShader;

		if (pShader == NULL)
		{
			printf("the pointer of shader in basic unit is NULL\n");
			exit(1);
		}

		createVertexBuffer();
		createIndexBuffer();

		myTexture = new texture("../res/metal.jpg");
		myTexture->load();
	}

	void render()
	{
		glUniform3f(pShader->materialLocation.ambientColor,
			ambientColor.x, ambientColor.y, ambientColor.z);
		glUniform3f(pShader->materialLocation.diffuseColor,
			diffuseColor.x, diffuseColor.y, diffuseColor.z);
		myTexture->bind(GL_TEXTURE0);
		glUniform1i(pShader->samplerLoc, 0);
		glUniform1i(pShader->hasTexLoc, 1);
		glUniform1i(pShader->isManualSetColorLoc, 0);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

		glEnableVertexAttribArray(pShader->positionLoc);
		glVertexAttribPointer(pShader->positionLoc, 3, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), 0);
		glEnableVertexAttribArray(pShader->texCoorLocation);
		glVertexAttribPointer(pShader->texCoorLocation, 2, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (void*)(3 * sizeof(float)));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(pShader->positionLoc);
		glDisableVertexAttribArray(pShader->texCoorLocation);
		
	}

private:
	GLuint VBO;
	GLuint IBO;
	texture* myTexture = NULL;
	shader* pShader;
	vec3 diffuseColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 ambientColor = vec3(0.2f, 0.2f, 0.2f);

	void createVertexBuffer()
	{
		Vertex Vertices[8];

		vec2 t00 = vec2(0.0f, 0.0f);  // Bottom left
		vec2 t01 = vec2(0.0f, 1.0f);  // Top left
		vec2 t10 = vec2(1.0f, 0.0f);  // Bottom right
		vec2 t11 = vec2(1.0f, 1.0f);  // Top right

		Vertices[0] = Vertex(vec3(0.5f, 0.5f, 0.5f), t00);
		Vertices[1] = Vertex(vec3(-0.5f, 0.5f, -0.5f), t01);
		Vertices[2] = Vertex(vec3(-0.5f, 0.5f, 0.5f), t10);
		Vertices[3] = Vertex(vec3(0.5f, -0.5f, -0.5f), t11);
		Vertices[4] = Vertex(vec3(-0.5f, -0.5f, -0.5f), t00);
		Vertices[5] = Vertex(vec3(0.5f, 0.5f, -0.5f), t10);
		Vertices[6] = Vertex(vec3(0.5f, -0.5f, 0.5f), t01);
		Vertices[7] = Vertex(vec3(-0.5f, -0.5f, 0.5f), t11);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices,
			GL_STATIC_DRAW);
	}

	void createIndexBuffer()
	{
		unsigned int Indices[] = {
								  0, 1, 2,
								  1, 3, 4,
								  5, 6, 3,
								  7, 3, 6,
								  2, 4, 7,
								  0, 7, 6,
								  0, 5, 1,
								  1, 5, 3,
								  5, 0, 6,
								  7, 4, 3,
								  2, 1, 4,
								  0, 2, 7
		};

		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices),
			Indices, GL_STATIC_DRAW);
	}
};
