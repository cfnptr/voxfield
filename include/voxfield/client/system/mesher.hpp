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
#include "voxfield/cluster.hpp"
#include "garden/system/graphics.hpp"

namespace voxfield::client
{

#define CHUNK_VERT_POS_BITS 14u
#define CHUNK_VERT_NORM_BITS 3u
#define CHUNK_VERT_UV_BITS 9u

#define CHUNK_VERT_POS_MASK 16383u // 2 ^ 14 - 1
#define CHUNK_VERT_NORM_MASK 7u // 2 ^ 3 - 1
#define CHUNK_VERT_UV_MASK 511u // 2 ^ 9 - 1

using namespace ecsm;
using namespace garden;
class MesherSystem;

// Chunk Vertex Memory Layout
//
// data.x (32bit) :
//   position.x (14bit)
//   position.y (14bit)
//   normal.xyz (3bit)
//   _unused (1bit)
//
// data.y (32bit) :
//   position.z (14bit)
//   texCoords.x (9bit)
//   texCoords.y (9bit)

struct ChunkVertex
{
	uint32 x, y;

	ChunkVertex() = default;
	ChunkVertex(uint16 x, uint16 y, uint16 z, uint8 normal)
	{
		this->x = (x * 511u) | ((y * 511u) << CHUNK_VERT_POS_BITS) | (normal << (CHUNK_VERT_POS_BITS * 2u)); // TODO: 511 is temporal
		this->y = (z * 511u); // TODO: texCoords
	}
};

//----------------------------------------------------------------------------------------
class MesherSystem final : public System
{
public:
	struct ChunkMesh final
	{
		Buffer vertexBuffer;
		Buffer stagingBuffer;
		uint32 structureID = 0;
		int3 position = int3(0);
	};
private:
	ThreadSystem* threadSystem = nullptr;
	GraphicsSystem* graphicsSystem = nullptr;
	const Registry* registry = nullptr;
	vector<vector<ChunkVertex>> buffers;
	vector<ChunkMesh> meshes;
	ID<Buffer> indexBuffer = {};
	uint32 indexBufferSize = 0;
	mutex meshMutex;

	void initialize() final;
	static void generate(const ThreadPool::Task& task);
	friend class ecsm::Manager;
public:
	void generateMesh(const Cluster& cluster);
	void flush(std::function<void(ChunkMesh&, uint32)> onMesh);
	ID<Buffer> getVertexBuffer(ChunkMesh& chunkMesh);
	ID<Buffer> getIndexBuffer() const noexcept { return indexBuffer; }
};

} // namespace voxfield::client