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

#include "ecsm.hpp"
#include "garden/main.hpp"
#include "garden/thread.hpp"
#include "garden/system/fpv.hpp"
#include "garden/system/graphics.hpp"
#include "garden/system/graphics/editor.hpp"
#include "voxfield/client/system/world.hpp"

using namespace ecsm;
using namespace garden;
using namespace voxfield;
using namespace voxfield::client;

//----------------------------------------------------------------------------------------
GARDEN_MAIN
{
	Thread::setForegroundPriority();

	auto manager = new Manager();
	manager->createSystem<GeneratorSystem>();
	manager->createSystem<WorldSystem>();
	manager->createSystem<FpvSystem>();
	manager->createSystem<TransformSystem>();
	manager->createSystem<GraphicsSystem>();
	#if GARDEN_DEBUG
	manager->createSubsystem<GraphicsSystem, EditorRenderSystem>();
	#endif
	//manager->createSystem<ThreadSystem>();

	manager->initialize();
	manager->start();
	
	delete manager;
	return EXIT_SUCCESS;
}