#include "Util.hpp"

Effect* GameUtil::effects;
Stopwatch GameUtil::timer;
double GameUtil::waitSecond;
std::function<void()> GameUtil::waitFunc;
Array<WaitObj*> GameUtil::waits;

void GameUtil::Load()
{
	GameUtil::LoadFont();
	GameUtil::LoadTexture();

	GameUtil::effects = new Effect();
}

void GameUtil::LoadFont()
{
	FontAsset::Register(U"CardNumber", 16, Typeface::Bold);
	FontAsset::Register(U"CardText", 12, Typeface::Medium);
	FontAsset::Register(U"DamageText", 28, Typeface::Heavy);
	FontAsset::Register(U"NormalText", 18, Typeface::Medium);
	FontAsset::Register(U"BadgeNumber", 22, Typeface::Bold);
	FontAsset::Register(U"LargeText", 64, Typeface::Bold);
	//FontAsset(U"NormalText").getGlyph('a').
}

void GameUtil:: LoadTexture()
{
	TextureAsset::Register(U"CardBack", U"assets/graphics/card_back.png");
	TextureAsset::Register(U"CardTest", U"assets/graphics/card_test.png");
	TextureAsset::Register(U"CardSpellTest", U"assets/graphics/card_spell.png");
}

void GameUtil::Wait(double time, std::function<void()> ptr)
{
	WaitObj* wit = new WaitObj();
	wit->func = ptr;
	wit->time = time;
	wit->watch = new Stopwatch();
	wit->watch->reset();
	wit->watch->start();
	
	GameUtil::waits << wit;
	/*
	waits << wit;
	GameUtil::waitFunc = ptr;
	waitSecond = time;
	timer.reset();
	timer.start();
	*/
}

void GameUtil::Update()
{
	GameUtil::effects->update();

	for (int32 i = 0; i < GameUtil::waits.size(); i++)
	{
		if (waits[i]->watch->isRunning() && waits[i]->watch->s() >= waits[i]->time)
		{
			waits[i]->watch->pause();
			if (waits[i]->func != NULL)
				waits[i]->func();
		}
	}
	/*

	if (timer.isRunning() && timer.s() >= waitSecond)
	{
		timer.pause();
		timer.reset();

		auto func_ = GameUtil::waitFunc;
		GameUtil::waitFunc = NULL;
		if(func_ != NULL)
			func_();
	}
	*/
}


void GameUtil::Unload()
{
	delete GameUtil::effects;
	for (int32 i = 0; i < waits.size(); i++)
	{
		delete waits[i]->watch;
		delete waits[i];
	}
}

void EdgeFont::draw(DrawableText text, int32 x, int32 y)
{
	EdgeFont::draw(text, x, y, Palette::White);
}

void EdgeFont::draw(DrawableText text, int32 x, int32 y, Color col)
{
	text.draw(x - 1, y, Palette::Black);
	text.draw(x + 1, y, Palette::Black);
	text.draw(x, y - 1, Palette::Black);
	text.draw(x, y + 1, Palette::Black);
	text.draw(x, y, col);
}

void EdgeFont::drawAt(DrawableText text, int32 x, int32 y)
{
	EdgeFont::drawAt(text, x, y, Palette::White);
}

void EdgeFont::drawAt(DrawableText text, int32 x, int32 y, Color col)
{
	text.drawAt(x - 1, y, Palette::Black);
	text.drawAt(x + 1, y, Palette::Black);
	text.drawAt(x, y - 1, Palette::Black);
	text.drawAt(x, y + 1, Palette::Black);
	text.drawAt(x, y, col);
}

