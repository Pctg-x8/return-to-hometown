#include "Map.h"

void Map::init(int w, int h)
{
	this->map_data = Grid<unsigned int>(w, h);
}
void Map::clean()
{
	this->map_data.clear();
}
void Map::set(int x, int y, int d)
{
	this->map_data.at(y, x) = d;
}
int Map::get(int x, int y)
{
	while (y < 0) y += this->map_data.height;
	while (x < 0) x += this->map_data.width;

	return this->map_data.at(y % this->map_data.height, x % this->map_data.width);
}
int Map::getAt(double x, double y){ return this->get(Math::Floor(x / 16), Math::Floor(y / 16)); }
void Map::render(const std::function<void(int, int, int)>& renderFunc)
{
	for (int x = 0; x < this->map_data.width; x++)
	{
		for (int y = 0; y < this->map_data.height; y++)
		{
			if (map_data.at(y, x) > 0) renderFunc(x, y, map_data.at(y, x));
		}
	}
}