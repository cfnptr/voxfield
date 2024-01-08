//--------------------------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------------------------

#include "voxfield/client/system/mesher.hpp"
#include "voxfield/client/system/world.hpp"
#include "garden/graphics/api.hpp"

using namespace voxfield;
using namespace voxfield::client;

namespace
{
	struct MeshCluster final : public Cluster
	{
		MesherSystem* system = nullptr;

		MeshCluster(MesherSystem* system = nullptr, Chunk* c = nullptr,
			Chunk* nx = nullptr, Chunk* px = nullptr,
			Chunk* ny = nullptr, Chunk* py = nullptr,
			Chunk* nz = nullptr, Chunk* pz = nullptr) :
			Cluster(c, nx, px, ny, py, nz, pz) { this->system = system; }
	};
};

//--------------------------------------------------------------------------------------------------
static void generateSideNX(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	vertices[vertexIndex    ] = ChunkVertex(x    , y    , z + 1, 0);
	vertices[vertexIndex + 1] = ChunkVertex(x    , y + 1, z + 1, 0);
	vertices[vertexIndex + 2] = ChunkVertex(x    , y + 1, z    , 0);
	vertices[vertexIndex + 3] = ChunkVertex(x    , y    , z    , 0);
}
static void generateSidePX(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	vertices[vertexIndex    ] = ChunkVertex(x + 1, y    , z    , 1);
	vertices[vertexIndex + 1] = ChunkVertex(x + 1, y + 1, z    , 1);
	vertices[vertexIndex + 2] = ChunkVertex(x + 1, y + 1, z + 1, 1);
	vertices[vertexIndex + 3] = ChunkVertex(x + 1, y    , z + 1, 1);
}
static void generateSideNY(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	vertices[vertexIndex    ] = ChunkVertex(x    , y    , z + 1, 2);
	vertices[vertexIndex + 1] = ChunkVertex(x    , y    , z    , 2);
	vertices[vertexIndex + 2] = ChunkVertex(x + 1, y    , z    , 2);
	vertices[vertexIndex + 3] = ChunkVertex(x + 1, y    , z + 1, 2);
}
static void generateSidePY(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	vertices[vertexIndex    ] = ChunkVertex(x    , y + 1, z    , 3);
	vertices[vertexIndex + 1] = ChunkVertex(x    , y + 1, z + 1, 3);
	vertices[vertexIndex + 2] = ChunkVertex(x + 1, y + 1, z + 1, 3);
	vertices[vertexIndex + 3] = ChunkVertex(x + 1, y + 1, z    , 3);
}
static void generateSideNZ(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	vertices[vertexIndex    ] = ChunkVertex(x    , y    , z    , 4);
	vertices[vertexIndex + 1] = ChunkVertex(x    , y + 1, z    , 4);
	vertices[vertexIndex + 2] = ChunkVertex(x + 1, y + 1, z    , 4);
	vertices[vertexIndex + 3] = ChunkVertex(x + 1, y    , z    , 4);
}
static void generateSidePZ(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	vertices[vertexIndex    ] = ChunkVertex(x + 1, y    , z + 1, 5);
	vertices[vertexIndex + 1] = ChunkVertex(x + 1, y + 1, z + 1, 5);
	vertices[vertexIndex + 2] = ChunkVertex(x    , y + 1, z + 1, 5);
	vertices[vertexIndex + 3] = ChunkVertex(x    , y    , z + 1, 5);
}

//--------------------------------------------------------------------------------------------------
static uint32 generateSides000000(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept { return 0; }
static uint32 generateSides000001(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	return QUAD_VERTEX_COUNT;
}
static uint32 generateSides000010(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNZ(vertices, vertexIndex, x, y, z);
	return QUAD_VERTEX_COUNT;
}
static uint32 generateSides000011(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	return QUAD_VERTEX_COUNT * 2;
}
static uint32 generateSides000100(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNY(vertices, vertexIndex, x, y, z);
	return QUAD_VERTEX_COUNT;
}
static uint32 generateSides000101(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	return QUAD_VERTEX_COUNT * 2;
}
static uint32 generateSides000110(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNZ(vertices, vertexIndex, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	return QUAD_VERTEX_COUNT * 2;
}
static uint32 generateSides000111(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}

//--------------------------------------------------------------------------------------------------
static uint32 generateSides001000(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSidePX(vertices, vertexIndex, x, y, z);
	return QUAD_VERTEX_COUNT;
}
static uint32 generateSides001001(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	return QUAD_VERTEX_COUNT * 2;
}
static uint32 generateSides001010(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNZ(vertices, vertexIndex, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	return QUAD_VERTEX_COUNT * 2;
}
static uint32 generateSides001011(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides001100(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNY(vertices, vertexIndex, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	return QUAD_VERTEX_COUNT * 2;
}
static uint32 generateSides001101(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides001110(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNZ(vertices, vertexIndex, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides001111(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	return QUAD_VERTEX_COUNT * 4;
}

//--------------------------------------------------------------------------------------------------
static uint32 generateSides010000(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSidePZ(vertices, vertexIndex, x, y, z);
	return QUAD_VERTEX_COUNT;
}
static uint32 generateSides010001(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	return QUAD_VERTEX_COUNT * 2;
}
static uint32 generateSides010010(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNZ(vertices, vertexIndex, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	return QUAD_VERTEX_COUNT * 2;
}
static uint32 generateSides010011(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides010100(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNY(vertices, vertexIndex, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	return QUAD_VERTEX_COUNT * 2;
}
static uint32 generateSides010101(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides010110(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNZ(vertices, vertexIndex, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides010111(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	return QUAD_VERTEX_COUNT * 4;
}

//--------------------------------------------------------------------------------------------------
static uint32 generateSides011000(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSidePX(vertices, vertexIndex, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	return QUAD_VERTEX_COUNT * 2;
}
static uint32 generateSides011001(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides011010(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNZ(vertices, vertexIndex, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides011011(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	return QUAD_VERTEX_COUNT * 4;
}
static uint32 generateSides011100(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNY(vertices, vertexIndex, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides011101(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	return QUAD_VERTEX_COUNT * 4;
}
static uint32 generateSides011110(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNZ(vertices, vertexIndex, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	return QUAD_VERTEX_COUNT * 4;
}
static uint32 generateSides011111(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 4, x, y, z);
	return QUAD_VERTEX_COUNT * 5;
}

//--------------------------------------------------------------------------------------------------
static uint32 generateSides100000(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSidePY(vertices, vertexIndex, x, y, z);
	return QUAD_VERTEX_COUNT;
}
static uint32 generateSides100001(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	return QUAD_VERTEX_COUNT * 2;
}
static uint32 generateSides100010(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNZ(vertices, vertexIndex, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	return QUAD_VERTEX_COUNT * 2;
}
static uint32 generateSides100011(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides100100(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNY(vertices, vertexIndex, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	return QUAD_VERTEX_COUNT * 2;
}
static uint32 generateSides100101(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides100110(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNZ(vertices, vertexIndex, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides100111(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	return QUAD_VERTEX_COUNT * 4;
}

//--------------------------------------------------------------------------------------------------
static uint32 generateSides101000(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSidePX(vertices, vertexIndex, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	return QUAD_VERTEX_COUNT * 2;
}
static uint32 generateSides101001(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides101010(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNZ(vertices, vertexIndex, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides101011(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	return QUAD_VERTEX_COUNT * 4;
}
static uint32 generateSides101100(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNY(vertices, vertexIndex, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides101101(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	return QUAD_VERTEX_COUNT * 4;
}
static uint32 generateSides101110(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNZ(vertices, vertexIndex, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	return QUAD_VERTEX_COUNT * 4;
}
static uint32 generateSides101111(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 4, x, y, z);
	return QUAD_VERTEX_COUNT * 5;
}

//--------------------------------------------------------------------------------------------------
static uint32 generateSides110000(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSidePZ(vertices, vertexIndex, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	return QUAD_VERTEX_COUNT * 2;
}
static uint32 generateSides110001(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides110010(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNZ(vertices, vertexIndex, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides110011(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	return QUAD_VERTEX_COUNT * 4;
}
static uint32 generateSides110100(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNY(vertices, vertexIndex, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides110101(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	return QUAD_VERTEX_COUNT * 4;
}
static uint32 generateSides110110(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNZ(vertices, vertexIndex, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	return QUAD_VERTEX_COUNT * 4;
}
static uint32 generateSides110111(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 4, x, y, z);
	return QUAD_VERTEX_COUNT * 5;
}

//--------------------------------------------------------------------------------------------------
static uint32 generateSides111000(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSidePX(vertices, vertexIndex, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	return QUAD_VERTEX_COUNT * 3;
}
static uint32 generateSides111001(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	return QUAD_VERTEX_COUNT * 4;
}
static uint32 generateSides111010(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNZ(vertices, vertexIndex, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	return QUAD_VERTEX_COUNT * 4;
}
static uint32 generateSides111011(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 4, x, y, z);
	return QUAD_VERTEX_COUNT * 5;
}
static uint32 generateSides111100(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNY(vertices, vertexIndex, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	return QUAD_VERTEX_COUNT * 4;
}
static uint32 generateSides111101(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 4, x, y, z);
	return QUAD_VERTEX_COUNT * 5;
}
static uint32 generateSides111110(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNZ(vertices, vertexIndex, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 4, x, y, z);
	return QUAD_VERTEX_COUNT * 5;
}
static uint32 generateSides111111(ChunkVertex* vertices,
	uint32 vertexIndex, uint8 x, uint8 y, uint8 z) noexcept
{
	generateSideNX(vertices, vertexIndex, x, y, z);
	generateSideNZ(vertices, vertexIndex + QUAD_VERTEX_COUNT, x, y, z);
	generateSideNY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 2, x, y, z);
	generateSidePX(vertices, vertexIndex + QUAD_VERTEX_COUNT * 3, x, y, z);
	generateSidePZ(vertices, vertexIndex + QUAD_VERTEX_COUNT * 4, x, y, z);
	generateSidePY(vertices, vertexIndex + QUAD_VERTEX_COUNT * 5, x, y, z);
	return QUAD_VERTEX_COUNT * 6;
}

//--------------------------------------------------------------------------------------------------
static uint32(*generateSides[64])(ChunkVertex*, uint32, uint8, uint8, uint8) = // 2 ^ 6
{
	generateSides000000, generateSides000001, generateSides000010, generateSides000011,
	generateSides000100, generateSides000101, generateSides000110, generateSides000111,
	generateSides001000, generateSides001001, generateSides001010, generateSides001011,
	generateSides001100, generateSides001101, generateSides001110, generateSides001111,
	generateSides010000, generateSides010001, generateSides010010, generateSides010011,
	generateSides010100, generateSides010101, generateSides010110, generateSides010111,
	generateSides011000, generateSides011001, generateSides011010, generateSides011011,
	generateSides011100, generateSides011101, generateSides011110, generateSides011111,
	generateSides100000, generateSides100001, generateSides100010, generateSides100011,
	generateSides100100, generateSides100101, generateSides100110, generateSides100111,
	generateSides101000, generateSides101001, generateSides101010, generateSides101011,
	generateSides101100, generateSides101101, generateSides101110, generateSides101111,
	generateSides110000, generateSides110001, generateSides110010, generateSides110011,
	generateSides110100, generateSides110101, generateSides110110, generateSides110111,
	generateSides111000, generateSides111001, generateSides111010, generateSides111011,
	generateSides111100, generateSides111101, generateSides111110, generateSides111111
};

//--------------------------------------------------------------------------------------------------
static ID<Buffer> createIndexBuffer(GraphicsSystem* graphicsSystem, uint32 indexCount)
{
	GARDEN_ASSERT(indexCount % 6 == 0);

	const auto bind = Buffer::Bind::TransferDst | Buffer::Bind::Index;
	const auto access = Buffer::Access::None;
	const auto usage = Buffer::Usage::PreferGPU;
	const auto strategy = Buffer::Strategy::Size;

	ID<Buffer> buffer;
	if (indexCount > UINT16_MAX)
	{
		vector<uint32> indices(indexCount);
		for (uint32 i = 0, j = 0; i < indexCount; i += 6, j += 4)
		{
			indices[i] = j; indices[i + 1] = j + 1; indices[i + 2] = j + 2;
			indices[i + 3] = j + 2; indices[i + 4] = j + 3; indices[i + 5] = j;
		}
		buffer = graphicsSystem->createBuffer(bind, access, indices, 0, 0, usage, strategy);
	}
	else
	{
		vector<uint16> indices(indexCount);
		for (uint32 i = 0, j = 0; i < indexCount; i += 6, j += 4)
		{
			indices[i] = j; indices[i + 1] = j + 1; indices[i + 2] = j + 2;
			indices[i + 3] = j; indices[i + 4] = j + 2; indices[i + 5] = j + 3;
		}
		buffer = graphicsSystem->createBuffer(bind, access, indices, 0, 0, usage, strategy);
	}

	SET_RESOURCE_DEBUG_NAME(graphicsSystem, buffer, "buffer.index.chunk");
	return buffer;
}

void MesherSystem::initialize()
{
	auto manager = getManager();
	graphicsSystem = manager->get<GraphicsSystem>();

	threadSystem = manager->get<ThreadSystem>();
	auto threadCount = threadSystem->getBackgroundPool().getThreadCount();
	buffers.resize(threadCount);

	for (uint32 i = 0; i < threadCount; i++)
		buffers[i].resize(CHUNK_SIZE * VOXEL_VERTEX_COUNT);

	auto worldSystem = manager->get<WorldSystem>();
	registry = &worldSystem->getRegistry();

	indexBufferSize = VOXEL_INDEX_COUNT * CHUNK_LENGTH * CHUNK_LENGTH;
	indexBuffer = createIndexBuffer(graphicsSystem, indexBufferSize);
}

//--------------------------------------------------------------------------------------------------
void MesherSystem::generate(const ThreadPool::Task& task)
{
	auto cluster = (MeshCluster*)task.getArgument();
	auto system = cluster->system;
	auto registry = system->registry;
	const auto voxels = cluster->c->getVoxels();
	auto buffer = system->buffers[task.getThreadIndex()].data();
	uint32 index = 0, vertexCount = 0;

	for (uint8 z = 0; z < CHUNK_LENGTH; z++)
	{
		for (uint8 y = 0; y < CHUNK_LENGTH; y++)
		{
			for (uint8 x = 0; x < CHUNK_LENGTH; x++)
			{
				auto voxel = voxels[index++];
				auto mask = cluster->getNearMask(x, y, z, *registry);
				vertexCount += generateSides[mask](buffer, vertexCount, x, y, z);
			}
		}
	}

	system->meshMutex.lock();
	if (vertexCount > 0)
	{
		auto bufferByteSize = vertexCount * sizeof(ChunkVertex);

		ChunkMesh mesh =
		{
			BufferExt::create(Buffer::Bind::TransferDst | Buffer::Bind::Vertex,
				Buffer::Access::None, Buffer::Usage::PreferGPU,
				Buffer::Strategy::Size, bufferByteSize, 0),
			BufferExt::create(Buffer::Bind::TransferSrc, Buffer::Access::SequentialWrite,
				Buffer::Usage::Auto, Buffer::Strategy::Speed, bufferByteSize, 0),
			cluster->c->getStructureID(),
			cluster->c->position
		};

		auto stagingMap = mesh.stagingBuffer.getMap();
		memcpy(stagingMap, buffer, bufferByteSize);
		mesh.stagingBuffer.flush();
		system->meshes.push_back(std::move(mesh));
	}
	else
	{
		ChunkMesh mesh =
		{
			BufferExt::create((Buffer::Bind)0, (Buffer::Access)0,
				(Buffer::Usage)0, (Buffer::Strategy)0, 0),
			BufferExt::create((Buffer::Bind)0, (Buffer::Access)0,
				(Buffer::Usage)0, (Buffer::Strategy)0, 0),
			cluster->c->getStructureID(),
			cluster->c->position
		};

		system->meshes.push_back(std::move(mesh));
	}
	system->meshMutex.unlock();

	delete cluster->c;
	delete cluster;
}

//--------------------------------------------------------------------------------------------------
void MesherSystem::generateMesh(const Cluster& cluster)
{
	GARDEN_ASSERT(cluster.isMeshingReady());

	auto chunks = new Chunk[CHUNK_CLUSTER_SIZE];
	chunks[0] = *cluster.c;
	chunks[1] = *cluster.nx;
	chunks[2] = *cluster.px;
	chunks[3] = *cluster.ny;
	chunks[4] = *cluster.py;
	chunks[5] = *cluster.nz;
	chunks[6] = *cluster.pz;

	auto meshCluster = new MeshCluster(this, &chunks[0],
		&chunks[1], &chunks[2], &chunks[3], &chunks[4], &chunks[5], &chunks[6]);

	auto& threadPool = threadSystem->getBackgroundPool();
	threadPool.addTask(ThreadPool::Task(generate, meshCluster));
}
void MesherSystem::flush(std::function<void(ChunkMesh&, uint32)> onMesh)
{
	GARDEN_ASSERT(onMesh);
	meshMutex.lock();

	uint32 biggestIndexCount = 0;
	for (auto& mesh : meshes)
	{
		auto binarySize = mesh.stagingBuffer.getBinarySize();
		auto indexCount = (uint32)((binarySize / (sizeof(ChunkVertex) * 4)) * 6);
		biggestIndexCount = std::max(biggestIndexCount, indexCount);
		onMesh(mesh, indexCount);

		GraphicsAPI::isRunning = false;
		BufferExt::destroy(mesh.stagingBuffer);
		BufferExt::destroy(mesh.vertexBuffer);
		GraphicsAPI::isRunning = true;
	}

	meshes.clear();
	meshMutex.unlock();

	if (biggestIndexCount > indexBufferSize)
	{
		graphicsSystem->destroy(indexBuffer);
		indexBuffer = createIndexBuffer(graphicsSystem, biggestIndexCount);
		indexBufferSize = biggestIndexCount;
	}
}

//--------------------------------------------------------------------------------------------------
ID<Buffer> MesherSystem::getVertexBuffer(ChunkMesh& chunkMesh)
{
	// TODO: measure if default memory strategy is better
	GARDEN_ASSERT(chunkMesh.vertexBuffer.getBinarySize() > 0);
	auto vertexBuffer = GraphicsAPI::bufferPool.create(
		Buffer::Bind::TransferDst | Buffer::Bind::Vertex, Buffer::Access::None,
		Buffer::Usage::PreferGPU, Buffer::Strategy::Size, 0);
	auto stagingBuffer = GraphicsAPI::bufferPool.create(Buffer::Bind::TransferSrc,
		Buffer::Access::SequentialWrite, Buffer::Usage::Auto, Buffer::Strategy::Speed, 0);
	auto vertexView = GraphicsAPI::bufferPool.get(vertexBuffer);
	auto stagingView = GraphicsAPI::bufferPool.get(stagingBuffer);
	BufferExt::moveInternalObjects(chunkMesh.vertexBuffer, **vertexView);
	BufferExt::moveInternalObjects(chunkMesh.stagingBuffer, **stagingView);
	SET_RESOURCE_DEBUG_NAME(graphicsSystem, vertexBuffer,
		"buffer.vertex.chunk." + chunkMesh.position.toString());
	SET_RESOURCE_DEBUG_NAME(graphicsSystem, stagingBuffer,
		"buffer.staging.chunk." + chunkMesh.position.toString());
	Buffer::copy(stagingBuffer, vertexBuffer);
	GraphicsAPI::bufferPool.destroy(stagingBuffer);
	return vertexBuffer;
}