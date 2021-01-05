#include "MainGame.hpp"
#include "CPU.h"

bool Place::isEmpty()
{
	return cards.isEmpty();
}

bool Place::isValid()
{
	return !killed;
}

bool Place::isFightable()
{
	return isValid() && !isEmpty();
}

bool Place::add(Card* card)
{
	if (card->getType() == CardType::Fighter)
	{
		maxhp += ((CardFighter*)card)->hp;
		hp += ((CardFighter*)card)->hp;

		offence += ((CardFighter*)card)->offence;
		defence += ((CardFighter*)card)->defence;

		cards << card;
		return true;
	}
	return false;
}

int Place::attack(int damage, bool simulate)
{
	int32 hpbar = hp;
	if (damage < 0)
		return 0;
	int32 hpc = hp;

	hpc -= damage;
	if (hpc < 0)
		hpc = 0;


	if (simulate)
	{
		hp = hpc;
		killed = hpc == 0;

	}
	return hpbar - hpc;
}

void Place::draw(int32 x, int32 y)
{
	if (killed)
	{
		Card::DrawCardBack(x, y);
		return;
	}
	for (int32 i = cards.size() - 1; i >= 0; --i)
	{
		cards[i]->draw(x + i * 3, y + i * 3);
	}
	if (isEmpty())
		return;

	Array<int32> args = { hp, offence, defence };
	cards[0]->drawBadge(x, y, args);

	int32 width = (int32)((double)hp / maxhp * (Card::CardWidth - 6 - 10 - 5));
	if (width < 0) width = 0;

	Rect(x + 12 + 5, y - 7, Card::CardWidth - 4 - 10 - 5, 6).draw(Arg::top = Color(128, 128, 128), Arg::bottom = Color(64, 64, 64));
	Rect(x + 13 + 5, y - 6, width, 4).draw(Arg::top = Color(0, 207, 33), Arg::bottom = Color(32, 109, 0));

}

void Place::drawToolTip(int32 x, int32 y, bool drawBackGround)
{
	if (isEmpty())
		return;

	x += 10; y += 10;
	if (drawBackGround)
	{
		Rect(x, y, 212, 54).draw(ColorF(0, 0.3));
	}
	String name = ((CardFighter*)cards[0])->name;
	EdgeFont::draw(FontAsset(U"NormalText")(name), x + 2, y + 2);

	Line(x + 2, y + 25, x + 200, y + 25).draw(1, Palette::White);
	EdgeFont::draw(FontAsset(U"NormalText")(U"[{}/{}/{}]"_fmt(hp, offence, defence)), x + 2, y + 2 + 25);

}

bool Place::canAdd()
{
	return cards.size() < 3;
}

User::User()
{
	deck.shuffle();

	place = { new Place(), new Place(), new Place() };
}

void User::drawCardFromDeck()
{
	if (deck.size() == 0)
		return;

	auto c = Random(deck.size() - 1);
	hand << deck[c];
	deck.remove_at(c);
}

bool User::summonFromHand(Card* card, int32 place, bool simulate)
{
	if (card == NULL || place < 0 || place > 2 || !hand.includes(card))
		return false;

	auto p = this->place[place];
	if (!p->canAdd())
		return false;

	if (simulate)
	{
		p->add(card);
		hand.remove(card);
	}
	return true;
}

void User::dispose()
{
	for (auto k : place)
	{
		delete k;
	}
}


//MainGame
//MainGame::renderer;

MainGame::MainGame()
	:enemyAI(this)
{
	
	for(int32 i = 0; i < 3; i++)
		info.me.drawCardFromDeck();
	for (int32 i = 0; i < 3; i++)
		info.enemy.drawCardFromDeck();

	info.state = GameState::MyTurnSelect;

	info._userHandHas = -1;
	toolTip = NULL;
	targetPlace = NULL;
	have = NULL;
	targetIndex = -1;

	info.maxActionCount = info.actionCount = 1;

}

void MainGame::update()
{
	renderer.renderDeck(info.me);
	renderer.renderCircle(info);
	int32 posx = 0;
	Card* remove = NULL;
	
	bool controlDid = false;
	auto handaction = renderer.renderHandCards(info);
	controlDid |= ((int)handaction > 0);
	renderer.renderPlace(info.enemy, info, 20);
	renderer.renderPlace(info.me, info, 220);
	renderer.renderKillCounts(info.me, info.enemy);

	renderer.renderButtons(info);

	enemyAI.update(info.enemy, info.me, info);

	if (handaction == RenderResult::LeftAction)	//select card;
	{
		if (info.me.hand[info._userHandHas]->getType() == CardType::Spell)
		{
			renderer.showToast(U"もう一度クリックでスペルを発動することができます。");
		}
		else
		{
			renderer.showToast(U"出す場所を選択してください。");
		}
	}
	else if (handaction == RenderResult::RightAction)	//execute spell
	{
		renderer.hideToast();
		((CardSpell*)info.me.hand[info._userHandHas])->execute(&info.me, &info.enemy, &info);
		info.me.hand.remove_at(info._userHandHas);

		info._userHandHas = -1;
		info.state = GameState::EnemyTurn;
		controlDid = true;
	}

	if (!controlDid && info.state != GameState::AnimationWait)
	{
		if (info.state == GameState::MyTurnSelect)
		{
			if (MouseL.down() && Cursor::Pos().y >= 220 && Cursor::Pos().y <= 220 + Card::CardHeight)
			{
				auto index = (Cursor::Pos().x - 10) / (Card::CardWidth + 10);
				if (index >= 0 && index < 3 && !info.me.place[index]->isEmpty())
				{
					info.state = GameState::AttackSelect;
					targetPlace = info.me.place[index];
					targetIndex = index;
					renderer.showToast(U"攻撃する相手、移動する場所を選択してください。");
				}
			}
			/*
			if (MouseR.down())
			{
				info.me.drawCardFromDeck();
			}
			*/
		}
		else if (info.state == GameState::PlaceSelect)
		{
			if (info._userHandHas != -1 && info.me.hand[info._userHandHas]->getType() == CardType::Fighter)
			{
				if (renderer.renderPlaceFrame(info.me, info, NULL, 220))
				{
					renderer.hideToast();

					info.me.place[info._targetPlace]->add(info.me.hand[info._userHandHas]);

					info.me.hand.remove_at(info._userHandHas);
					info._userHandHas = -1;
					info._targetPlace = -1;
					info.actionCount--;
					info.state = GameState::EnemyTurn;

					/*
					std::function<void()> fn = [&]() {
						nextTurn(info.enemy, info.me);
						info.state = GameState::MyTurnSelect;
						//Print << U"oi";
					};
					*/
					info.state = GameState::AnimationWait;
					GameUtil::Wait(0.5, [&]()
						{
							nextTurn(info.enemy, info.me);
							//info.state = GameState::MyTurnSelect;
						});
						
				}
			}
		}
		else if (info.state == GameState::AttackSelect)
		{
			if (MouseR.down())
			{
				targetIndex = -1;
				targetPlace = NULL;
				renderer.hideToast();
				info.state = GameState::MyTurnSelect;
			}
			else if (renderer.renderPlaceFrameAttack(info.enemy, info, targetPlace, targetIndex))
			{
				renderer.hideToast();
				if (info._targetAttack >= 100)
				{
					info._targetAttack -= 100;
					auto to_vec = Vec2(info._targetAttack * (Card::CardWidth + 10) + 10 + Card::CardWidth / 2, 220 + Card::CardHeight / 2);
					auto from_vec = Vec2(targetIndex * (Card::CardWidth + 10) + 10 + Card::CardWidth / 2, 220 + Card::CardHeight / 2);
					GameUtil::effects->add<ArrowEffect>(from_vec, to_vec);
					info.actionCount--;

					info.state = GameState::AnimationWait;
					GameUtil::Wait(2, [&]()
						{
							auto n = info.me.place[targetIndex];
							info.me.place[targetIndex] = info.me.place[info._targetAttack];
							info.me.place[info._targetAttack] = n;

							//info.me.place[info._targetAttack]->attack(targetPlace->offence - info.enemy.place[info._targetAttack]->defence, true);
							info._targetAttack = -1;
							targetPlace = NULL;
							targetIndex = -1;
							nextTurn(info.enemy, info.me);
							//info.state = GameState::MyTurnSelect;
						});
				}
				else

				{
					int32 dmg = info.enemy.place[info._targetAttack]->attack(targetPlace->offence - info.enemy.place[info._targetAttack]->defence, false);
					auto to_vec = Vec2(info._targetAttack * (Card::CardWidth + 10) + 10 + Card::CardWidth / 2, 20 + Card::CardHeight / 2);
					auto from_vec = Vec2(targetIndex * (Card::CardWidth + 10) + 10 + Card::CardWidth / 2, 220 + Card::CardHeight / 2);

					GameUtil::effects->add<AttackEffect>(new DamageEffect(to_vec, dmg), new ArrowEffect(from_vec, to_vec));

					info.actionCount--;

					//nextTurn(info.enemy, info.me);

					info.state = GameState::AnimationWait;
					GameUtil::Wait(1, [&]()
						{
							info.enemy.place[info._targetAttack]->attack(targetPlace->offence - info.enemy.place[info._targetAttack]->defence, true);
							info._targetAttack = -1;
							info.enemy.killCount = 0;
							targetPlace = NULL;
							targetIndex = -1;

							for (int32 i = 0; i < info.enemy.place.size(); i++)
							{
								if (info.enemy.place[i]->killed)
									info.enemy.killCount++;
							}
							if (info.enemy.killCount == 3)
							{
								info.gameOver = true;
								info.win = true;
							}

							nextTurn(info.enemy, info.me);
							//info.state = GameState::MyTurnSelect;
						});
				}
			}

		}
	}

	//drawtooltip
	if (info.tooltipCard != NULL)
	{
		info.tooltipCard->drawToolTip(Cursor::Pos().x, Cursor::Pos().y, true);
		info.tooltipCard = NULL;
	}

	if (toolTip != NULL)
	{
		toolTip->drawToolTip(Cursor::Pos().x, Cursor::Pos().y, true);
		toolTip = NULL;
	}
	else
	{
		auto c = Cursor::Pos();
		auto cardX = (c.x - 10) / (Card::CardWidth);
		if (!(cardX < 0 || cardX > 2))
		{

			int32 p[] = { 20, 200 };
			for (int32 i = 0; i < 2; i++)
			{
				if (c.y >= p[i] && c.y <= p[i] + Card::CardHeight)
				{
					if (i == 0)
					{
						info.enemy.place[cardX]->drawToolTip(c.x, c.y, true);
					}
					else
					{
						info.me.place[cardX]->drawToolTip(c.x, c.y, true);
					}
					break;
				}
			}
		}

	}
	renderer.renderGameover(info);
	renderer.updateToast();

}

void MainGame::dispose()
{
	info.me.dispose();
	info.enemy.dispose();
}

void MainGameRenderer::showToast(String str)
{
	if (toastWatch == NULL)
	{
		toastWatch = new Stopwatch();
	}
	toastText = str;
	toastWatch->start();
}

void MainGameRenderer::hideToast()
{
	toastWatch->pause();
	toastWatch->reset();
}

void MainGameRenderer::updateToast()
{
	if (toastWatch == NULL || !toastWatch->isRunning())
		return;

	int32 posx = 0;

	posx = Max((int64)(Scene::Width() - 260.0), (int64)(Scene::Width() - EaseOutQuad( (double)toastWatch->sF() *2.0) * 260.0));
	if (toastWatch->sF() > 0.5)
		posx = (Scene::Width() - 260.0);

	auto rectbase = Rect(posx, 20, 260, 160).draw(ColorF(0.5, 0.5));
	FontAsset(U"NormalText")(toastText).draw(rectbase.stretched(-5), Palette::White);

}

MainGameRenderer::~MainGameRenderer()
{
	if (toastWatch != NULL)
		delete toastWatch;
}

void MainGameRenderer::renderDeck(User& user, int32 posx, int32 posy)
{
	if (user.deck.size() <= 0)
		return;
	for (int32 i = 0; i < Min((int32)3, (int32)user.deck.size()); i++)
	{
		Card::DrawCardBack(posx - 3 * i, posy - 3 * i);
	}
}

Vec2 MainGameRenderer::getHandCardPos(int index, GameInfo& info)
{
	int32 posx = 20 - handScrollX;
	posx += (Card::CardWidth + 10) * index;
	const int32 defaultPosY = 480;
	int32 posy = defaultPosY;
	if (info._userHandHas == index)
		posy = defaultPosY - 40;
	return Vec2(posx, posy);
}

RenderResult MainGameRenderer::renderHandCards(GameInfo& info)
{
	const int32 maxScroll = Max(0, (int32)info.me.hand.size() - 5) * Card::CardWidth;
	handScrollX += Mouse::Wheel() * 15;
	if (Cursor::Pos().y >= 480)
	{
		if (Cursor::Pos().x <= 20)
			handScrollX -= 8;
		if (Cursor::Pos().x >= 780)
			handScrollX += 8;
	}
	handScrollX = Max(0, handScrollX);
	handScrollX = Min(maxScroll, handScrollX);

	int32 posx = 20 - handScrollX;
	const int32 defaultPosY = 480;
	int32 posy = defaultPosY;
	int32 index = 0;
	RenderResult returnVal = RenderResult::Success;

	for (auto f : info.me.hand)
	{
		posy = defaultPosY;
		if (info._userHandHas == index)
			posy = defaultPosY - 40;

		
		f->draw(posx, posy);
		if (f->getType() == CardType::Fighter)
		{
			auto ptr = (CardFighter*)f;
			Array<int> args = { ptr->hp, ptr->offence, ptr->defence };
			f->drawBadge(posx, posy, args);
		}
		if (f->contains(posx, posy, Cursor::Pos()))
		{
			const ScopedRenderStates2D state(BlendState::Additive);
			RoundRect(posx, posy, Card::CardWidth, Card::CardHeight, 3).drawFrame(10, ColorF(1.0, 1.0, 0, Periodic::Sine0_1(1s) * 0.5));
			info.tooltipCard = f;
			if (MouseL.down())
			{
				if (info._userHandHas == index)
				{
					if (f->getType() == CardType::Spell)
					{
						
						return RenderResult::RightAction;
						//returnVal = true;
					}
				}
				else
				{
					info._userHandHas = index;

					info.state = GameState::PlaceSelect;
					returnVal = RenderResult::LeftAction;
				}
			}
		}
		
		posx += f->CardWidth + 10;
		index++;
	}
	//draw scroll bar;
	Line(20, 590, 780, 590).draw(1, ColorF(1, 0.2));
	int32 scrollBarW = (int32)(800.0 / (maxScroll + 800) * 750);
	int32 scrollBarX = maxScroll > 0 ? (int32)(((double)handScrollX /maxScroll * (750 - scrollBarW))) : 0;
	Line(25 + scrollBarX, 590, 25 + scrollBarX + scrollBarW, 590).draw(7, ColorF(1, 0.4));

	if (info.state == GameState::PlaceSelect /*&& Cursor::Pos().y >= 430*/ && MouseR.down())
	{
		info._userHandHas = -1;
		info.state = GameState::MyTurnSelect;
		hideToast();
		return RenderResult::Success;
	}

	return returnVal;
}

Vec2 MainGameRenderer::getUserPlacePos(int index)
{
	int32 posx = index * (Card::CardWidth + 10) + 10;
	return Vec2(posx, 220);
}

Vec2 MainGameRenderer::getEnemyPlacePos(int index)
{
	int32 posx = index * (Card::CardWidth + 10) + 10;
	return Vec2(posx, 20);
}

RenderResult MainGameRenderer::renderPlace(User& user, GameInfo& info, int32 y)
{
	int32 i = 0;
	auto renderVal = RenderResult::Success;
	for (auto p : user.place)
	{
		if (p == NULL)
			continue;
		int32 posx = i * (Card::CardWidth + 10) + 10;
		int32 posy = y;
		p->draw(posx, posy);
		if (!p->isEmpty() && p->cards[0]->contains(posx, posy, Cursor::Pos()))
		{
			const ScopedRenderStates2D state(BlendState::Additive);
			RoundRect(posx, posy, Card::CardWidth, Card::CardHeight, 3).drawFrame(10, ColorF(1.0, 0, 0, Periodic::Sine0_1(1s) * 0.5));

			if (MouseL.down() && info._targetPlace == -1)
			{
				info._targetPlace = i;
			}
		}
		i++;
	}

	return renderVal;
}

bool MainGameRenderer::renderPlaceFrame(User& user, GameInfo& info, Card* c, int32 y)
{
	bool result = false;
	for (int32 i = 0; i < 3; i++)
	{
		if (user.place[i]->isValid() && user.place[i]->canAdd())
		{
			int32 xbegin = i * (Card::CardWidth + 10) + 10;
			auto r = Rect(xbegin, y, Card::CardWidth, Card::CardHeight);
			r.draw(ColorF(1, Periodic::Sine0_1(1s) * 0.5));

			if (MouseL.down() && r.contains(Cursor::Pos()))
			{
				info._targetPlace = i;
				result |= true;
			}

		}
	}
	return result;
}

bool MainGameRenderer::renderPlaceFrameAttack(User& enemy, GameInfo& info, Place* target, int32 targetIndex)
{
	auto y = 20;
	bool result = false;
	for (int32 i = -1; i < 2; i++)
	{
		int32 index = targetIndex + i;

		if (index < 0 || index >= 3)
			continue;

		if (!enemy.place[index]->isEmpty() && enemy.place[index]->isValid())
		{

			auto rect = Rect(index * (Card::CardWidth + 10) + 10, y, Card::CardWidth, Card::CardHeight);
			rect.draw(ColorF(1, Periodic::Sine0_1(1s) * 0.5));

			if (rect.contains(Cursor::Pos()) && MouseL.down())
			{
				info._targetAttack = index;
				result |= true;
			}

		}

		if (i != 0 )
		{
			auto rect = Rect(index * (Card::CardWidth + 10) + 10, y+200, Card::CardWidth, Card::CardHeight);
			rect.draw(ColorF(1, Periodic::Sine0_1(1s) * 0.5));
			if (rect.contains(Cursor::Pos()) && MouseL.down())
			{
				info._targetAttack = index+100;
				result |= true;
			}
		}
	}
	return result;
}

void MainGameRenderer::renderCircle(GameInfo& info)
{
	Circle(540, 160, 35).drawShadow(Vec2(1, 1), 8).draw(Palette::White);
	Circle(540, 160, 28).draw(Palette::Skyblue);
	EdgeFont::drawAt(FontAsset(U"DamageText")(Format(Max(0, info.actionCount))), 540, 160);
}

void MainGame::nextTurn(User& enemy, User& user)
{

	info.currentTurn++;

	if (info.currentTurn % 2 == 0)
	{
		GameUtil::effects->add<TurnStartEffect>(info.currentTurn % 2);

		GameUtil::Wait(2.5, [&]()
			{
				info.maxActionCount = Min(info.maxActionCount + 1, 3);
				info.actionCount = info.maxActionCount;
				info.enemy.drawCardFromDeck();
				//myAI turnStart;
				enemyAI.turnStart(enemy, user, info);

			});
	}
	else
	{
		GameUtil::effects->add<TurnStartEffect>(info.currentTurn % 2);
		GameUtil::Wait(2.5, [&]()
			{
				info.maxActionCount = Min(info.maxActionCount + 1, 3);
				info.actionCount = info.maxActionCount;
				info.me.drawCardFromDeck();
				//myAI turnStart;
			});
	}

	/*

	GameUtil::Wait(0.05, [&]()
		{
			info.currentTurn++;

			GameUtil::Wait(2.5, [&]()
				{
					info.maxActionCount = Min(info.maxActionCount + 1, 3);
					info.actionCount = info.maxActionCount;
					renderer.hideToast();
					info.enemy.drawCardFromDeck();

					enemyAI.turnStart(info.enemy, info.me, info);
					GameUtil::Wait(0.3, [&]()
						{
							info.currentTurn++;
							GameUtil::effects->add<TurnStartEffect>(info.currentTurn % 2);

							GameUtil::Wait(2.5, [&]()
								{
									info.maxActionCount = Min(info.maxActionCount + 1, 3);
									info.actionCount = info.maxActionCount;
									info.me.drawCardFromDeck();
								});
						});
				});
			GameUtil::effects->add<TurnStartEffect>(info.currentTurn % 2);
		});
	//nextTurn

	//Logger::log(U"Your Turn");
	*/
}

void MainGameRenderer::renderKillCounts(User& user, User& enemy)
{
	//enemy.killCount = 1;
	for (int32 i = 0; i < 3; i++)
	{
		Circle(460, 30 + i * 40, 10).drawShadow(Vec2(1, 1), 8).draw(Palette::White);
		if (i < enemy.killCount)
		{
			Circle(460, 30 + i * 40, 6).draw(Palette::Indianred);
		}
	}
	for (int32 i = 0; i < 3; i++)
	{
		Circle(460, 230 + i * 40, 10).drawShadow(Vec2(1, 1), 8).draw(Palette::White);
		if (i < user.killCount)
		{
			Circle(460, 230 + i * 40, 6).draw(Palette::Indianred);
		}
	}
}

void MainGameRenderer::renderGameover(GameInfo& info)
{
	if (!info.gameOver)
		return;
	Rect(0, 0, Scene::Width(), Scene::Height()).draw(ColorF(0.2, 0.5));
	String str = info.win ? U"You Win": U"You Lose";
	FontAsset(U"NormalText")(str).drawAt(Scene::Center());
}

void MainGameRenderer::renderTurnStart(GameInfo& info)
{
	if (info.currentTurn)
	{

	}
}

void MainGameRenderer::renderButtons(GameInfo& info)
{
	if (info.currentTurn % 2 == 0)
	{
		auto rect = Rect(630, 330, 160, 60).drawShadow(Vec2(2,2), 8, 1).draw(ColorF(0.8));
		if (rect.contains(Cursor::Pos()))
		{
			rect.drawFrame(4, ColorF(1, Periodic::Sine0_1(1s)));
		}
		EdgeFont::drawAt(FontAsset(U"NormalText")(U"次の\nターンへ"), 630 + 80, 330 + 30);
	}
}