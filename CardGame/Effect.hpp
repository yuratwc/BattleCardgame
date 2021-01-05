#pragma once
# include <Siv3D.hpp>

class DescriptionEffect : public IEffect
{
private:
	String str;
	bool cancel;
public:
	DescriptionEffect(const String str);
	bool update(double t) override;

};

class TurnStartEffect : public IEffect
{
private:
	int32 turn;
public:
	TurnStartEffect(const int32 turn);
	bool update(double t) override;
};


class DamageEffect : public IEffect
{
private:
	Vec2 m_pos;
	int32 damage;
public:
	DamageEffect(const Vec2& pos, const int32 damage);
	bool update(double t) override;

};

class ArrowEffect : public IEffect
{
private:
	Vec2 m_pos;
	Vec2 to_pos;
public:
	ArrowEffect(const Vec2& pos, const Vec2 to);
	bool update(double t) override;

};

class AttackEffect : public IEffect
{
private:
	DamageEffect* ef_dmg;
	ArrowEffect* ef_arrow;
public:
	AttackEffect(DamageEffect* k, ArrowEffect* c);
	bool update(double t) override;
	~AttackEffect();

};