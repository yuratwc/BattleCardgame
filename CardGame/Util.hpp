#pragma once
#include<Siv3d.hpp>

class WaitObj
{
public:
	double time;
	Stopwatch* watch;
	std::function<void()> func;
};

class GameUtil
{
public:
	static Effect* effects;
	static Stopwatch timer;
	static double waitSecond;
	static std::function<void()> waitFunc;
	static Array<WaitObj*> waits;

	static void Load();
	static void LoadFont();
	static void LoadTexture();
	static void Unload();
	static void Update();
	static void Wait(double time, std::function<void()> ptr);
};

class EdgeFont
{
public:
	static void draw(DrawableText text, int32 x, int32 y);
	static void draw(DrawableText text, int32 x, int32 y, Color col);
	static void drawAt(DrawableText text, int32 x, int32 y);
	static void drawAt(DrawableText text, int32 x, int32 y, Color col);

};
