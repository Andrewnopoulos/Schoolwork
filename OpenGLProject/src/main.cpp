#include "Tutorial1.h"
#include "Tutorial2.h" // shaders
#include "Tutorial3.h"
#include "Tutorial4.h" // texture
#include "Tutorial5.h" // normal map
#include "Tutorial6.h" // animation
#include "Tutorial7.h" // animated model (with lighting)
#include "Tutorial8.h" // cpu particles
#include "Tutorial9.h" // gpu particles
#include "Tutorial10.h" // post processing effects
#include "Tutorial11.h" // shadows (with animated model)
#include "Tutorial12.h" // procedural terrain generation
#include "Tutorial13.h" // realistic lighting

#include "Assignment1.h"

int main() {

	srand(time(NULL));

	auto tute = new Assignment1();

	tute->Run();

	delete tute;
	return 0;
}