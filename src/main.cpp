#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") // Prevents console from opening
#include "Game.h"

int main()
{
    Game tetris;
    tetris.run();
    return 0;
}
