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
#include "voxfield/chunk.hpp"
#include "voxfield/registry.hpp"
#include "voxy/cluster.hpp"

namespace voxfield
{

struct Cluster : public Cluster3<Chunk, Voxel>
{
	Cluster(Chunk* c = nullptr,
		Chunk* nx = nullptr, Chunk* px = nullptr,
		Chunk* ny = nullptr, Chunk* py = nullptr,
		Chunk* nz = nullptr, Chunk* pz = nullptr) :
		Cluster3(c, nx, px, ny, py, nz, pz) { }

	bool isMeshingReady() const noexcept
	{
		return
			(uint8)nx->state > (uint8)ChunkState::Generating &&
			(uint8)px->state > (uint8)ChunkState::Generating &&
			(uint8)ny->state > (uint8)ChunkState::Generating &&
			(uint8)py->state > (uint8)ChunkState::Generating &&
			(uint8)nz->state > (uint8)ChunkState::Generating &&
			(uint8)pz->state > (uint8)ChunkState::Generating;
	}

	uint8 getNearMask(int16 x, int16 y, int16 z, const Registry& registry) const noexcept
	{
		GARDEN_ASSERT(x >= 0 && x < CHUNK_LENGTH);
		GARDEN_ASSERT(y >= 0 && y < CHUNK_LENGTH);
		GARDEN_ASSERT(z >= 0 && z < CHUNK_LENGTH);

		auto voxel = c->get(x, y, z);
		auto voxelData = &registry.getVoxelData(voxel);
		if (voxelData->drawMode == VoxelDrawMode::Transparent) return 0;

		Voxel nearVoxel;
		const VoxelData* nearVoxelData;
		uint8 mask = 0;

		if (x == 0)
		{
			nearVoxel = nx->get(CHUNK_LENGTH - 1, y, z);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData);
			nearVoxel = c->get(1, y, z);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData) << 3u;
		}
		else if (x == CHUNK_LENGTH - 1)
		{
			nearVoxel = c->get(CHUNK_LENGTH - 2, y, z);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData);
			nearVoxel = px->get(0, y, z);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData) << 3u;
		}
		else
		{
			nearVoxel = c->get(x - 1, y, z);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData);
			nearVoxel = c->get(x + 1, y, z);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData) << 3u;
		}

		if (y == 0)
		{
			nearVoxel = ny->get(x, CHUNK_LENGTH - 1, z);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData) << 2u;
			nearVoxel = c->get(x, 1, z);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData) << 5u;
		}
		else if (y == CHUNK_LENGTH - 1)
		{
			nearVoxel = c->get(x, CHUNK_LENGTH - 2, z);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData) << 2u;
			nearVoxel = py->get(x, 0, z);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData) << 5u;
		}
		else
		{
			nearVoxel = c->get(x, y - 1, z);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData) << 2u;
			nearVoxel = c->get(x, y + 1, z);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData) << 5u;
		}

		if (z == 0)
		{
			nearVoxel = nz->get(x, y, CHUNK_LENGTH - 1);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData) << 1u;
			nearVoxel = c->get(x, y, 1);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData) << 4u;
		}
		else if (z == CHUNK_LENGTH - 1)
		{
			nearVoxel = c->get(x, y, CHUNK_LENGTH - 2);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData) << 1u;
			nearVoxel = pz->get(x, y, 0);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData) << 4u;
		}
		else
		{
			nearVoxel = c->get(x, y, z - 1);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData) << 1u;
			nearVoxel = c->get(x, y, z + 1);
			nearVoxelData = &registry.getVoxelData(nearVoxel);
			mask |= (uint8)voxelData->shouldDraw(*nearVoxelData) << 4u;
		}

		return mask;
	}
};

} // namespace voxfield