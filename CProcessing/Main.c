#include "cprocessing.h"
#include "game.h"


// main() the starting point for the program
// CP_Engine_SetNextGameState() tells CProcessing which functions to use for init, update and exit
// CP_Engine_Run() is the core function that starts the simulation
int main(void)
{
	CP_Engine_SetNextGameState(Game_Init, Game_Update, Game_Exit);
	CP_System_SetWindowSize(1600, 900);
	CP_Engine_Run(0);
	return 0;
}