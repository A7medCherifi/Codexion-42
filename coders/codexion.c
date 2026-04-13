#include "codexion.h"


int main(int argc, char **argv) {
	arguments_t		*args;

	if (parsing(&args, argc, argv))
		return (1);
	
	
	return (0);
}

