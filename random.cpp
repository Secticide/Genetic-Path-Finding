#include <iostream>
#include <time.h>

using namespace std;

// Seed random
void seedRandom()
{
	srand(time(NULL));
	rand();
}

// Random implementation
float random()
{
	float temp = (rand() % 10);

	return (temp / 10);
}