#pragma once
#include "ofMain.h"
#include "utils.h"

class Block
{
private:
	Vec2 pos;

	
public:
	virtual Block(Vec2 pos) : pos{ pos } {};
};
