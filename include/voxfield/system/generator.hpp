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
#include "garden/system/thread.hpp"

namespace voxfield
{

using namespace ecsm;
using namespace garden;

enum class GenType : uint8
{
	DebugSphere, Count
};

class GeneratorSystem final : public System
{
	ThreadSystem* threadSystem = nullptr;
	void** noiseGens = nullptr;
	vector<Chunk*> chunks;
	mutex chunkMutex;
	uint32 noiseCount = 0;

	void initialize() final;
	void terminate() final;

	static void generate(const ThreadPool::Task& task);
	static bool generateDebugSphere(const void* data, Chunk* chunk);
	
	friend class ecsm::Manager;
public:
	void generateChunk(const int3& position, uint32 structureID, GenType genType);
	void flush(std::function<void(const Chunk*)> onChunk);
};

} // namespace voxfield