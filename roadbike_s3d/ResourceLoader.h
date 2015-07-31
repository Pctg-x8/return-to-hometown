#pragma once

#include <Siv3D.hpp>

// Loading All Resources //

class ResourceLoader;	// forward reference
class TextureResourceLoader
{
	friend class ResourceLoader;

	Texture playerPattern, playerHumanPattern;
	Texture mcTop, mcRoad, mcGrass, mcGrassUnder, mcUnder;
	Texture bkFrontSun, bkBehindSun, bkSun;
	Array<Texture> bkBuildings;
	Array<Texture> hurdles;
	Texture titleTexture;
	TextureRegion tMain, tSub, tSpace;

	TextureResourceLoader();
	void dividePlayerGraphs(const Image& baseImage);
	void divideMapchipGraphs(const Image& baseImage);
	void divideBackgroundGraphs(const Image& baseImage);
	void divideHurdleGraphs(const Image& baseImage);
	void initTitleTexture(const FilePath& pathImage);

	// utility method //
	static Image clipImage(const Image& baseImage, const RectF& rectClip, const ColorF& transColor);
public:
	// getters //
	const decltype(playerPattern)& getPlayerTexture() const { return this->playerPattern; }
	const decltype(playerHumanPattern)& getPlayerBodyTexture() const { return this->playerHumanPattern; }

	const Texture& getMapchipTexture(size_t index) const;
	const decltype(bkFrontSun)& getBackgroundTextureFrontSun() const { return this->bkFrontSun; }
	const decltype(bkSun)& getBackgroundSunTexture() const { return this->bkSun; }
	const decltype(bkBehindSun)& getBackgroundTextureBehindSun() const { return this->bkBehindSun; }
	const Texture& getBackgroundBuildingTexture(size_t index) const { return this->bkBuildings[index]; }

	const Texture& getHurdleTexture(size_t index) const { return this->hurdles[index]; }
	const decltype(tMain)& getMainTitleTexture() const { return this->tMain; }
	const decltype(tSub)& getSubTitleTexture() const { return this->tSub; }
	const decltype(tSpace)& getSpacePressTexture() const { return this->tSpace; }
};
class SoundResourceLoader
{
	friend class ResourceLoader;

	SoundResourceLoader();
public:
	static const String decide, jump, died;
};
class FontResourceLoader
{
	friend class ResourceLoader;

	FontResourceLoader();
public:
	static const String score, gameover, gameoverMini, result;
};

class ResourceLoader
{
	TextureResourceLoader textureLoader;
	SoundResourceLoader soundLoader;
	FontResourceLoader fontLoader;
public:
	ResourceLoader();

	const TextureResourceLoader& getTextures() const { return this->textureLoader; }
	const SoundResourceLoader& getSoundEffects() const { return this->soundLoader; }
	const FontResourceLoader& getFonts() const { return this->fontLoader; }
};