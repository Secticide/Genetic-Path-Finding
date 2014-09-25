#pragma once
#ifndef PAIANT
#define PAIANT

// Include
#include "PBox.h"
#include <CoreStructures\GUMatrix4.h>
#include <CoreStructures\GUVector4.h>

// ---------------------------------------------------------------------
// PAIAnt (Project AI Ant) - Mainly a data class used to control the ants
// ---------------------------------------------------------------------

class PAIAnt
{
// ---------------------------------------------------------------------
private:

	// ATTRIBUTES

	// AI Model (Ant)
	PBox ant;

// ---------------------------------------------------------------------
public:

	// ATTRIBUTES

	// Settings
	int carrying;
	int colour;

	// Constructor / Decontructor
	PAIAnt();
	~PAIAnt();

	// Initialise
	void Initialise();

	// Render
	void Render(const CoreStructures::GUMatrix4& T);

	// Getters and Setters
	CoreStructures::GUVector4 getPosition();

	void setPosition(CoreStructures::GUVector4 pos);
	void setPosition(float x, float y, float z);

};

#endif