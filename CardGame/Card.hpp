#pragma once
#include<Siv3d.hpp>
#include "Util.hpp"

class User;
class GameInfo;

enum class CardType
{
	Fighter = 0,
	Spell = 2,
};

class Card
{
public:
	static const int32 CardWidth = 252 / 2;
	static const int32 CardHeight = 352 / 2;

	static HashTable<int32, Card*> cards;


	static void UnLoadCards();
	static void DrawCardBack(int32 x, int32 y);

	String name;

	virtual void draw(int32 x, int32 y) = 0;
	virtual void drawBadge(int32 x, int32 y, Array<int> args) {}

	bool contains(int32 x, int32 y, Point cursor);


	virtual CardType getType() = 0;
	virtual void drawToolTip(int32 x, int32 y, bool drawBackGround) { }

	virtual void onSupportTurnEnd() {}
	virtual void onSupportTurnStart() {}

};

class CardFighter : public Card
{
public:
	int32 hp;
	int32 offence;
	int32 defence;

	CardFighter(String _name, int32 _hp, int32 _offence, int32 _deffence);

	void draw(int32 x, int32 y) override;
	void drawBadge(int32 x, int32 y, Array<int> args) override;
	CardType getType() override;
	void drawToolTip(int32 x, int32 y, bool drawBackGround) override;
};

class CardSpell : public Card
{
public:
	String description;

	CardSpell(String _name, String _description);
	CardType getType() override;
	virtual void execute(User* user, User* enemy, GameInfo* info);
	void draw(int32 x, int32 y) override;
	void drawToolTip(int32 x, int32 y, bool drawBackGround) override;

};

class CardSpellMagicWand : public CardSpell
{
public:
	int32 drawCount;
	CardSpellMagicWand(String _name, String _description, int32 _drawCount);
	void execute(User* user, User* enemy, GameInfo* info) override;

};