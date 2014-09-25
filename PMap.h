#pragma once
#ifndef PMAP
#define PMAP

// Include
#include "PBox.h"
#include "PGround.h"

#include <CoreStructures\GUVector4.h>
#include <CoreStructures\GUMatrix4.h>

// ---------------------------------------------------------------------
// PMap (Project Map) - Used to store the data for the map
// ---------------------------------------------------------------------

class PMap
{
// ---------------------------------------------------------------------
private:

	// ATTRIBUTES

	// Map ground
	PGround ground;

	//PBox black[10];
	//PBox grey[10];
	//PBox white[10];
	//PBox ant[10];

	PBox walls[80];

	// Basic
	int map[20][20];

// ---------------------------------------------------------------------
public:

	// Constructor / Deconstructor
	PMap();
	~PMap();

	// Initialise
	void Initialise();

	// Render
	void Render(const CoreStructures::GUMatrix4& T);

};

#endif