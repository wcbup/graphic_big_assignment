#pragma once

#include "my_utilis.h"
#include "myShader.h"

//for loading the obj model and render
//support texture
class modelLoader
{
public:
	modelLoader(shader* _pShader)
	{
		pShader = _pShader;
	}
	~modelLoader()
	{
		clear();
	}

	bool loadMesh(const string& fileName)
	{
		clear();

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		//create buffers for vertices attributes
		glGenBuffers(ARRAY_SIZE(buffers), buffers);

		bool ret = false;
		Assimp::Importer importer;

		const aiScene* pScene = importer.ReadFile(fileName.c_str(),
			ASSIMP_LOAD_FLAGS);

		if (pScene)
		{
			ret = initFromScene(pScene, fileName);
		}
		else
		{
			printf("error parsing '%s': '%s'",
				fileName.c_str(), importer.GetErrorString());
		}

		glBindVertexArray(0);

		return ret;
	}

	void render()
	{
		glBindVertexArray(VAO);

		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			unsigned int materialIndex = meshes[i].materialIndex;

			if (materialIndex >= materials.size())
			{
				printf("the material index in wrong!\n");
				exit(1);
			}

			setMaterial(materials[materialIndex]);

			if (materials[materialIndex].pDiffuse != NULL)
			{
				materials[materialIndex].pDiffuse->bind(GL_TEXTURE0);
				glUniform1i(pShader->samplerLoc, 0);
				glUniform1i(pShader->hasTexLoc, 1);
			}
			else
			{
				glUniform1i(pShader->hasTexLoc, 0);
			}

			glUniform1i(pShader->isManualSetColorLoc, 0);

			glDrawElementsBaseVertex(GL_TRIANGLES,
				meshes[i].numIndices,
				GL_UNSIGNED_INT,
				(void*)(sizeof(unsigned int) * meshes[i].baseIndex),
				meshes[i].baseVertex);
		}

		glBindVertexArray(0);
	}

private:
	void clear()
	{
		if (buffers[0] != 0)
		{
			glDeleteBuffers(ARRAY_SIZE(buffers), buffers);
		}

		if (VAO != 0)
		{
			glDeleteVertexArrays(1, &VAO);
			VAO = 0;
		}
	}

	bool initFromScene(const aiScene* pScene, const string& fileName)
	{
		meshes.resize(pScene->mNumMeshes);
		materials.resize(pScene->mNumMaterials);

		unsigned int numVertices = 0;
		unsigned int numIndices = 0;

		//also update the meshes array
		countVerticesAndIndices(pScene, numVertices, numIndices);

		//reserve space for vertex stuff
		reserveSpace(numVertices, numIndices);

		initAllMeshes(pScene);

		if (!initMaterials(pScene, fileName))
		{
			return false;
		}

		populateBuffers();

		return glGetError() == GL_NO_ERROR;
	}

	//count the verticese and indices
	//also update the members in meshes array
	void countVerticesAndIndices(const aiScene* pScene,
		unsigned int& numVertices, unsigned int& numIndices)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].materialIndex = pScene->mMeshes[i]
				->mMaterialIndex;
			meshes[i].numIndices = pScene->mMeshes[i]
				->mNumFaces * 3;//every face is triangle
			meshes[i].baseVertex = numVertices;
			meshes[i].baseIndex = numIndices;

			numVertices += pScene->mMeshes[i]->mNumVertices;
			numIndices += meshes[i].numIndices;
		}
	}

	//reserve space for vertex stuff
	void reserveSpace(unsigned int numVertices,
		unsigned int numIndices)
	{
		positions.reserve(numVertices);
		normals.reserve(numVertices);
		texcoords.reserve(numVertices);
		indices.reserve(numIndices);
	}

	void initAllMeshes(const aiScene* pScene)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			const aiMesh* pAimesh = pScene->mMeshes[i];
			initSingleMesh(pAimesh);
		}
	}

	void initSingleMesh(const aiMesh* pAiMesh)
	{
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

		//init the vertex attribute vectors
		for (unsigned int i = 0; i < pAiMesh->mNumVertices; i++)
		{
			const aiVector3D& pPos = pAiMesh->mVertices[i];
			const aiVector3D& pNormal = pAiMesh->mNormals[i];
			const aiVector3D& pTexCoord =
				pAiMesh->HasTextureCoords(0) ?
				pAiMesh->mTextureCoords[0][i] : Zero3D;
			positions.push_back(vec3(pPos.x, pPos.y, pPos.z));
			normals.push_back(vec3(pNormal.x, pNormal.y, pNormal.z));
			texcoords.push_back(vec2(pTexCoord.x, pTexCoord.y));
		}

		//init index buffer
		for (unsigned int i = 0; i < pAiMesh->mNumFaces; i++)
		{
			const aiFace& face = pAiMesh->mFaces[i];
			if (face.mNumIndices != 3)
			{
				printf("the num of indices in aiFace is not 3!!\n");
				exit(1);
			}
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}
	}

	string getDirFromFileName(const string& fileName)
	{
		string::size_type slashIndex;
		slashIndex = fileName.find_last_of("/");

		string dir;
		if (slashIndex == string::npos)
		{
			dir = ".";
		}
		else if (slashIndex == 0)
		{
			dir = "/";
		}
		else
		{
			dir = fileName.substr(0, slashIndex);
		}

		return dir;
	}

	bool initMaterials(const aiScene* pScene, const string& fileName)
	{
		string dir = getDirFromFileName(fileName);

		bool ret = true;

		for (unsigned int i = 0; i < pScene->mNumMaterials; i++)
		{
			const aiMaterial* pMaterial = pScene->mMaterials[i];

			loadTextures(dir, pMaterial, i);

			loadColors(pMaterial, i);
		}

		return ret;
	}

	void loadTextures(const string& dir, const aiMaterial* pMaterial,
		int index)
	{
		loadDiffuseTexture(dir, pMaterial, index);
	}

	void loadDiffuseTexture(const string& dir,
		const aiMaterial* pMaterial, int index)
	{
		materials[index].pDiffuse = NULL;
		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString path;
			if (pMaterial->GetTexture(aiTextureType_DIFFUSE,
				0, &path, NULL, NULL, NULL, NULL, NULL)
				== AI_SUCCESS)
			{
				string p(path.data);

				if (p.substr(0, 2) == ".\\")
				{
					p = p.substr(2, p.size() - 2);
				}

				string fullPath = dir + "/" + p;

				materials[index].pDiffuse = new texture(fullPath.c_str());
				materials[index].pDiffuse->load();
			}
		}
	}

	void loadColors(const aiMaterial* pMaterial, int index)
	{
		aiColor3D ambientColor(0.0f, 0.0f, 0.0f);

		if (pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor)
			== AI_SUCCESS)
		{
			materials[index].ambientColor.x = ambientColor.r;
			materials[index].ambientColor.y = ambientColor.g;
			materials[index].ambientColor.z = ambientColor.b;
		}

		aiColor3D diffuseColor(0.0f, 0.0f, 0.0f);

		if (pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor)
			== AI_SUCCESS)
		{
			materials[index].diffuseColor.x = diffuseColor.r;
			materials[index].diffuseColor.y = diffuseColor.g;
			materials[index].diffuseColor.z = diffuseColor.b;
		}
	}

	void setMaterial(const material& m)
	{
		glUniform3f(pShader->materialLocation.ambientColor,
			m.ambientColor.x,
			m.ambientColor.y,
			m.ambientColor.z
		);
		glUniform3f(pShader->materialLocation.diffuseColor,
			m.diffuseColor.x,
			m.diffuseColor.y,
			m.diffuseColor.z
		);
	}

	//load the data into the buffers
	void populateBuffers()
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffers[POS_VB]);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(positions[0]) * positions.size(), &positions[0],
			GL_STATIC_DRAW);
		glEnableVertexAttribArray(pShader->positionLoc);
		glVertexAttribPointer(pShader->positionLoc, 3, GL_FLOAT,
			GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[TEXCOORD_VB]);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(texcoords[0]) * texcoords.size(), &texcoords[0],
			GL_STATIC_DRAW);
		glEnableVertexAttribArray(pShader->texCoorLocation);
		glVertexAttribPointer(pShader->texCoorLocation, 2, GL_FLOAT,
			GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[NORMAL_VB]);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(normals[0]) * normals.size(),
			&normals[0],
			GL_STATIC_DRAW);
		glEnableVertexAttribArray(pShader->normalLocation);
		glVertexAttribPointer(pShader->normalLocation, 3, GL_FLOAT,
			GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			sizeof(indices[0]) * indices.size(),
			&indices[0], GL_STATIC_DRAW);
	}

#define INVALID_MATERIAL 0xFFFFFFFF

	enum BUFFER_TYPE
	{
		INDEX_BUFFER = 0,
		POS_VB = 1,	//position bufer
		TEXCOORD_VB = 2,	//texture coord buffer
		NORMAL_VB = 3,	//normal buffer
		NUM_BUFFERS = 4
	};

	GLuint VAO = 0;
	GLuint buffers[NUM_BUFFERS] = { 0 };

	struct basicMeshEntry
	{
		basicMeshEntry()
		{
			numIndices = baseVertex = baseIndex = 0;
			materialIndex = INVALID_MATERIAL;
		}
		unsigned int numIndices;
		unsigned int baseVertex;
		unsigned int baseIndex;
		unsigned int materialIndex;
	};

	vector<basicMeshEntry> meshes;
	vector<material> materials;

	//temporary space for vertex stuff before loading into GPU
	vector<vec3> positions;
	vector<vec3> normals;
	vector<vec2> texcoords;
	vector<unsigned int> indices;

	shader* pShader = NULL;
};
