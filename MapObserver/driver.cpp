// driver class implementation.

#include "driver.h"
#include "logger.h"
#include "gamelogger.h"
#include "playerlogger.h"
#include "phaselogger.h"
#include "Fortification.h"
#include "Continent.h"
#include "Reinforcement.h"
#include "SelectandBattle.h"
#include "MapManager.h"
#include "PlayerSubject.h"
#include "PlayerView.h"
#include "BattlesWonDecorator.h"
#include "WorldControlledDecorator.h"
#include "GameStatDecorator.h"
#include "GameStatObserver.h"
#include "GameStatSubject.h"

//Map observer
#include "MapView.h"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>

using namespace std;

// Default constructor
driver::driver() {
}

/* Function for Reinforcement phase. Player is given armies at the beginning of each of their turns,
and is asked if they wish to send any extras to their countries. param p is a reference to the player
whose turn it is. */
void driver::reinforce(Player& p) {
	
	


	stringstream phaselog; // add text to this log throughout the phase, then send to game logger at the end

	cout << "--Reinforcement Phase--" << endl;
	phaselog << "--Reinforcement Phase--" << endl;
	cout << "REINFORCE? (enter anything to continue, blank to skip): ";
	string input = "";
	getline(cin, input);

	// skip reinforcement if left blank
	if (input.empty()) {
		cout << "Skipping reinforcement phase.\n" << endl;
		phaselog << "skipping reinforcement phase." << endl;
		notify(phaselog.str(), p.getPlayerName(), "reinforcement");
		return;
	}

	// otherwise, select number of armies to move and execute
	else {
		Reinforcement rein(p, countries, continents, cardcounter);
		rein.calculateReinforcement();
		rein.calculateCardsReinforcement();
		phaselog << rein.ReinforcementPhase();
		notify(phaselog.str(), p.getPlayerName(), "reinforcement");		
	}
}

/* Function for Attack phase. Asks the player if they wish to attack any countries, and if yes,
lets them choose which country to attack with. param p is a reference to the player whose turn it is. */
void driver::attack(Player& p) {
	stringstream phaselog;

	cout << "--Attack Phase--" << endl;
	phaselog << "--Attack Phase--" << endl;
	cout << "ATTACK? (enter anything to continue, blank to skip): ";
	string target = "";
	getline(cin, target);

	// skip attack if left blank
	if (target.empty()) {
		cout << "Skipping attack phase.\n" << endl;
		phaselog << "Skipping attack phase." << endl;
		notify(phaselog.str(), p.getPlayerName(), "attack");
		return;
	}

	// otherwise, attack the other country
	else {
		SelectandBattle newbattle(p, countries);
		phaselog << newbattle.AttackPhase();

		notify(phaselog.str(), p.getPlayerName(), "attack");
	}
}

/* Function for Fortification phase. Lets the player choose to move armies from one of their owned
countries to another. param p is a reference to the player whose turn it is. */
void driver::fortify(Player& p) {
	string source;
	string target;
	stringstream phaselog;

	cout << "--Fortification Phase--" << endl;
	phaselog << "--Fortification Phase--" << endl;
	cout << "FORTIFY? (enter anything to continue, blank to skip): ";

	string input = "";
	getline(cin, input);
	target = input;

	// skip fortify if left blank
	if (input.empty()) {
		cout << "Skipping fortification phase.\n" << endl;
		phaselog << "Skipping fortification phase.\n" << endl;
		notify(phaselog.str(), p.getPlayerName(), "fortification");
		return;
	}

	//Fortification(p,countries,countries length)
	//3 methods .setmovingfrom, .setmovingto , .movearmy
	Fortification fort(p, countries);
	fort.setMovingFrom();
	fort.setMovingTo();
	phaselog << fort.moveArmy();

	cout << "Ending fortification phase.\n" << endl;
	phaselog << "Ending fortification phase." << endl;
	notify(phaselog.str(), p.getPlayerName(), "fortification");

	return;
}

/* The function run() contains the game loop: it is to be called once in the program's
main method, after which it will run until the game is over. */
void driver::run() {
	cout << "==================================================" << endl;
	cout << "\tWelcome to RISK: COMP 345 Edition." << endl;
	cout <<	"==================================================\n" << endl;

	// =====================================
	// stage 1: startup phase
	// =====================================
	cout << "=============================" << endl;
	cout << "\tSTARTUP PHASE" << endl;
	cout << "=============================\n" << endl;

	// ===========================================
	// A. player setup: determine player number and
	// get player names from user input.
	// ===========================================
	
	// select number of players
	string input = "";
	vector<Player> tempplayerlist;

	while (true) {
		cout << "Please choose the number of players for this game (minimum 2): ";
		getline(cin, input);

		stringstream stream(input);

		if (stream >> numberofplayers) {
			if (stream.eof() && numberofplayers >= 2)
				break; // good number of players
		}
		cout << "\nInvalid input." << endl;
	}

	cout << "\nUnderstood. This match will have " << numberofplayers << " players.\n" << endl;

	// fill the (temp)playerlist with user-chosen names
	for (int i = 1; i <= numberofplayers; ++i) {
		string name = "";
		cout << "Choose player " << i << "\'s name: ";
		getline(cin, name);
		Player temp = Player(name);
		playerlist.push_back(temp);
	}
	

	// for the purpose of demonstration, two players are pre-initialized in playerlist.
	//Player firstplayer("player1"), secondplayer("player2");
	//playerlist.push_back(firstplayer);
	//playerlist.push_back(secondplayer);

	cout << "Players in this match:" << endl;
	for (Player &p : playerlist) {
		cout << p.getPlayerName() << endl;
	}
	cout << endl;

	// ===========================================================
	// B. map setup: load a map, or create one from scratch with the
	// map creation wizard.
	// ===========================================================
	// testmap.map is a simple map with two adjacent countries, c1 and c2. c1 will go to player1 and c2 to player2, so use that to test victory conditions.
	MapManager* manager = new MapManager("testmap");
	manager->loadMap();
	if (manager->isValid()) {
		cout << "Map file is valid!" << endl;
	}

	// fill the driver's country and continents list, based on the map loaded above.
	continents = manager->getContinents();
	countries = manager->getCountries();

	for (Continent& continent : continents) {
		continent.setArmyBonus(2);
	}



	int i = 0;
	int currentindex;
	for (Country& country : countries)
		for (int indexofcountries = 0; indexofcountries < countries.size();indexofcountries++)
	{
		string name = countries[indexofcountries].getName();
		string continent = countries[indexofcountries].getContinent();

		currentindex = rand() % (playerlist.size());//randomly adds countries to players
		countries[indexofcountries].setAll(playerlist[currentindex], 10, name, continent);//sets the countries owner, armycount(equal to 10), name and continent
		i++;
	}

	
	
	
	//display Country ownership
	cout << "\nCountry ownership:" << endl;
	for(Country& country : countries) {
		cout << country.getOwner()->getPlayerName() << " owns " << country.getName() << "." << endl;
	}	

	// Instantiate Map observer pattern
	MapView *mapView = new MapView(countries, continents);


	// =============================================
	// C. set up the game logger, based on user input.
	// =============================================
	/**
	* HOW TO SET UP GAME LOGGER:
	* Once the players are established, you can select which players/phases you want to track.
	* For tracking players:
	*		Create a playerlogger decorator and pass the player's name, as a string. This is why the playerlist must be
	*		esblished before you can choose which players to track. The names must be passed exactly as they are:
	*		there is no case-correction applied.
	* For tracking phases:
	*		Create a phaselogger decorator and pass one of three phases, as strings: "reinforcement", "attack" or "fortification",
	*		exactly as they are written, without quotes. The phases each call notify() with these exact arguments, and so the decorator
	*		must match them to the letter in order for them to be recorded.
	* Chaining decorators:
	*		Decorators can be chained. For example, you can track player1 and also track their reinforcement + attack phases. Simply add
	*		a playerlogger for "player1", a phaselogger for "reinforcement" and a phaselogger for "attack".
	* On modifying the basic logger:
	*		The basic logger simply logs everything. However, as soon as you add one decorator, it nullifies the entire logger, meaning
	*		it will be set to track NO phase and NO player until you explicitly add them yourself. In other words, you will have either no
	*		decorators, or *at least* two: one for the player, and one for the phase. Example: adding only a playerlogger for player1 and nothing
	*		else will result in nothing being logged: you must also provide at least one phase to be logged, as well.
	* Happy logging!!!
	*/
	logger *log = new gamelogger(this);
	string logsetupinput = "";
	cout << "\nGAME LOGGER SETUP:" << endl;
	cout << "\n==Player logging==" << endl;
	cout << "Enter the name of a player to log (leave empty to skip): ";
	getline(cin, logsetupinput);

	if (logsetupinput.empty()) {
		cout << "\nSkipping log setup: using default settings (log all)." << endl;
	}

	else {
		while (true) {
			cout << "Adding player \"" << logsetupinput << "\" to the logger." << endl;
			log = new playerlogger(log, logsetupinput, this);

			cout << "Add next player (leave empty to continue): ";
			getline(cin, logsetupinput);

			if (logsetupinput.empty()) break;
		}

		cout << "\n==Phase logging==" << endl;
		cout << "Enter the name of the phase to log (choose from \"reinforcement\", \"attack\" and \"fortification\"): ";
		getline(cin, logsetupinput);

		while (true) {
			cout << "Adding phase \"" << logsetupinput << "\" to the logger." << endl;
			log = new phaselogger(log, logsetupinput, this);

			cout << "Add next phase (leave empty to end setup): ";
			getline(cin, logsetupinput);

			if (logsetupinput.empty()) break;
		}
	}

	cout << "\nLog setup complete. Stack of custom decorators:\n" << endl;



	// GAME STATISTICS OBSERVER/DECORATOR HERE
	//asks for user input to determine what type of game statistics to display
	    bool statcheck=true;
	GameStatSubject *sub;
	Observer *obs;
	while(statcheck)
	{
	string userinput;
	std::cout<< "Press 1 for basic statistics, 2 for World %, 3 for Win %, 4 for both World and Win %"<<std::endl;
	getline(cin, userinput);

	if (userinput=="1")
	{
	sub=new GameStatSubject(playerlist,countries);//creates a basic observer pattern
	obs=new GameStatObserver(sub);
	statcheck=false;
	}
	else if (userinput=="2")
	{
		sub = new GameStatSubject(playerlist, countries);//creates an observer pattern with a single decorator (the world % owned)
	obs=new GameStatObserver(sub);
	obs = new WorldControlledDecorator(obs, playerlist, countries, sub);
	statcheck=false;
	}
	else if (userinput=="3")
	{
		sub = new GameStatSubject(playerlist, countries);//creates an observer pattern with a single decorator (the win %)
	obs=new GameStatObserver(sub);
	obs = new BattlesWonDecorator(obs, playerlist, sub);
	statcheck=false;
	}
	else if (userinput=="4")
	{
		sub = new GameStatSubject(playerlist, countries);//creates an observer pattern with both decorators, the world % owned first then the win %
	obs=new GameStatObserver(sub);
	obs = new WorldControlledDecorator(obs, playerlist, countries, sub);
	obs = new BattlesWonDecorator(obs, playerlist, sub);
	statcheck=false;
	}
	}

	

	printobservers();

	cout << "\nGame is ready.\n" << endl;

	// =====================================
	// stage 2: main phase
	// =====================================
	// An iterator loops through playerlist until there is only one player remaining, who is then declared the winner.
	// For each player, go through reinforce, attack and fortify stages. Advance the iterator at the end of each turn.
	// note: in the actual game, there would likely be a certain number of turns that must pass before players are permitted to attack others.

	cout << "==========================" << endl;
	cout << "\tGAME START" << endl;
	cout << "==========================\n" << endl;

	vector<Player>::iterator it = playerlist.begin(); // iterator is placed at beginning of playerlist
	while (playerlist.size() > 1) {
		stringstream currentturn;


		// before doing anything, check if this player should still be in the game:
		// if no countries return the current player as the owner, they are removed from the game
		// and we move on to the next player's turn, or declare the winner if there is only one player left.
		bool lost = true;
		for (Country &country : countries) {
			if ((country.getOwner()->getPlayerName() == it->getPlayerName())) {
				lost = false;
				break;
			}
			// otherwise, it means we must continue checking the other countries to find at least one country owned by the
			// player before we can declare them defeated.
		}

		// by now, lost = false if player owns at least one country, or = true if they have no more countries.
		if (lost) {
			cout << it->getPlayerName() << " has no more countries: they are removed from the game." << endl;
			currentturn << it->getPlayerName() << " has no more countries: they are removed from the game." << endl;
			notify(currentturn.str(), "required", "required");
			it = playerlist.erase(it); // erase current player, capture the new iterator position into original iterator.
			if (it == playerlist.end()) it = playerlist.begin();
			continue; // continue to next player's turn
		}

		// everything checks out: player still has at least one country and may begin their turn.
		cout << "===" << it->getPlayerName() << "\'s turn===\n" << endl;
		
		currentturn << "===" << it->getPlayerName() << "\'s turn===" << endl;
		notify(currentturn.str(), it->getPlayerName(), "required");

		// run through all phases
		reinforce(*it);
		attack(*it);
		fortify(*it);
		sub->Notify();

		////////////////////////////////////////////////////player observer
		PlayerSubject *ps1 = new PlayerSubject(*it, countries, continents);//create observable and observer classes
		PlayerView *pv1 = new PlayerView(ps1);

		ps1->Notify();


		// after all phases, move to next player, or if end of playerlist has been reached, move back to beginning
		++it;
		if (it == playerlist.end()) it = playerlist.begin();
	}

	// reaching this point means there is only one player left, so they win the match
	// last player is declared winner
	cout << "\nCongratulations, " << playerlist[0].getPlayerName() << " wins the game!" <<endl;
	stringstream winner;
	winner << "\nCongratulations, " << playerlist[0].getPlayerName() << " wins the game!" << endl;
	notify(winner.str(), "required", "required");

	delete mapView;
}

// for debug
void driver::printobservers() {
	for (list<Observer*>::iterator it = _observers->begin(); it != _observers->end(); ++it) {
		(*it)->printself();
	}
}
