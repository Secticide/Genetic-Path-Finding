////////////////////////////////////////////////////////////
// terminal.c - implementation for S-Expression terminals
//
// written by Larry I. Gritz, 1993
// The George Washington University, Dept. of EE&CS
// Computer Graphics and Animation Lab
////////////////////////////////////////////////////////////

#include <iostream>
#include <malloc.h>
#include <string>
#include <stdio.h>
#include "gp.h"

using namespace std;

// Constructor for a TerminalSet
TerminalSet::TerminalSet (void)
{
	n = 0; // Currently no terminals in the list
	maxn = 16; // Allocate room for 16 terminals to start
	terminals = (Terminal *) malloc (maxn*sizeof(Terminal));
}

// Destructor
TerminalSet::~TerminalSet (void)
{
	if (terminals)
	{
		//for (int i = 0; i < n; ++i)
			//free (terminals[i].name);

		//delete terminals;
		//free (terminals);
	}
}

// Add a new terminal to the set
void TerminalSet::add (const char *name, float val)
{
	// First, check to see if the name is already in the list
	for (int i = 0; i < n; ++i)
	{
		if (! strcmp (terminals[i].name, name))
		{
			cout << "TerminalSet Error: attempt to add existing terminal: " << terminals[i].name << '\n';
			return;
		}
	}

	// If we need more space, allocate it
	if (n == maxn - 1)
	{
		maxn *= 2;
		terminals = (Terminal *) realloc (terminals,
		maxn * sizeof (Terminal));
	}

	// Okay, now add it
	terminals[n].name = strdup (name);
	terminals[n++].val = val;
}

// Modify the value of a terminal in the set
//
void TerminalSet::modify (const char *name, float val)
{
	for (int i = 0; i < n; ++i)
	{
		if (! strcmp (terminals[i].name, name))
		{
			terminals[i].val = val;
			return;
		}
	}

	// Oops! We didn't find the terminal
	cout << "TerminalSet Error: attempt to modify non-existing terminal: " << name << '\n';
}

// Retrieve the current value of a terminal
float TerminalSet::get (const char *name)
{
	for (int i = 0; i < n; ++i)
		if (! strcmp (terminals[i].name, name))
			return terminals[i].val;
	
	// Oops! We didn't find the terminal
	cout << "TerminalSet Error: could not find terminal: " << name << '\n';

	return 0;
}

// Return the index number for a named terminal
int TerminalSet::index (const char *name)
{
	for (int i = 0; i < n; ++i)
		if (! strcmp (terminals[i].name, name))
			return i;

	// Oops! We didn't find the terminal
	cout << "TerminalSet Error: could not index terminal: " << name << '\n';
	return -1;
}

// Print the entire table of terminals
void TerminalSet::print (void)
{
	cout << "\nTerminal set listing:";
	cout << "\n---------------------\n";

	for (int i = 0; i < n; ++i)
		cout << '\"' << terminals[i].name << "\" = " << terminals[i].val << '\n';

	cout << '\n';
}

// Declaration for the globally visible terminal set
TerminalSet Tset;