
// ---------------------------------------------------------------------
// PDebug Implementation
// ---------------------------------------------------------------------

#include "PDebug.h"

#include <iostream>

using namespace std;

// ---------------------------------------------------------------------
// PRIVATE
// ---------------------------------------------------------------------

// Console Creation
// Precondition: N/A
// Postcondition: Debug window allocated/Standard output redirected
bool PDebug::Create()
{
	// Allocate a new console
	// If creation fails,
	if(AllocConsole())
	{
		// Redirect Standard output to the console
		freopen("CONOUT$", "w", stdout);

		// Return creation success
		return 1;
	}

	// Return creation failure
	return 0;
}

// ---------------------------------------------------------------------
// PUBLIC
// ---------------------------------------------------------------------

// Constructor
PDebug::PDebug()
{
	// Initialise attributes
	initComplete = 0;
}

// Deconstructor
PDebug::~PDebug()
{
	// Free the console
	FreeConsole();
}

// Initialise
// Precondition: N/A
// Postcondition: Debug Window setup
void PDebug::Initialise()
{
	// If the window has not been create
	if(!initComplete)
	{
		// Console info structure
		CONSOLE_SCREEN_BUFFER_INFO coninfo;

		// Console handle
		HANDLE console = GetConsoleWindow();

		// Console Mode
		DWORD lpMode;

		// Create window
		initComplete = Create();

		// Set console title
		SetConsoleTitle(TEXT("Debug Console"));

		// Set larger buffer size
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
		
		coninfo.dwSize.Y = 2500; // Large enough to remain useful
		coninfo.dwSize.X = 237; // Full HD Max buffer width

		// Set the size of the output buffer
		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

		// Allow Console scrolling
		GetConsoleMode(console, &lpMode);
		SetConsoleMode(console, lpMode & ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT);
	}
}

