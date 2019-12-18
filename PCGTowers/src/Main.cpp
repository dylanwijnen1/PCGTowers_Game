#include <Application/PCGTowersApp.h>

#include <Game/TowerDefense/Enemy.h>

#include <vld.h>

int main()
{
	PCGTowersApp app;
	if (!app.Init())
		return 0;

	app.Run();

	return 0;
}