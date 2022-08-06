#pragma once

class Engine;

class GameApp
{
public:
	bool Init(Engine* engine);
	void Close();

	void Frame(float deltaTime);

private:
	Engine* m_engine = nullptr;
};