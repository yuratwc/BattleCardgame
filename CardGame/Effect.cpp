#include "Effect.hpp"

DescriptionEffect::DescriptionEffect(const String str)
	:str(str), cancel(false) {}

bool DescriptionEffect::update(double t)
{
	int32 posx = 0;
	posx = Max((int32)(Scene::Width() - 220), (int32)(Scene::Width() - t * 220));

	Rect(posx, 20, 220, 160).draw(ColorF(1, 0.5));

	return cancel;
}

TurnStartEffect::TurnStartEffect(const int32 currentTurn)
	:turn(currentTurn){}

bool TurnStartEffect::update(double t)
{
	auto text = U"Your Turn";
	auto trans = t >= 1 ? (1 - (t - 1)) : (t / 1);

	ColorF mainColor(0.4, 0.4, 1, trans);
	if(turn==1)
	{
		mainColor.setRGB(1, 0.4, 0.4);

		text = U"Enemy Turn";
	}
	int32 veca = (int32)(EaseInOutExpo(t/2) * 15.0);
	{
		const ScopedRenderStates2D state(BlendState::Additive);
		FontAsset(U"LargeText")(text).drawAt(Scene::Center() + Vec2(veca, 0), ColorF(0.2, trans));
		FontAsset(U"LargeText")(text).drawAt(Scene::Center() + Vec2(-veca, 0), ColorF(0.2, trans));
		FontAsset(U"LargeText")(text).drawAt(Scene::Center() + Vec2(0, veca), ColorF(0.2, trans));
		FontAsset(U"LargeText")(text).drawAt(Scene::Center() + Vec2(0, -veca), ColorF(0.2, trans));
	}
	FontAsset(U"LargeText")(text).drawAt(Scene::Center() + Vec2(0, 0), mainColor);

	return t < 2;
}


DamageEffect::DamageEffect(const Vec2& pos, const int32 damage)
	: m_pos(pos), damage(damage) {}

bool DamageEffect::update(double t)
{
	FontAsset(U"DamageText")(Format(damage)).drawAt(m_pos);
	m_pos.y -= 2;
	return t < 0.3;
}


ArrowEffect::ArrowEffect(const Vec2& pos, const Vec2 to)
	: m_pos(pos), to_pos(to) {}

bool ArrowEffect::update(double t)
{
	const double ease = EaseOutCubic(t);

	Line(m_pos, m_pos + (to_pos - m_pos) * ease).drawArrow(10, Vec2(50, 50), Palette::White);
	return t < 0.5;
}

AttackEffect::AttackEffect(DamageEffect* de, ArrowEffect* ae)
	: ef_arrow(ae), ef_dmg(de) {}

bool AttackEffect::update(double t)
{
	if (t < 1)
		ef_arrow->update(t);
	else
		ef_dmg->update(t - 1);
	return t < 2;
}

AttackEffect::~AttackEffect()
{
	if (ef_arrow != NULL)
		delete ef_arrow;
	if (ef_dmg != NULL)
		delete ef_dmg;

}
