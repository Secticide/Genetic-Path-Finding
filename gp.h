#pragma once
#ifndef LIBGP
#define LIBGP
#define NULL 0

///////////////////////////////////////////////////////////
// gp.h -- class definitions for Genetic Programming in C++
//
// written by Larry I. Gritz, 1993
// The George Washington University, Dept. of EE&CS
// Computer Graphics and Animation Lab
///////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Modified by Jak R. Boulton, 2013
// University of Glamorgam, Software Engineering
////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>

using namespace std;

///////////////////////////////////////////////////////////
// Some type declarations and forward declarations
///////////////////////////////////////////////////////////

class S_Expression;
class GP;

typedef float (*impfunc)(S_Expression **);
typedef S_Expression* (*editfunc)(S_Expression *);
typedef int (*CONDITION)(GP *);		// A condition
									// function pointer
typedef float (*FLOATFUNC)(float);	// takes float,
									// returns float

// takes nothing, returns float
typedef float (*EPHEMERAL)(void);

// Types of S_Expression nodes
enum SEXP_TYPE {STnone, STconstant, STterminal, STfunction};

// Ways of generating random S-Expression trees
enum GenerativeMethod { GROW, FULL, RAMPED_HALF_AND_HALF };

// Maximum number of arguments to an S-Expression function
#define MAX_SEXP_ARGS 4

// Fitness evaluation function
typedef float (*FITNESSFUNC)(S_Expression *s, int *hits);

// Methods of selecting individuals for reproduction
enum SelectionMethod { UNIFORM, FITNESS_PROPORTIONATE, TOURNAMENT, RANK };

// Types of fitness measures
enum FitnessMeasure { RAW, ADJUSTED };

// Flags for how much stuff to print out during a run
enum GPVerbosity {	QUIET = 0,
					TELL_INITIALIZE = 1,
					LIST_INITIAL_EXPRESSIONS = 2,
					GENERATION_UPDATE = 4,
					LIST_GENERATIONAL_FITNESSES = 8,
					SHOW_EDITED_BEST = 16,
					END_REPORT = 32,
					LIST_PARAMETERS = 64,
					DEBUG = -1
					};


///////////////////////////////////////////////////////////
// TerminalSet class
///////////////////////////////////////////////////////////

class TerminalSet
{
private:
	struct Terminal
	{
		char *name; // Name of the terminal
		float val; // Current value
	};

	int maxn; // Total terminals allocated
	Terminal *terminals; // The actual terminals

public:
	int n; // Current number of terminals

public:
	// Constructor and destructor
	TerminalSet (void);
	~TerminalSet (void);

	// Add a new terminal to the set
	void add (const char *name, float val = 0);

	// Modify the value of a terminal specified by name
	void modify (const char *name, float val);

	// Modify the value of a terminal specified by index
	void modify (const int index, float val)
	{ terminals[index].val = val; }

	// Retrieve the value of a terminal specified by name
	float get (const char *name);

	// Look up the value based on the index number
	float get (int ind) { return terminals[ind].val; }

	// Return the index number for the named terminal
	int index (const char *name);

	// Look up the value based on the index number
	float lookup (int ind) { return terminals[ind].val; }

	// Look up the name based on the index number
	char *getname (int ind) { return terminals[ind].name; }

	// Print entire table
	void print (void);
};

// Definition for the terminal set
extern TerminalSet Tset;

//////////////////////////////////////////////////////////
// FunctionSet class
//////////////////////////////////////////////////////////

class FunctionSet
{
private:
	struct SFunction
	{
		char *name; // Name of the function
		int nargs; // Number of arguments it takes
		impfunc func; // The implementation
		editfunc edit; // The editing function
		int active; // 0 == don't use this function
		int side_effects; // 1 == has side effects
		S_Expression *s; // non-NULL means that it's
						// encapsulated function.
	};

	int maxn; // Total functions allocated
	SFunction *functions; // The actual function records
	int nencapsulated; // Number of encaps. functions
public:
	int n; // Current number of functions

	// Constructor & Destructor
	FunctionSet (void);
	~FunctionSet (void);

	// Add a new function to the set
	void add (const char *name, int nargs, impfunc implementation, editfunc edit_function = NULL, int has_sides = 0);

	// Add a new function which executes an S-Expression
	int encapsulate (S_Expression *s, int nargs = 0);

	// Remove a named function from the set
	void remove (const char *name)
	{
		int i = index (name);

		if (i >= 0)
			functions[i].active = 0;
	}

	// Remove an indexed function from the set
	void remove (int ind)
	{
		if (ind >= 0 || ind < n)
			functions[ind].active = 0;
	}

	// Return the index number for the named function
	int index (const char *name);

	// Look up the name of a function based on the index
	char *getname (int ind);

	// Look up the # of arguments of a function
	int nargs (int ind) { return functions[ind].nargs; }

	// Return 1 if the indexed function has side effects
	int has_sideeffects (int ind)
	{ return functions[ind].side_effects; }

	// Return a pointer to indexed function implementation
	impfunc lookup_implementation (int ind)
	{ return functions[ind].func; }

	// Return a pointer to the indexed function encapsulation
	S_Expression *lookup_encapsulation (int ind)
	{ return functions[ind].s; }

	// Return 1 if the indexed function is an encapsulation
	int is_encapsulated (int ind)
	{ return (functions[ind].s ? 1 : 0); }

	// Return a pointer to indexed function edit operation
	editfunc editop (int ind) { return functions[ind].edit; }

	// Print entire table
	void print (void);
};

// Declare the globally visible function set
extern FunctionSet Fset;

//////////////////////////////////////////////////////////
// S_Expression class
//////////////////////////////////////////////////////////

class S_Expression
{
public:
	SEXP_TYPE type;
	float val; // value if type == STconstant
	int which; // index of terminal or function

	S_Expression* args[MAX_SEXP_ARGS];

	// Constructor
	S_Expression();

	// new and delete operators
	void* operator new (size_t size);
	void operator delete (void *);

	// Runs an encapsulated program, specified by index
	friend float run_encapsulated_program(int ind);

	// Evaluate this S_Expression
	float eval(void)
	{
		float f;

		if (type == STfunction) 
		{
			if (Fset.is_encapsulated (which))
			{
				f = run_encapsulated_program(which);
			}
			else
			{
				impfunc func = (impfunc) Fset.lookup_implementation (which);
				f = (*func)(args);
			}
		}
		else if (type == STterminal)
		{
			f = Tset.lookup(which);
		}
		else if (type == STconstant)
		{
			f = val;
		}
		else
		{
			cout << "Error: bad case in S_Expression::eval\n";
			std::string buffer;
			write(&buffer);
			cout << buffer << '\n';
			f = 0;
		}

		return f;
	};

	// Edit the tree (destructively), return ptr to new root
	friend S_Expression *edit(S_Expression *s);

	// Permute the arguments of this functional S-Expression
	void permute (void);

	// Make another copy
	S_Expression* copy (void);

	// Test for equivalence between two S_Expressions
	friend int equiv(S_Expression *s1, S_Expression *s2);

	// Is the S_Expression a numerical constant?
	int is_numerical(float& f);

	// Count stuff
	void characterize(int *depth, int *total, int *internal, int *external);

	// Does the tree below have functions with side effects?
	int side_effects(void);

	// Select points
	S_Expression * selectany(int, int *, S_Expression ***ptr);
	S_Expression * selectinternal(int, int *, S_Expression ***ptr);
	S_Expression * selectexternal(int, int *, S_Expression ***ptr);
	S_Expression * select(float pip, S_Expression ***ptr);

	// Perform crossover operation
	friend void crossover (S_Expression **s1, S_Expression **s2, float pip);

	// Make a random tree
	friend S_Expression *random_sexpression(GenerativeMethod strategy, int maxdepth=6, int depth=0);

	// Chop off below a certain depth
	void restrict_depth(int maxdepth = 17, int depth = 0);

	// Write the lisp code into a string
	void write(std::string* s, int level = 0);

	// Output to a stream
	friend ostream& operator<< (ostream& out, S_Expression *s);

	// Make an S_Expression from a LISP string
	friend S_Expression *sexify(char **s);
	friend S_Expression *sexify(char *s);
};

extern EPHEMERAL ephemeral_constant;

////////////////////////////////////////////////////////////
// Individual and GP classes
////////////////////////////////////////////////////////////

class Individual
{
public:
	S_Expression *s; // The S-expression
	float rfit, sfit; // raw, standardized, adjusted, and
	float afit, nfit; // normalized fitness measures
	float sumnfit; // Sum of nfitnesses up to this guy
	int hits; // Number of hits
	int recalc_needed; // Do we need to recalculate?

	// Constructor and destructor
	Individual (void)
	{
		s = NULL;
		rfit = 0;
		sfit = 0;
		afit = 0;
		nfit = 0;
		hits = 0;
		recalc_needed = 1;
	}
	~Individual (void) { if (s) delete s; }

	// Assignment operator
	void operator= (Individual& i)
	{
		if (s) delete s;
			if (i.s) s = i.s->copy();
		else
			s = NULL;

		rfit = i.rfit;
		sfit = i.sfit;
		afit = i.afit;
		nfit = i.nfit;
		sumnfit = i.sumnfit;
		hits = i.hits;
		recalc_needed = i.recalc_needed;
	}
};


class GP
{
public:
	// Parameters controlling the genetic programming run
	int M; // The population size
	int G; // The number of generations to run
	float pc; // Probability of crossover
	float pr; // Probability of reproduction
	float pip; // Probability of internal crossover
	int Dcreated; // Max depth of S-Expressions
	int Dinitial; // Max depth of initial population
	float pm; // Probability of mutation
	float pp; // Probability of permutation
	int fed; // Frequency of performing editing
	float pen; // Probability of encapsulation
	CONDITION dec_cond; // Condition for decimation
	float pd; // Decimation percentage

	GenerativeMethod generative_method;
	SelectionMethod reproduction_selection;
	SelectionMethod second_parent_selection;
	int use_greedy_overselection;
	float overselection_boundary;
	int use_elitist_strategy;

	// Number to use when using tournament selection
	int tournament_size;

	// Housekeeping information
	int initialized;
	int gen; // Current generation number
	Individual *pop; // The actual population
	Individual *newpop; // Population we're building
	Individual best_of_run; // Housekeeping information
	int bestofrun_gen; // Gen when best_so_far was found
	int bestofgen_index; // index of this generation's best
	int bestofgen_hits; // hits of this generation's best
	float bestofgen_sfit; // sfitness of this gen's best
	float worstofgen_sfit; // sfitness of this gen's worst
	float avgofgen_sfit; // average sfitness of this gen

	// User-defined funcs for controlling the GP run and I/O
	int verbose;
	FITNESSFUNC fitness_function; // The fitness evaluation function
	CONDITION termination_criteria; // When do we terminate?
	FLOATFUNC standardize_fitness; // Fitness standardization
	float sfit_dontreport; // Don't report fitness >= this
	CONDITION generation_callback; // Call me after each gen
	int bestworst_freq; // How often to report best/worst?
	char *stat_filename;
	FILE *stat_file;

// Public methods
public:
	// Constructor & destructor
	GP (FITNESSFUNC fitness_function, int popsize = 500);
	~GP (void);

	// The actual GP run is invoked with "go":
	void go (int maxgens = 50);

	// Print all the S-expressions and fitness values
	void print_population (void);

// Stuff used internally
private:

	// Initialize various structures
	void init (void);

	// Create the initial random population
	void create_population (void);

	// Make a new generation from the current one
	void nextgen (void);

	// Sort pop by normalized fitness
	void sort_fitness (void);

	// Calculate fitnesses & stats
	void eval_fitnesses (void);

	// Print some end-of-run statistics
	void report_on_run (void);

	// List the parameters for the run
	void list_parameters (void);
};

float default_ephemeral_generator (void);


////////////////////////////////////////////////////////////
// "Novice" canned declarations for commonly used functions
// found in stdfunc.c.
////////////////////////////////////////////////////////////

void use_addition (void), use_subtraction (void);
void use_multiplication (void), use_protected_division (void);
void use_sin (void), use_cos (void), use_atg (void);
void use_exp (void), use_rlog (void);
void use_gt (void), use_abs (void), use_sig (void);
void use_and (void), use_or (void), use_not (void);
void use_if (void), use_eq (void);
void use_progn2 (void), use_progn3 (void);
void use_srt (void), use_sq (void), use_cub (void);
void use_ifltz (void), use_iflte (void);
void use_srexpt (void), use_pow (void);
void use_du (void);
extern int max_du_iterations;
void use_setsv (void);

#endif