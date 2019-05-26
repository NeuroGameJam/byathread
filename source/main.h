#pragma once

enum ENTITY_TYPE 
{ 
	UNKNOWN, LABEL, BUTTON,
	GUNIT, WORLD, SPOT, CONNECTOR,
	PLAYER, ENEMY,
	EMITTER, SHIELD, CRYSTAL
};

#include "core/env.h"
#include "core/math.h"
#include "core/io.h"
#include "core/game.h"

#include "android/main.h"

#include "systems/types.h"
#include "systems/static.h"
#include "systems/collider.h"
#include "systems/audio.h"
#include "systems/sound.h"
#include "systems/shader.h"
#include "systems/image.h"
#include "systems/font.h"
#include "systems/texture.h"
#include "systems/model.h"
#include "systems/camera.h"
#include "systems/renderer.h"
#include "systems/manager.h"

#include "android/main.c"

// components
#include "components/button.h"
#include "components/label.h"
#include "components/gunit.h"

#include "entities/types.h"
#include "common.h"
#include "entities/emitter.h"
#include "entities/enemy.h"
#include "entities/player.h"
#include "entities/connector.h"
#include "entities/rope.h"
#include "entities/world.h"
#include "entities/ambient.h"
#include "entities/snow.h"

// scenes
#include "scenes/loading.h"
#include "scenes/menu.h"
#include "scenes/gameplay.h"
#include "scenes/stats.h"
