#pragma once
#ifndef NODE
#define NODE

// Includes
#include <math.h>

// Definition of the world size (20 x 20)
#define WORLD_SIZE 20

// ---------------------------------------------------------------------
// Node - Simple node structure used with the pathfinding algorithm
// ---------------------------------------------------------------------

struct Node
{
public:
	int x, z;
	int ID;
	Node* parent;
	float G; // Cost from start node
	float H; // Heuristic distance to goal

	Node() : parent(0) {}; // Default constructor ( : parent(0) is same as {parent = 0;})
	Node(int x, int z, Node *_parent = 0) : x(x), z(z), parent(_parent), ID(z * WORLD_SIZE + x), G(0), H(0) {};

	float getF() { return G + H; };

	// Manhatten Distance to goal
	float manhattenDistance( Node *nodeEnd )
	{
		float x = (fabs((float)this->x - (float)nodeEnd->x));
		float z = (fabs((float)this->z - (float)nodeEnd->z));

		return x+ z;
	}

	// Euclidian Distance
	float EuclidianDistance( Node *nodeEnd )
	{
		float x = ((float)this->x - (float)nodeEnd->x) * ((float)this->x - (float)nodeEnd->x);
		float z = ((float)this->z - (float)nodeEnd->z) * ((float)this->z - (float)nodeEnd->z);

		return sqrt(x + z);
	}
};

#endif