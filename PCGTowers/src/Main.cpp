#include <Application/PCGTowersApp.h>

#include <Game/TowerDefense/Enemy.h>

/// <summary>
/// Things that I would improve:
/// 
/// Definitely make things more event driven, I don't think I have an Event class like I did in my last version of the engine.
/// But that would improve a lot already.
/// 
/// The world is currently responsible of a lot of things updating every single thing that needs to be updated.
/// I didn't use my ECS or GameObject stuff because it isn't tested and didn't wanna fall to far behind.
/// 
/// I have a new idea about how I want to handle graphics in the engine that I want to try out so that will make the explicit calls to SFML hopefully unnecessary.
/// 
/// 
/// Otherwise I am generally happy with the outcome.
/// Anyway Happy Holidays ! And I hope you feel better soon from the breakdown.
/// </summary>
int main()
{
	PCGTowersApp app;
	if (!app.Init())
		return 0;

	app.Run();

	return 0;
}