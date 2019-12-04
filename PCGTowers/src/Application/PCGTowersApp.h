#pragma once

#include <Game/PCGTowersLayer.h>
#include <Dragon/Application/Application.h>

class PCGTowersApp final : public dragon::Application
{
	virtual bool OnInit() final override
	{
		PushLayer(new PCGTowersLayer());
		return true;
	}
};