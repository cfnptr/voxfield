//----------------------------------------------------------------------------------------
// Voxfield - An open source voxel based multiplayer sandbox game.
// Copyright (C) 2022-2024  Nikita Fediuchin
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
#include <vector>

namespace voxfield
{

using namespace std;

class Registry
{
protected:
	vector<VoxelData> voxelData;
	bool isFinalized = false;
public:
	Registry()
	{
		VoxelData data;
		data.voxelID = NULL_VOXEL;
		data.drawMode = VoxelDrawMode::Transparent;
		data.textureID = NULL_VOXEL;
		voxelData.push_back(data);

		data.voxelID = UNKNOWN_VOXEL;
		data.drawMode = VoxelDrawMode::Opaque;
		data.textureID = UNKNOWN_VOXEL;
		voxelData.push_back(data);
		
		data.voxelID = DEBUG_VOXEL;
		data.drawMode = VoxelDrawMode::Opaque;
		data.textureID = DEBUG_VOXEL;
		voxelData.push_back(data);
	}

	bool isFull() const noexcept { return voxelData.size() >= UINT16_MAX; }
	
	Voxel registerVoxel(const VoxelData& data) noexcept
	{
		GARDEN_ASSERT(!isFull());
		GARDEN_ASSERT(!isFinalized);
		auto voxel = (Voxel)voxelData.size();
		voxelData.push_back(data);
		return voxel;
	}
	void finalize() noexcept { isFinalized = true; }

	const VoxelData& getVoxelData(Voxel voxel) const noexcept
	{
		GARDEN_ASSERT(isFinalized);
		if (voxel > voxelData.size()) return voxelData[UNKNOWN_VOXEL];
		return voxelData[voxel];
	}
};

} // namespace voxfield