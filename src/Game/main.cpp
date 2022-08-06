#include "stdafx.h"
#include "GameApp.h"
//-----------------------------------------------------------------------------
#if defined(_MSC_VER)
#	pragma comment( lib, "MiniEngine.lib" )
#endif
//-----------------------------------------------------------------------------
inline EngineCreateInfo getEngineCreateInfo()
{
	EngineCreateInfo engineCreateInfo;
	return engineCreateInfo;
}
//-----------------------------------------------------------------------------
inline void ImplMain()
{
	Engine engine;
	if (engine.Create(getEngineCreateInfo()))
	{
		GameApp game;
		if (game.Init(&engine))
		{
			while (engine.IsRun())
			{
				engine.BeginFrame();
				game.Frame(engine.GetFrameTime());
				engine.EndFrame();
			}
		}
		game.Close();
	}
	engine.Destroy();
}
//-----------------------------------------------------------------------------
int main(
	[[maybe_unused]] int   argc,
	[[maybe_unused]] char* argv[])
{
	ImplMain();
	return 0;
}
//-----------------------------------------------------------------------------