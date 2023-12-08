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
#include "voxfield/voxel.hpp"
#include "garden/defines.hpp"
#include "voxy/chunk.hpp"
#include "math/vector.hpp"
#include "ecsm.hpp"

namespace voxfield
{

using namespace voxy;
using namespace math;
using namespace ecsm;

// Optimal for performance
#define CHUNK_LENGTH 32
// CHUNK_LENGTH ^ 3
#define CHUNK_SIZE 32768
// CHUNK_LENGTH / 2
#define CHUNK_HALF_LENGTH 16

static int3 worldToChunkPos(const float3& position) noexcept
{
	return (int3)floor(position / CHUNK_LENGTH);
}
static float3 chunkToWorldPos(const int3& position) noexcept
{
	return position * CHUNK_LENGTH;
}

enum class ChunkState : uint8
{
	Allocated, Generating, Generated, Meshing, Meshed, Count
};

//----------------------------------------------------------------------------------------
struct Chunk : public Chunk3<CHUNK_LENGTH, CHUNK_LENGTH, CHUNK_LENGTH, Voxel>
{
protected:
	int3 position = int3(0);
	uint32 structureID = 0;
	ID<Entity> entity = {};
public:
	ChunkState state = ChunkState::Allocated;
	bool isEmpty = false;
	
	Chunk() = default;
	Chunk(Voxel voxel, const int3& position,
		uint32 structureID, ID<Entity> entity) : Chunk3(voxel)
	{
		this->position = position;
		this->structureID = structureID;
		this->entity = entity;
	}
	
	const int3& getPosition() const noexcept{ return position; }
	uint32 getStructureID() const noexcept { return structureID; }
	ID<Entity> getEntity() const noexcept { return entity; }
private:
	uint16 _alignment = 0;
};

} // namespace voxfield