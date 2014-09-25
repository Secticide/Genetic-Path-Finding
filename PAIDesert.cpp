
// ---------------------------------------------------------------------
// PAIDesert Implementation
// ---------------------------------------------------------------------

#include "PAIDesert.h"

// Other includes
#include "random.h"

#include <time.h>
#include <iostream>
#include <CoreStructures\GUVector4.h>

// Namespace use
using namespace CoreStructures;
using namespace std;

// Static varibles (for fitness evaluation)
static int startMap[20][20];
static int map[20][20];
static int renderMap[20][20];
static GUVector4 antPos[20];

// Data collection
static std::vector<char> actions[20];
static int collectIndex = 0;
static bool collectData = 0;

// ---------------------------------------------------------------------
// Fitness Functions/Problem specific functions
// ---------------------------------------------------------------------

// Termination Criteria
// Precondition: GP setup/this function set as termination criteria
// Postcondition: GP will stop generating once a program has a hit
int desertTermination(GP* g)
{
	if(g->bestofgen_hits > 0)
		return 1;
	else
		return 0;
}

// Init map
// Precondition: startMap setup
// Postcondition: map is reset
void initialiseMap()
{
	for(int i = 0; i < 20; i += 1)
	{
		for(int j = 0; j < 10; j += 1)
		{
			map[j][i] = startMap[j][i];
		}
	}
}

// Fitness function for ant problem
// Precondition: GP setup and this function added as fitness function
// Postcondition: Fitness tested
float antFitness(S_Expression* s, int* hits)
{
	// Reset Map
	initialiseMap();

	// Setup fitness variable
	float fitness = 0;

	for(int i = 0; i < 20; i += 1)
	{
		// Set x and y to ant positions
		Tset.modify("X", antPos[i].x);
		Tset.modify("Y", antPos[i].z);

		// Loop to run program - 300 moves
		for(int j = 0; j < 300; j += 1)
		{
			// Run the program
			s->eval();

			// Ant should have dropped sand
			if(Tset.get("CARRYING") > 0)
			{
				fitness += 40;

				// Set carrying back to default
				Tset.modify("CARRYING", -1);
			}
		}
	}

	// Nested loop checks positions of sand
	for(int i = 0; i < 20; i += 1)
	{
		for(int j = 0; j < 20; j += 1)
		{
			switch(map[j][i])
			{
			case 1:
				if(j != 0)
					fitness += j;

				break;

			case 2:
				if(j == 0)
					fitness += 1;
				else if(j != 1)
					fitness += j;

				break;

			case 3:
				if(j == 0)
					fitness += 2;
				else if(j == 1)
					fitness += 1;
				else if(j != 2)
					fitness += j;

				break;
			}
		}
	}

	if(fitness == 0)
		*hits += 1;

	return fitness;
}

// Function Set

// Update Colour
// Precondition: GP setup and "COLOUR" added as terminal
// Postcondition: "COLOUR" set to colour of grain at terminal positon x/y
float checkColour()
{
	// Get current position
	int X = (int)Tset.get("X");
	int Y = (int)Tset.get("Y");

	// If there is something at map positon
	if(map[X][Y])
	{
		Tset.modify("COLOUR", map[X][Y]);
		return map[X][Y];
	}
	else
	{
		Tset.modify("COLOUR", -1);
		return -1;
	}
}

// Movement
// Precondition: GP setup and terminals "X" and "Y" have been added
// Postcondition: "Y" altered to move north
float goNorth(S_Expression** params)
{
	// Update data collection
	if(collectData)
		actions[collectIndex].push_back('N');

	// Get current position
	int X = (int)Tset.get("X");
	int Y = (int)Tset.get("Y");

	// Check if the ant moves off the grid
	if(Y <= 0)
		Tset.modify("Y", 19);
	else
		Tset.modify("Y", Y - 1);

	return checkColour();
}

// Precondition: GP setup and terminals "X" and "Y" have been added
// Postcondition: "X" altered to move east
float goEast(S_Expression** params)
{
	// Update data collection
	if(collectData)
		actions[collectIndex].push_back('E');

	// Get current position
	int X = (int)Tset.get("X");
	int Y = (int)Tset.get("Y");

	// Check if the ant moves off the grid
	if(X >= 19)
		Tset.modify("X", 0);
	else
		Tset.modify("X", X + 1);

	return checkColour();
}

// Precondition: GP setup and terminals "X" and "Y" have been added
// Postcondition: "Y" altered to move south
float goSouth(S_Expression** param)
{
	// Update data collection
	if(collectData)
		actions[collectIndex].push_back('S');

	// Get current position
	int X = (int)Tset.get("X");
	int Y = (int)Tset.get("Y");

	// Check if the ant moves off the grid
	if(Y >= 19)
		Tset.modify("Y", 0);
	else
		Tset.modify("Y", Y + 1);

	return checkColour();
}

// Precondition: GP setup and terminals "X" and "Y" have been added
// Postcondition: "X" altered to move west
float goWest(S_Expression** params)
{
	// Update data collection
	if(collectData)
		actions[collectIndex].push_back('W');

	// Get current position
	int X = (int)Tset.get("X");
	int Y = (int)Tset.get("Y");

	// Check if the ant moves off the grid
	if(X <= 0)
		Tset.modify("X", 19);
	else
		Tset.modify("X", X - 1);



	return checkColour();
}

// Precondition: n/a
// Postcondition: One of the four functions above are called
float goRandom(S_Expression** params)
{
	int ran = rand() % 4;

	switch(ran)
	{
	case 0:
		return goNorth(params);

	case 1:
		return goEast(params);

	case 2:
		return goSouth(params);

	case 3:
		return goWest(params);

	}
}

// Pick up
// Precondition: GP setup and terminals "X", "Y" and "CARRYING" have been added
// Postcondition: "CARRYING" updated to sand grain colour
float pickUp(S_Expression** params)
{
	// Update data collection
	if(collectData)
		actions[collectIndex].push_back('P');

	// Get current position
	int X = (int)Tset.get("X");
	int Y = (int)Tset.get("Y");

	// Check if the ant is carrying sand
	if(Tset.get("CARRYING") > 0)
		return map[X][Y];

	// Check if there is something at the position
	if(map[X][Y] > 0)
	{
		// Pick up the sand
		Tset.modify("CARRYING", map[X][Y]);

		// Remove the sand from the map
		map[X][Y] = 0;

		// Return what's left, -1
		return -1;
	}

	// If there isn't anything at the x, y pos
	// Return -1
	return -1;
}

// IFLTE - If less then or equal
float iflte(S_Expression** params)
{
	if (params[0]->eval() <= params[1]->eval())
		return (params[2]->eval());
	else 
		return (params[3]->eval());
}

// IFLTZ - If less than zero
float ifltz(S_Expression** params)
{
	if (params[0]->eval() < 0)
		return (params[1]->eval());
	else 
		return (params[2]->eval());
}

// IF-DROP
// Precondition: GP setup and terminals "X", "Y" and "CARRYING" have been added
// Postcondition: "CARRYING" updated to sand grain colour/map position updated
float ifDrop(S_Expression** params)
{
	// Update data collection
	if(collectData)
		actions[collectIndex].push_back('D');

	// Get current position
	int X = (int)Tset.get("X");
	int Y = (int)Tset.get("Y");

	// Check if sand is being carried
	if(Tset.get("CARRYING") > 0)
	{
		// Check that the current x, y is empty
		if(map[X][Y] == 0)
		{
			// Drops the sand on the map
			map[X][Y] == Tset.get("CARRYING");
			Tset.modify("CARRYING", -1);

			return params[0]->eval();
		}
	}

	return params[1]->eval();
}

// ---------------------------------------------------------------------
// PAIDesert class functions

// ---------------------------------------------------------------------
// PRIVATE
// ---------------------------------------------------------------------

// Problem specific functions
// Movement
// Pre and post conditions apply for each of the movement functions below
// Precondition: An ant exists
// Postcondition: ant[antIndex]'s x and y positions are adjusted
void PAIDesert::AIMoveNorth(int antIndex)
{
	// Get current position
	int X = ant[antIndex].getPosition().x;
	int Y = ant[antIndex].getPosition().z;

	// Check if the ant moves off the grid
	if(Y <= 0)
		ant[antIndex].setPosition(X, 0, 19);
	else
		ant[antIndex].setPosition(X, 0, Y - 1);
}

void PAIDesert::AIMoveEast(int antIndex)
{
	// Get current position
	int X = ant[antIndex].getPosition().x;
	int Y = ant[antIndex].getPosition().z;

	// Check if the ant moves off the grid
	if(X >= 19)
		ant[antIndex].setPosition(0, 0, Y);
	else
		ant[antIndex].setPosition(X + 1, 0.0, Y);
}

void PAIDesert::AIMoveSouth(int antIndex)
{
	// Get current position
	int X = ant[antIndex].getPosition().x;
	int Y = ant[antIndex].getPosition().z;

	// Check if the ant moves off the grid
	if(Y >= 19)
		ant[antIndex].setPosition(X, 0, 0);
	else
		ant[antIndex].setPosition(X, 0, Y + 1);
}

void PAIDesert::AIMoveWest(int antIndex)
{
	// Get current position
	int X = ant[antIndex].getPosition().x;
	int Y = ant[antIndex].getPosition().z;

	// Check if the ant moves off the grid
	if(X <= 0)
		ant[antIndex].setPosition(19, 0, Y);
	else
		ant[antIndex].setPosition(X - 1, 0, Y);
}

// Precondition: An ant exists
// Postcondition: ant[antIndex]'s carrying and rendermap values adjusted
void PAIDesert::AIPickUp(int antIndex)
{
	// Get current position
	int X = ant[antIndex].getPosition().x;
	int Y = ant[antIndex].getPosition().z;

	// Check if the ant is carrying sand
	if(ant[antIndex].carrying > 0)
		return;

	// Check if there is something at the position
	if(renderMap[X][Y])
	{
		// Pick up the sand
		ant[antIndex].carrying = renderMap[X][Y];

		// Remove the sand from the map
		renderMap[X][Y] = 0;
	}
}

// Precondition: An ant exists
// Postcondition: ant[antIndex]'s carrying and rendermap values adjusted
void PAIDesert::AIIfDrop(int antIndex)
{
	// Get current position
	int X = ant[antIndex].getPosition().x;
	int Y = ant[antIndex].getPosition().z;

	// Check if sand is being carried
	if(ant[antIndex].carrying > 0)
	{
		// Check that the current x, y is empty
		if(renderMap[X][Y] == 0)
		{
			// Drops the sand on the map
			renderMap[X][Y] = ant[antIndex].carrying;
			ant[antIndex].carrying = 0;
		}
	}
}

// Setup Objects
// Precondition: n/a
// Postcondition: All ants and grains positions set
void PAIDesert::setupObjects()
{
	// Initialise all of the boxes
	black.Initialise();
	grey.Initialise();
	white.Initialise();

	// Set correct textures
	black.setTexture(L"Resources\\Textures\\black.png");
	grey.setTexture(L"Resources\\Textures\\grey.png");
	white.setTexture(L"Resources\\Textures\\white.png");

	for(int i = 0; i < 20; i += 1)
	{
		// int for whether positions are set
		int set = 0;

		// Initialise ant
		ant[i].Initialise();

		// Set random initial positions
		ant[i].setPosition(GUVector4((int)(rand() % 20), 0.0, (int)(rand() % 20)));
		antPos[i] = ant[i].getPosition();

		while(set < 3)
		{
			// Generate random Vector
			GUVector4 randVec = GUVector4( (int)(rand() % 20), 0.0, (int)(rand() % 20));

			// Check if it has already been taken
			if(startMap[(int)(randVec.x)][(int)randVec.z] == 0)
			{
				// Assign if it's not taken
				switch(set)
				{
				case 0:
					startMap[(int)(randVec.x)][(int)randVec.z] = 1;
					set += 1;
					break;

				case 1:
					startMap[(int)(randVec.x)][(int)randVec.z] = 2;
					set += 1;
					break;

				case 2:
					startMap[(int)(randVec.x)][(int)randVec.z] = 3;
					set += 1;
					break;
				}
			}
		}
	}
}

// Run best individual
// Precondition: GP setup and best individual acquired
// Postcondition: Data in the form of character string collected
void PAIDesert::runBestIndividual()
{
	collectData = 1;

	for(collectIndex = 0; collectIndex < 20; collectIndex += 1)
	{
		actions[collectIndex].clear();

		// Set x and y to ant positions
		Tset.modify("X", antPos[collectIndex].x);
		Tset.modify("Y", antPos[collectIndex].z);

		// Loop to run program - 50 moves
		for(int i = 0; i < 300; i += 1)
		{
			best.s->eval();
		}
	}

	collectData = 0;
}

// Initialise render map
// Precondition: startMap setup
// Postcondition: rendermap is reset
void PAIDesert::initialiseRendermap()
{
	for(int i = 0; i < 20; i += 1)
	{
		for(int j = 0; j < 20; j += 1)
		{
			renderMap[j][i] = startMap[j][i];
		}
	}
}

// ---------------------------------------------------------------------
// PUBLIC
// ---------------------------------------------------------------------

// Constructor
PAIDesert::PAIDesert()
{
	// Set initial values
	current = 0;
	moveTime = 0;
	move = 0;

	gp = NULL;
}

// Deconstructor
PAIDesert::~PAIDesert()
{
	// EMPTY
}

// Initialise
// Precondition: N/A
// Postcondition: GP/Desert AI setup complete
void PAIDesert::initialise()
{
	// Init objects
	setupObjects();

	// Init fitness based variables
	initialiseMap();
	initialiseRendermap();

	// 1. Specify terminal set
	myTSet.add("X");
	myTSet.add("Y");

	myTSet.add("CARRYING", -1.0);
	myTSet.add("COLOUR", -1.0);

	// 2. Specify function set
	myFSet.add("GO-N", 0, *goNorth, NULL, 1);
	myFSet.add("GO-E", 0, *goEast, NULL, 1);
	myFSet.add("GO-S", 0, *goSouth, NULL, 1);
	myFSet.add("GO-W", 0, *goWest, NULL, 1);
	myFSet.add("GO-Rand", 0, *goRandom, NULL, 1);
	myFSet.add("PICK-UP", 0, *pickUp, NULL, 1);

	myFSet.add("IFLTE", 4, *iflte, NULL);
	myFSet.add("IFLTZ", 3, *ifltz, NULL);
	myFSet.add("IF-DROP", 2, *ifDrop, NULL);

	// 3. Precalculate your set of fitness test cases
	// NONE

	// 4. Declare the GP object
	if(!gp)
		gp = new GP((*antFitness), 100);

	// 5. Specify any non-default GP parameters
	gp->verbose = DEBUG | END_REPORT;
	gp->termination_criteria = *desertTermination;
}

// Run GP and get best individual so far
// Precondition: GP setup
// Postcondition: A run of 10 generations is started (and data collected)
void PAIDesert::run()
{
	// Reset variables
	initialiseMap();
	initialiseRendermap();
	current = 0;

	// Set ants to default positions and variables
	for(int i = 0; i < 20; i += 1)
	{
		ant[i].carrying = 0;
		ant[i].setPosition(antPos[i]);
	}

	// 1. Start the evolution
	gp->go(gp->gen + 10);

	// 2. Run best and collect data
	best = gp->best_of_run;
	runBestIndividual();

	// 3. Examine results
	move = 1;
}

// Generate new population
// Precondition: GP setup
// Postcondition: GP is restarted, forced to generate a new population
void PAIDesert::refreshPop()
{
	// Set GP generation to 0
	// Forces a regeneration on next run
	gp->gen = 0;

	// Run
	run();
}

// Update
// Precondition: n/a
// Postcondition: All ants and objects updated
void PAIDesert::update(float delta)
{
	if(move)
	{
		moveTime += delta;

		if(moveTime > 0.01)
		{
			moveTime = 0;

			current += 1;

			// Move everything to the next place
			for(int i = 0; i < 20; i += 1)
			{
				if(current < actions[i].size())
				{
					switch(actions[i][current])
					{
					case 'N': // North
						AIMoveNorth(i);
						break;

					case 'E': // East
						AIMoveEast(i);
						break;

					case 'S': // South
						AIMoveSouth(i);
						break;

					case 'W': // West
						AIMoveWest(i);
						break;

					case 'P': // Pickup
						AIPickUp(i);
						break;

					case 'D': // Drop
						AIIfDrop(i);
						break;
					}
				}
			}
		}
	}
}

// Render
// Precondition: n/a
// Postcondition: All ants and objects rendered
void PAIDesert::render(const CoreStructures::GUMatrix4& T)
{
	// Render each of the boxes
	for(int i = 0; i < 20; i += 1)
	{
		ant[i].Render(T);

		for(int j = 0; j < 20; j += 1)
		{
			if(renderMap[j][i] == 1)
			{
				black.setPosition(j, 0.0, i);
				black.Render(T);
			}
			else if(renderMap[j][i] == 2)
			{
				grey.setPosition(j, 0.0, i);
				grey.Render(T);
			}
			else if(renderMap[j][i] == 3)
			{
				white.setPosition(j, 0.0, i);
				white.Render(T);
			}
		}
	}
}

// Set in use
// Precondition: FunctionSet setup
// Postcondition: Active functions set is the DesertAI function set
void PAIDesert::setInUse()
{
	// Set the terminal and function sets
	Fset = myFSet;
	Tset = myTSet;
}
