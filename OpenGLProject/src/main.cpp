#include "Tutorial1.h"
#include "Tutorial2.h"
#include "Tutorial3.h"
#include "Tutorial4.h"
#include "Tutorial5.h"
#include "Tutorial6.h"
#include "Tutorial7.h"
#include "Tutorial8.h"
#include "Tutorial9.h"
#include "Tutorial10.h"
#include "Tutorial11.h"
#include "Tutorial12.h"

int main() {

	srand(time(NULL));

	auto tute = new Tutorial12();

	tute->Run();

	delete tute;
	return 0;

}