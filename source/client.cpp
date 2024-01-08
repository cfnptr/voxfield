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

#include "garden/main.hpp"
#include "garden/system/fpv.hpp"
#include "garden/system/settings.hpp"
#include "garden/system/resource.hpp"
#include "garden/system/render/ssao.hpp"
#include "garden/system/render/fxaa.hpp"
#include "garden/system/render/editor.hpp"
#include "garden/system/render/skybox.hpp"
#include "garden/system/render/lighting.hpp"
#include "garden/system/render/atmosphere.hpp"
#include "garden/system/render/tone-mapping.hpp"
#include "garden/system/render/auto-exposure.hpp"
#include "voxfield/client/system/world.hpp"

extern "C"
{
#include "mpmt/thread.h"
}

using namespace ecsm;
using namespace garden;
using namespace voxfield;
using namespace voxfield::client;

static void run()
{
	// TODO: detect if avx2 instructions are supported in the OS if avx2 is enabled.

	setThreadForegroundPriority();

	auto manager = new Manager();
	manager->createSystem<DoNotDestroySystem>();
	manager->createSystem<BakedTransformSystem>();
	manager->createSystem<SettingsSystem>();
	manager->createSystem<LogSystem>();
	manager->createSystem<ResourceSystem>();
	manager->createSystem<GeneratorSystem>();
	manager->createSystem<MesherSystem>();
	manager->createSystem<WorldSystem>();
	manager->createSystem<FpvSystem>();
	manager->createSystem<CameraSystem>();
	manager->createSystem<TransformSystem>();
	manager->createSystem<GraphicsSystem>();
	manager->createSystem<ThreadSystem>();

	// TODO: implement VK_NV_low_latency2 on nvidia GPUs.

	manager->createSubsystem<GraphicsSystem, MeshRenderSystem>(true);
	manager->createSubsystem<GraphicsSystem, DeferredRenderSystem>(true);
	manager->createSubsystem<GraphicsSystem, SsaoRenderSystem>();
	manager->createSubsystem<GraphicsSystem, SkyboxRenderSystem>();
	manager->createSubsystem<GraphicsSystem, AtmosphereRenderSystem>();
	manager->createSubsystem<GraphicsSystem, OpaqVoxRenderSystem>();
	manager->createSubsystem<GraphicsSystem, LightingRenderSystem>();
	manager->createSubsystem<GraphicsSystem, BloomRenderSystem>();
	manager->createSubsystem<GraphicsSystem, ToneMappingRenderSystem>();
	manager->createSubsystem<GraphicsSystem, AutoExposureRenderSystem>();
	manager->createSubsystem<GraphicsSystem, FxaaRenderSystem>();

	#if GARDEN_DEBUG || GARDEN_EDITOR
	manager->createSubsystem<GraphicsSystem, EditorRenderSystem>();
	#endif

	manager->registerSubsystem<DeferredRenderSystem>(manager->get<MeshRenderSystem>());
	manager->registerSubsystem<DeferredRenderSystem>(manager->get<LightingRenderSystem>());
	manager->registerSubsystem<DeferredRenderSystem>(manager->get<SkyboxRenderSystem>());
	manager->registerSubsystem<DeferredRenderSystem>(manager->get<BloomRenderSystem>());
	manager->registerSubsystem<DeferredRenderSystem>(manager->get<ToneMappingRenderSystem>());
	manager->registerSubsystem<DeferredRenderSystem>(manager->get<FxaaRenderSystem>());

	manager->registerSubsystem<MeshRenderSystem>(manager->get<OpaqVoxRenderSystem>());
	// manager.registerSubsystem<MeshRenderSystem>(manager.get<TransVoxRenderSystem>());
	manager->registerSubsystem<LightingRenderSystem>(manager->get<SsaoRenderSystem>());

	manager->initialize();
	manager->start();

	delete manager;
}

//--------------------------------------------------------------------------------------------------
GARDEN_MAIN
{
	#if GARDEN_DEBUG
	run();
	#else
	try
	{
		run();
	}
	catch (const std::exception& e)
	{
		GARDEN_MESSAGE_ERROR(e.what());
		return EXIT_FAILURE;
	}
	#endif
	return EXIT_SUCCESS;
}