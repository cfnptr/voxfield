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
#include <map>

namespace voxfield
{

// Optimal for hash: 64bit / 3
#define STRUCTURE_POS_BITS 21
// 2 ^ STRUCTURE_POS_BITS
#define STRUCTURE_LENGTH 2097152
// STRUCTURE_LENGTH / 2
#define STRUCTURE_HALF_LENGTH 1048576
// STRUCTURE_HALF_LENGTH
#define STRUCTURE_POS_MIN 1048576
// STRUCTURE_HALF_LENGTH - 1
#define STRUCTURE_POS_MAX 1048575

#define posToChunkHash(x, y, z) \
	(((int64)(z) << 42LL) | ((int64)(y) << 21LL) | (int64)(x))

class Structure final
{
	map<int64, Chunk*> chunks;
public:
	bool tryGetChunk(int64 hash)
	{
		
	}
};

} // namespace voxfield