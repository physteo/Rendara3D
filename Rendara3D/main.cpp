
#include "./Demos/OutBreak/OutBreak.h"
#include "./Demos/Shadows/ShadowsDemoGame.h"
#include "./Demos/Instancing/demo_instancing.h"

int main() 
{	
	std::cout << "########## Rendara3D ##########" << std::endl;
	std::cout << "Welcome to the demo of Rendara3D." << std::endl;

	std::cout << "Please choose which demo to run (or exit):" << std::endl;
	std::cout << "\t 1: Shadows" << std::endl;
	std::cout << "\t 2: Instancing" << std::endl;
	std::cout << "\t 3: outBreak game (reversed Breakout)" << std::endl;
	std::cout << "\t 0: exit." << std::endl;

	int choice = 0;
	do
	{
		std::cin >> choice;
	} while (choice != 1 && choice != 2 && choice != 3 && choice != 0);

	if (choice == 1)
	{
		// demo: shadows
		ShadowsDemoGame game{1000,1000};
		game.execute();
	}
	else if (choice == 2)
	{
		// demo: instancing
		demo_instancing();
	}
	else if (choice == 3)
	{
		// demo: outBreak game
		OutBreak outBreak{ 1000, 1000 };
		outBreak.execute();
	}
	
	return 0;
}

