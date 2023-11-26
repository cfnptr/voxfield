//----------------------------------------------------------------------------------------
// Voxfield - An open source voxel based multiplayer sandbox game.
// Copyright (C) 2022-2023  Nikita Fediuchin
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------------------

#include "voxfield/client/system/world.hpp"

using namespace voxfield;
using namespace voxfield::client;

void WorldSystem::initialize()
{
	auto manager = getManager();
	graphicsSystem = manager->get<GraphicsSystem>();
	generatorSystem = manager->get<GeneratorSystem>(); 
	mesherSystem = manager->get<MesherSystem>();
}

void WorldSystem::update()
{
	if (!graphicsSystem->camera) return;

	auto manager = getManager();
	auto cameraTransform = manager->get<TransformComponent>(graphicsSystem->camera);
	auto clampedPosition = clamp(cameraTransform->position,
		float3(minBorder), float3(maxBorder));
	cameraTransform->position = clampedPosition;
	auto cameraPosition = worldToChunkPos(clampedPosition);

	for (int16 z = chunkViewRadius - 1; z < chunkViewRadius; z++)
	{
		for (int16 y = chunkViewRadius - 1; y < chunkViewRadius; y++)
		{
			for (int16 x = chunkViewRadius - 1; x < chunkViewRadius; x++)
			{
				auto chunkPosition = cameraPosition + int3(x, y, z);
				
			}
		}
	}
}