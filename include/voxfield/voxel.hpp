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
#include "voxy/voxel.hpp"
#include "math/types.hpp"

namespace voxfield
{

// One polygonal quad vertex count
#define QUAD_VERTEX_COUNT 4
// One polygonal quad vertex count
#define QUAD_INDEX_COUNT 6

// Cube side count
#define VOXEL_SIDE_COUNT 6
// VOXEL_SIDE_COUNT * QUAD_VERTEX_COUNT
#define VOXEL_VERTEX_COUNT 24
// VOXEL_SIDE_COUNT * QUAD_INDEX_COUNT
#define VOXEL_INDEX_COUNT 36

using namespace math;

// More than enough ID count (65536)
typedef uint16 Voxel;

enum class VoxelDrawMode : uint8
{
	Opaque, Translucent, Transparent, Count
};

struct VoxelData
{
	Voxel voxelID = NULL_VOXEL;
	VoxelDrawMode drawMode = {};
	bool isHollow : 1;
	uint8 _unused : 7;
	uint32 textureID = NULL_VOXEL;

	VoxelData(Voxel _voxelID = NULL_VOXEL,
		VoxelDrawMode _drawMode = {}, uint32 _textureID = 0) :
		voxelID(_voxelID), drawMode(_drawMode),
		isHollow(true), _unused(0), textureID(_textureID) { }

	bool shouldDraw(const VoxelData& nearVoxel) const noexcept
	{
		if (nearVoxel.drawMode == VoxelDrawMode::Opaque) return false;
		if (voxelID == nearVoxel.voxelID && isHollow) return false;
		// TODO: process half blocks
		return true;
	}
};

} // namespace voxfield