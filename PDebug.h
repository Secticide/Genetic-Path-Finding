#pragma once
#ifndef PDEBUG
#define PDEBUG

// Includes
#include <windows.h>
#include <cstdio>

// ---------------------------------------------------------------------
// PDebug (Project Debug) - Creates and manages a debug window
// ---------------------------------------------------------------------

class PDebug
{
// ---------------------------------------------------------------------
private:

	// Initialise
	bool initComplete;

	// Window Creation
	bool Create();

// ---------------------------------------------------------------------
public:

	// Constructor/Deconstructor
	PDebug();
	~PDebug();

	// Initialise
	void Initialise();
};

#endif