#pragma once

#include "my_utilis.h"

class shader
{
public:
	//the location of uniform and attribute
	GLuint samplerLoc;
	GLint positionLoc;
	GLint texCoorLocation;
	GLint normalLocation;
	materialLoc materialLocation;
	GLint hasTexLoc;
	GLint isManualSetColorLoc;
	GLint myColorLoc;

	//generate the shader program
	bool init()
	{
		shaderProg = glCreateProgram();
		if (shaderProg == 0)
		{
			printf("Error creating shader program\n");
			return false;
		}

		if (!addShader(GL_VERTEX_SHADER, "shader.vs"))
		{
			return false;
		}
		if (!addShader(GL_FRAGMENT_SHADER, "shader.fs"))
		{
			return false;
		}

		if (!linkAndValidate())
		{
			return false;
		}

		positionLoc = getAtribLocation("Position");
		texCoorLocation = getAtribLocation("TexCoord");
		normalLocation = getAtribLocation("Normal");
		WVPLoc = getUniformLocation("gWVP");
		samplerLoc = getUniformLocation("gSampler");
		materialLocation.ambientColor =
			getUniformLocation("gMaterial.AmbientColor");
		materialLocation.diffuseColor =
			getUniformLocation("gMaterial.DiffuseColor");
		dirLightLocation.color =
			getUniformLocation("gDirectionalLight.Color");
		dirLightLocation.ambientIntensity =
			getUniformLocation("gDirectionalLight.AmbientIntensity");
		dirLightLocation.direction =
			getUniformLocation("gDirectionalLight.Direction");
		dirLightLocation.diffuseIntensity =
			getUniformLocation("gDirectionalLight.DiffuseIntensity");
		hasTexLoc =
			getUniformLocation("hasTexture");
		isManualSetColorLoc =
			getUniformLocation("isManualSetColor");
		myColorLoc =
			getUniformLocation("myColor");


		if (positionLoc == -1 ||
			texCoorLocation == -1 ||
			normalLocation == -1 ||
			WVPLoc == -1 ||
			samplerLoc == -1 ||
			materialLocation.ambientColor == -1 ||
			materialLocation.diffuseColor == -1 ||
			dirLightLocation.color == -1 ||
			dirLightLocation.ambientIntensity == -1 ||
			dirLightLocation.direction == -1 ||
			dirLightLocation.diffuseIntensity == -1 ||
			hasTexLoc == -1 ||
			isManualSetColorLoc == -1 ||
			myColorLoc == -1
			)
		{
			return false;
		}
		return true;
	}

	//enable the shader program
	void enable()
	{
		glUseProgram(shaderProg);
	}

	void setWVP(const mat4& WVP)
	{
		glUniformMatrix4fv(WVPLoc, 1, GL_TRUE, &WVP.m[0][0]);
	}

	void setTextureUnit(unsigned int textureUnit)
	{
		glUniform1i(samplerLoc, textureUnit);
	}

	void setDirectionalLight(const dirctionalLight& light)
	{
		glUniform3f(dirLightLocation.color,
			light.color.x,
			light.color.y,
			light.color.z);
		glUniform1f(dirLightLocation.ambientIntensity,
			light.ambientIntensity);
		vec3 localDirection = light.getlocalDirection();
		glUniform3f(dirLightLocation.direction,
			localDirection.x,
			localDirection.y,
			localDirection.z
		);
		glUniform1f(dirLightLocation.diffuseIntensity,
			light.diffuseIntensity);
	}
private:
	vector<GLuint> shaderObjList;
	GLuint shaderProg;

	//the location of uniform
	GLuint WVPLoc;
	dirLightLoc dirLightLocation;

	//add shader to the program
	bool addShader(GLenum shaderType, const char* pFileName)
	{
		string s;
		if (!readFile(pFileName, s))
		{
			return false;
		}

		GLuint shaderObj = glCreateShader(shaderType);

		if (shaderObj == 0)
		{
			printf("Error creating shader type %d\n", shaderType);
			return false;
		}

		//save the shader object
		shaderObjList.push_back(shaderObj);

		const GLchar* p[1];
		p[0] = s.c_str();
		GLint lengths[1] = { (GLint)s.size() };

		glShaderSource(shaderObj, 1, p, lengths);

		glCompileShader(shaderObj);

		GLint success;
		glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			GLchar infoLog[1024];
			glGetShaderInfoLog(shaderObj, 1024, NULL, infoLog);
			printf("Error compiling '%s': '%s'\n", pFileName, infoLog);
			return false;
		}

		glAttachShader(shaderProg, shaderObj);

		return true;
	}

	//link and validate the program
	bool linkAndValidate()
	{
		GLint success = 0;
		GLchar errorLog[1024] = { 0 };

		glLinkProgram(shaderProg);

		glGetProgramiv(shaderProg, GL_LINK_STATUS, &success);
		if (success == 0)
		{
			glGetProgramInfoLog(shaderProg, sizeof(errorLog), NULL, errorLog);
			printf("Error linking shader program: '%s'\n", errorLog);
			return false;
		}

		glValidateProgram(shaderProg);
		glGetProgramiv(shaderProg, GL_VALIDATE_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProg, sizeof(errorLog), NULL, errorLog);
			printf("Invalid shader program: '%s'\n", errorLog);
			return false;
		}

		//delete the intermediate shader objects
		for (auto it : shaderObjList)
		{
			glDeleteShader(it);
		}

		shaderObjList.clear();

		return glGetError() == GL_NO_ERROR;
	}

	GLint getUniformLocation(const char* pUniformName)
	{
		GLuint location = glGetUniformLocation(shaderProg, pUniformName);
		if (location == -1)
		{
			printf("Error getting the location of '%s'\n", pUniformName);
		}
		return location;
	}

	GLint getAtribLocation(const char* pAttribName)
	{
		GLint location = glGetAttribLocation(shaderProg, pAttribName);
		if (location == -1)
		{
			printf("Error getting the location of '%s'\n", pAttribName);
		}
		return location;
	}
};
