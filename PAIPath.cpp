
// ---------------------------------------------------------------------
// PAIPath Implementation
// ---------------------------------------------------------------------

#include "PAIPath.h"

// Other includes
#include "random.h"

// Namespace
using namespace std;
using namespace CoreStructures;

// Static variables (for fitness evaluation)
static float fitness;
static int map[20][20];

// These are for data collection
static bool collectData = 0;
static vector<GUVector4> path;

// Map checker for path finding
bool checkPosition(int x, int z)
{
	if((x < 20 && z < 20) && (x >= 0 && z >= 0))
		return map[x][z];
	else
		return 1;
}

// ---------------------------------------------------------------------
// Fitness Functions/Problem specific functions
// ---------------------------------------------------------------------

// Termination Criteria
// Precondition: GP setup/this function set as termination criteria
// Postcondition: GP will stop generating once a program has a hit
int pathTermination(GP* g)
{
	if(g->bestofgen_hits > 0)
		return 1;
	else
		return 0;
}

// Fitness function
// Precondition: GP setup and this function added as fitness function
// Postcondition: Fitness tested
float pathFitness(S_Expression* s, int* hits)
{
	// Fitness value
	fitness = 0;

	// 1. Set x, y to start position
	Tset.modify("X", 19.0);
	Tset.modify("Y", 19.0);

	// 2. Run 50 moves
	for(int i = 0; i < 50; i += 1)
	{
		s->eval();
		fitness += (Tset.get("X") + Tset.get("Y"));
	}

	// 3. Calculate fitness of final position
	// Goal location is 0, 0
	// This will be the Manhatten distance from the goal
	float pos = (Tset.get("X") + Tset.get("Y"));

	// 4. Update hits, is the evaluated fitness acceptable?
	if(pos == 0)
		*hits += 1;
	else
		fitness *= 2;

	// 5. Return the fitness value
	return fitness;
}

// ---------------------------------------------------------------------
// Function Set

// Move North
// Precondition: GP setup and terminals "X" and "Y" have been added
// Postcondition: "Y" altered to move north
float moveNorth(S_Expression** params)
{
	// Get current position
	int X = (int)Tset.get("X");
	int Y = (int)Tset.get("Y");

	// Check if the ant moves off the grid
	// Or is about to hit into a wall
	if(Y == 0)
	{
		// Move failed
		return -1;
	}
	// If the map space is clear
	else if(!map[X][Y - 1])
	{
		// Make the move
		Tset.modify("Y", Y - 1);

		// Create path or update fitness
		if(collectData)
			path.push_back(GUVector4(X, 0.0, Y - 1));
		else
			fitness += (X + Y) - 1;

		// Move Success
		return 1;
	}
	// Else, map isn't clear
	else
	{
		// Move failed
		return -1;
	}
}

// Move East
// Precondition: GP setup and terminals "X" and "Y" have been added
// Postcondition: "X" altered to move east
float moveEast(S_Expression** params)
{
	// Get current position
	int X = (int)Tset.get("X");
	int Y = (int)Tset.get("Y");

	// Check if the ant moves off the grid
	// Or is about to hit into a wall
	if(X == 19)
	{
		// Move failed
		return -1;
	}
	// If the map space is clear
	else if(!map[X + 1][Y])
	{
		// Make the move
		Tset.modify("X", X + 1);

		// Create path or update fitness
		if(collectData)
			path.push_back(GUVector4(X + 1, 0.0, Y));
		else
			fitness += (X + Y) + 1;

		// Move Success
		return 1;
	}
	// Else, map isn't clear
	else
	{
		// Move failed
		return -1;
	}
}

// Move South
// Precondition: GP setup and terminals "X" and "Y" have been added
// Postcondition: "Y" altered to move south
float moveSouth(S_Expression** params)
{
	// Get current position
	int X = (int)Tset.get("X");
	int Y = (int)Tset.get("Y");

	// Check if the ant moves off the grid
	// Or is about to hit into a wall
	if(Y == 19)
	{
		// Move failed
		return -1;
	}
	// If the map space is clear
	else if(!map[X][Y + 1])
	{
		// Make the move
		Tset.modify("Y", Y + 1);

		// Create path or update fitness
		if(collectData)
			path.push_back(GUVector4(X, 0.0, Y + 1));
		else
			fitness += (X + Y) + 1;

		// Move Success
		return 1;
	}
	// Else, map isn't clear
	else
	{
		// Move failed
		return -1;
	}
}

// Move West
// Precondition: GP setup and terminals "X" and "Y" have been added
// Postcondition: "X" altered to move west
float moveWest(S_Expression** params)
{
	// Get current position
	int X = (int)Tset.get("X");
	int Y = (int)Tset.get("Y");

	// Check if the ant moves off the grid
	// Or is about to hit into a wall
	if(X == 0)
	{
		// Move failed
		return -1;
	}
	// If the map space is clear
	else if(!map[X - 1][Y])
	{
		// Make the move
		Tset.modify("X", X - 1);

		// Create path or update fitness
		if(collectData)
			path.push_back(GUVector4(X - 1, 0.0, Y));
		else
			fitness += (X + Y) - 1;

		// Move Success
		return 1;
	}
	// Else, map isn't clear
	else
	{
		// Move failed
		return -1;
	}
}

// IFLTE - If less then or equal
float IFLTE(S_Expression** params)
{
	if (params[0]->eval() <= params[1]->eval())
		return (params[2]->eval());
	else 
		return (params[3]->eval());
}

// IFLTZ - If less than zero
float IFLTZ(S_Expression** params)
{
	if (params[0]->eval() < 0)
		return (params[1]->eval());
	else 
		return (params[2]->eval());
}

// ---------------------------------------------------------------------
// PAIPath class functions

// ---------------------------------------------------------------------
// PRIVATE
// ---------------------------------------------------------------------

// Run the best found
// Precondition: GP setup and best individual acquired
// Postcondition: Data in the form of a vector of float4s
void PAIPath::runBestProgram()
{
	// Collect path data
	collectData = 1;

	// Set position to default
	Tset.modify("X", 19.0);
	Tset.modify("Y", 19.0);
	
	// Check for first run,
	// It is impossible to score 0
	if(best.rfit == 0)
		best = gp->best_of_run;

	// Check run best against stored best
	if(gp->best_of_run.rfit < best.rfit)
		best = gp->best_of_run;

	// If the current stored best hasn't reached the end,
	// but the best_of_run has; it's a better 'solution'
	else if(best.hits == 0 && gp->best_of_run.hits > 0)
		best = gp->best_of_run;

	// Add initial position
	path.push_back(GUVector4(19.0, 0.0, 19.0));

	for(int i = 0; i < 50; i += 1)
		best.s->eval();

	// Stop collecting data
	collectData = 0;
}

// Print best individual
// Precondition: GP setup and best individual acquired
// Postcondition: Best individual is output to the debug window
void PAIPath::printBest()
{
	// Print best individual
	cout << "\nThe best individual program has a standardized fitness of "
		<< best.sfit << " and " << best.hits << " hits:\n";

	cout << best.s << endl;
}

// Add Genetic path tile
// Precondition: n/a
// Postcondition: Path tile added to GP pathtile vector
void PAIPath::addGPTile()
{
	GUVector4 pos = genetic.getPosition();

	pos.y = 0.001;

	newTile.Initialise();
	newTile.setPosition(pos);
	newTile.mode = FILL;

	gpPath.push_back(newTile);
}

// Add A* path tile
// Precondition: n/a
// Postcondition: Path tile added to A* pathtile vector
void PAIPath::addASTile()
{
	GUVector4 pos = aStar.getPosition();

	pos.y = 0.002;

	newTile.Initialise();
	newTile.setTexture(L"Resources\\Textures\\astar.png");
	newTile.setPosition(pos);
	newTile.mode = LINE;

	gpPath.push_back(newTile);
}

// Setup Objects
// Precondition: n/a
// Postcondition: All objects setup/positions set
void PAIPath::setupObjects()
{
	// Setup boxes
	genetic.Initialise();
	genetic.setTexture(L"Resources\\Textures\\ant.png");
	genetic.setPosition(19.0, 0.0, 19.0);

	aStar.Initialise();
	aStar.setTexture(L"Resources\\Textures\\astar.png");
	aStar.setPosition(19.0, 0.0, 19.0);

	// Randomise wall postions
	int set = 0;

	while(set < 80)
	{
		int randX = rand() % 20;
		int randY = rand() % 20;

		if(!map[randX][randY])
		{
			map[randX][randY] = 1;
			walls[set].Initialise();
			walls[set].setTexture(L"Resources\\Textures\\black.png");
			walls[set].setPosition(randX, 0.0, randY);
			set += 1;
		}
	}
}

// ---------------------------------------------------------------------
// PUBLIC
// ---------------------------------------------------------------------

// Constructor
PAIPath::PAIPath()
{
	// Initial values
	gp = NULL;
	move = 0;
	moveTime = 0;

	// Initialise map
	for(int i = 0; i < 20; i += 1)
		for(int j = 0; j < 20; j += 1)
				map[j][i] = 0;
}

// Deconstructor
PAIPath::~PAIPath()
{
	// Clear lists
	gpPath.clear();
	asPath.clear();
}

// Initialise
// Precondition: N/A
// Postcondition: GP/Path AI setup complete
void PAIPath::initialise()
{
	// Setup Objects
	setupObjects();

	// Setup pathfinding
	pf.attachMapCheck(*checkPosition);

	// Find optimal path
	pf.FindPath(aStar.getPosition(), GUVector4(0.0, 0.0, 0.0));

	// 1. Specify terminal set
	myTSet.add("X", 19.0);
	myTSet.add("Y", 19.0);

	// 2. Specify function set
	myFSet.add("MOVE-N", 0, *moveNorth, NULL, 1);
	myFSet.add("MOVE-E", 0, *moveEast, NULL, 1);
	myFSet.add("MOVE-S", 0, *moveSouth, NULL, 1);
	myFSet.add("MOVE-W", 0, *moveWest, NULL, 1);

	myFSet.add("IFLTE", 4, *IFLTE, NULL);
	myFSet.add("IFLTZ", 3, *IFLTZ, NULL);

	// 3. Precalculate your set of fitness test cases
	// N/A

	// 4. Declare the GP object
	if(!gp)
		gp = new GP(*pathFitness, 500);

	// 5. Specify any non-default GP parameters
	gp->verbose = DEBUG | END_REPORT;
	gp->termination_criteria = *pathTermination;
}

// Run
// Precondition: GP setup
// Postcondition: A run of 10 generations is started (and data collected)
void PAIPath::run()
{
	// Reset variables/Stop animation
	move = 0;
	genetic.setPosition(19.0, 0.0, 19.0); // Start position
	aStar.setPosition(19.0, 0.0, 19.0);
	current = 0; // Set current
	path.clear(); // Empty vector
	gpPath.clear(); // Empty vector
	asPath.clear(); // Empty vector
	pf.ResetPath(); // Reset Path

	// 1. Start the evolution
	gp->go(gp->gen + 10);

	// 2. Examine results
	runBestProgram();

	// 3. Output current best
	if(gp->verbose == DEBUG)
		printBest();

	// 4. Start the animation
	move = 1;
}

// Generate new population
// Precondition: GP setup
// Postcondition: GP is restarted, forced to generate a new population
void PAIPath::refreshPop()
{
	// Set GP generation to 0
	// Forces a regeneration on next run
	gp->gen = 0;

	// Run
	run();
}

// Update
// Precondition: n/a
// Postcondition: All objects updated
void PAIPath::Update(float delta)
{
	if(move)
	{
		moveTime += delta;

		if(moveTime > 0.1)
		{
			moveTime = 0;

			if(aStar.getPosition() != GUVector4(0.0, 0.0, 0.0))
			{
				addASTile();
				aStar.setPosition(pf.GetNextPosition());
			}

			// Check if box is at next position
			if(current < path.size())
			{
				addGPTile();
				genetic.setPosition(path[current].x, 0.0, path[current].z);

				if(path[current].x == 0 && path[current].z == 0)
					// End reached, complete
					current = path.size() - 1;
				else
				{
					current += 1;
				}
			}
		}
	}
}

// Render
// Precondition: n/a
// Postcondition: All objects rendered
void PAIPath::render(const CoreStructures::GUMatrix4& T)
{
	// Render box
	genetic.Render(T);
	aStar.Render(T);

	for(int i = 0; i < 80; i += 1)
		walls[i].Render(T);

	// Draw path (GP)
	for(list<PTile>::iterator tileIt = gpPath.begin(); tileIt != gpPath.end(); tileIt++)
	{
		tileIt->Render(T);
	}

	// Draw path (AStar)
	for(list<PTile>::iterator tileIt = asPath.begin(); tileIt != asPath.end(); tileIt++)
	{
		tileIt->Render(T);
	}
}

// Set in use
// Precondition: FunctionSet setup
// Postcondition: Active functions set is the PathAI function set
void PAIPath::setInUse()
{
	// Set the terminal and function sets
	Fset = myFSet;
	Tset = myTSet;
}
