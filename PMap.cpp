
// ---------------------------------------------------------------------
// PMap Implementation
// ---------------------------------------------------------------------

#include "PMap.h"

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
PMap::PMap()
{
	// Set Initial values
	// Initialise map values
	for(int i = 0; i < 20; i += 1)
		for(int j = 0; j < 20; j += 1)
			map[j][i] = 0;
}

// Deconstructor
PMap::~PMap()
{
	// Empty
}

// Initialise
// Precondition: n/a
// Postcondition: Map objects initialised
void PMap::Initialise()
{
	// Initialise map objects
	ground.Initialise();
}

// Render
// Precondition: Camera matrix
// Postcondition: Map objects rendered
void PMap::Render(const GUMatrix4& T)
{
	// Render all of the objects associated with the map
	ground.Render(T);
}