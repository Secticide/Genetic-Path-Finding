////////////////////////////////////////////////////////////
// gp.c - implementation for Genetic Programming in C++
// written by Larry I. Gritz, 1993
// The George Washington University, Dept. of EE&CS
// Computer Graphics and Animation Lab
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Modified by Jak R. Boulton, 2013
// University of Glamorgam, Software Engineering
////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <math.h>
#include <stdio.h>

#include "gp.h"
#include "random.h"

float default_ephemeral_generator (void)
{
	// Return a number between -1.0 and 1.0
	return (2.0 * random() - 1.0);
}

// GP constructor
GP::GP(FITNESSFUNC fitfun, int pop_size)
{
	// Set up the default values
	M = pop_size;
	G = 51;
	pc = 0.9;
	pr = 0.1;
	pip = 0.9;
	Dcreated = 17;
	Dinitial = 6;
	pm = 0;
	pp = 0;
	fed = 0;
	pen = 0;
	dec_cond = NULL;
	pd = 0;

	generative_method = RAMPED_HALF_AND_HALF;
	reproduction_selection = FITNESS_PROPORTIONATE;
	second_parent_selection = FITNESS_PROPORTIONATE;
	use_greedy_overselection = (G >= 1000);
	overselection_boundary = (float)((G < 1000) ? 0.32 : (320 / M));
	use_elitist_strategy = 0;

	// Set up housekeeping info
	initialized = 0;
	gen = 0;
	pop = new Individual[M+1];
	newpop = new Individual[M+1];
	best_of_run.s = NULL;
	bestofrun_gen = 0;
	bestofgen_index = 0;
	bestofgen_hits = 0;
	bestofgen_sfit = 0;
	worstofgen_sfit = 0;
	avgofgen_sfit = 0;

	// Other defaults
	verbose = QUIET;
	termination_criteria = NULL;
	fitness_function = fitfun;
	standardize_fitness = NULL;
	sfit_dontreport = 1.0e20;
	generation_callback = NULL;
	bestworst_freq = 1;
	stat_filename = NULL;
	stat_file = NULL;
}

// GP Destructor
GP::~GP (void)
{
	if (stat_file) fclose (stat_file);
	if (pop) delete[M+1] pop;
	if (newpop) delete[M+1] newpop;
}

// Initialize the population
void GP::init (void)
{
	if (stat_filename)
		stat_file = fopen (stat_filename, "wt");

	float ptotal = pr + pc + pm + pp + pen;

	if (ptotal < 0.001)
		cout << "Error: (pr + pc + pm + pp + pen) should sum to 1.0\n";

	pr /= ptotal;
	pc /= ptotal;
	pm /= ptotal;
	pp /= ptotal;
	pen /= ptotal;
	best_of_run.sfit = 1.0e20;
	gen = 0;
}

// Create the generation 0 population
void GP::create_population (void)
{
	if (verbose & TELL_INITIALIZE)
		cout << "Creating initial population...\n";

	for (int i = 0; i < M; ++i)
	{
		pop[i].s = random_sexpression(generative_method, Dinitial, 0);

		// Make sure we don't have a duplicate
		int duplicated = 0;

		for (int j = 0; j < i && !duplicated; ++j)
		{
			if (equiv (pop[i].s, pop[j].s))
			{
				delete pop[i].s;
				pop[i].s = NULL;
				duplicated = 1;
			}
		}

		if (duplicated)
		{
			i -= 1;
		}
		else
		{
			pop[i].recalc_needed = 1;
			newpop[i].s = NULL;

			if (verbose & LIST_INITIAL_EXPRESSIONS)
			{
				cout << i << ": " << pop[i].s << '\n';
				cout.flush();
			}
		}
	}
}

void GP::list_parameters (void)
{
	if (verbose & LIST_PARAMETERS)
	{
		cout << "Parameters used for this run:\n";
		cout << "=============================\n";
		cout << "Max generations: " << G << '\n';
		cout << "Population size: " << M << '\n';
		cout << "Max depth for new individuals: " << Dinitial << '\n';
		cout << "Max depth for crossed and mutated individuals: " << Dcreated << '\n';
		cout << "Fitness-proportionate reproduction fraction: " << pr << '\n';
		cout << "Crossover fraction: " << pc << " (" << pip*10.0 << "% at internal points)\n";
		cout << "Mutation fraction: " << pm << '\n';
		cout << "Permutation fraction: " << pp << '\n';
		cout << "Encapsulation fraction: " << pen << '\n';
		cout << "Selection method: ";

		switch (reproduction_selection)
		{
		case UNIFORM:
			cout << "uniform\n";
			break;

		case FITNESS_PROPORTIONATE:
			cout << "fitness-proportionate\n";
			break;

		case TOURNAMENT:
			cout << "tournament, size = " << tournament_size << '\n';
			break;

		case RANK:
			cout << "rank\n";
			break;
		}
		
		cout << "Generation method: ";

		switch (generative_method)
		{
		case GROW:
			cout << "grow\n";
			break;

		case FULL:
			cout << "full\n";
			break;

		case RAMPED_HALF_AND_HALF:
			cout << "ramped half-and-half\n";
			break;
		}
	}
}

// Given a particular selection method, choose a random
// member of the population and return its index.
static int choose_random (GP *gp, SelectionMethod method)
{
	int which = -1;
	int i, j;
	float f = 0;

	switch (method)
	{
	case FITNESS_PROPORTIONATE:
		f = random();

		if (gp->use_greedy_overselection)
		{
			// 80% of the time, select from best
			if (random() < 0.8)
				f *= gp->overselection_boundary;
			else // 20% of the time, use the rest
				f = gp->overselection_boundary + f * (1.0 - gp->overselection_boundary);
		}

		for (j = 0; j < gp->M; ++j)
			if (f <= gp->pop[j].sumnfit)
				return j;

		cout << "Ran past end in choose_random "<< f <<"\n";
		cout.flush();

	// Purposely go to next case if we hit this
	case UNIFORM:

		which = (int) floor (random() * gp->M);
		break;

	case TOURNAMENT:

		for (i = 0; i < gp->tournament_size; ++i)
		{
			j = (int) floor (gp->M * random());

			if (!i || gp->pop[j].nfit > gp->pop[which].nfit)
				which = j;
		}

		break;
	}

	if (which < 0 || which >= gp->M)
	{
		cout << "Ug! invalid choice in choose_random\n";
		return 0;
	}

	return which;
}

// Sort pop by normalized fitness value. Do a selection
// sort, using newpop as temporary storage.
void GP::sort_fitness (void)
{
	float total = 0;

	for (int i = 0; i < M; ++i)
	{
		// Invariant: pop[0..i-1] contains the i largest
		//fitnesses, sorted
		int biggest = i;

		for (int j = i+1; j < M; ++j)
			if (pop[j].nfit > pop[biggest].nfit)
				biggest = j;

		// Now j contains the biggest element in [i+1..M-1] so
		// we swap elem i with biggest element in [i+1..M-1].
		if (biggest != i)
		{
			S_Expression *temp1 = pop[i].s;
			S_Expression *temp2 = pop[biggest].s;
			pop[i].s = pop[biggest].s = NULL;
			Individual temp = pop[i];
			pop[i] = pop[biggest];
			pop[biggest] = temp;
			pop[i].s = temp2;
			pop[biggest].s = temp1;
		}

		total += pop[i].nfit;
		pop[i].sumnfit = total;
		// Now pop[0..i] contains i+1 largest fitnesses
	}
}

// Create the next generation of individuals
void GP::nextgen (void)
{
	S_Expression *s, **parentptr;

	if(use_greedy_overselection)
		sort_fitness();

	for (int i = 0; i < M; ++i)
	{
		if (! i && use_elitist_strategy)
		{
			newpop[i] = best_of_run;
			continue;
		}

		// All ops require reproducing one individual first
		newpop[i] = pop[choose_random (this, reproduction_selection)];

		float option = random();

		if (option <= pc)
		{
			// Crossover operation
			newpop[i+1] = pop[choose_random (this, second_parent_selection)];

			crossover (&(newpop[i].s), &(newpop[i+1].s), pip);

			newpop[i].s->restrict_depth (Dcreated);
			newpop[i].recalc_needed = 1;
			newpop[i+1].s->restrict_depth (Dcreated);
			newpop[i+1].recalc_needed = 1;
			++i;
		}
		// Any non-plain varieties of reproduction?
		else if (option <= (pc+pm))
		{
			// Mutation operation
			int depth, total, internal, external, n = -1, m;
			newpop[i].s->characterize (&depth, &total, &internal, &external);

			m = (int) floor (total * random());
			s = newpop[i].s->selectany (m, &n, &parentptr);

			if (! parentptr)
				parentptr = &(newpop[i].s);

			*parentptr = random_sexpression (GROW, 6);
			delete s;
		}
		else if (option <= (pc+pm+pp))
		{
			// Permutation operation
			s = newpop[i].s->select (1.0, &parentptr);
			s->permute();
		}
		else if (option <= (pc+pm+pp+pen))
		{
			// Encapsulation operation
			// For now, just do reproduction
			s = newpop[i].s->select (1.0, &parentptr);

			if (! parentptr)
				parentptr = &(newpop[i].s);

			int e = Fset.encapsulate (s);

			delete s;
			s = new S_Expression;
			s->type = STfunction;
			s->which = e;
			*parentptr = s;
		}
		// else Just plain reproduction, don't do any additional work
	}

	// Swap the current and next generations
	Individual *temp = pop;
	pop = newpop;
	newpop = temp;
}

// Evaluate the fitness of each individual in the population
void GP::eval_fitnesses (void)
{
	float total_afitness = 0.0;
	int i;
	bestofgen_sfit = 1.0e20;
	worstofgen_sfit = -1.0e20;
	avgofgen_sfit = 0;

	for (i = 0; i < M; ++i)
	{
		if (pop[i].recalc_needed)
		{
			pop[i].rfit = (*fitness_function)(pop[i].s, &(pop[i].hits));
			
			if (standardize_fitness)
				pop[i].sfit = standardize_fitness (pop[i].rfit);
			else
				pop[i].sfit = pop[i].rfit;

			pop[i].afit = 1.0 / (1.0 + pop[i].sfit);
			pop[i].recalc_needed = 0;
		}

		total_afitness += pop[i].afit;
		avgofgen_sfit += pop[i].sfit;

		if (pop[i].sfit < bestofgen_sfit)
		{
			bestofgen_sfit = pop[i].sfit;
			bestofgen_index = i;
			bestofgen_hits = pop[i].hits;
		}
		else if(pop[i].sfit > worstofgen_sfit)
			worstofgen_sfit = pop[i].sfit;

		if (verbose & LIST_GENERATIONAL_FITNESSES && pop[i].sfit < sfit_dontreport)
		{
			cout << "\t\t" << i << ": ";
			cout << pop[i].rfit << " " << pop[i].afit << " " << pop[i].hits << " hits";

			if (bestofgen_index == i)
				cout << " (best-of-generation)";

			if (pop[i].sfit < best_of_run.sfit)
				cout << " (best-so-far)";

			cout << '\n';
			cout.flush();
		}
	}

	avgofgen_sfit /= (float)M;
	float total = 0;

	for (i = 0; i < M; ++i)
	{
		pop[i].nfit = pop[i].afit / total_afitness;
		total += pop[i].nfit;
		pop[i].sumnfit = total;
	}

	if (bestofgen_sfit < best_of_run.sfit)
	{
		best_of_run = pop[bestofgen_index];
		bestofrun_gen = gen;
	}

	if (stat_file)
		fprintf (stat_file, "%d %g %g %g\n", gen, bestofgen_sfit, worstofgen_sfit, avgofgen_sfit);
}

void GP::report_on_run (void)
{
	if (verbose & END_REPORT)
	{
		cout << "\nThe best-of-run individual program was found on generation "
			<< bestofrun_gen << "\nand had a standardized fitness measure of "
			<< best_of_run.sfit << " and " << best_of_run.hits
			<< " hits:\n";

		cout << best_of_run.s << '\n';

		if (verbose & SHOW_EDITED_BEST)
		{
			cout << "The edited version of the best-of-run individual is:\n";

			S_Expression *edited = best_of_run.s->copy();
			edited = edit(edited);

			cout << edited << "\n\n";
			delete edited;
		}

		cout.flush();
	}
}

// The actual GP run is invoked with "go":
void GP::go(int maxgens)
{
	string buffer;
	G = maxgens;

	if (gen == 0)
	{
		init ();
		create_population ();
	}

	for ( ; gen <= G; ++gen)
	{
		int report; // 1 if we report info on this generation

		if (bestworst_freq == 0)
			report = 0;
		else if (bestworst_freq == 1)
			report = 1;
		else
			report = !(gen % bestworst_freq);

		if (gen > 0)
			nextgen ();

		if (verbose & GENERATION_UPDATE)
		{
			cout << "\rGeneration " << gen << ' ';
			cout.flush();
		}

		eval_fitnesses();

		if (report && (verbose & GENERATION_UPDATE))
		{
			cout << "\n--------------\n";
			cout << "average standardized fitness of gen was " << avgofgen_sfit << ".\n";
			cout << "worst of gen had standardized fitness " << worstofgen_sfit << ".\n";
			cout << "best of gen had standardized fitness " << bestofgen_sfit << " and " << bestofgen_hits << " hits:\n";

			pop[bestofgen_index].s->write(&buffer);
			cout << buffer;
			cout << "\n";

			if (verbose & SHOW_EDITED_BEST)
			{
				S_Expression *s=pop[bestofgen_index].s->copy();
				s = edit (s);
				s->write(&buffer);
				cout << "\nEdited = " << buffer << "\n";
				delete s;
			}

			cout << '\n';
		}
		else if (verbose & GENERATION_UPDATE)
		{
			cout << "(best sfit " << best_of_run.sfit << ", "
			<< best_of_run.hits << " hits) ";
		}

		cout.flush();

		if(generation_callback)
			generation_callback(this);

		if(termination_criteria)
			if((*termination_criteria)(this))
				break;
	}
	if (verbose & GENERATION_UPDATE)
	{
		cout << '\n';
		cout.flush();
	}

	report_on_run();
}

// Debugging tool: print the entire population and fitness
void GP::print_population (void)
{
	for (int i = 0; i < M; ++i)
		cout << i << ": [sfit " << pop[i].sfit << "] " << pop[i].s << '\n';
}