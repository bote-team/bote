#pragma once

struct GameResources
{
public:
	GameResources(void);
	GameResources(const GameResources& o);
	GameResources(int Food_, int Titan_, int Deuterium_, int Duranium_, int Crystal_, int Iridium_, int Deritium_);
	~GameResources(void);

	void Serialize(CArchive& ar);

	void Cap();

	void Reset();

	GameResources& operator=(const GameResources& o);

	GameResources& operator*=(double factor);
	GameResources& operator+=(const GameResources& add);

public:
	int Food;
	int Titan;
	int Deuterium;
	int Duranium;
	int Crystal;
	int Iridium;
	int Deritium;
};
