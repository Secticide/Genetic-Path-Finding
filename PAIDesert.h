#pragma once
#ifndef PAIDESERT
#define PAIDESERT

// Includes
#include "gp.h"
#include "PBox.h"
#include "PAIAnt.h"
#include "CoreStructures\GUVector4.h"

#include <string>
#include <vector>

// ---------------------------------------------------------------------
// PAIDesert (Project AI Desert) - Controls the AI (and GP) for the desert problem
// ---------------------------------------------------------------------

class PAIDesert
{
// ---------------------------------------------------------------------
private:

	// ATTRIBUTES

	// Movetime update & move
	float moveTime;
	bool move;

	// Current indexes
	int current;

	// Genetic Program
	GP* gp;

	// Best individual found
	Individual best;

	// AI Objects - Ants, Sand
	PAIAnt ant[20];
	PBox black;
	PBox grey;
	PBox white;

	// Problem Specific Terminal and Function sets
	FunctionSet myFSet;
	TerminalSet myTSet;

	// METHODS

	void setupObjects();
	void runBestIndividual();

	// Problem specific functions
	// Movement
	void AIMoveNorth(int antIndex);
	void AIMoveEast(int antIndex);
	void AIMoveSouth(int antIndex);
	void AIMoveWest(int antIndex);

	// Actions
	void AIPickUp(int antIndex);
	void AIIfDrop(int antIndex);

	// Init render map
	void initialiseRendermap();

// ---------------------------------------------------------------------
public:

	// Constructor/Deconstructor
	PAIDesert();
	~PAIDesert();

	// Initialise
	void initialise();

	// Run GP and get best individual so far
	void run();

	// Create new population
	void refreshPop();

	// Update and render
	void update(float delta);
	void render(const CoreStructures::GUMatrix4& T);

	// Set not in use
	void setInUse();
};

#endif