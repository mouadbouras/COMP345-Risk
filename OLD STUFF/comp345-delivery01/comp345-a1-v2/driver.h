/* 
 * Driver class. Implements the entire game loop within itself, to be called once in the program's
 * main function with run().
 */

#include "player.h"
#include <vector>
#include "Country.h"
#include"Continent.h"
#include"Fortification.h"
#include "SelectandBattle.h"
#include "MapManager.h"

#ifndef DRIVER_H
#define DRIVER_H
class driver {
private:
	int numberofplayers;
	std::vector<Player> playerlist;
	MapManager mapmanager;
	Country countries[100];
	Continent continents[50];
public:
	driver();
	void reinforce(Player& p);
	void attack(Player& p);
	void fortify(Player& p);
	void run();
};
#endif