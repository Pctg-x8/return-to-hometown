#include "ResourceLoader.h"

const auto tsTransColor = ColorF(0.0f, 0.5f, 0.5f);
TextureResourceLoader::TextureResourceLoader()
{
	// Load setted graphs //
	const auto player_graph = Image(L"res/player.bmp");
	const auto mapchip_graph = Image(L"res/mapchip.bmp");
	const auto background_graph = Image(L"res/background.bmp");
	const auto hurdle_graph = Image(L"res/hurdles.bmp");

	// Divide graphs //
	this->dividePlayerGraphs(player_graph);
	this->divideMapchipGraphs(mapchip_graph);
	this->divideBackgroundGraphs(background_graph);
	this->divideHurdleGraphs(hurdle_graph);

	// init textures //
	this->initTitleTexture(L"res/title.png");
}

void TextureResourceLoader::dividePlayerGraphs(const Image& baseImage)
{
	this->playerPattern			= Texture(clipImage(baseImage, RectF(Vec2( 6.0, 6.0), Vec2(25.0, 17.0)), tsTransColor));
	this->playerHumanPattern	= Texture(clipImage(baseImage, RectF(Vec2(35.0, 6.0), Vec2(24.0, 27.0)), tsTransColor));
}
void TextureResourceLoader::divideMapchipGraphs(const Image& baseImage)
{
	const auto mcSize = Vec2(17.0, 17.0);

	this->mcTop			= Texture(clipImage(baseImage, RectF(Vec2(5.0, 5.0), mcSize), tsTransColor));
	this->mcRoad		= Texture(clipImage(baseImage, RectF(Vec2(5.0 + 20.0, 5.0), mcSize), tsTransColor));
	this->mcGrass		= Texture(clipImage(baseImage, RectF(Vec2(5.0 + 40.0, 5.0), mcSize), tsTransColor));
	this->mcGrassUnder	= Texture(clipImage(baseImage, RectF(Vec2(5.0 + 60.0, 5.0), mcSize), tsTransColor));
	this->mcUnder		= Texture(clipImage(baseImage, RectF(Vec2(5.0 + 80.0, 5.0), mcSize), tsTransColor));
}
void TextureResourceLoader::divideBackgroundGraphs(const Image& baseImage)
{
	const auto bkSize = Vec2(321.0f, 241.0f);

	this->bkFrontSun	=  Texture(clipImage(baseImage, RectF(Vec2( 334.0, 10.0), bkSize), tsTransColor));
	this->bkSun			=  Texture(clipImage(baseImage, RectF(Vec2(1630.0, 10.0), Vec2(97.0, 97.0)), tsTransColor));
	this->bkBehindSun	=  Texture(clipImage(baseImage, RectF(Vec2( 658.0, 10.0), bkSize), tsTransColor));
	this->bkBuildings.emplace_back(clipImage(baseImage, RectF(Vec2( 982.0, 10.0), bkSize), tsTransColor));
	this->bkBuildings.emplace_back(clipImage(baseImage, RectF(Vec2(1306.0, 10.0), bkSize), tsTransColor));
}
void TextureResourceLoader::divideHurdleGraphs(const Image& baseImage)
{
	const auto hdSize = Vec2(17.0, 17.0);

	this->hurdles.emplace_back(clipImage(baseImage, RectF(Vec2(6.0, 6.0), hdSize), tsTransColor));
}
void TextureResourceLoader::initTitleTexture(const String& pathImage)
{
	this->titleTexture	= Texture(pathImage);
	this->tMain			= this->titleTexture.uv(RectF(Vec2(  0.0,  0.0), Vec2(384.0, 64.0)));
	this->tSub			= this->titleTexture.uv(RectF(Vec2(384.0, 24.0), Vec2( 88.0, 36.0)));
	this->tSpace		= this->titleTexture.uv(RectF(Vec2( 16.0, 72.0), Vec2(224.0, 18.0)));
}

Image TextureResourceLoader::clipImage(const Image& baseImage, const RectF& rectClip, const ColorF& transColor)
{
	// from Siv3D Slack

	auto new_image = baseImage.clip(rectClip);
	for (auto& p : new_image) if (p == transColor) p = Alpha(0);
	return new_image;
}
const Texture& TextureResourceLoader::getMapchipTexture(size_t index) const
{
	switch (index)
	{
	case 1: return this->mcGrass;
	case 2: return this->mcRoad;
	case 3: return this->mcGrass;
	case 4: return this->mcGrassUnder;
	case 5: return this->mcUnder;
	default: assert(false);
	}
}

const String SoundResourceLoader::decide = L"decide";
const String SoundResourceLoader::jump = L"jump";
const String SoundResourceLoader::died = L"died";
SoundResourceLoader::SoundResourceLoader()
{
	SoundAsset::Register(SoundResourceLoader::decide, L"res/kettei.wav");
	SoundAsset::Register(SoundResourceLoader::jump, L"res/jump.wav");
	SoundAsset::Register(SoundResourceLoader::died, L"res/owari.wav");
}

const String FontResourceLoader::score = L"ScoreFont";
const String FontResourceLoader::gameover = L"GameoverFont";
const String FontResourceLoader::gameoverMini = L"GameoverFontMini";
const String FontResourceLoader::result = L"ResultFont";
FontResourceLoader::FontResourceLoader()
{
	FontAsset::Register(FontResourceLoader::score, 12);
	FontAsset::Register(FontResourceLoader::gameover, 30);
	FontAsset::Register(FontResourceLoader::gameoverMini, 12);
	FontAsset::Register(FontResourceLoader::result, 16);
}

ResourceLoader::ResourceLoader()
	: textureLoader(), soundLoader(), fontLoader()
{

}