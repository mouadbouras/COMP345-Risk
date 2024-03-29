#ifndef CONTINENT_H
#define CONTINENT_H

#include "stdafx.h"
#include <string>
#include <vector>
#include "Country.h"
#include "Player.h"

using namespace std;

class Continent {
public:
	Continent();
	Continent(string name);
	Continent(const Continent &c);
	~Continent();
	string getContinentName();
	vector< Country > getCountries();
	int getNumCountries();
	Player* getOwner();
	int getArmyBonus();
	int getCaptureCount();

	void setContinentName(string name);
	void setCountries(vector< Country > list);
	void setOwner(Player* o);
	void setArmyBonus(int b);
	void setCaptureCount(int cc);

	void addCountry(Country c);
	bool Continent::isPlayerOwned();

private:

	string continentName;
	vector< Country > countries;
	Player* owner;
	int armyBonus;
	int captureCount;
};

#endif