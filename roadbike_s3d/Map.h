#pragma once

#include <Siv3D.hpp>

class Map
{
	Map() : map_data(40, 15) {}
	~Map(){ this->clean(); }
	
	Grid<unsigned int> map_data;
public:
	static Map& instance()
	{
		static Map o;
		return o;
	}

	void clean();
	unsigned int width(){ return this->map_data.width; }

	void init(int w, int h);
	void set(int x, int y, int d);
	int get(int x, int y);
	int getAt(double x, double y);
	void render(const std::function<void(int, int, int)>& renderFunc);
};
