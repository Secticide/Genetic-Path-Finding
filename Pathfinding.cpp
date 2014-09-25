
// ---------------------------------------------------------------------
// Pathfinding Implementation
// ---------------------------------------------------------------------

#include "Pathfinding.h"

#include <iostream>

// Namespace use
using namespace std;
using namespace CoreStructures;

// ---------------------------------------------------------------------
// PRIVATE
// ---------------------------------------------------------------------

// Set start and end nodes
void Pathfinding::SetStartAndEnd(Node startNode, Node endNode)
{
	//Create start and end cells
	start = new Node( startNode.x, startNode.z, NULL);
	end = new Node( endNode.x, endNode.z, &endNode );

	start->G = NULL; // we havent moved so this is 0
	start->H = start->EuclidianDistance( end ); // set the hueristic to the end goal
	start->parent = NULL; // just started, has no parent

	// place the start node on the open list ready for inquiry
	open.push_back(start);
}

// Path Opened
void Pathfinding::PathOpened(int x, int z, float newCost, Node* parent)
{
	// If cell block function is not initialised,
	// output error
	if(isCellBlocked(x, z))
	{
		return;
	}

	// z * ( number of cell in a column/row ) + number of x cells - gives a sequential block number for a grid of WORLD_SIZE * WORLD_SIZE
	int id = z * WORLD_SIZE + x;

	for( int i = 0; i < closed.size(); i++ )
	{
		// if the cell is already in the visited list, abort
		if( id == closed[i]->ID )
		{
			return;
		}
	}

	// If the cell is not blocked or already in the visited list then continue
	Node* newChild = new Node(x, z, parent);
	newChild->G = newCost;
	newChild->H = parent->EuclidianDistance(end);

	// for all open list cells
	for( int i = 0; i < open.size(); i++ )
	{
		if( id == open[i]->ID )
		{
			float newF = newChild->G + newCost + open[i]->H;

			if( open[i]->getF() > newF )
			{
				open[i]->G = newChild->G + newCost;
				open[i]->parent = newChild;
			}
			else // if F is not better
			{
				delete newChild;
				return;
			}
		}
	}

	open.push_back(newChild);
}

// Get the next best node in the path
Node* Pathfinding::GetNextNode()
{
	float bestF = 999999.0f; // need a default high value to get next best in path and 'start the ball rolling'
	int nodeIndex = -1;
	Node* nextNode = NULL;

	// Find next best cell
	for( int i = 0; i < open.size(); i ++ )
	{
		if( open[i]->getF() < bestF )
		{
			bestF = open[i]->getF();
			nodeIndex = i;
		}
	}

	// Update lists for best cell
	if( nodeIndex >= 0 )
	{
		nextNode = open[nodeIndex]; // grab the best cell from the above index
		closed.push_back(nextNode); // add it to the visited(closed) list
		open.erase(open.begin() + nodeIndex); // remove it from the open list
	}

	return nextNode;
}

// Continue path
void Pathfinding::ContinuePath()
{
	if( open.empty() )
	{
		return;
	}

	Node* currentNode = GetNextNode();

	if(currentNode->ID == end->ID)
	{
		end->parent = currentNode->parent;

		Node* getPath;

		// gets the shortest path to the goal
		for(getPath = end; getPath != NULL; getPath = getPath->parent)
		{
			path.push_back( new GUVector4( getPath->x, 0, getPath->z ) );
		}

		endFound = true;
		current = path.size() - 1;
		return;
	}
	else
	{
		// South Node
		PathOpened( currentNode->x, currentNode->z + 1, currentNode->G + 1, currentNode );
		// East Node
		PathOpened( currentNode->x + 1, currentNode->z, currentNode->G + 1, currentNode );
		// North Node
		PathOpened( currentNode->x, currentNode->z - 1, currentNode->G + 1, currentNode );
		// West Node
		PathOpened( currentNode->x - 1, currentNode->z, currentNode->G + 1, currentNode );

		for( int i = 0; i < open.size(); i++ )
		{
			if( currentNode->ID == open[i]->ID )
			{
				open.erase( open.begin() + i );
			}
		}

		// Continue until full path is found
		ContinuePath();
	}
}

// ---------------------------------------------------------------------
// PUBLIC
// ---------------------------------------------------------------------

// Constructor
Pathfinding::Pathfinding()
{
	startInitComplete = 0;
	endFound = 0;
	current = 0;
}

// Deconstructor
Pathfinding::~Pathfinding()
{
	// EMPTY
}

// Find Path
void Pathfinding::FindPath(GUVector4 currentPos, GUVector4 endPos)
{
	// if the start and end goal are not initialised
	if( !startInitComplete )
	{
		// loop through open list and delete (if any) all items
		for( int i = 0; i < open.size(); i++ )
		{
			delete open[i];
		}

		// Clear open list
		open.clear();

		// loop through visited list and delete (if any) all items
		for( int i = 0; i < closed.size(); i++ )
		{
			delete closed[i];
		}

		// Clear closed list
		closed.clear();

		// loop through path list and delete (if any) all items
		for( int i = 0; i < path.size(); i++ )
		{
			delete path[i];
		}

		// Clear path list
		path.clear();

		// Initialise start
		Node sNode;
		sNode.x = currentPos.x;
		sNode.z = currentPos.z;

		// Initialise goal
		Node eNode;
		eNode.x = endPos.x;
		eNode.z = endPos.z;

		SetStartAndEnd( sNode, eNode );

		// Start initialisation complete
		startInitComplete = 1;
	}
	
	//If start and end goal are initialised then continue the path
	if(startInitComplete)
	{
		ContinuePath();
	}
}

// Reset path
void Pathfinding::ResetPath()
{
	current = path.size() - 1;
}

// Next Path Position
GUVector4 Pathfinding::GetNextPosition()
{
	GUVector4 nextPos = GUVector4(19.0, 0.0, 19.0);

	if(path.size())
	{
		if(current > 0)
		{
			nextPos = GUVector4(path[current]->x, 0, path[current]->z);
			current -= 1;
		}
		else
			nextPos = GUVector4(path[0]->x, 0, path[0]->z);
	}

	return nextPos;
}

// Attach checker function
void Pathfinding::attachMapCheck(checkMap func)
{
	// This function is used to attach a map checker function,
	// allowing the algorithm to be portable and encapsulated.
	isCellBlocked = func;
}