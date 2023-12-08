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

#pragma once
#include "garden/system/graphics/mesh.hpp"
#include "voxfield/client/system/mesher.hpp"

namespace voxfield::client
{

using namespace ecsm;
using namespace garden;

class VoxGeoRenderSystem;
class VoxGeoShadRenderSystem;

struct VoxGeoRenderComponent : public MeshRenderComponent
{
	ID<Buffer> vertexBuffer = {};
	uint32 indexCount = 0;
protected:
	friend class VoxGeoRenderSystem;
};

class VoxGeoRenderSystem : public System, public IRenderSystem, public IMeshRenderSystem
{
public:
	struct InstanceData
	{
		float4x4 mvp = float4x4(0.0f);
	};
protected:
	MesherSystem* mesherSystem = nullptr;
	vector<vector<ID<Buffer>>> instanceBuffers = {};
	ID<GraphicsPipeline> pipeline = {};
	ID<DescriptorSet> descriptorSet = {};
	View<GraphicsPipeline> pipelineView = {};
	InstanceData* instanceMap = nullptr;
	int2 framebufferSize = int2(0);
	ID<Buffer> indexBuffer = {};

	void initialize() override;
	
	bool isDrawReady() override;
	void prepareDraw(const float4x4& viewProj,
		ID<Framebuffer> framebuffer, uint32 drawCount) override;
	void beginDraw(int32 taskIndex) override;
	void draw(TransformComponent* transformComponent,
		MeshRenderComponent* meshRenderComponent, const float4x4& viewProj,
		const float4x4& model, uint32 drawIndex, int32 taskIndex) override;
	void finalizeDraw(const float4x4& viewProj,
		ID<Framebuffer> framebuffer, uint32 drawCount) override;
	void recreateSwapchain(const SwapchainChanges& changes) override;

	virtual ID<GraphicsPipeline> createPipeline() = 0;
	virtual map<string, DescriptorSet::Uniform> getUniforms();
public:
	ID<GraphicsPipeline> getPipeline();
};

//--------------------------------------------------------------------------------------------------
struct VoxGeoShadRenderComponent : public VoxGeoRenderComponent
{ friend class VoxGeoShadRenderSystem; };

class VoxGeoShadRenderSystem : public System, public IRenderSystem, public IMeshRenderSystem
{
protected:
	MesherSystem* mesherSystem = nullptr;
	ID<GraphicsPipeline> pipeline = {};
	ID<Buffer> indexBuffer = {};
	View<GraphicsPipeline> pipelineView = {};
	int2 framebufferSize = int2(0);

	void initialize() override;

	bool isDrawReady() override;
	void prepareDraw(const float4x4& viewProj,
		ID<Framebuffer> framebuffer, uint32 drawCount) override;
	void beginDraw(int32 taskIndex) override;
	
	void draw(TransformComponent* transformComponent,
		MeshRenderComponent* meshRenderComponent, const float4x4& viewProj,
		const float4x4& model, uint32 drawIndex, int32 taskIndex) override;

	virtual ID<GraphicsPipeline> createPipeline() = 0;
	friend class GeometryEditor;
public:
	ID<GraphicsPipeline> getPipeline();
};

} // namespace voxfield::client