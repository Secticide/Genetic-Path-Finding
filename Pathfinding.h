#pragma once
#ifndef PATHFINDING
#define PATHFINDING

// Includes
#include "Node.h"

#include <vector>
#include <CoreStructures\GUVector4.h>

// Typedef for function pointer
typedef bool (*checkMap)(int x, int z);

// ---------------------------------------------------------------------
// Pathfinding - Used to find paths across the map
// ---------------------------------------------------------------------

class Pathfinding
{
private:

	// ATTRIBUTES
	
	Node *start;
	Node *end;
	std::vector<Node*> open;
	std::vector<Node*> closed;
	std::vector<CoreStructures::GUVector4*> path;

	int current;

	// METHODS

	void SetStartAndEnd(Node startNode, Node endNode);
	void PathOpened(int x, int z, float newCost, Node* parent);
	Node* GetNextNode();
	void ContinuePath();

	// Map Checker function
	checkMap isCellBlocked;

public:

	// ATTRIBUTES

	// Has the start node been initialised?
	bool startInitComplete;

	// Has the end node been found?
	bool endFound;

	// METHODS

	// Constructor/Deconstructor
	Pathfinding();
	~Pathfinding();

	// Find a path from the start to the end
	void FindPath(CoreStructures::GUVector4 currentPos, CoreStructures::GUVector4 endPos);

	// Return the next path position
	CoreStructures::GUVector4 GetNextPosition();

	// Reset to the start of the path
	void ResetPath();

	// Clear lists
	void ClearOpenList() { open.clear(); }
	void ClearClosedList() { closed.clear(); }
	void ClearPath() { path.clear(); }

	// Attach checker function
	void attachMapCheck(checkMap func);
};

#endif