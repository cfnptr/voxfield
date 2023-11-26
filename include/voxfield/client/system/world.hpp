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

#pragma once
#include "garden/system/graphics.hpp"
#include "voxfield/structure.hpp"
#include "voxfield/system/generator.hpp"
#include "voxfield/client/system/mesher.hpp"

namespace voxfield::client
{

class WorldSystem final : public System
{
	GraphicsSystem* graphicsSystem = nullptr;
	GeneratorSystem* generatorSystem = nullptr;
	MesherSystem* mesherSystem = nullptr;
	Structure structure = {};
	vector<Chunk*> freeChunks;

	void initialize() final;
	void update() final;

	friend class ecsm::Manager;
public:
	uint8 chunkViewRadius = 8;
	int32 minBorder = STRUCTURE_POS_MIN;
	int32 maxBorder = STRUCTURE_POS_MAX;
};

} // namespace voxfield::client