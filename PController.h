#pragma once
#ifndef PCTRL
#define PCTRL

// Include
#include "PMap.h"
#include "PAIPath.h"
#include "PTextures.h"

#include "PAIDesert.h"

#include <CoreStructures\GUMatrix4.h>
#include <CoreStructures\GUVector4.h>

// ---------------------------------------------------------------------
// PController (Project Controller) - Used as a central class for the project
// ---------------------------------------------------------------------

class PController
{
// ---------------------------------------------------------------------
private:

	// ATTRIBUTES
	PMap level;

	// Resources
	//PTextures textures;

	// AI
	PAIPath pathAI;
	PAIDesert desertAI;
	bool currentAI;

	// Map Offset (centres the map when rendering)
	CoreStructures::GUVector4 mapOffset;

	// METHODS


// ---------------------------------------------------------------------
public:

	// Constructor / Decontructor
	PController();
	~PController();

	// Initialise
	void Initialise();

	// Update Objects
	void Update(float delta);

	// Render
	void Render(const CoreStructures::GUMatrix4& T);

	// CONTROLS

	// Run AI
	void runAI();

	// Create new population
	void refreshPop();

	// Swap AI
	void swapAI();
};

#endif