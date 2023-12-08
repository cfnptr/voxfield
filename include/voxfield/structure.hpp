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
#include "voxfield/client/system/graphics/geometry/opaque.hpp"

#include <map>
#include <stack>

namespace voxfield
{

// Optimal for hash: 64bit / 3
#define STRUCTURE_POS_BITS 21
// 2 ^ STRUCTURE_POS_BITS
#define STRUCTURE_LENGTH 2097152
// STRUCTURE_LENGTH / 2
#define STRUCTURE_HALF_LENGTH 1048576
// STRUCTURE_HALF_LENGTH
#define STRUCTURE_POS_MIN -1048576
// STRUCTURE_HALF_LENGTH - 1
#define STRUCTURE_POS_MAX 1048575

using namespace voxfield::client;

static constexpr int64 posToChunkHash(int32 x, int32 y, int32 z) noexcept
{
	return (((int64)(z) << 42LL) | ((int64)(y) << 21LL) | (int64)(x));
}
static constexpr int64 posToChunkHash(const int3& position) noexcept
{
	return posToChunkHash(position.x, position.y, position.z);
}

static constexpr void hashToChunkPos(int64 hash, int32& x, int32& y, int32& z) noexcept
{
	x = hash & (int64)(STRUCTURE_LENGTH - 1);
	y = hash & ((int64)(STRUCTURE_LENGTH - 1) << 21LL);
	z = hash & ((int64)(STRUCTURE_LENGTH - 1) << 42LL);
}
static constexpr void hashToChunkPos(int64 hash, int3& position) noexcept
{
	hashToChunkPos(hash, position.x, position.y, position.z);
}

//----------------------------------------------------------------------------------------
class Structure
{
protected:
	Manager* manager = nullptr;
	stack<Chunk*>* freeChunks = nullptr;
	map<int64, Chunk*> chunks;
	uint32 id = 0;
public:
	Structure() = default;
	Structure(Manager* manager, uint32 id, stack<Chunk*>* freeChunks)
	{
		GARDEN_ASSERT(manager);
		this->manager = manager;
		this->freeChunks = freeChunks;
		this->id = id;
	}

	map<int64, Chunk*>& getChunks() noexcept { return chunks; }
	const map<int64, Chunk*>& getChunks() const noexcept { return chunks; }

	Chunk* getChunk(int64 hash)
	{
		return chunks.at(hash);
	}
	Chunk* getChunk(const int3& position)
	{
		return chunks.at(posToChunkHash(position));
	}

	bool tryGetChunk(int64 hash, Chunk*& chunk)
	{
		auto result = chunks.find(hash);
		if (result == chunks.end()) return false;
		chunk = result->second;
		return true;
	}
	bool tryGetChunk(const int3& position, Chunk*& chunk)
	{
		return tryGetChunk(posToChunkHash(position), chunk);
	}

//----------------------------------------------------------------------------------------
	Chunk* addChunk(const int3& position, Voxel voxel = NULL_VOXEL)
	{
		Chunk* chunk;
		if (freeChunks && freeChunks->size() > 0)
		{
			chunk = freeChunks->top();
			freeChunks->pop();
			chunk->fill(voxel);
			chunk->state = ChunkState::Allocated;
		}
		else
		{
			auto entity = manager->createEntity();
			manager->add<TransformComponent>(entity);
			manager->add<OpaqVoxRenderComponent>(entity);
			chunk = new Chunk(voxel, position, id, entity);
		}

		auto hash = posToChunkHash(position);
		auto result = chunks.emplace(hash, chunk);

		#if GARDEN_DEBUG
		if (!result.second)
		{
			manager->destroy(chunk->getEntity());
			delete chunk;

			throw runtime_error("Chunk already exists. ("
				"position: " + position.toString() + ", " +
				"hash: " + to_string(posToChunkHash(position)) + ")");
		}

		auto transformComponent = manager->get<TransformComponent>(chunk->getEntity());
		transformComponent->name = "Chunk " + position.toString();
		#endif

		return chunk;
	}

	Chunk* getOrAddChunk(const int3& position, Voxel voxel = NULL_VOXEL)
	{
		Chunk* chunk;
		if (tryGetChunk(position, chunk)) return chunk;
		return addChunk(position, voxel);
	}

//----------------------------------------------------------------------------------------
	void removeChunk(int64 hash)
	{
		auto result = chunks.find(hash);

		#if GARDEN_DEBUG
		if (result == chunks.end())
		{
			int3 position; hashToChunkPos(hash, position);
			throw runtime_error("Chunk doesn't exist. ("
				"position: " + position.toString() + ", " +
				"hash: " + to_string(hash) + ")");
		}
		#endif

		if (freeChunks) freeChunks->push(result->second);
		chunks.erase(result);
	}
	void removeChunk(const int3& position)
	{
		removeChunk(posToChunkHash(position));
	}

	bool tryRemoveChunk(int64 hash)
	{
		auto result = chunks.find(hash);
		if (result == chunks.end()) return false;
		if (freeChunks) freeChunks->push(result->second);
		chunks.erase(result);
		return true;
	}
	bool tryRemoveChunk(const int3& position)
	{
		return tryRemoveChunk(posToChunkHash(position));
	}

	void removeOutOfView(const int3& position, int32 radius)
	{
		auto radius2 = radius * radius;
		for (auto i = chunks.begin(); i != chunks.end(); i++)
		{
			int3 chunkPosition; hashToChunkPos(i->first, chunkPosition);
			if (distance2(position, chunkPosition) <= radius2) continue;
			if (freeChunks) freeChunks->push(i->second);
			i = chunks.erase(i);
		}
	}
};

} // namespace voxfield