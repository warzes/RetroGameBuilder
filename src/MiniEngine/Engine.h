#pragma once

#include "ConfigEngine.h"
#include "Core.h"

/******************************************************************************
* CONTENT
*	EngineCreateInfo 
*	Engine
*		-> Create()				= bool
*		-> Destroy()
*		-> BeginFrame()
*		-> EndFrame()
*		-> IsRun()				= bool
*		-> GetDeltaTime()		= float
*		-> Exit()
******************************************************************************/

struct EngineCreateInfo
{/*
	WindowCreateInfo window;
	RendererCreateInfo renderer;*/
};

class Engine
{
public:
	bool Create(const EngineCreateInfo& createInfo);
	void Destroy();
	void BeginFrame();
	void EndFrame();
	bool IsRun() const;

	float GetFrameTime() const { return 0.016f; }

	void Exit();

private:
	bool m_isRun = false;
};