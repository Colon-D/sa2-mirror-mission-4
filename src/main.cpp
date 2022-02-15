// happy Valentine's day <3

#include <array>
#include <SA2ModLoader.h>
#include <IniFile.hpp>
// I probably should not be including a source file, 
// but it has a function I want (matrix4x4_Lookat())
namespace flipscreen {
	#include <flipscreen.cpp>
}

// flipmode's flipmode that has been loaded by the IniFile,
// flipscreen::active_flipmode is what is currently... active
auto loaded_flipmode = flipscreen::flipmode::flipmode_None;
// flipmode's rotation in radians that has been loaded by the IniFile,
// flipscreen::rotationRadians is what is currently active
float loaded_rotation{ 0.f };
// flipmode's rotation speed that has been loaded by the IniFile,
// flipscreen::rotationSpeed is what is currently active
float loaded_rotation_speed{ 0.f };
// flipmode to add to loaded_flipmode
auto mm4_flipmode = flipscreen::flipmode::flipmode_Vertical;
// rotation to add to loaded_rotation
float mm4_rotation{ 0.f };
// rotation to add to loaded_speed
float mm4_rotation_speed{ 0.f };

/// whether a main mission should be mirrored or not
std::array mirror_mission_main{
	false, // normal
	false, // 100 rings
	false, // lost chao
	true,  // timer
	false  // hard mode
};

/// whether a route 101/route 280 mission should be mirrored or not
std::array mirror_mission_route_101_280{
	false, // normal
	false, // 100 rings
	false, // dodge cars
	false, // dodge walls
	false  // hard mode
};

/// whether the chao world should be mirrored or not (buggy)
bool mirror_chao_world{ false };

/// action characters mirror (boards animations are buggy)
enum class mirror_action_characters {
	no,
	yes,
	yes_off_board
};
auto mirror_action_characters = mirror_action_characters::yes_off_board;
/// shooting characters mirror (shooting can be buggy)
bool mirror_shooting_characters{ true };
/// treasure hunting characters mirror (nothing I know of is buggy)
bool mirror_treasure_hunting_characters{ true };
// kart racing and mechless tails and eggman do not deserve an option >:(
// unless someone wants one, I guess

/// the player should be flipped on the x axis, facing the camera...
bool mirror_player_x_axis{ false };
/// the player should be flipped on the y axis, up and down and all around!
bool mirror_player_y_axis{ false };
/// the player should be flipped on the z axis, kinda epic moment?
auto mirror_player_z_axis{ true };

/// character classes: action, shooting, treasure hunting or other
enum class character_class {
	action, // sonic, shadow, amy and metal sonic
	shooting, // tails, eggman, chao walker and dark chao walker
	treasure_hunting, // knuckles, rouge, tikal and chaos
	other // super sonic, super shadow, mechless tails, mechless eggman and
	      // whoever the hell B is
};

/// returns the current character class
character_class current_character_class() {
	switch (CurrentCharacter) {
	case Characters_Sonic:
		return character_class::action;
		break;
	case Characters_Shadow:
		return character_class::action;
		break;
	case Characters_Knuckles:
		return character_class::treasure_hunting;
		break;
	case Characters_Rouge:
		return character_class::treasure_hunting;
		break;
	case Characters_MechTails:
		return character_class::shooting;
		break;
	case Characters_MechEggman:
		return character_class::shooting;
		break;
	case Characters_Amy:
		return character_class::action;
		break;
	case Characters_MetalSonic:
		return character_class::action;
		break;
	case Characters_ChaoWalker:
		return character_class::shooting;
		break;
	case Characters_DarkChaoWalker:
		return character_class::shooting;
		break;
	case Characters_Tikal:
		return character_class::treasure_hunting;
		break;
	case Characters_Chaos:
		return character_class::treasure_hunting;
		break;
	default:
		return character_class::other;
		break;
	}
}

/// result of adding two flipscreens together
struct flipmode_addition_result {
	flipscreen::flipmode flipmode{ flipscreen::flipmode::flipmode_None };
	float rotation{ 0.f };
};

/// returns result of adding two flipscreens together
constexpr flipmode_addition_result operator+(
	const flipscreen::flipmode lhs,
	const flipscreen::flipmode rhs
) {
	if (lhs == rhs) {
		return {};
	}
	else if (rhs == flipscreen::flipmode::flipmode_None) {
		return { lhs };
	}
	else if (lhs == flipscreen::flipmode::flipmode_None) {
		return { rhs };
	}
	else {
		return { .rotation = PI };
	}
}

/// returns whether a mission should be mirrored
bool mirror_mission(const LevelIDs level_id, const char mission_number) {
	switch (CurrentLevel) {
	case LevelIDs_Route101280:
		return mirror_mission_route_101_280[MissionNum];
		break;
	case LevelIDs_ChaoWorld:
		return mirror_chao_world;
		break;
	default:
		return mirror_mission_main[MissionNum];
		break;
	}
}

/// returns whether the current mission should be mirrored
bool mirror_current_mission() {
	return mirror_mission(static_cast<LevelIDs>(CurrentLevel), MissionNum);
}

/// returns whether the current character is on a board
bool on_board() {
	return
		MainCharObj1[0] &&
		current_character_class() == character_class::action &&
		(
			MainCharObj1[0]->Action == Actions::Action_Board ||
			MainCharObj1[0]->Action == Actions::Action_BoardBrake ||
			MainCharObj1[0]->Action == Actions::Action_BoardBump ||
			MainCharObj1[0]->Action == Actions::Action_BoardFall ||
			MainCharObj1[0]->Action == Actions::Action_BoardJump ||
			MainCharObj1[0]->Action == Actions::Action_BoardTrick
		);
}

/// returns whether the current character in their current state in the 
/// current mission should be mirrored (true) or unmirrored (false)
bool mirror_player() {
	if (mirror_current_mission()) {
		switch (current_character_class()) {
		case character_class::action:
			switch (mirror_action_characters) {
			case mirror_action_characters::no:
				return false;
				break;
			case mirror_action_characters::yes:
				return true;
				break;
			case mirror_action_characters::yes_off_board:
				return !on_board();
				break;
			}
			break;
		case character_class::shooting:
			return mirror_shooting_characters;
			break;
		case character_class::treasure_hunting:
			return mirror_treasure_hunting_characters;
			break;
		case character_class::other:
			return true;
			break;
		}
		// here should never be reached... unless?
		return true;
	}
	else {
		return false;
	}
}

/// flips the controls, left becomes right and vice versa
void flip_controls() {
	// swap both analog sticks
	ControllerPointers[0]->x1 = -ControllerPointers[0]->x1;
	ControllerPointers[0]->x2 = -ControllerPointers[0]->x2;
	// swap lt and rt, these triggers are represented by both an analog short 
	// and a boolean bit
	std::swap(ControllerPointers[0]->l, ControllerPointers[0]->r);
	const auto l = (ControllerPointers[0]->on & Buttons_L) != 0;
	const auto r = (ControllerPointers[0]->on & Buttons_R) != 0;
	if (l) {
		ControllerPointers[0]->on |= Buttons_R;
	} else {
		ControllerPointers[0]->on &= ~Buttons_R;
	}
	if (r) {
		ControllerPointers[0]->on |= Buttons_L;
	}
	else {
		ControllerPointers[0]->on &= ~Buttons_L;
	}
}

/// mirrors screens when current mission should be mirrored
flipscreen::Matrix4x4* matrix4x4_Lookat_replacement(
	flipscreen::Vector3* origin,
	flipscreen::Vector3* target,
	flipscreen::Vector3* up,
	flipscreen::Matrix4x4* output
) {
	if (mirror_current_mission()) {
		const auto result = loaded_flipmode + mm4_flipmode;
		flipscreen::active_flipmode = result.flipmode;
		flipscreen::rotationRadians =
			loaded_rotation + mm4_rotation + result.rotation;
		flipscreen::rotationSpeed = loaded_rotation_speed + mm4_rotation_speed;
	}
	else {
		flipscreen::active_flipmode = loaded_flipmode;
		flipscreen::rotationRadians = loaded_rotation;
		flipscreen::rotationSpeed = loaded_rotation_speed;
	}
	return flipscreen::matrix4x4_Lookat(origin, target, up, output);
}

// stolen from flipscreen
void __declspec(naked) matrix4x4_Lookat_hook_replacement() {
	__asm {
		push        edx
		push        ecx
		push        ebx
		push        dword ptr[esp + 14h]
		push        dword ptr[esp + 14h]
		push        eax
		call        matrix4x4_Lookat_replacement
		add         esp, 0Ch
		pop         ebx
		pop         ecx
		pop         edx
		ret
	}
}

extern "C" {
	__declspec(dllexport) void Init(
		const char* path,
		const HelperFunctions& helperFunctions
	) {
		const IniFile settings{ std::string{ path } + "\\config.ini" };

		// my flipscreen "compatibility" fix (ie, stealing their entire mod)
		if (const auto fs_settings =
				settings.getGroup("FlipScreen Settings")
		) {
			const auto s_flipmode = fs_settings->getString("Flipmode", "None");
			if (s_flipmode == "Horizontal") {
				loaded_flipmode = flipscreen::active_flipmode =
					flipscreen::flipmode::flipmode_Horizontal;
			}
			else if (s_flipmode == "Vertical") {
				loaded_flipmode = flipscreen::active_flipmode =
					flipscreen::flipmode::flipmode_Vertical;
			}

			loaded_rotation = flipscreen::rotationRadians =
				fs_settings->getFloat("Rotate Screen", loaded_rotation) /
				flipscreen::Rad2Deg;
			loaded_rotation_speed = flipscreen::rotationSpeed =
				fs_settings->getFloat(
					"Rotation Animation Speed",
					loaded_rotation_speed
				) /
				flipscreen::Rad2Deg;
		}

		// my mod settings
		if (const auto mm4_settings =
				settings.getGroup("Mirror Mission 4 Settings")
		) {
			const auto s_flipmode =
				mm4_settings->getString("Mirrored Flipmode", "Vertical");
			if (s_flipmode == "Horizontal") {
				mm4_flipmode = flipscreen::flipmode::flipmode_Horizontal;
			} else if (s_flipmode != "Vertical") {
				mm4_flipmode = flipscreen::flipmode::flipmode_None;
			}
			mm4_rotation =
				mm4_settings->getFloat(
					"Mirrored Rotate Screen",
					mm4_rotation
				) /
				flipscreen::Rad2Deg;
			mm4_rotation_speed =
				mm4_settings->getFloat(
					"Mirrored Rotation Animation Speed",
					mm4_rotation_speed
				) /
				flipscreen::Rad2Deg;

			mirror_mission_main[0] = mm4_settings->getBool(
				"Mirror 1st Mission",
				mirror_mission_main[0]
			);
			mirror_mission_main[1] = mm4_settings->getBool(
				"Mirror 2nd Mission: \"Collect 100 Rings!\"",
				mirror_mission_main[1]
			);
			mirror_mission_main[2] = mm4_settings->getBool(
				"Mirror 3rd Mission: \"Find the lost Chao!\"",
				mirror_mission_main[2]
			);
			mirror_mission_main[3] = mm4_settings->getBool(
				"Mirror 4th Mission: Timed",
				mirror_mission_main[3]
			);
			mirror_mission_main[4] = mm4_settings->getBool(
				"Mirror 5th Mission: \"Clear Hard Mode!\"",
				mirror_mission_main[4]
			);

			mirror_mission_route_101_280[0] = mm4_settings->getBool(
				"Mirror Route 101/Route 280's 1st Mission",
				mirror_mission_route_101_280[0]
			);
			mirror_mission_route_101_280[1] = mm4_settings->getBool(
				"Mirror Route 101/Route 280's 2nd Mission: "
				"\"Collect 100 Rings!\"",
				mirror_mission_route_101_280[1]
			);
			mirror_mission_route_101_280[2] = mm4_settings->getBool(
				"Mirror Route 101/Route 280's 3rd Mission: "
				"\"Don't hit other cars!\"",
				mirror_mission_route_101_280[2]
			);
			mirror_mission_route_101_280[3] = mm4_settings->getBool(
				"Mirror Route 101/Route 280's 4th Mission: "
				"\"Don't hit the walls!\"",
				mirror_mission_route_101_280[3]
			);
			mirror_mission_route_101_280[4] = mm4_settings->getBool(
				"Mirror Route 101/Route 280's 5th Mission: "
				"\"Clear Hard Mode!\"",
				mirror_mission_route_101_280[4]
			);

			mirror_chao_world = mm4_settings->getBool(
				"Mirror Chao World (BROKEN)",
				mirror_chao_world
			);

			mirror_player_x_axis = mm4_settings->getBool(
				"Mirror Player X Axis",
				mirror_player_x_axis
			);
			mirror_player_y_axis = mm4_settings->getBool(
				"Mirror Player Y Axis",
				mirror_player_y_axis
			);
			mirror_player_z_axis = mm4_settings->getBool(
				"Mirror Player Z Axis",
				mirror_player_z_axis
			);

			const auto s_mir_act_char = mm4_settings->getString(
				"Mirror Action Characters",
				"Yes, off board"
			);
			if (s_mir_act_char == "No") {
				mirror_action_characters = mirror_action_characters::no;
			} else if (s_mir_act_char == "Yes") {
				mirror_action_characters = mirror_action_characters::yes;
			}
			mirror_shooting_characters = mm4_settings->getBool(
				"Mirror Shooting Characters",
				mirror_shooting_characters
			);
			mirror_treasure_hunting_characters = mm4_settings->getBool(
				"Mirror Treasure Hunting Characters",
				mirror_treasure_hunting_characters
			);
		}

		// my flipscreen::hookFlipScreen() replacement
		WriteJump(
			reinterpret_cast<void*>(0x427AA0),
			matrix4x4_Lookat_hook_replacement
		);
	}

	__declspec(dllexport) void __cdecl OnFrame() {
		if (MainCharObj1[0]) {
			// WET principle in action, pro programmer hours
			// x axis
			if (mirror_player_x_axis) {
				// if not mirrored and should be mirrored, mirror
				if (MainCharObj1[0]->Scale.x > 0.f) {
					if (mirror_player()) {
						MainCharObj1[0]->Scale.x = -MainCharObj1[0]->Scale.x;
					}
				}
				// else, if mirrored and should not be mirrored, unmirror
				else {
					if (!mirror_player()) {
						MainCharObj1[0]->Scale.x = -MainCharObj1[0]->Scale.x;
					}
				}
			}
			// y axis
			if (mirror_player_y_axis) {
				// if not mirrored and should be mirrored, mirror
				if (MainCharObj1[0]->Scale.y > 0.f) {
					if (mirror_player()) {
						MainCharObj1[0]->Scale.y = -MainCharObj1[0]->Scale.y;
					}
				}
				// else, if mirrored and should not be mirrored, unmirror
				else {
					if (!mirror_player()) {
						MainCharObj1[0]->Scale.y = -MainCharObj1[0]->Scale.y;
					}
				}
			}
			// z axis
			if (mirror_player_z_axis) {
				// if not mirrored and should be mirrored, mirror
				if (MainCharObj1[0]->Scale.z > 0.f) {
					if (mirror_player()) {
						MainCharObj1[0]->Scale.z = -MainCharObj1[0]->Scale.z;
					}
				}
				// else, if mirrored and should not be mirrored, unmirror
				else {
					if (!mirror_player()) {
						MainCharObj1[0]->Scale.z = -MainCharObj1[0]->Scale.z;
					}
				}
			}
		}
	}

	__declspec(dllexport) void __cdecl OnInput() {
		// hacking controls
		// if current mission should be mirrored
		if (mirror_current_mission()) {
			// if flipped vertically, flip controls
			if (mm4_flipmode == flipscreen::flipmode::flipmode_Vertical) {
				flip_controls();
			}
			// if rotation is upside down (not in [-PI/2, PI/2])
			if (mm4_rotation < -PI / 2.f || mm4_rotation > PI / 2.f) {
				// flip controls
				flip_controls();
			}
		}
	}

	__declspec(dllexport) ModInfo SA2ModInfo { ModLoaderVer };
}
