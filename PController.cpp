
// ---------------------------------------------------------------------
// PController Implementation
// ---------------------------------------------------------------------

#include "PController.h"

// Other includes
#include "random.h"

#include <iostream>

using namespace std;
using namespace CoreStructures;

// ---------------------------------------------------------------------
// PRIVATE
// ---------------------------------------------------------------------


// ---------------------------------------------------------------------
// PUBLIC
// ---------------------------------------------------------------------

// Constructor
PController::PController()
{
	// The only random seed in the program
	seedRandom();

	// Current AI
	currentAI = 0;

	// Map offset of -10, 0, -10 - Centres the map
	mapOffset = GUVector4(-10.0, 0.0, -10.0);
}

// Decontructor
PController::~PController()
{
	// Empty
}

// Initialise
// Precondition: OpenGL setup
// Postcondition: All project objects initialised
void PController::Initialise()
{
	// Initialise Objects
	level.Initialise();
	pathAI.initialise();
	desertAI.initialise();

	// Set the path AI as 'in use'
	pathAI.setInUse();
}

// Update
// Precondition: Delta
// Postcondition: All project objects updated
void PController::Update(float delta)
{
	// Update AI
	if(currentAI)
	{
		desertAI.update(delta);
	}
	else
	{
		pathAI.Update(delta);
	}
}

// Render
// Precondition: Camer Matrix
// Postcondition: All project objects renderd
void PController::Render(const GUMatrix4& T)
{
	// Multiply with offset
	GUMatrix4 MVP = T * GUMatrix4::translationMatrix(mapOffset);

	// Render the map
	level.Render(MVP);

	if(currentAI)
	{
		desertAI.render(MVP);
	}
	else
	{
		pathAI.render(MVP);
	}
}

// Run AI
// Precondition: DesertAI and PathAI setup/Space is pressed
// Postcondition: Current AI set to run 10 more generations
void PController::runAI()
{
	if(currentAI)
		desertAI.run();
	else
		pathAI.run();
}

// Create new population
// Precondition: DesertAI and PathAI setup/'N' is pressed
// Postcondition: Current AI set to refresh population
void PController::refreshPop()
{
	if(currentAI)
		desertAI.refreshPop();
	else
		pathAI.refreshPop();
}

// Swap AI
// Precondition: DesertAI and PathAI setup/left or right arrow key is pressed
// Postcondition: Current AI swapped
void PController::swapAI()
{
	currentAI = !currentAI;

	if(currentAI)
		desertAI.setInUse();
	else
		pathAI.setInUse();
}