////////////////////////////////////////////////////////////
// sexp.c -- class implementation for S-Expression in C++
//
// written by Larry I. Gritz, 1993
// The George Washington University, Dept. of EE&CS
// Computer Graphics and Animation Lab
////////////////////////////////////////////////////////////

#include <iostream>
#include <malloc.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gp.h"

#include "random.h"

using namespace std;

static S_Expression *free_list = NULL;

// Constructor
S_Expression::S_Expression (void)
{
	type = STnone;
	val = 0;
	which = 0;

	for (int i = 0; i < MAX_SEXP_ARGS; ++i)
		args[i] = NULL;
}

void* S_Expression::operator new (size_t size)
{
	static long retrieved = 0;
	static long numalloced = 0;
	S_Expression *s;

	if (free_list)
	{
		s = free_list;
		free_list = s->args[0];
		++retrieved;
	}
	else
	{
		s = (S_Expression *) malloc (sizeof (S_Expression));
		++numalloced;
		if (! (numalloced % 100000))
			cout << "\nAllocated " << numalloced << " sexp cells (" << retrieved << ")\n";
	}

	return s;
}

void S_Expression::operator delete (void *s)
{
	S_Expression *se = (S_Expression *) s;

	for (int i = 0; i < MAX_SEXP_ARGS; ++i)
		if (se->args[i])
			delete se->args[i];

	((S_Expression *)s)->args[0] = free_list;
	free_list = (S_Expression *)s;
}

// Execute an encapsulated program, specified by index.
// This takes the recursion out of S_Expression::eval(),
// so that we can inline it.
float run_encapsulated_program(int ind)
{
	S_Expression *s = Fset.lookup_encapsulation (ind);

	return s->eval();
}

// Perform editing operation on this S_Expression,
// then return new expr
S_Expression *edit (S_Expression *s)
{
	if (s && (s->type == STfunction))
	{
		for (int i = 0; i < Fset.nargs (s->which); ++i)
			s->args[i] = edit (s->args[i]);

		editfunc e = Fset.editop (s->which);
		return e ? (*e)(s) : s;
	}
	return s;
}

// Perform permutation on a random node in this S-expression
void S_Expression::permute (void)
{
	cout << "Permuting:\nold = " << this << "\nnew = ";
	cout.flush();

	if (type == STfunction && Fset.nargs(which) > 1)
	{
		for (int i = Fset.nargs(which)-1; i; --i)
		{
			float u = random();
			int k = (int) floor (i * u);

			S_Expression *temp = args[k];
			args[k] = args[i];
			args[i] = temp;
		}
	}

	cout << this << "\n";
	cout.flush();
}

// Return a duplicate copy of the given S-Expression
S_Expression * S_Expression::copy (void)
{
	S_Expression *se = new S_Expression;
	se->type = type;
	se->val = val;
	se->which = which;

	for (int i = 0; i < MAX_SEXP_ARGS; ++i)
		if (args[i])
			se->args[i] = args[i]->copy();
		else se->args[i] = NULL;
			return se;
}

// Test for equivalence, return 1 if S-expressions are same
int equiv (S_Expression *s1, S_Expression *s2)
{
	if (! s1 && ! s2)
		return 1;

	if (!s1 || !s2)
		return 0;

	if (s1->type != s2->type)
		return 0;

	switch (s1->type)
	{
	case STnone:
		cout << "Error: bad case in S_Expression::==\n";
		return 0;

	case STconstant:
		return (s1->val == s2->val);

	case STterminal:
		return (s1->which == s2->which);

	case STfunction:
		if (s1->which != s2->which)
			return 0;

		for (int i = 0; i < MAX_SEXP_ARGS; ++i)
			if (!equiv (s1->args[i], s2->args[i]))
				return 0;

		return 1;
	}
}

// If the S_Expression is a numerical constant, put it in f.
int S_Expression::is_numerical (float& f)
{
	if (type == STconstant)
	{
		f = val;
		return 1;
	}
	else if (type == STterminal)
	{
		char *name = Tset.getname (which);

		if ((name[0] >= '0' && name[0] <= '9') || name[0] == '-')
		{
			f = Tset.lookup (which);
			return 1;
		}
	}

	return 0;
}

// Characterize the tree, returning its depth, the total
// number of nodesin the tree, the total number of internal
// nodes (those with children), and the total number of
// external nodes (those without children).
void S_Expression::characterize (int *depth, int *totalnodes, int *internal, int *external)
{
	*depth = 1; *totalnodes = 1;
	if (type == STfunction && Fset.nargs(which) >= 1)
	{
		int child_depth, child_total, child_internal;
		int child_external;

		*internal = 1;
		*external = 0;

		int nargs = Fset.nargs (which);
		int max_child_depth = 0;

		for (int i = 0; i < nargs; ++i)
		{
			args[i]->characterize (&child_depth, &child_total, &child_internal, &child_external);

			*totalnodes += child_total;
			*internal += child_internal;
			*external += child_external;

			if (child_depth > max_child_depth)
				max_child_depth = child_depth;
		}

		*depth += max_child_depth;
	}
	else
	{
		*internal = 0;
		*external = 1;
	}
}

// Return if the tree contains functions with side effects
int S_Expression::side_effects (void)
{
	if (type == STfunction)
	{
		if (Fset.has_sideeffects (which))
			return 1;

		for (int i = 0; i < Fset.nargs (which); ++i)
			if (args[i]->side_effects())
				return 1;
	}
	return 0;
}

S_Expression * S_Expression::selectany (int m, int *n, S_Expression ***ptr)
{
	(*n)++;

	if (*n == m)
		return (this);

	if (type == STfunction)
	{
		int nargs = Fset.nargs (which);

		for (int i = 0; i < nargs; ++i)
		{
			S_Expression *s = args[i]->selectany (m, n, ptr);

			if (s)
			{
				if (args[i] == s)
					*ptr = &(args[i]);

				return s;
			}
		}
	}
	return NULL;
}

S_Expression* S_Expression::selectexternal (int m, int *n, S_Expression ***ptr)
{
	if (type == STfunction && Fset.nargs(which) >= 1)
	{
		int nargs = Fset.nargs (which);
		for (int i = 0; i < nargs; ++i)
		{
			S_Expression *s=args[i]->selectexternal (m,n,ptr);

			if (s)
			{
				if (args[i] == s)
					*ptr = &(args[i]);

				return s;
			}
		}
		return NULL;
	}
	else
	{
		(*n)++;

		if (*n == m)
			return (this);
		else 
			return NULL;
	}
}

S_Expression* S_Expression::selectinternal (int m, int *n, S_Expression ***ptr)
{
	if (type == STfunction && Fset.nargs(which) >= 1)
	{
		(*n)++;

		if (*n == m)
			return (this);

		int nargs = Fset.nargs (which);

		for (int i = 0; i < nargs; ++i)
		{
			S_Expression *s=args[i]->selectinternal (m,n,ptr);

			if (s)
			{
				if (args[i] == s)
					*ptr = &(args[i]);

				return s;
			}
		}
	}

	return NULL;
}

S_Expression * S_Expression::select (float pip, S_Expression ***ptr)
{
	int depth, total, internal, external;
	int n = -1;
	int which;

	*ptr = NULL;

	characterize (&depth, &total, &internal, &external);

	if (random() < pip && internal >= 1)
	{
		which = (int) floor (random() * internal);
		return selectinternal (which, &n, ptr);
	}
	else if (external >= 1)
	{
		which = (int) floor (random() * external);
		return selectexternal (which, &n, ptr);
	}
}

// Perform the crossover between these two S-Expressions
void crossover(S_Expression **s1, S_Expression **s2, float pip)
{
	S_Expression **parent1ptr = NULL, **parent2ptr = NULL;
	S_Expression *fragment1, *fragment2;

	//char buffer[1024];

	fragment1 = (*s1)->select (pip, &parent1ptr);

	if (! parent1ptr)
		parent1ptr = s1;

	fragment2 = (*s2)->select (pip, &parent2ptr);

	if (! parent2ptr)
		parent2ptr = s2;

	cout.flush();
	*parent1ptr = fragment2;
	*parent2ptr = fragment1;
}

// Choose a random terminal (possibly including the
// ephemeral constant
static void random_terminal (S_Expression *s)
{
	int i = Tset.n + (ephemeral_constant ? 1 : 0);

	s->which = (int) floor (i * random());

	if (s->which >= Tset.n)
	{
		s->type = STconstant;
		s->val = ephemeral_constant();
	}
	else
		s->type = STterminal;
}

// Choose a random function
static void random_function (S_Expression *s)
{
	s->type = STfunction;
	s->which = (int) floor (Fset.n * random());
}

// Choose a random terminal or function
static void random_terminal_or_function (S_Expression *s)
{
	int i = Tset.n + Fset.n + (ephemeral_constant ? 1 : 0);

	s->which = (int) floor (i * random());

	if (s->which < Fset.n)
		s->type = STfunction;
	else
	{
		s->which -= Fset.n;

		if (s->which >= Tset.n)
		{
			s->type = STconstant;
			s->val = ephemeral_constant();
		}
		else s->type = STterminal;
	}
}

// Create a random S-Expression
S_Expression *random_sexpression(GenerativeMethod strategy, int maxdepth, int depth)
{
	S_Expression *s = new S_Expression;

	if(!depth)
	{
		maxdepth = 2 + (int) floor ((maxdepth - 1) * random());

		if (strategy == RAMPED_HALF_AND_HALF)
		{
			if (random() < 0.5)
				strategy = GROW;
			else
				strategy = FULL;
		}
	}

	++depth;

	switch (strategy)
	{
	case GROW :
		if (depth == maxdepth)
			random_terminal (s);
		else
			random_terminal_or_function (s);

		break;

	case FULL :
		if (depth == maxdepth)
			random_terminal (s);
		else
			random_function (s);

		break;

	case RAMPED_HALF_AND_HALF :
		cout << "random_sexpression: bad case\n";
		break;
	}

	if (s->type == STfunction)
		for (int i = 0; i < Fset.nargs(s->which); ++i)
			s->args[i] = random_sexpression (strategy, maxdepth, depth);

	return s;
}

// Chop off everything below a given depth
//
void S_Expression::restrict_depth (int maxdepth, int depth)
{
	++depth;

	if (type == STfunction)
	{
		if (depth == maxdepth-1)
		{
			for (int i = 0; i < Fset.nargs(which); ++i)
			if (args[i]->type == STfunction)
			{
				delete args[i];
				args[i]=random_sexpression(GROW,maxdepth-depth);
			}
		}
	}
	else
	{
		for (int i = 0; i < Fset.nargs(which); ++i)
			args[i]->restrict_depth (maxdepth, depth);
	}
}

// Put an ASCII representation of the S-expression into the
// given string
void S_Expression::write(string* s, int level)
{
	string buffer;

	//if(!level)
		//s[0] = 0;

	switch (type)
	{
	case STnone :
		cout << "Error: bad case in S_Expression::write\n";
		break;

	case STconstant:
		buffer += "%g";
		buffer += val;
		*s += buffer;
		break;

	case STterminal:
		*s += Tset.getname(which);
		break;

	case STfunction:
		*s += "(";
		*s += Fset.getname(which);

		for (int i = 0; i < Fset.nargs(which); ++i)
		{
			*s += " ";
			args[i]->write(s, level+1);
		}

		*s += ")";
		break;
	}
}

// Stream output of S-expressions
ostream& operator<< (ostream& out, S_Expression *s)
{
	string buffer;

	s->write(&buffer);

	return (out << buffer);
}

EPHEMERAL ephemeral_constant = NULL;