#pragma once

#include <freeglut.h>
#include "my_utilis.h"
#include "myShader.h"
#include "modelLoader.h"


class product
{
public:
	product(const string& modelPath, shader* _pshader, 
		const dirctionalLight& _dirLight):dirLight(_dirLight)
	{
		if (_pshader == NULL)
		{
			printf("shader in product is NULL\n");
			exit(1);
		}
		pShader = _pshader;
		pMesh = new modelLoader(pShader);
		pMesh->loadMesh(modelPath);
	}
	~product()
	{
		if (pMesh != NULL)
		{
			delete pMesh;
		}
	}

	void setDirLight(const dirctionalLight& _dirLight)
	{
		dirLight = _dirLight;
	}

	void setClick(bool _isClick[5][5])
	{
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				isClick[i][j] = _isClick[i][j];
			}
		}
		reset();
	}

	void setWVP(const mat4& _wvp)
	{
		wvp = _wvp;
	}

	void render()
	{
		if (!isMoving)
		{
			pShader->setWVP(wvp);
			dirLight.calLocation(wvp);
			pShader->setDirectionalLight(dirLight);
			pMesh->render();
		}
		else
		{
			myWorldTransform.transplate(targetDirection.x, targetDirection.y,
				targetDirection.z);
			mat4 tmpWVP = wvp * myWorldTransform.getMatrix();
			pShader->setWVP(tmpWVP);
			dirLight.calLocation(tmpWVP);
			pShader->setDirectionalLight(dirLight);
			pMesh->render();
			remainingTimes--;
			if (remainingTimes <= 0)
			{
				isVisited[targetIndexX][targetIndexY] = true;
				findTargetIndex(targetIndexX, targetIndexY);
				if (targetIndexX < 0)
				{
					reset();
				}
			}
		}
	}

private:
	modelLoader* pMesh = NULL;
	shader* pShader = NULL;
	dirctionalLight dirLight;
	mat4 wvp;

	bool isClick[5][5] = { false };

	bool isVisited[5][5] = { false };
	vec3 targetDirection;//the direction the product is moving to
	int targetIndexX;
	int targetIndexY;
	worldTransform myWorldTransform;
	bool isMoving;
	float offset = 0.005f;
	int remainingTimes;//the times remaining the product should move

	void reset()
	{
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				isVisited[i][j] = false;
			}
		}
		myWorldTransform.reset();
		isVisited[0][0] = true;
		if (!isClick[0][0])
		{
			isMoving = false;
			return;
		}
		findTargetIndex(0, 0);
		if (targetIndexX < 0)
		{
			isMoving = false;
		}
		else
		{
			isMoving = true;
		}
	}

	//find the target index, also set the target direction and remaning times
	void findTargetIndex(int currX, int currY)
	{
		remainingTimes = 1.0f / offset;
		if (currX > 0 && isClick[currX - 1][currY] && !isVisited[currX - 1][currY])
		{
			targetIndexX = currX - 1;
			targetIndexY = currY;
			targetDirection = vec3(0, 0, offset);
			return;
		}
		if (currY > 0 && isClick[currX][currY - 1] && !isVisited[currX][currY - 1])
		{
			targetIndexX = currX;
			targetIndexY = currY - 1;
			targetDirection = vec3(-offset, 0, 0);
			return;
		}
		if (isClick[currX + 1][currY] && !isVisited[currX + 1][currY])
		{
			targetIndexX = currX + 1;
			targetIndexY = currY;
			targetDirection = vec3(0, 0, -offset);
			return;
		}
		if (currY < 4 && isClick[currX][currY + 1] && !isVisited[currX][currY + 1])
		{
			targetIndexX = currX;
			targetIndexY = currY + 1;
			targetDirection = vec3(offset, 0, 0);
			return;
		}
		targetIndexX = -1;
		targetIndexY = -1;
		return;
	}
};
