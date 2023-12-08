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

#include "voxfield/client/system/graphics/geometry.hpp"

using namespace voxfield;
using namespace voxfield::client;

namespace
{
	struct VoxGeoPC final
	{
		uint32 instanceIndex;
	};
	struct VoxGeoShadPC final
	{
		float4x4 mvp;
	};
}

static void createInstanceBuffers(GraphicsSystem* graphicsSystem,
	uint64 bufferSize, vector<vector<ID<Buffer>>>& instanceBuffers)
{
	auto swapchainSize = graphicsSystem->getSwapchainSize();
	instanceBuffers.resize(swapchainSize);

	for (uint32 i = 0; i < swapchainSize; i++)
	{
		auto buffer = graphicsSystem->createBuffer(
			Buffer::Bind::Storage, Buffer::Usage::CpuToGpu, bufferSize);
		SET_RESOURCE_DEBUG_NAME(graphicsSystem, buffer,
			"buffer.storage.geometry.instances" + to_string(i));
		instanceBuffers[i].push_back(buffer);
	}
}
static void destroyInstanceBuffers(GraphicsSystem* graphicsSystem,
	vector<vector<ID<Buffer>>>& instanceBuffers)
{
	for (auto& sets : instanceBuffers) graphicsSystem->destroy(sets[0]);
	instanceBuffers.clear();
}

void VoxGeoRenderSystem::initialize()
{
	mesherSystem = getManager()->get<MesherSystem>();
	if (!pipeline) pipeline = createPipeline();

	auto graphicsSystem = getGraphicsSystem();
	createInstanceBuffers(graphicsSystem, 16 * sizeof(InstanceData), instanceBuffers);
}

//--------------------------------------------------------------------------------------------------
bool VoxGeoRenderSystem::isDrawReady()
{
	auto graphicsSystem = getGraphicsSystem();
	auto pipelineView = graphicsSystem->get(pipeline);
	if (!pipelineView->isReady()) return false;

	if (!descriptorSet)
	{
		auto uniforms = getUniforms();
		descriptorSet = graphicsSystem->createDescriptorSet(pipeline, std::move(uniforms));
		SET_RESOURCE_DEBUG_NAME(graphicsSystem, descriptorSet, "descriptorSet.geometry");
	}
	
	return true;
}
void VoxGeoRenderSystem::prepareDraw(const float4x4& viewProj,
	ID<Framebuffer> framebuffer, uint32 drawCount)
{
	auto graphicsSystem = getGraphicsSystem();
	if (graphicsSystem->get(instanceBuffers[0][0])->getBinarySize() <
		drawCount * sizeof(InstanceData))
	{
		auto bufferSize = drawCount * sizeof(InstanceData);
		destroyInstanceBuffers(graphicsSystem, instanceBuffers);
		createInstanceBuffers(graphicsSystem, bufferSize, instanceBuffers);

		graphicsSystem->destroy(descriptorSet);
		auto uniforms = getUniforms();
		descriptorSet = graphicsSystem->createDescriptorSet(pipeline, std::move(uniforms));
		SET_RESOURCE_DEBUG_NAME(graphicsSystem, descriptorSet, "descriptorSet.geometry");
	}

	auto swapchainIndex = graphicsSystem->getSwapchainIndex();
	auto framebufferView = graphicsSystem->get(framebuffer);
	auto instanceBufferView = graphicsSystem->get(instanceBuffers[swapchainIndex][0]);
	pipelineView = graphicsSystem->get(pipeline);
	instanceMap = (InstanceData*)instanceBufferView->getMap();
	framebufferSize = framebufferView->getSize();
	indexBuffer = mesherSystem->getIndexBuffer();
}
void VoxGeoRenderSystem::beginDraw(int32 taskIndex)
{
	pipelineView->bindAsync(0, taskIndex);
	pipelineView->setViewportScissorAsync(float4(float2(0), framebufferSize), taskIndex);
	pipelineView->bindDescriptorSetAsync(descriptorSet, 0, taskIndex);
}

//--------------------------------------------------------------------------------------------------
void VoxGeoRenderSystem::draw(
	TransformComponent* transformComponent, MeshRenderComponent* meshRenderComponent,
	const float4x4& viewProj, const float4x4& model, uint32 drawIndex, int32 taskIndex)
{
	auto voxGeoComponent = (VoxGeoRenderComponent*)meshRenderComponent;
	if (!voxGeoComponent->vertexBuffer) return;

	auto graphicsSystem = getGraphicsSystem();
	auto vertexBufferView = graphicsSystem->get(voxGeoComponent->vertexBuffer);
	if (!vertexBufferView->isReady()) return;

	auto& instance = instanceMap[drawIndex];
	instance.mvp = viewProj * model;

	auto pushConstants = pipelineView->getPushConstantsAsync<VoxGeoPC>(taskIndex);
	pushConstants->instanceIndex = drawIndex;
	pipelineView->pushConstantsAsync(taskIndex);

	auto indexType = voxGeoComponent->indexCount > UINT16_MAX ?
		GraphicsPipeline::Index::Uint32 : GraphicsPipeline::Index::Uint16;
	pipelineView->drawIndexedAsync(taskIndex, voxGeoComponent->vertexBuffer,
		indexBuffer, indexType, voxGeoComponent->indexCount);
}

//--------------------------------------------------------------------------------------------------
void VoxGeoRenderSystem::finalizeDraw(const float4x4& viewProj,
	ID<Framebuffer> framebuffer, uint32 drawCount)
{
	auto graphicsSystem = getGraphicsSystem();
	auto swapchainIndex = graphicsSystem->getSwapchainIndex();
	auto instanceBufferView = getGraphicsSystem()->get(instanceBuffers[swapchainIndex][0]);
	instanceBufferView->flush(drawCount * sizeof(InstanceData));
}

void VoxGeoRenderSystem::recreateSwapchain(const SwapchainChanges& changes)
{
	if (changes.bufferCount)
	{
		auto graphicsSystem = getGraphicsSystem();
		auto bufferView = graphicsSystem->get(instanceBuffers[0][0]);
		auto bufferSize = bufferView->getBinarySize();
		destroyInstanceBuffers(graphicsSystem, instanceBuffers);
		createInstanceBuffers(graphicsSystem, bufferSize, instanceBuffers);

		if (descriptorSet)
		{
			auto descriptorSetView = graphicsSystem->get(descriptorSet);
			auto uniforms = getUniforms();
			descriptorSetView->recreate(std::move(uniforms));
		}
	}
}

map<string, DescriptorSet::Uniform> VoxGeoRenderSystem::getUniforms()
{
	map<string, DescriptorSet::Uniform> uniforms =
	{ { "instance", DescriptorSet::Uniform(instanceBuffers) } };
	return uniforms;
}

ID<GraphicsPipeline> VoxGeoRenderSystem::getPipeline()
{
	if (!pipeline) pipeline = createPipeline();
	return pipeline;
}

//--------------------------------------------------------------------------------------------------
void VoxGeoShadRenderSystem::initialize()
{
	mesherSystem = getManager()->get<MesherSystem>();
	if (!pipeline) pipeline = createPipeline();
}

bool VoxGeoShadRenderSystem::isDrawReady()
{
	auto pipelineView = getGraphicsSystem()->get(pipeline);
	return pipelineView->isReady();
}
void VoxGeoShadRenderSystem::prepareDraw(const float4x4& viewProj,
	ID<Framebuffer> framebuffer, uint32 drawCount)
{
	auto graphicsSystem = getGraphicsSystem();
	auto framebufferView = graphicsSystem->get(framebuffer);
	pipelineView = graphicsSystem->get(pipeline);
	pipelineView->updateFramebuffer(framebuffer);
	framebufferSize = framebufferView->getSize();
	indexBuffer = mesherSystem->getIndexBuffer();
}
void VoxGeoShadRenderSystem::beginDraw(int32 taskIndex)
{
	pipelineView->bindAsync(0, taskIndex);
	pipelineView->setViewportScissorAsync(float4(float2(0), framebufferSize), taskIndex);
}

void VoxGeoShadRenderSystem::draw(
	TransformComponent* transformComponent, MeshRenderComponent* meshRenderComponent,
	const float4x4& viewProj, const float4x4& model, uint32 drawIndex, int32 taskIndex)
{
	auto voxGeoShadComponent = (VoxGeoShadRenderComponent*)meshRenderComponent;
	if (!voxGeoShadComponent->vertexBuffer) return;

	auto graphicsSystem = getGraphicsSystem();
	auto vertexBufferView = graphicsSystem->get(voxGeoShadComponent->vertexBuffer);
	if (!vertexBufferView->isReady()) return;

	auto& cameraConstants = graphicsSystem->getCurrentCameraConstants();
	auto pushConstants = pipelineView->getPushConstantsAsync<VoxGeoShadPC>(taskIndex);
	pushConstants->mvp = viewProj * model;
	pipelineView->pushConstantsAsync(taskIndex);

	auto indexType = voxGeoShadComponent->indexCount > UINT16_MAX ?
		GraphicsPipeline::Index::Uint32 : GraphicsPipeline::Index::Uint16;
	pipelineView->drawIndexedAsync(taskIndex, voxGeoShadComponent->vertexBuffer,
		indexBuffer, indexType, voxGeoShadComponent->indexCount);
}

ID<GraphicsPipeline> VoxGeoShadRenderSystem::getPipeline()
{
	if (!pipeline) pipeline = createPipeline();
	return pipeline;
}