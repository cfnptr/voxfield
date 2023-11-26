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
#include "voxy/chunk.hpp"
#include "math/vector.hpp"

namespace voxfield
{

using namespace math;
using namespace voxy;

// Optimal for performance
#define CHUNK_LENGTH 32
// CHUNK_LENGTH ^ 3
#define CHUNK_SIZE 32768
// CHUNK_LENGTH / 2
#define CHUNK_HALF_LENGTH 16

// More than enough ID count (65536)
typedef uint16 Voxel;

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
	Allocated, Generated, Meshed, Count
};


class Chunk final : public Chunk3<CHUNK_LENGTH, CHUNK_LENGTH, CHUNK_LENGTH, Voxel>
{
private:
	int3 position = int3(0);
	uint32 structureID = 0;
	uint16 _alignment0 = 0;
	uint8 _alignment1 = 0;
public:
	ChunkState state = ChunkState::Allocated;
	
	Chunk() = default;
	Chunk(Voxel voxel, const int3& position,
		uint32 structureID) : Chunk3(voxel) { }
	
	const int3& getPosition() const noexcept{ return position; }
	uint32 getStructureID() const noexcept { return structureID; }
};

} // namespace voxfield