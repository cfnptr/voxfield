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

// Use this to detect pool memory corruptions.
// #define ECSM_DEEP_ID_TRACKING

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
#include "mpmt/thread.hpp"

using namespace ecsm;
using namespace garden;
using namespace voxfield;
using namespace voxfield::client;

static void entryPoint()
{
	// TODO: detect if avx2 instructions are supported in the OS if avx2 is enabled.

	mpmt::Thread::setForegroundPriority();

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

	manager->createSystem<MeshRenderSystem>(true);
	manager->createSystem<DeferredRenderSystem>(true);
	manager->createSystem<SsaoRenderSystem>();
	manager->createSystem<SkyboxRenderSystem>();
	manager->createSystem<AtmosphereRenderSystem>();
	manager->createSystem<OpaqVoxRenderSystem>();
	manager->createSystem<LightingRenderSystem>();
	manager->createSystem<BloomRenderSystem>();
	manager->createSystem<ToneMappingRenderSystem>();
	manager->createSystem<AutoExposureRenderSystem>();
	manager->createSystem<FxaaRenderSystem>();

	#if GARDEN_DEBUG || GARDEN_EDITOR
	manager->createSystem<EditorRenderSystem>();
	#endif

	manager->initialize();
	manager->start();

	delete manager;
}

GARDEN_DECLARE_MAIN(entryPoint)