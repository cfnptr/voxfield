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

#include "voxfield/client/system/graphics/geometry/opaque.hpp"
#include "garden/system/resource.hpp"

using namespace voxfield;
using namespace voxfield::client;

void OpaqVoxRenderSystem::initialize()
{
	VoxGeoRenderSystem::initialize();
}

//--------------------------------------------------------------------------------------------------
type_index OpaqVoxRenderSystem::getComponentType() const
{
	return typeid(OpaqVoxRenderComponent);
}
ID<Component> OpaqVoxRenderSystem::createComponent(ID<Entity> entity)
{
	GARDEN_ASSERT(getManager()->has<TransformComponent>(entity));
	auto instance = components.create();
	components.get(instance)->entity = entity;
	return ID<Component>(instance);
}
void OpaqVoxRenderSystem::destroyComponent(ID<Component> instance)
{
	auto component = components.get(ID<OpaqVoxRenderComponent>(instance));
	getGraphicsSystem()->destroy(component->vertexBuffer);
	components.destroy(ID<OpaqVoxRenderComponent>(instance));
}
View<Component> OpaqVoxRenderSystem::getComponent(ID<Component> instance)
{
	return View<Component>(components.get(ID<OpaqVoxRenderComponent>(instance)));
}
void OpaqVoxRenderSystem::disposeComponents()
{
	components.dispose();
}
MeshRenderType OpaqVoxRenderSystem::getMeshRenderType() const
{
	return MeshRenderType::Opaque;
}
const LinearPool<MeshRenderComponent>& OpaqVoxRenderSystem::getMeshComponentPool() const
{
	return *((const LinearPool<MeshRenderComponent>*)&components);
}
psize OpaqVoxRenderSystem::getMeshComponentSize() const
{
	return sizeof(OpaqVoxRenderComponent);
}
ID<GraphicsPipeline> OpaqVoxRenderSystem::createPipeline()
{
	auto deferredSystem = getManager()->get<DeferredRenderSystem>();
	return ResourceSystem::getInstance()->loadGraphicsPipeline(
		"geometry/opaq-vox", deferredSystem->getGFramebuffer(), true, true);
}

//--------------------------------------------------------------------------------------------------
void OpaqVoxShadRenderSystem::initialize()
{
	VoxGeoShadRenderSystem::initialize();
}

//--------------------------------------------------------------------------------------------------
type_index OpaqVoxShadRenderSystem::getComponentType() const
{
	return typeid(OpaqVoxShadRenderComponent);
}
ID<Component> OpaqVoxShadRenderSystem::createComponent(ID<Entity> entity)
{
	GARDEN_ASSERT(getManager()->has<TransformComponent>(entity));
	auto instance = components.create();
	components.get(instance)->entity = entity;
	return ID<Component>(instance);
}
void OpaqVoxShadRenderSystem::destroyComponent(ID<Component> instance)
{
	auto component = components.get(ID<OpaqVoxShadRenderComponent>(instance));
	getGraphicsSystem()->destroy(component->vertexBuffer);
	components.destroy(ID<OpaqVoxShadRenderComponent>(instance));
}
View<Component> OpaqVoxShadRenderSystem::getComponent(ID<Component> instance)
{
	return View<Component>(components.get(ID<OpaqVoxShadRenderComponent>(instance)));
}
void OpaqVoxShadRenderSystem::disposeComponents()
{
	components.dispose();
}
MeshRenderType OpaqVoxShadRenderSystem::getMeshRenderType() const
{
	return MeshRenderType::OpaqueShadow;
}
const LinearPool<MeshRenderComponent>& OpaqVoxShadRenderSystem::getMeshComponentPool() const
{
	return *((const LinearPool<MeshRenderComponent>*)&components);
}
psize OpaqVoxShadRenderSystem::getMeshComponentSize() const
{
	return sizeof(OpaqVoxShadRenderComponent);
}
ID<GraphicsPipeline> OpaqVoxShadRenderSystem::createPipeline()
{
	// auto shadowMappingSystem = getManager()->get<ShadowMappingRenderSystem>();
	// return ResourceSystem::getInstance()->loadGraphicsPipeline(
	//	"geometry/opaq-vox-shad", shadowMappingSystem->getFramebuffers()[0], true);
	return {};
}