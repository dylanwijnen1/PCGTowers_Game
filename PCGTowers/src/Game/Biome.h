#pragma once

#include <cstdint>

enum struct BiomeType : uint32_t
{
	kUnknown = 0,
	kTundra = 0x93A7ACFF,
	kTaiga = 0x5B8F52FF,
	kWoodland = 0xB37C06FF,
	kGrassland = 0x927E30FF,
	kSeasonalForest = 0x2C89A0FF,
	kRainForestTemperate = 0x0A546DFF,
	kRainForestTropical = 0x075330FF,
	kSavannah = 0x97A527FF,
	kDesert = 0xC87137FF,

	kCount = 9
};