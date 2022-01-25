#pragma once

#include <freeglut.h>
#include "my_utilis.h"
#include "myCamera.h"
#include "myGrid.h"
#include "myShader.h"

class assemblyLine
{
public:
	worldTransform myWorldTransform;

	assemblyLine(int windowWidth, int winodwHeight, shader* _pshader)
	{
		pUnit = new basicUnit(_pshader);

		pShader = _pshader;
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
	}

	void setWVP(const mat4& _wvp)
	{
		wvp = _wvp;
	}

	void render()
	{
		worldTransform tmpTransform = myWorldTransform;
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				pShader->setWVP(wvp * tmpTransform.getMatrix());
				tmpTransform.transplate(1.0f, 0, 0);
				if (isClick[i][j])
				{
					//printf("render unit %d:%d\n", i, j);
					pUnit->render();
				}
			}
			tmpTransform.transplate(-1.0f * 5, 0, -1.0f);
		}
	}
private:
	int windowWidth;
	int windowHeight;
	basicUnit* pUnit;
	bool isClick[5][5] = { false };
	mat4 wvp;


	shader* pShader = NULL;
};
