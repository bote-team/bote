#include "stdafx.h"
#include "GlobalTypes.h"
#include "Constants.h"

GameResources::GameResources(void)
{
	Reset();
}

GameResources::GameResources(const GameResources& o) :
	Food(o.Food),
	Titan(o.Titan),
	Deuterium(o.Deuterium),
	Duranium(o.Duranium),
	Crystal(o.Crystal),
	Iridium(o.Iridium),
	Deritium(o.Deritium)
{
}

GameResources::GameResources(
		int Food_, int Titan_, int Deuterium_, int Duranium_, int Crystal_, int Iridium_, int Deritium_) :
	Food(Food_),
	Titan(Titan_),
	Deuterium(Deuterium_),
	Duranium(Duranium_),
	Crystal(Crystal_),
	Iridium(Iridium_),
	Deritium(Deritium_)
{
}

GameResources::~GameResources(void)
{
}

void GameResources::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar << Food;
		ar << Titan;
		ar << Deuterium;
		ar << Duranium;
		ar << Crystal;
		ar << Iridium;
		ar << Deritium;
	}
	else
	{
		ar >> Food;
		ar >> Titan;
		ar >> Deuterium;
		ar >> Duranium;
		ar >> Crystal;
		ar >> Iridium;
		ar >> Deritium;
	}
}

void GameResources::Cap()
{
	// Lagerobergrenzen
	if (Food > MAX_FOOD_STORE)
		Food = MAX_FOOD_STORE;
	if (Titan > MAX_RES_STORE)
		Titan = MAX_RES_STORE;
	if (Deuterium > MAX_RES_STORE)
		Deuterium = MAX_RES_STORE;
	if (Duranium > MAX_RES_STORE)
		Duranium = MAX_RES_STORE;
	if (Crystal > MAX_RES_STORE)
		Crystal = MAX_RES_STORE;
	if (Iridium > MAX_RES_STORE)
		Iridium = MAX_RES_STORE;
}

void GameResources::Reset()
{
	Food = 1000;
	Titan = 0;
	Deuterium = 0;
	Duranium = 0;
	Crystal = 0;
	Iridium = 0;
	Deritium = 0;
}

GameResources& GameResources::operator=(const GameResources& o)
{
	Food = o.Food;
	Titan = o.Titan;
	Deuterium = o.Deuterium;
	Duranium = o.Duranium;
	Crystal = o.Crystal;
	Iridium = o.Iridium;
	Deritium = o.Deritium;

	return *this;
}

GameResources& GameResources::operator*=(double factor) {
	Food *= factor;
	Titan *= factor;
	Deuterium *= factor;
	Duranium *= factor;
	Crystal *= factor;
	Iridium *= factor;
	Deritium *= factor;
	return *this;
}

GameResources& GameResources::operator+=(const GameResources& add) {
	Food += add.Food;
	Titan += add.Titan;
	Deuterium += add.Deuterium;
	Duranium += add.Duranium;
	Crystal += add.Crystal;
	Iridium += add.Iridium;
	Deritium += add.Deritium;
	return *this;
}