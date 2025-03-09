#include "RendererTests.h"

#include "magic_enum.hpp"

#include "../portability/port_filesystem.h"
#include "../engine/CommandLineParser.h"
#include "../engine/engine_support.h"

int renderer_tests_frame_count = 0;
bool renderer_tests_success = false;
bool renderer_tests_quit = false;

// TODO: inject sprites in one of the levels and use this for all sprite code paths
std::array<RendererTestsForLevel,25> renderer_tests{
	RendererTestsForLevel{100, 0, 240, false, 0, 0, RendererTestsMovements::move_left, { // level 0
		{RendererTestsHitCheckpoint::RendTest_HD_Draw_Rasterline_Standard, false}, {RendererTestsHitCheckpoint::RendTest_Orig_Draw_Rasterline_Standard, false},
	}},
	RendererTestsForLevel{200, 320, 240, false, 0, 0, RendererTestsMovements::move_right, { // level 1
		{RendererTestsHitCheckpoint::RendTest_HD_Draw_Rasterline_Standard, false}, {RendererTestsHitCheckpoint::RendTest_Orig_Draw_Rasterline_Standard, false},
		{RendererTestsHitCheckpoint::RendTest_HD_Draw_Rasterline_Reflections, false}, {RendererTestsHitCheckpoint::RendTest_Orig_Draw_Rasterline_Reflections, false},
	}}, 
	RendererTestsForLevel{150, 310, 240, true, 0, 2, RendererTestsMovements::move_forward, { // level 2 - flat shading
 		{RendererTestsHitCheckpoint::RendTest_HD_Draw_Rasterline_Flatshading, false}, {RendererTestsHitCheckpoint::RendTest_Orig_Draw_Rasterline_Flatshading, false},
	}},
	RendererTestsForLevel{100, 0, 480, false, 0, 0, RendererTestsMovements::move_forward, { // level 3 - looking up to the ceiling
		{RendererTestsHitCheckpoint::RendTest_HD_Draw_Rasterline_Standard, false}, {RendererTestsHitCheckpoint::RendTest_Orig_Draw_Rasterline_Standard, false},
	}},
	RendererTestsForLevel{100, 480, 200, false, 0, 0, RendererTestsMovements::move_forward, { // level 4 - looking down to the floor, right turn, goat sprite rendering
		{RendererTestsHitCheckpoint::RendTest_HD_Draw_Sprite_0, false}, {RendererTestsHitCheckpoint::RendTest_Orig_Draw_Sprite_0, false},
	}},
	RendererTestsForLevel{150, 420, 220, false, 0, 0, RendererTestsMovements::move_forward, {}},
	RendererTestsForLevel{150, 0, 240, false, 0, 15, RendererTestsMovements::move_forward, {}}, // level 6
	RendererTestsForLevel{150, 0, 240, false, 0, 0, RendererTestsMovements::move_forward, {}},
	RendererTestsForLevel{150, 140, 240, false, 0, 4, RendererTestsMovements::move_forward, {}}, // level 8 - lot of sprites
	RendererTestsForLevel{150, 320, 240, false, 0, 2, RendererTestsMovements::move_forward, {}}, // level 9
	RendererTestsForLevel{1, 0, 240, false, 0, 0, RendererTestsMovements::no_movement, {}}, // level 10
	RendererTestsForLevel{1, 0, 240, false, 0, 0, RendererTestsMovements::no_movement, {}}, // level 11
	RendererTestsForLevel{1, 0, 240, false, 0, 0, RendererTestsMovements::no_movement, {}}, // level 12
	RendererTestsForLevel{1, 0, 240, false, 0, 0, RendererTestsMovements::no_movement, {}}, // level 13
	RendererTestsForLevel{1, 0, 240, false, 0, 0, RendererTestsMovements::no_movement, {}}, // level 14
	RendererTestsForLevel{1, 0, 240, false, 0, 0, RendererTestsMovements::no_movement, {}}, // level 15
	RendererTestsForLevel{1, 0, 240, false, 0, 0, RendererTestsMovements::no_movement, {}}, // level 16
	RendererTestsForLevel{1, 0, 240, false, 0, 0, RendererTestsMovements::no_movement, {}}, // level 17
	RendererTestsForLevel{1, 0, 240, false, 0, 0, RendererTestsMovements::no_movement, {}}, // level 18
	RendererTestsForLevel{1, 0, 240, false, 0, 0, RendererTestsMovements::no_movement, {}}, // level 19
	RendererTestsForLevel{1, 0, 240, false, 0, 0, RendererTestsMovements::no_movement, {}}, // level 20
	RendererTestsForLevel{1, 0, 240, false, 0, 0, RendererTestsMovements::no_movement, {}}, // level 21
	RendererTestsForLevel{1, 0, 240, false, 0, 0, RendererTestsMovements::no_movement, {}}, // level 22
	RendererTestsForLevel{1, 0, 240, false, 0, 0, RendererTestsMovements::no_movement, {}}, // level 23
	RendererTestsForLevel{100, 320, 240, false, 0, 0, RendererTestsMovements::barrel_roll, {}}, // level 24
};

bool stop_renderer_tests() {
	if (renderer_tests_frame_count >= renderer_tests[CommandLineParams.GetSetLevel()].max_frames) {
		renderer_tests_eval_findings();
		return true;
	}
	return false;
}

void renderer_tests_eval_findings() {
	renderer_tests_quit = true;

	renderer_tests_success = true;
	for (auto& [key, hit] : renderer_tests[CommandLineParams.GetSetLevel()].must_hit_checkpoints) {
		if (!hit) {
			Logger->error("Renderer test failed: {} not hit", magic_enum::enum_name(key));
			renderer_tests_success = false;
		}
	}

	if (renderer_tests[CommandLineParams.GetSetLevel()].differences > 0) {
		Logger->warn("Differences between HD and Original renderer: {0}", renderer_tests[CommandLineParams.GetSetLevel()].differences);

		if (renderer_tests[CommandLineParams.GetSetLevel()].differences > renderer_tests[CommandLineParams.GetSetLevel()].differencesThreshold) {
			// NOTE: We accept some differences as compared to the original renderer,
			//       because the originial renderer accesses illegal texture memory.
			//       Fixing this yields pixel differences in some corner cases.
			Logger->error("Too many differences between HD and Original renderer");
			renderer_tests_success = false;
		}
	}

	if (!renderer_tests_success) {
		allert_error();
	}
	else {
		Logger->info("No differences between HD and Original renderer and all checkpoints hit");
	}
}

void renderer_tests_register_hit(const RendererTestsHitCheckpoint& checkpoint) {
	renderer_tests[CommandLineParams.GetSetLevel()].must_hit_checkpoints[checkpoint] = true;
}
