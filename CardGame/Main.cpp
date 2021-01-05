# include <Siv3D.hpp> // OpenSiv3D v0.4.1

#include "MainGame.hpp"
#include "BackgroundRenderer.h"

struct LogEffect : IEffect
{
	String text;
	LogEffect(const String str)
		: text(str) {}

	bool update(double t) override
	{
		Rect r = FontAsset(U"NormalText")(text).region();
		r.w += 10;
		if(t < 0.5)
			EdgeFont::draw(FontAsset(U"NormalText")(text), Scene::Width() - r.w / 0.5 * t, Scene::Height() - r.h);
		else if(t < 2)
			EdgeFont::draw(FontAsset(U"NormalText")(text), Scene::Width() - r.w, Scene::Height() - r.h);
		else
			EdgeFont::draw(FontAsset(U"NormalText")(text), Scene::Width() - r.w, Scene::Height() - r.h + (t-2.0) * (r.h / 1.0));

		return t < 3.0;
	}
};

struct Logger
{
	static void log(String str)
	{
		GameUtil::effects->add<LogEffect>(str);
	}
};


void Main()
{
	// 背景を水色にする
	//Scene::SetBackground(ColorF(0.8, 0.9, 1.0));

	Window::SetTitle(U"CardGame");

	// 大きさ 60 のフォントを用意
	//const Font font(60);

	// 猫のテクスチャを用意
	//const Texture cat(Emoji(U"🐈"));

	// 猫の座標
	//Vec2 catPos(640, 450);
	GameUtil::Load();

	MainGame game;
	BackgroundRenderer background;
	while (System::Update())
	{
		ClearPrint();
		Print << Cursor::Pos();
		//Print << GameUtil::effects->num_effects();
		background.render();
		//draw yamahuda
		game.update();
		GameUtil::Update();
	}

	game.dispose();
	Card::UnLoadCards();
	GameUtil::Unload();
}

//
// = アドバイス =
// Debug ビルドではプログラムの最適化がオフになります。
// 実行速度が遅いと感じた場合は Release ビルドを試しましょう。
// アプリをリリースするときにも、Release ビルドにするのを忘れないように！
//
// 思ったように動作しない場合は「デバッグの開始」でプログラムを実行すると、
// 出力ウィンドウに詳細なログが表示されるので、エラーの原因を見つけやすくなります。
//
// = お役立ちリンク =
//
// OpenSiv3D リファレンス
// https://siv3d.github.io/ja-jp/
//
// チュートリアル
// https://siv3d.github.io/ja-jp/tutorial/basic/
//
// よくある間違い
// https://siv3d.github.io/ja-jp/articles/mistakes/
//
// サポートについて
// https://siv3d.github.io/ja-jp/support/support/
//
// Siv3D Slack (ユーザコミュニティ) への参加
// https://siv3d.github.io/ja-jp/community/community/
//
// 新機能の提案やバグの報告
// https://github.com/Siv3D/OpenSiv3D/issues
//
