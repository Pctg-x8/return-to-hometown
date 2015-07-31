#include <Siv3D.hpp>

#include "ScriptEngine.h"

#include "Record.h"
#include "Map.h"
#include <algorithm>

#include "ResourceLoader.h"

// 10HGJ3(テーマ:街)

class HurdleObject
{
	Vec2 posGlobal;
	bool hasDead;
public:
	HurdleObject() : hasDead(false){}
	HurdleObject(const Vec2& p)
	{
		this->posGlobal = p;
		this->hasDead = false;
	}

	Vec2 getPos() const { return this->posGlobal; }
	bool hasDeadObject() const { return this->hasDead; }
	void die(){ this->hasDead = true; }
};
class BuildingLayer
{
	Vec2 posLocal;
	bool hasDead;
public:
	BuildingLayer() : hasDead(false){}
	BuildingLayer(const Vec2& p)
	{
		this->posLocal = p;
		this->hasDead = false;
	}

	void move(const Vec2& offs) { this->posLocal.x += offs.x; this->posLocal.y += offs.y; }

	Vec2 getPos() const { return this->posLocal; }
	bool hasDeadObject() const { return this->hasDead; }
	void die(){ this->hasDead = true; }
};
class BuildingLayer2
{
	Vec2 posLocal;
	bool hasDead;
public:
	BuildingLayer2() : hasDead(false){}
	BuildingLayer2(const Vec2& p)
	{
		this->posLocal = p;
		this->hasDead = false;
	}

	void move(const Vec2& offs) { this->posLocal.x += offs.x; this->posLocal.y += offs.y; }

	Vec2 getPos() const { return this->posLocal; }
	bool hasDeadObject() const { return this->hasDead; }
	void die(){ this->hasDead = true; }
};

enum ScreenState
{
	InTitle,
	Gaming,
	Result
};
void renderBackgrounds(double viewLeft, const ResourceLoader& resources);
void renderBuildingLayers(Array<BuildingLayer>& layer1, Array<BuildingLayer2>& layer2, const ResourceLoader& resources);
void renderMap(double viewOffset, const ResourceLoader& resources);
bool renderHurdlesAndHitTest(double viewOffset, const Vec2& mychar_pos, Array<HurdleObject>& objects, const ResourceLoader& resources);
void renderPlayer(double viewOffset, double playerAngle, const Vec2& playerPos, const ResourceLoader& resources);
void renderInfo(int score);
void renderResult(int score, bool updated, int fc);
void renderTitle(double frameCount, const ResourceLoader& resources);
void renderCurrentMax();

int toScore(double mychar_left)
{
	return int((mychar_left - 160.0) / 4.0);
}

void PlayJumpSound()
{
	SoundAsset(L"jump").playMulti();
}

void Main()
{
	ResourceLoader resources;

	auto sampler = SamplerState(SamplerState::Default2D);
	sampler.filter = TextureFilter::MinMagMipPoint;
	Graphics2D::SetSamplerState(sampler);

	Window::Resize(320 * 2, 240 * 2);
	Window::SetTitle(L"まちへかえろう(仮)");

	Array<HurdleObject> hurdles;
	Array<BuildingLayer> buildingLayers;
	Array<BuildingLayer2> buildingLayers2;

	Map::instance().init(40, 15);

	for (int i = 0; i < 40; i++)
	{
		Map::instance().set(i, 14, 5);
		Map::instance().set(i, 13, 5);
		Map::instance().set(i, 12, 4);
		Map::instance().set(i, 11, 3);
		Map::instance().set(i, 10, 3);
		Map::instance().set(i, 9, 2);
		Map::instance().set(i, 8, 1);
	}
	auto viewLeft = 0.0;
	auto mycharDead = false;
	ScreenState currentState = ScreenState::InTitle, prevState = currentState;
	bool prevSpacePressed = false;

	auto TitleTimer = TimerMillisec();

	ScriptEngine asEngine;
	asEngine.RegisterMethod(L"int getAt(double x, double y)", &Map::getAt, Map::instance());
	asEngine.RegisterProperty(L"double _viewX", &viewLeft);
	asEngine.RegisterProperty(L"bool _isGameOvered", &mycharDead);
	asEngine.RegisterFunction(L"void playJumpSound()", PlayJumpSound);
	auto context_mychar = asEngine.CreateContextFromScript(L"res/scripts/Mychar.as");

	Record::instance().init();

	int fc = 0;
	while (System::Update())
	{
		bool spacePressedInFrame = !prevSpacePressed && Input::KeySpace.pressed;
		prevSpacePressed = Input::KeySpace.pressed;

		asEngine.setFrameCount(fc);

		switch (currentState)
		{
		case ScreenState::InTitle:
			if (!TitleTimer.isActive) TitleTimer.start();
			renderBackgrounds(viewLeft, resources);
			renderBuildingLayers(buildingLayers, buildingLayers2, resources);
			renderMap(viewLeft, resources);
			renderTitle(TitleTimer.elapsed(), resources);
			renderCurrentMax();

			if (spacePressedInFrame)
			{
				// GameInit
				SoundAsset(SoundResourceLoader::decide).play();
				viewLeft = 0.0;
				mycharDead = false;
				context_mychar.Reenter();
				buildingLayers.clear();
				buildingLayers2.clear();
				hurdles.clear();
				currentState = ScreenState::Gaming;
			}
			break;
		case ScreenState::Gaming:
			// appear hurdles at random
			if (RandomBool(0.02))
			{
				if (hurdles.empty() || hurdles.back().getPos().x - (viewLeft + 320.0f) < -48.0f)
				{
					// allocate space least 48 logical-pixels
					hurdles.emplace_back(HurdleObject(Vec2(viewLeft + 320.0f, 8.0f * 16.0f + 8.0f)));
				}
			}
			// appear building at random
			if (RandomBool(0.02 / 30.0))
			{
				if (buildingLayers.empty() || buildingLayers.back().getPos().x < 0.0f)
				{
					buildingLayers.emplace_back(BuildingLayer(Vec2(320.0f, -60.0f)));
				}
			}
			if (RandomBool(0.01 / 30.0))
			{
				if (buildingLayers2.empty() || buildingLayers2.back().getPos().x < 0.0f)
				{
					buildingLayers2.emplace_back(BuildingLayer2(Vec2(320.0f, -60.0f)));
				}
			}

			asEngine.setFrameCount(fc);
			context_mychar.Execute();

			renderBackgrounds(viewLeft, resources);
			renderBuildingLayers(buildingLayers, buildingLayers2, resources);
			renderMap(viewLeft, resources);
			if (renderHurdlesAndHitTest(viewLeft, context_mychar.GetGlobalDoubleVec2(L"x", L"y"), hurdles, resources))
			{
				if (!mycharDead) SoundAsset(SoundResourceLoader::died).play();
				mycharDead = true;
			}
			renderPlayer(viewLeft, context_mychar.GetGlobalDouble(L"angle"), context_mychar.GetGlobalDoubleVec2(L"x", L"y"), resources);
			renderInfo(toScore(context_mychar.GetGlobalDouble(L"x")));

			// Host Processes
			if (mycharDead && context_mychar.GetGlobalDouble(L"x") < viewLeft - 120.0f)
			{
				// update record
				Record::instance().recordScore(toScore(context_mychar.GetGlobalDouble(L"x")));
				currentState = ScreenState::Result;
			}
			break;
		case ScreenState::Result:
			renderBackgrounds(viewLeft, resources);
			renderBuildingLayers(buildingLayers, buildingLayers2, resources);
			renderMap(viewLeft, resources);
			renderHurdlesAndHitTest(viewLeft, context_mychar.GetGlobalDoubleVec2(L"x", L"y"), hurdles, resources);
			renderResult(toScore(context_mychar.GetGlobalDouble(L"x")), Record::instance().isUpdated(), fc);

			if (spacePressedInFrame)
			{
				// GameInit
				SoundAsset(SoundResourceLoader::decide).play();
				TitleTimer.reset();
				currentState = ScreenState::InTitle;
			}
			break;
		}
		if (prevState != currentState)
		{
			fc = 0;
		}
		prevState = currentState;

		// Common Postprocesses
		fc++;
		viewLeft += 2.5f;
	}

	Map::instance().clean();
}

void renderBackgrounds(double viewLeft, const ResourceLoader& resources)
{
	// render background
	auto tBehindSun = resources.getTextures().getBackgroundTextureBehindSun().scale(2);
	auto tSun = resources.getTextures().getBackgroundSunTexture().scale(2);
	auto tFrontSun = resources.getTextures().getBackgroundTextureFrontSun().scale(2);

	auto map_offset_slow = Math::Fmod(viewLeft * 0.1f, Map::instance().width() * 8.0);
	auto map_offset_slowest = Math::Fmod(viewLeft * 0.05f, 320.0 + 96.0);

	tBehindSun.draw(Vec2(int(-map_offset_slow), 0.0f) * 2);
	tBehindSun.draw(Vec2(int(320.0f - map_offset_slow), 0.0f) * 2);
	tSun.draw(Vec2(int(320.0f - map_offset_slowest) * 2, Window::Height() / 3.0));
	tFrontSun.draw(Vec2(int(-map_offset_slow), 0.0f) * 2);
	tFrontSun.draw(Vec2(int(320.0f - map_offset_slow), 0.0f) * 2);
}
void renderBuildingLayers(Array<BuildingLayer>& layer1, Array<BuildingLayer2>& layer2, const ResourceLoader& resources)
{
	// building layers
	auto tBuildingLayer1 = resources.getTextures().getBackgroundBuildingTexture(0).scale(2);
	auto tBuildingLayer2 = resources.getTextures().getBackgroundBuildingTexture(1).scale(2);

	for (auto& o : layer1)
	{
		o.move(Vec2(-2.5f * 0.3f, 0.0));
		tBuildingLayer1.draw(o.getPos() * 2);
		if (o.getPos().x < -320.0) o.die();
	}
	Erase_if(layer1, [](const BuildingLayer& l){ return l.hasDeadObject(); });
	for (auto& o : layer2)
	{
		o.move(Vec2(-2.5f * 0.5f, 0.0));
		tBuildingLayer2.draw(o.getPos() * 2);
		if (o.getPos().x < -320.0) o.die();
	}
	Erase_if(layer2, [](const BuildingLayer2& l){ return l.hasDeadObject(); });
}
void renderMap(double viewOffset, const ResourceLoader& resources)
{
	// map
	Map::instance().render([&](int x, int y, int d)
	{
		resources.getTextures().getMapchipTexture(d)
			.scale(2).draw(Vec2(Math::Floor(x * 16.0f - Math::Fmod(viewOffset, Map::instance().width() * 8.0)), y * 16.0f) * 2);
	});
}
bool renderHurdlesAndHitTest(double viewOffset, const Vec2& mychar_pos, Array<HurdleObject>& objects, const ResourceLoader& resources)
{
	auto tHurdle = resources.getTextures().getHurdleTexture(0).scale(2);

	auto isDead = false;
	// hurdles
	for (auto& o : objects)
	{
		// hit test between player
		if (Math::Abs(o.getPos().x - mychar_pos.x) <= 12 && Math::Abs(o.getPos().y - mychar_pos.y) <= (8 + 8))
		{
			// hit
			isDead = true;
		}
		tHurdle.draw((o.getPos() - Vec2(8 + viewOffset, 4)) * 2);
		if (o.getPos().x < viewOffset - 8) o.die();
	}
	auto itr = std::remove_if(std::begin(objects), std::end(objects), [](const HurdleObject& o){ return o.hasDeadObject(); });
	objects.erase(itr, std::end(objects));

	return isDead;
}
void renderPlayer(double viewOffset, double playerAngle, const Vec2& playerPos, const ResourceLoader& resources)
{
	// player render
	auto tPlayer = resources.getTextures().getPlayerTexture().scale(2);
	auto tPlayerBody = resources.getTextures().getPlayerBodyTexture().scale(2);

	tPlayer.rotateAt(Vec2(12, 8) * 2, Math::Radians(playerAngle)).draw((playerPos - Vec2(12 + viewOffset, 8)) * 2);
	tPlayerBody.rotateAt(Vec2(12, 13) * 2, Math::Radians(playerAngle)).draw((playerPos - Vec2(12 + viewOffset, 18)) * 2);
}
void renderInfo(int score)
{
	// hud
	FontAsset(L"ScoreFont")(L"距離:", score, L" m").draw(Vec2(2, 2) * 2, Palette::White);
}
void renderResult(int score, bool updated, int fc)
{
	FontAsset(L"GameoverFont")(L"GAME OVER!!").drawCenter(Vec2(Window::Width() / 2.0, Window::Height() / 3.0), Palette::Red);
	FontAsset(L"ResultFont")(L"最終記録:", score, L" m").drawCenter(Vec2(Window::Width() / 2.0, Window::Height() / 3.0 * 2));
	if(updated && fc % 2 == 0) FontAsset(L"ResultFont")(L"記録更新！").drawCenter(Vec2(Window::Width() / 2.0, Window::Height() / 3.0 * 2 + 24.0f), Palette::Yellow);
	FontAsset(L"GameoverFontMini")(L"スペースキーでタイトルに戻る").drawCenter(Vec2(Window::Width() / 2.0, Window::Height() / 4.0 * 3.0 + 16.0f));
}
void renderTitle(double elapsedTime, const ResourceLoader& resources)
{
	auto tMain = resources.getTextures().getMainTitleTexture();
	auto tSub = resources.getTextures().getSubTitleTexture();
	auto tSpace = resources.getTextures().getSpacePressTexture();

	auto main_y = elapsedTime < 1000 ? (elapsedTime / 1000.0) * (Window::Height() / 4.0f * 2.0f) - Window::Height() / 4.0f : Window::Height() / 4.0f;
	auto sub_y = 0.0;
	auto sub_angle = 0.0;
	auto main_w = tMain.size.x;

	if (elapsedTime < 1500) sub_y = -Window::Height() / 4.0f;
	else if (elapsedTime < 2500) sub_y = EaseOut(Easing::Bounce, (elapsedTime - 1500) / 1000.0) * (Window::Height() / 4.0f * 2.0f) - Window::Height() / 4.0f + 24.0f;
	else sub_y = Window::Height() / 4.0f + 24.0f;

	if (elapsedTime < 3000) sub_angle = 0.0;
	else if (elapsedTime < 3200) sub_angle = ((elapsedTime - 3000) / 200) * 5.0f;
	else sub_angle = 5.0f;

	tMain.draw(Vec2((Window::Width() - main_w) / 2.0f, main_y));
	tSub.rotateAt(Vec2(0.0f, tSub.size.y), Math::Radians(sub_angle)).draw(Vec2((Window::Width() + main_w) / 2.0f, sub_y));

	if (elapsedTime >= 4000)
	{
		if (Math::Fmod(elapsedTime, 1000.0) < 500.0)
		{
			tSpace.draw(Vec2((Window::Width() - tSpace.size.x) / 2.0, Window::Height() / 4.0 * 3.0));
		}
	}
}
void renderCurrentMax()
{
	FontAsset(L"ScoreFont")(L"現在の最長距離:", Record::instance().getCurrentMaxScore(), L" m").draw(Vec2(2, 2) * 2, Palette::White);
}
