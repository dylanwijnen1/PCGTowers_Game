#include <Application/PCGTowersApp.h>

/// <summary>
/// 
/// Tower Defense PCG
/// 
/// Map Generation
/// - Maps have a climate/biome which determines how it is generated. Map climates also impact towers. (Increased/Decreased stats)
///		- Tropical Forest
///			- Bosses: 
///			- Minibosses:
///			- Monsters:
///		- Dry: Desert, Grasslands, Savannah
///			- Bosses:
///			- Minibosses:
///			- Monsters:
///		- Temperate Plains
///			- Bosses:
///			- Minibosses:
///			- Monsters:
///		- Cold: Tundra
///			- Bosses:
///			- Minibosses:
///			- Monsters:
///		- Volcanic Rocks
///			- Bosses:
///			- Minibosses:
///			- Monsters:
/// 
/// Story Line
/// - Every 5 Rounds in a map you move on to another map and progress in the "story line".
/// - 
/// 
/// Tower Generation
/// - Grammar System much like the LSystem
/// 
/// </summary>

int main()
{
	PCGTowersApp app;
	if (!app.Init())
		return 0;

	app.Run();

	return 0;
}