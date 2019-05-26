#define APP "BYATHREAD"
#define APP_NAME "by a thread"
//#define NVSYNC
//#define SECONDARY
//#define DEBUG
#define SIDES (size) { 1080, 1920 }
#define SCALE .55
#define BGCOLOR { 0, 0, 0, 255 }
#define BG { 110, 10, 20, 255 }

#include "main.h"

////////////////////////////////////////////////////////////////////////////////
// scenes

void scene_alloc()
{
    switch (next_scene)
    {    	
    	case 1: current = loading_alloc(); break;
    	case 2: current = menu_alloc(); break;
    	case 3: current = gameplay_alloc(); break;
	    case 4: current = stats_alloc(); break;
    }
}