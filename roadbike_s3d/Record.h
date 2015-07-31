#pragma once

#include <Siv3D.hpp>

class Record
{
	int recorded;
	bool is_updated;
public:
	static Record& instance()
	{
		static Record o;
		return o;
	}

	void init();
	int getCurrentMaxScore();
	void recordScore(int s);
	bool isUpdated(){ return this->is_updated; }
};
