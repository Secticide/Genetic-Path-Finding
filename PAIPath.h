#pragma once
#ifndef PAIPATH
#define PAIPATH

// Includes
#include "gp.h"
#include "PBox.h"
#include "PTile.h"
#include "Pathfinding.h"

#include <list>
#include <CoreStructures\GUVector4.h>
#include <CoreStructures\GUMatrix4.h>

// ---------------------------------------------------------------------
// PAIPath (Project AI Path) - Controls the AI (and GP) for the path problem
// ---------------------------------------------------------------------

class PAIPath
{
// ---------------------------------------------------------------------
private:

	// ATTRIBUTES

	// Path positions
	std::list<PTile> gpPath;
	std::list<PTile> asPath;
	PTile newTile;

	// Movetime update & move
	float moveTime;
	bool move;

	// Current
	unsigned int current;

	// Genetic Program
	GP* gp;

	// Path finding instance
	Pathfinding pf;

	// Best Individual found
	Individual best;

	// Genetic and AStar boxes
	PBox genetic;
	PBox aStar;

	// Walls
	PBox walls[80];

	// Problem Specific Terminal and Function sets
	FunctionSet myFSet;
	TerminalSet myTSet;

	// METHODS
	// Run the best found
	void runBestProgram();

	// Print best individual
	void printBest();

	// Setup path objects
	void addGPTile();
	void addASTile();

	// Setup Objects
	void setupObjects();

// ---------------------------------------------------------------------
public:

	// Constructor/Deconstructor
	PAIPath();
	~PAIPath();

	// Initialise
	void initialise();

	// Run GP and get best individual so far
	void run();

	// Create new population
	void refreshPop();

	// Update/Render
	void Update(float delta);
	void render(const CoreStructures::GUMatrix4& T);

	// Set in use
	void setInUse();
};

#endif