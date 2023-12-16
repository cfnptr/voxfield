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

#include "voxfield/client/system/world.hpp"

using namespace voxfield;
using namespace voxfield::client;

#include "garden/system/graphics/skybox.hpp" // TODO: remove
#include "garden/system/graphics/lighting.hpp" // TODO: remove

void WorldSystem::initialize()
{
	auto manager = getManager();
	graphicsSystem = manager->get<GraphicsSystem>();
	generatorSystem = manager->get<GeneratorSystem>(); 
	mesherSystem = manager->get<MesherSystem>();
	structure = Structure(manager, WORLD_STRUCTURE_ID, &freeChunks);

	auto camera = manager->createEntity();
	auto transformComponent = manager->add<TransformComponent>(camera);
	#if GARDEN_DEBUG || GARDEN_EDITOR
	transformComponent->name = "Camera";
	#endif
	manager->add<DoNotDestroyComponent>(camera);
	manager->add<CameraComponent>(camera);
	graphicsSystem->camera = camera;

	auto sun = manager->createEntity();
	transformComponent = manager->add<TransformComponent>(sun);
	transformComponent->rotation = lookAtQuat(normalize(-float3(1.0f, 6.0f, 2.0f)));
	#if GARDEN_DEBUG || GARDEN_EDITOR
	transformComponent->name = "Sun";
	#endif
	manager->add<DoNotDestroyComponent>(sun);
	graphicsSystem->directionalLight = sun;

	// TODO: load mods voxels.
	registry.finalize();
}

static bool isCubeLoaded = false; // TODO: remove

//--------------------------------------------------------------------------------------------------
void WorldSystem::update()
{
	if (!graphicsSystem->camera) return;

	if (!isCubeLoaded)
	{
		// TODO: temporal, use physical atmosphere.
		auto manager = getManager();
		auto lightingSystem = manager->get<LightingRenderSystem>();
		auto lightingComponent = manager->add<LightingRenderComponent>(graphicsSystem->camera);
		lightingSystem->loadCubemap("cubemaps/pure-sky",
			lightingComponent->cubemap, lightingComponent->sh, lightingComponent->specular);
		lightingComponent->descriptorSet = lightingSystem->createDescriptorSet(
			lightingComponent->sh, lightingComponent->specular);

		auto skyboxComponent = manager->add<SkyboxRenderComponent>(graphicsSystem->camera);
		skyboxComponent->cubemap = lightingComponent->cubemap;
		isCubeLoaded = true;
	}

	auto manager = getManager();
	auto cameraTransform = manager->get<TransformComponent>(graphicsSystem->camera);
	auto cameraPosition = clamp(worldToChunkPos(
		cameraTransform->position), int3(minBorder), int3(maxBorder));
	structure.removeOutOfView(cameraPosition, chunkViewRadius + 1);

	generatorSystem->flush([this](const Chunk* genChunk)
	{
		Chunk* worldChunk;
		if (!structure.tryGetChunk(genChunk->getPosition(), worldChunk) ||
			worldChunk->state != ChunkState::Generating) return;
		if (genChunk->isEmpty) worldChunk->state = ChunkState::Meshed;
		else
		{
			worldChunk->copy(genChunk->getVoxels());
			worldChunk->state = ChunkState::Generated;
		}
		worldChunk->isEmpty = genChunk->isEmpty;
	});

	graphicsSystem->startRecording(CommandBufferType::TransferOnly);
	mesherSystem->flush([this](MesherSystem::ChunkMesh& chunkMesh, uint32 indexCount)
	{
		Chunk* worldChunk;
		if (!structure.tryGetChunk(chunkMesh.position, worldChunk) ||
			worldChunk->state != ChunkState::Meshing) return;
		if (chunkMesh.vertexBuffer.getBinarySize() > 0)
		{
			auto opaqVoxComponent = getManager()->get<
				OpaqVoxRenderComponent>(worldChunk->getEntity());
			opaqVoxComponent->isEnabled = true;
			opaqVoxComponent->vertexBuffer = mesherSystem->getVertexBuffer(chunkMesh);
			opaqVoxComponent->indexCount = indexCount;
		}
		worldChunk->state = ChunkState::Meshed;
	});
	graphicsSystem->stopRecording();

	auto chunkViewRadius2 = (int32)(chunkViewRadius * chunkViewRadius);
	for (int16 z = 0; z <= chunkViewRadius;)
	{
		for (int16 y = 0; y <= chunkViewRadius;)
		{
			for (int16 x = 0; x <= chunkViewRadius;)
			{
				auto chunkPosition = cameraPosition + int3(x, y, z);
				if (distance2(cameraPosition, chunkPosition) > chunkViewRadius2)
				{
					if (x > 0) x = -x;
					else x = -x + 1;
					continue;
				}

				auto chunk = structure.getOrAddChunk(chunkPosition);
				if (chunk->state == ChunkState::Allocated)
				{
					generatorSystem->generateChunk(chunkPosition,
						WORLD_STRUCTURE_ID, GenType::DebugSoloVoxel);
					chunk->state = ChunkState::Generating;
				}
				else if (chunk->state == ChunkState::Generated)
				{
					auto cluster = Cluster(chunk,
						structure.getOrAddChunk(chunkPosition + int3(-1,  0,  0)),
						structure.getOrAddChunk(chunkPosition + int3( 1,  0,  0)), 
						structure.getOrAddChunk(chunkPosition + int3( 0, -1,  0)), 
						structure.getOrAddChunk(chunkPosition + int3( 0,  1,  0)), 
						structure.getOrAddChunk(chunkPosition + int3( 0,  0, -1)), 
						structure.getOrAddChunk(chunkPosition + int3( 0,  0,  1)));

					if (cluster.isMeshingReady())
					{
						mesherSystem->generateMesh(cluster);
						chunk->state = ChunkState::Meshing;
					}
				}

				if (x > 0) x = -x;
				else x = -x + 1;
			}

			if (y > 0) y = -y;
			else y = -y + 1;
		}

		if (z > 0) z = -z;
		else z = -z + 1;
	}
}