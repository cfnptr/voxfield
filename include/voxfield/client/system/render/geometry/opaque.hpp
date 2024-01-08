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

#pragma once
#include "voxfield/client/system/render/geometry.hpp"

namespace voxfield::client
{

class OpaqVoxRenderSystem;
class OpaqVoxShadRenderSystem;

//--------------------------------------------------------------------------------------------------
struct OpaqVoxRenderComponent final : public VoxGeoRenderComponent
{
	friend class OpaqVoxRenderSystem;
};

//--------------------------------------------------------------------------------------------------
class OpaqVoxRenderSystem final : public VoxGeoRenderSystem
{
	LinearPool<OpaqVoxRenderComponent, false> components;

	void initialize() final;

	type_index getComponentType() const final;
	ID<Component> createComponent(ID<Entity> entity) final;
	void destroyComponent(ID<Component> instance) final;
	View<Component> getComponent(ID<Component> instance) final;
	void disposeComponents() final;
	MeshRenderType getMeshRenderType() const final;
	const LinearPool<MeshRenderComponent>& getMeshComponentPool() const final;
	psize getMeshComponentSize() const final;
	ID<GraphicsPipeline> createPipeline() final;

	friend class ecsm::Manager;
};

//--------------------------------------------------------------------------------------------------
struct OpaqVoxShadRenderComponent final : public VoxGeoShadRenderComponent
{
	friend class OpaqVoxShadRenderSystem;
};

class OpaqVoxShadRenderSystem final : public VoxGeoShadRenderSystem
{
	LinearPool<OpaqVoxShadRenderComponent, false> components;

	void initialize() final;

	type_index getComponentType() const final;
	ID<Component> createComponent(ID<Entity> entity) final;
	void destroyComponent(ID<Component> instance) final;
	View<Component> getComponent(ID<Component> instance) final;
	void disposeComponents() final;
	MeshRenderType getMeshRenderType() const final;
	const LinearPool<MeshRenderComponent>& getMeshComponentPool() const final;
	psize getMeshComponentSize() const final;
	ID<GraphicsPipeline> createPipeline() final;

	friend class ecsm::Manager;
};

} // namespace voxfield::client