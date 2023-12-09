//--------------------------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------------------------

#include "voxfield/system/generator.hpp"
#include "FastNoise/FastNoise.h"

using namespace voxfield;

namespace 
{
	struct ChunkData final
	{
		GeneratorSystem* system;
		int3 position;
		uint32 structureID;
		GenType genType;
	};
};

void GeneratorSystem::initialize()
{
	auto manager = getManager();
	threadSystem = manager->get<ThreadSystem>();

	auto& threadPool = threadSystem->getBackgroundPool();
	noiseCount = threadPool.getThreadCount();
	noiseGens = (void**)malloc(noiseCount * sizeof(void*));
	if (!noiseGens) abort();

	for (uint32 i = 0; i < noiseCount; i++)
	{
		auto node = new FastNoise::SmartNode<FastNoise::Simplex>();
		*node = FastNoise::New<FastNoise::Simplex>();
		noiseGens[i] = node;
	}
}
void GeneratorSystem::terminate()
{
	for (uint32 i = 0; i < noiseCount; i++)
		delete (FastNoise::SmartNode<FastNoise::Simplex>*)noiseGens[i];
	free(noiseGens);
}

//----------------------------------------------------------------------------------------
void GeneratorSystem::generate(const ThreadPool::Task& task)
{
	auto data = (ChunkData*)task.getArgument();
	auto chunk = new Chunk(NULL_VOXEL, data->position, data->structureID, {});
	auto genType = data->genType;

	switch (genType)
	{
	case GenType::DebugSphere:
		chunk->isEmpty = !generateDebugSphere(data, chunk); break;
	default: abort();
	}

	auto system = data->system;
	system->chunkMutex.lock();
	system->chunks.push_back(chunk);
	system->chunkMutex.unlock();
	delete data;
}

//----------------------------------------------------------------------------------------
bool GeneratorSystem::generateDebugSphere(const void* _data, Chunk* chunk)
{
	auto voxels = chunk->getVoxels();
	auto center = int3(CHUNK_HALF_LENGTH);
	auto maxDist2 = CHUNK_HALF_LENGTH * CHUNK_HALF_LENGTH;
	psize index = 0;

	for (uint8 z = 0; z < CHUNK_LENGTH; z++)
	{
		for (uint8 y = 0; y < CHUNK_LENGTH; y++)
		{
			for (uint8 x = 0; x < CHUNK_LENGTH; x++)
			{
				if (x == 0 || x == CHUNK_LENGTH - 1 ||
					y == 0 || y == CHUNK_LENGTH - 1 ||
					z == 0 || z == CHUNK_LENGTH - 1 ||
					distance2(int3(x, y, z), center) < maxDist2)
				{
					voxels[index++] = DEBUG_VOXEL;
				}
			}
		}
	}

	return true;
}

//--------------------------------------------------------------------------------------------------
void GeneratorSystem::generateChunk(const int3& position,
	uint32 structureID, GenType genType)
{
	auto chunkData = new ChunkData();
	chunkData->system = this;
	chunkData->position = position;
	chunkData->structureID = structureID;
	chunkData->genType = genType;

	auto& threadPool = threadSystem->getBackgroundPool();
	threadPool.addTask(ThreadPool::Task(generate, chunkData));
}
void GeneratorSystem::flush(std::function<void(const Chunk*)> onChunk)
{
	GARDEN_ASSERT(onChunk);
	chunkMutex.lock();
	for (auto chunk : chunks)
	{
		onChunk(chunk);
		delete chunk;
	}
	chunks.clear();
	chunkMutex.unlock();
}