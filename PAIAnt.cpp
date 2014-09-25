
// ---------------------------------------------------------------------
// PAIAnt Implementation
// ---------------------------------------------------------------------

#include "PAIAnt.h"

using namespace std;
using namespace CoreStructures;

// ---------------------------------------------------------------------
// PRIVATE
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// PUBLIC
// ---------------------------------------------------------------------

// Constructor
PAIAnt::PAIAnt()
{
	carrying = -1;
	colour = -1;
}

// Deconstructor
PAIAnt::~PAIAnt()
{
	// Empty
}

// Initialise
// Precondition: N/A
// Postcondition: Ant setup/texture loaded/scaled
void PAIAnt::Initialise()
{
	// Initialise
	ant.Initialise();
	ant.setTexture(L"Resources\\Textures\\ant.png");
	ant.setScale(GUVector4(0.5, 2.0, 0.5));
}

// Render
// Precondition: Camera matrix
// Postcondition: PBox - render called
void PAIAnt::Render(const CoreStructures::GUMatrix4& T)
{
	// Slight offset to centre ants in grid
	GUMatrix4 MVP = T * GUMatrix4::translationMatrix(0.25, 0.0, 0.25);

	// Render box
	ant.Render(MVP);
}

// Getters and Setters
GUVector4 PAIAnt::getPosition()
{
	return ant.getPosition();
}

void PAIAnt::setPosition(CoreStructures::GUVector4 pos)
{
	ant.setPosition(pos);
}

void PAIAnt::setPosition(float x, float y, float z)
{
	ant.setPosition(x, y, z);
}