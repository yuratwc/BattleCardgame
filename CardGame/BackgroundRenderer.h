#pragma once
#include<Siv3D.hpp>

class BackgroundRenderer
{
	HSV color;
	int32 tx;
	int32 ty;
public:
	void render();
};