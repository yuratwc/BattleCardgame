#include "Card.hpp"
#include "MainGame.hpp"

bool Card::contains(int32 x, int32 y, Point cursor)
{
	return x <= cursor.x && y <= cursor.y && x + CardWidth >= cursor.x && y + CardHeight >= cursor.y;
}

void Card::DrawCardBack(int32 x, int32 y)
{
	RoundRect cardFrame(x, y, Card::CardWidth, Card::CardHeight, 3);
	cardFrame.drawShadow(Vec2(2, 2), 8, 1);
	cardFrame(TextureAsset(U"CardBack").scaled(0.5)).draw();
}

///CardFighter

CardFighter::CardFighter(String _name, int32 _hp, int32 _offence, int32 _deffence)
	: hp(_hp), offence(_offence), defence(_deffence) 
{
	name = _name;
}

void CardFighter::draw(int32 x, int32 y)
{
	RoundRect cardFrame(x, y, Card::CardWidth, Card::CardHeight, 3);
	cardFrame.drawShadow(Vec2(2, 2), 8, 1);
	cardFrame(TextureAsset(U"CardTest").scaled(0.5)).draw();

	Rect(x, y, Card::CardWidth, 22).draw(ColorF(0, 0, 0, 0.5));

	FontAsset(U"CardNumber")(Format(hp)).draw(x + 2 + 1, y + 1 + 1, Color(0x333333));
	FontAsset(U"CardNumber")(Format(hp)).draw(x + 2, y + 1);
	FontAsset(U"CardText")(name).draw(x + 20, y + 3);
}

void CardFighter::drawBadge(int32 x, int32 y, Array<int> args)
{
	if (args.size() != 3)
		return;

	Circle(x + 5, y + 5 + 70, 16).drawShadow(Vec2(1, 1), 4).draw(Palette::Lightskyblue).drawFrame(0.4, Palette::Blue);
	EdgeFont::drawAt(FontAsset(U"BadgeNumber")(Format(args[2])), x + 5, y + 75);

	Circle(x + 5, y + 5 + 35, 16).drawShadow(Vec2(1, 1), 4).draw(Palette::Palevioletred).drawFrame(0.4, Palette::Red);
	EdgeFont::drawAt(FontAsset(U"BadgeNumber")(Format(args[1])), x + 5, y + 40);

	Circle(x + 5, y + 5, 16).drawShadow(Vec2(1, 1), 4).draw(Palette::Lightgreen).drawFrame(0.4, Palette::Green);
	EdgeFont::drawAt(FontAsset(U"BadgeNumber")(Format(args[0])), x + 5, y + 5);

}

CardType CardFighter::getType()
{
	return CardType::Fighter;
}

void CardFighter::drawToolTip(int32 x, int32 y, bool drawBackGround)
{
	x += 10; y += 10;
	if (drawBackGround)
	{
		Rect(x, y, 212, 54).draw(ColorF(0, 0.3));
	}
	EdgeFont::draw(FontAsset(U"NormalText")(name), x + 2, y + 2);
	Line(x + 2, y + 25, x + 200, y + 25).draw(1, Palette::White);
	EdgeFont::draw(FontAsset(U"NormalText")(U"[{}/{}/{}]"_fmt(hp, offence, defence)), x + 2, y + 2 + 25);

}


CardSpell::CardSpell(String _name, String _description)
	:description(_description)
{
	name = _name;
}

CardType CardSpell::getType()
{
	return CardType::Spell;
}

void CardSpell::execute(User* user, User* enemy, GameInfo* info)
{
}

void CardSpell::draw(int32 x, int32 y)
{
	RoundRect cardFrame(x, y, Card::CardWidth, Card::CardHeight, 3);
	cardFrame.drawShadow(Vec2(2, 2), 8, 1);
	cardFrame(TextureAsset(U"CardSpellTest").scaled(0.5)).draw();

	Rect(x, y, Card::CardWidth, 22).draw(ColorF(0, 0, 0, 0.5));

	FontAsset(U"CardText")(name).draw(x + 20, y + 3);
}

void CardSpell::drawToolTip(int32 x, int32 y, bool drawBackGround)
{
	x += 10; y += 10;
	if (drawBackGround)
	{
		Rect(x, y, 212, 54).draw(ColorF(0, 0.3));
	}
	EdgeFont::draw(FontAsset(U"NormalText")(name), x + 2, y + 2);
	Line(x + 2, y + 25, x + 200, y + 25).draw(1, Palette::White);
	EdgeFont::draw(FontAsset(U"NormalText")(description), x + 2, y + 2 + 25);

}

CardSpellMagicWand::CardSpellMagicWand(String _name, String _description, int32 _drawCount)
	: drawCount(_drawCount), CardSpell(_name, _description)
{

}

void CardSpellMagicWand::execute(User* user, User* enemy, GameInfo* info)
{
	for(int32 i = 0; i < drawCount; i++)
		user->drawCardFromDeck();
}



///
///

HashTable<int32, Card*> Card::cards = {
		{ 101, new CardFighter(U"エルフ", 2,2,2)},
		{ 102, new CardFighter(U"オオカミ", 1,5,2) },
		{ 103, new CardFighter(U"ペガサス", 6,4,0) },
		{ 104, new CardFighter(U"ゾンビ", 3,2,2) },
		{ 105, new CardFighter(U"妖精", 1,1,4) },
		{ 106, new CardFighter(U"巨人", 3, 1, 4) },
		{ 107, new CardFighter(U"ゴブリン", 3, 3, 1) },
		{ 108, new CardFighter(U"雪だるま", 1, 1, 1) },
		{ 109, new CardFighter(U"幻影", 5, 0, 1) },

		{ 201, new CardSpellMagicWand(U"魔法の杖", U"山札からカードを1枚引く。", 1) },
		{ 202, new CardSpellMagicWand(U"妖精の魔法の杖", U"山札からカードを2枚引く。", 2) },
};

void Card::UnLoadCards()
{
	for (auto it = Card::cards.begin(); it != Card::cards.end(); it++)
	{
		delete it.value();
	}

};
