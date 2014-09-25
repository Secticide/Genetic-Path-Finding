////////////////////////////////////////////////////////////
// func.c -- class implementation for S-Expression functions
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

// Constructor
FunctionSet::FunctionSet (void)
{
	n = 0; // Currently no functions in the list
	maxn = 16; // Allocate room for 16 terminals to start
	functions = (SFunction *) malloc (maxn*sizeof(SFunction));
	nencapsulated = 0; // No encapsulated functions
}

// Destructor
FunctionSet::~FunctionSet (void)
{
	if (functions) 
	{
		//for (int i = 0; i < n; ++i)
			//delete functions[i].name;
			//free (functions[i].name);

		//delete functions;
		//free (functions);
	}
}

// Add a new function to the set
//
void FunctionSet::add (const char *name, int nargs, impfunc implementation, editfunc edit_function, int has_sides)
{
	// First, check to see if the name is already in the list
	for (int i = 0; i < n; ++i)
		if (! strcmp (functions[i].name, name))
		{
			cout << "FunctionSet Error: attempt to add existing function: " << functions[i].name << '\n';
			return;
		}

	// Check to see if nargs is in range
	if (nargs > MAX_SEXP_ARGS) {
		cout << "Error! Function " << name << " declared with " << nargs << "arguments.\nYou have to change MAX_SEXP_ARGS and recompile.\n";
	}

	// If we need more space, allocate it
	if (n == maxn - 1)
	{
		maxn *= 2;
		functions = (SFunction *) realloc (functions,
		maxn * sizeof (SFunction));
	}

	// Okay, now add it
	functions[n].name = strdup (name);
	functions[n].nargs = nargs;
	functions[n].func = implementation;
	functions[n].edit = edit_function;
	functions[n].active = 1;
	functions[n].side_effects = has_sides;
	functions[n++].s = NULL;
}

// Add a new function to the set
int FunctionSet::encapsulate (S_Expression *s, int nargs)
{
	// Construct a name
	char buffer[16];
	sprintf (buffer, "(E%d)", nencapsulated);
	++nencapsulated;

	// If we need more space, allocate it
	if (n == maxn - 1)
	{
		maxn *= 2;
		functions = (SFunction *)
		realloc (functions, maxn * sizeof (SFunction));
	}

	for (int i = 0; i < n; ++i)
	{
		if (functions[i].s && equiv (s, functions[i].s))
			return i;
	}

	// Okay, it's not a duplicate, so we add it
	functions[n].name = strdup (buffer);
	functions[n].nargs = nargs;
	functions[n].func = NULL;
	functions[n].edit = NULL;
	functions[n].active = 1;
	functions[n].side_effects = s->side_effects();
	functions[n].s = s->copy();
	cout << "\nEncapsulating " << buffer << " = " << functions[n].s << '\n';
	cout.flush();

	n++;
	return (n-1);
}

// Return the index number for the named function
int FunctionSet::index (const char *name)
{
	for (int i = 0; i < n; ++i)
		if (! strcmp (functions[i].name, name))
			return i;

	// Oops! We didn't find the function
	cout << "FunctionSet Error: could not index function: " << name << '\n';
	return -1;
}

// Look up the name of a function based on the index number
char * FunctionSet::getname (int ind)
{
	if (ind < 0 || ind >= n)
	{
		cout << "FunctionSet Error: attempted to getname with bad index (" << ind << ")\n";
		return NULL;
	}
	else
		return functions[ind].name;
}

// Print entire table
void FunctionSet::print (void)
{
	cout << "\nFunction set listing:\n"
	<< "---------------------\n";
	for (int i = 0; i < n; ++i) {
	cout << '\"' << functions[i].name << "\": "
	<< functions[i].nargs << " parameters ";
	if (! functions[i].active)
	cout << " (deleted) ";
	if (functions[i].s)
	cout << functions[i].s;
	cout << '\n';
	}
	cout << '\n';
}

// Declaration for the globally visible function set
FunctionSet Fset;