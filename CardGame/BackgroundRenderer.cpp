#include "BackgroundRenderer.h"

void BackgroundRenderer::render()
{
	Rect(0, 0, Scene::Width(), Scene::Height()).draw(color);

	tx = Periodic::Sawtooth0_1(10s) * 80;
	ty = Periodic::Sawtooth0_1(10s) * 80;
	for (int32 y = 0; y < 9; y++)
	{
		for (int32 x = 0; x < 11; x++)
		{
			if (y % 2 == 0 && x % 2 == 0)
				continue;
			if (y % 2 == 1 && x % 2 == 1)
				continue;
			Rect(x * 80 - tx, y * 80 - ty, 80, 80).draw(ColorF(1, 0.3));
		}
	}

	tx += 1;
	ty += 1;
	tx %= 80;
	ty %= 80;
	color.s = 0.4;
	color.v = 0.8;
	color.h = Periodic::Sawtooth0_1(10s) * 360;
}