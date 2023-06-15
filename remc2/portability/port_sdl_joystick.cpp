
// support for gamepads and joysticks
//
// adds configurable game specific controls to be bound to axes, buttons and hats present on a controller
// see README_controllers for sample configurations
// this implementation basically simulates mouse moves and key presses
//
// tested with Logitech Attack 3, Logitech Extreme PRO 3D and XBOX Elite Series 2
//
// author: Petre Rodan, 2023

#include <cstdint>
#include <stdio.h>

#ifdef _MSC_VER
    #include "SDL.h"
#else
    #include "SDL2/SDL.h"
#endif

#include "../engine/sub_main_mouse.h"
#include "../engine/read_config.h"
#include "port_sdl_vga_mouse.h"
#include "port_sdl_joystick.h"

SDL_Joystick *m_gameController = NULL;
SDL_Haptic *m_haptic = NULL;

#define              JOY_MIN_X  0       ///< minimum bounds for mouse position value for x axis
#define              JOY_MIN_Y  0       ///< minimum bounds for mouse position value for y axis
#define            JOY_NAV_INC  4       ///< coefficient that defines how many pixels to skip while navigating a menu

#define       GP_FLIGHT_UPDATE  0x1     ///< bitflag set if the axes controlling flight are out of the stick dead zone
#define          GP_NAV_UPDATE  0x2     ///< bitflag set if the axes controlling navigation are out of the stick dead zone
#define          GP_MOV_UPDATE  0x4     ///< bitflag set if the axes controlling movement are out of the stick dead zone

#define GP_MAX_KEY_RELEASE_ANN  4       ///< maximum number of key release announcements

///< simulated key presses
// to be modified once fully customized 
// keyboard control is implemented
#define          GP_KEY_EMU_UP  0x5252
#define        GP_KEY_EMU_DOWN  0x5151
#define       GP_KEY_EMU_RIGHT  0x4f4f
#define        GP_KEY_EMU_LEFT  0x5050
#define     GP_KEY_EMU_MINIMAP  0x280d
#define         GP_KEY_EMU_ESC  0x291b
#define       GP_KEY_EMU_SPELL  0xe0e0
#define       GP_KEY_EMU_PAUSE  0x1370
#define       GP_KEY_EMU_SPACE  0x2C20

///< structure that defines the current gamepad state ad it's simulated output
struct gamepad_state {
    int32_t x;                  ///< currently simulated x mouse position
    int32_t y;                  ///< currently simulated y mouse position
    int32_t rest_x;             ///< mouse x position to use when the stick is in the rest position (or dead zone)
    int32_t rest_y;             ///< mouse y position to use when the stick is in the rest position (or dead zone)
    int32_t max_x;              ///< maximum bounds for mouse position on the x axis based on current scene (sometimes we get garbage values here from upstream)
    int32_t max_y;              ///< maximum bounds for mouse position on the y axis based on current scene (sometimes we get garbage values here from upstream)
    uint8_t dead_zone_announced;        ///< slow infinite spin mitigation when joystick is in the resting position while in the flying window
    uint8_t mov_key_announced;  ///< counter of consecutive setPress(false, KEY) requests 
    uint8_t initialized;        ///< gamepad was initialized and it's ready to be queried
    uint8_t scene_id;           ///< current scene displayed by the recode. one of SCENE_PREAMBLE_MENU, SCENE_FLIGHT, SCENE_FLIGHT_MENU
    uint8_t nav_mode;           ///< true during menu navigation
    uint8_t last_trig_fire_R;   ///< detection of movement based on the right trigger button's axis value
    uint8_t last_trig_fire_L;   ///< detection of movement based on the left trigger button's axis value
    int32_t ip_xi_neg;          ///< inflection point, x axis, input, negative side
    int32_t ip_xi_pos;          ///< inflection point, x axis, input, positive side
    int32_t ip_yi_neg;          ///< inflection point, y axis, input, negative side
    int32_t ip_yi_pos;          ///< inflection point, y axis, input, positive side
    int32_t ip_xo_neg;          ///< inflection point, x axis, output, negative side
    int32_t ip_xo_pos;          ///< inflection point, x axis, output, positive side
    int32_t ip_yo_neg;          ///< inflection point, y axis, output, negative side
    int32_t ip_yo_pos;          ///< inflection point, y axis, output, positive side
};
typedef struct gamepad_state gamepad_state_t;

///< structure that defines the mouse pointer position
struct pointer_sys {
    int16_t x;
    int16_t y;
};
typedef struct pointer_sys pointer_sys_t;

///< hat position data
struct vec1d {
    int16_t x;                  ///< bitwise flags. SDL_HAT_UP | SDL_HAT_DOWN | SDL_HAT_LEFT | SDL_HAT_RIGHT or 0
    uint8_t x_conf;             ///< bitwise flags. cound be 0 or a combination of GAMEPAD_AXIS_INVERTED | GAMEPAD_ITEM_ENABLED
};
typedef struct vec1d vec1d_t;

///< axis position data
struct vec2d {
    int16_t x;                  ///< x axis value [-32767..32768]
    int16_t y;                  ///< y axis value [-32767..32768]
    uint8_t x_conf;             ///< bitwise flags. cound be 0 or a combination of GAMEPAD_AXIS_INVERTED | GAMEPAD_ITEM_ENABLED
    uint8_t y_conf;             ///< bitwise flags. cound be 0 or a combination of GAMEPAD_AXIS_INVERTED | GAMEPAD_ITEM_ENABLED
};
typedef struct vec2d vec2d_t;

///< force-feedback subsystem state
struct haptic_state {
    uint8_t enabled;            ///< if subsystem is currently enabled
    uint8_t initialized;        ///< subsystem initialized and ready
    uint8_t rumble;             ///< rumble is initialized
    uint8_t rumble_trig;        ///< rumble trigger is present
    uint32_t cap;               ///< controller capabilities
    int effect[GP_HAPTIC_EFFECT_CNT];  ///< haptic effects
};
typedef struct haptic_state haptic_state_t;

gamepad_state_t gps = { };
haptic_state_t hs = { };

int8_t haptic_load_effects(void);
void alsound_enable_scheduling(void); // provide the function signature without including the entire port_openal.h

/// \brief initialization of the SDL joystick subsystem
void gamepad_sdl_init(void)
{

    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0) {
        Logger->error("SDL joystick could not be initialized! SDL_Error: {}", SDL_GetError());
    } else {
        m_gameController = SDL_JoystickOpen(0);
        if (m_gameController == NULL) {
            Logger->debug("joystick/gamepad not detected. SDL Error: {}", SDL_GetError());
        } else {
            Logger->info("Found '{}' joystick", SDL_JoystickName(m_gameController));
            if (SDL_JoystickEventState(SDL_ENABLE) != 1) {
                Logger->error("unable to initialize joystick/gamepad events. SDL Error: {}",
                              SDL_GetError());
            } else {
                gps.initialized = 1;
            }
            if (gpc.haptic_enabled && (SDL_InitSubSystem(SDL_INIT_HAPTIC) == 0)
                && SDL_JoystickIsHaptic(m_gameController)) {
                m_haptic = SDL_HapticOpenFromJoystick(m_gameController);
                if (m_haptic == NULL) {
                    Logger->info("unable to init force feedback. SDL Error: {}", SDL_GetError());
                } else {
                    hs.cap = SDL_HapticQuery(m_haptic);
                    hs.rumble = SDL_HapticRumbleSupported(m_haptic);
                    //hs.rumble_trig = SDL_JoystickHasRumbleTriggers(m_gameController);
                    hs.initialized = 1;
                    hs.enabled = 1;
                    haptic_load_effects();
                    if (hs.rumble) {
                        if (SDL_HapticRumbleInit(m_haptic) < 0) {
                            hs.rumble = 0;
                        }
                    }
                    if (hs.cap & SDL_HAPTIC_GAIN) {
                        SDL_HapticSetGain(m_haptic, gpc.haptic_gain_max);
                    }
                    Logger->
                        info
                        ("found haptic support (cap {}), effect cnt {}, rumble {}, rumble_trig {}",
                         hs.cap, SDL_HapticNumEffects(m_haptic), hs.rumble, hs.rumble_trig);
                }
            }
        }
    }
}

/// \brief cleanup of the SDL joystick subsystem, to be used only on program exit
void gamepad_sdl_close(void)
{
    SDL_JoystickClose(m_gameController);
    m_gameController = NULL;
    if (m_haptic) {
        SDL_HapticClose(m_haptic);
        m_haptic = NULL;
    }
}

/// \brief initialize gamepad maximal coordinate values, default operating mode, etc
/// \param gameResWidth maximum x value
/// \param gameResHeight maximum y value
void gamepad_init(const int gameResWidth, const int gameResHeight)
{
    gps.max_x = gameResWidth;
    gps.max_y = gameResHeight;
    joystick_set_env(gps.max_x >> 1, gps.max_y >> 1);
    set_scene(SCENE_PREAMBLE_MENU);
}

/// \brief flight support via conversion from stick coordinates to pointer coordinates
/// \param  stick input axis values
/// \param  point output simulated mouse pointer values
/// \return 0 if stick is in the dead zone or GP_FLIGHT_UPDATE otherwise
uint16_t gamepad_axis_flight_conv(const vec2d_t *stick, pointer_sys_t *point)
{
    uint16_t ret = 0;
    int16_t axis_yaw = stick->x;
    int16_t axis_pitch = stick->y;
    float slope, offset, output;

    if ((axis_yaw < gpc.axis_yaw_dead_zone) && (axis_yaw > -gpc.axis_yaw_dead_zone)) {
        point->x = gps.rest_x;
    } else {
        // use two different linear interpolation equations since the
        // resting coordinate ain't ever the center of the display
        if (gpc.inflection_x && gpc.inflection_y) {
            if (axis_yaw < gps.ip_xi_neg) {
                slope = (float) gps.ip_xo_neg / ((float) gps.ip_xi_neg - -32767.0f);
                offset = (float) gps.ip_xo_neg - (float) gps.ip_xi_neg * slope;
                output = slope * axis_yaw + offset;
                point->x = (int16_t) output;
            } else if (axis_yaw < 0) {
                point->x = (gps.rest_x - gps.ip_xo_neg) * axis_yaw / (0 - gps.ip_xi_neg) + gps.rest_x;
            } else if (axis_yaw < gps.ip_xi_pos) {
                point->x = (gps.ip_xo_pos - gps.rest_x) * axis_yaw / gps.ip_xi_pos + gps.rest_x;
            } else {
                slope = ((float) gps.max_x - (float) gps.ip_xo_pos) / (32768.0f - (float) gps.ip_xi_pos);
                offset = (float) gps.max_x - 32768.0f * slope;
                output = slope * axis_yaw + offset;
                point->x = (int16_t) output;
            }
        } else {
            if (axis_yaw < 0) {
                point->x = ((gps.rest_x * axis_yaw) >> 15) + gps.rest_x;
            } else {
                point->x = (((gps.max_x - gps.rest_x) * axis_yaw) >> 15) + gps.rest_x;
            }
        }
        ret = GP_FLIGHT_UPDATE;
    }

    if ((axis_pitch < gpc.axis_pitch_dead_zone) && (axis_pitch > -gpc.axis_pitch_dead_zone)) {
        point->y = gps.rest_y;
    } else {
        // use two different linear interpolation equations since the
        // resting coordinate is not always the center of the display
        if (gpc.inflection_x && gpc.inflection_y) {
            if (axis_pitch < gps.ip_yi_neg) {
                slope = (float) gps.ip_yo_neg / ((float) gps.ip_yi_neg - -32767.0f);
                offset = (float) gps.ip_yo_neg - (float) gps.ip_yi_neg * slope;
                output = slope * axis_pitch + offset;
                point->y = (int16_t) output;
            } else if (axis_pitch < 0) {
                point->y = (gps.rest_y - gps.ip_yo_neg) * axis_pitch / (0 - gps.ip_yi_neg) + gps.rest_y;
            } else if (axis_pitch < gps.ip_yi_pos) {
                point->y = (gps.ip_yo_pos - gps.rest_y) * axis_pitch / gps.ip_yi_pos + gps.rest_y;
            } else {
                slope = ((float) gps.max_y - (float) gps.ip_yo_pos) / (32768.0f - (float) gps.ip_yi_pos);
                offset = (float) gps.max_y - 32768.0f * slope;
                output = slope * axis_pitch + offset;
                point->y = (int16_t) output;
            }
        } else {
            if (axis_pitch > 0) {
                point->y = (((gps.max_y - gps.rest_y) * axis_pitch) >> 15) + gps.rest_y;
            } else {
                point->y = ((gps.rest_y * axis_pitch) >> 15) + gps.rest_y;
            }
        }
        ret = GP_FLIGHT_UPDATE;
    }

    return ret;
}

/// \brief menu navigation support via conversion from stick coordinates to pointer coordinates
/// \param  stick input axis values
/// \param  point output simulated mouse pointer values
/// \return 0 if stick is in the dead zone or GP_NAV_UPDATE otherwise
uint16_t gamepad_axis_nav_conv(const vec2d_t *stick, pointer_sys_t *point)
{
    uint16_t ret = 0;
    int16_t axis_nav_ns = stick->x;
    int16_t axis_nav_ew = stick->y;

    if ((axis_nav_ns < gpc.axis_long_nav_dead_zone) && (axis_nav_ns > -gpc.axis_long_nav_dead_zone)) {
        // point->x remains unchanged
    } else {
        point->y += JOY_NAV_INC * (axis_nav_ns >> 13);
        ret = GP_NAV_UPDATE;
    }

    if ((axis_nav_ew < gpc.axis_trans_nav_dead_zone)
        && (axis_nav_ew > -gpc.axis_trans_nav_dead_zone)) {
        // point->y remains unchanged
    } else {
        point->x += JOY_NAV_INC * (axis_nav_ew >> 13);
        ret = GP_NAV_UPDATE;
    }

    return ret;
}

/// \brief menu navigation support via conversion from hat coordinates to pointer coordinates
/// \param  hat input value
/// \param  point output simulated mouse pointer values
/// \return 0 if stick is resting or GP_NAV_UPDATE otherwise
uint16_t gamepad_hat_nav_conv(const vec1d_t *hat, pointer_sys_t *point)
{
    uint16_t ret = 0;
    int16_t inv = 1;

    // dir can be a bitwise OR of two adjacent directions
    // so don't use 'else if' or 'switch'.

    if (hat->x_conf & GAMEPAD_AXIS_INVERTED) {
        inv = -1;
    }

    if (hat->x & SDL_HAT_UP) {
        point->y += inv * JOY_NAV_INC * 2;
        ret = GP_NAV_UPDATE;
    }

    if (hat->x & SDL_HAT_DOWN) {
        point->y -= inv * JOY_NAV_INC * 2;
        ret = GP_NAV_UPDATE;
    }

    if (hat->x & SDL_HAT_RIGHT) {
        point->x += JOY_NAV_INC * 2;
        ret = GP_NAV_UPDATE;
    }

    if (hat->x & SDL_HAT_LEFT) {
        point->x -= JOY_NAV_INC * 2;
        ret = GP_NAV_UPDATE;
    }

    return ret;
}

/// \brief longitudinal and transversal hat movement converted to hardcoded keyboard keypresses
/// \param  hat input value
void gamepad_hat_mov_conv(const vec1d_t *hat)
{
    uint16_t ret = 0;

    if (hat->x & SDL_HAT_UP) {
        setPress(false, GP_KEY_EMU_DOWN);
        setPress(true, GP_KEY_EMU_UP);
        ret = GP_MOV_UPDATE;
    }

    if (hat->x & SDL_HAT_DOWN) {
        setPress(false, GP_KEY_EMU_UP);
        setPress(true, GP_KEY_EMU_DOWN);
        ret = GP_MOV_UPDATE;
    }

    if (hat->x & SDL_HAT_RIGHT) {
        setPress(false, GP_KEY_EMU_LEFT);
        setPress(true, GP_KEY_EMU_RIGHT);
        ret = GP_MOV_UPDATE;
    }

    if (hat->x & SDL_HAT_LEFT) {
        setPress(false, GP_KEY_EMU_RIGHT);
        setPress(true, GP_KEY_EMU_LEFT);
        ret = GP_MOV_UPDATE;
    }

    if (hat->x == 0) {
        if (gps.mov_key_announced < GP_MAX_KEY_RELEASE_ANN) {
            setPress(false, GP_KEY_EMU_UP);
            setPress(false, GP_KEY_EMU_DOWN);
            setPress(false, GP_KEY_EMU_RIGHT);
            setPress(false, GP_KEY_EMU_LEFT);
            gps.mov_key_announced++;
        }
    }

    if (ret) {
        gps.mov_key_announced = 0;
    }
}

/// \brief longitudinal and transversal movement via conversion from stick coordinates to key presses
/// \param  stick input axis values
void gamepad_axis_mov_conv(const vec2d_t *stick)
{
    uint16_t ret = 0;
    int16_t axis_long_inv = 1;
    int16_t axis_long = stick->x;
    int16_t axis_trans = stick->y;

    if (stick->x_conf & GAMEPAD_AXIS_INVERTED) {
        axis_long_inv = -1;
    }

    if ((axis_long < gpc.axis_long_dead_zone) && (axis_long > -gpc.axis_long_dead_zone)) {
        // player seems to always have some inertia, so the following wont't actually stop
        // longitudinal movement
        if (gps.mov_key_announced < GP_MAX_KEY_RELEASE_ANN) {
            setPress(false, GP_KEY_EMU_UP);
            setPress(false, GP_KEY_EMU_DOWN);
        }
    } else {
        if (axis_long * axis_long_inv > 0) {
            setPress(false, GP_KEY_EMU_DOWN);
            setPress(true, GP_KEY_EMU_UP);
        } else {
            setPress(false, GP_KEY_EMU_UP);
            setPress(true, GP_KEY_EMU_DOWN);
        }
        ret = GP_MOV_UPDATE;
    }

    if ((axis_trans < gpc.axis_trans_dead_zone) && (axis_trans > -gpc.axis_trans_dead_zone)) {
        if (gps.mov_key_announced < GP_MAX_KEY_RELEASE_ANN) {
            setPress(false, GP_KEY_EMU_RIGHT);
            setPress(false, GP_KEY_EMU_LEFT);
        }
    } else {
        if (axis_trans > 0) {
            setPress(false, GP_KEY_EMU_LEFT);
            setPress(true, GP_KEY_EMU_RIGHT);
        } else {
            setPress(false, GP_KEY_EMU_RIGHT);
            setPress(true, GP_KEY_EMU_LEFT);
        }
        ret = GP_MOV_UPDATE;
    }

    if (!ret) {
        if (gps.mov_key_announced < GP_MAX_KEY_RELEASE_ANN) {
            gps.mov_key_announced++;
        }
    } else {
        gps.mov_key_announced = 0;
    }
}

/// \brief button-like action via conversion from axis coordinates to a boolean (for xbox trigger buttons)
/// \param  input axis value
/// \return 0 is button is inside the dead zone, 1 otherwise
void gamepad_axis_bool_conv(const int16_t input, bool *ret)
{
    if (input > -32767 + gpc.trigger_dead_zone) {
        *ret = 1;
    } else {
        *ret = 0;
    }
}

/// \brief emulate a mouse based on data provided by a gamepad or joystick
/// \param gpe  gamepad_event_t event strucure populated thru SDL_PollEvent()
void gamepad_event_mgr(gamepad_event_t *gpe)
{
    uint16_t button_state = 0;
    uint8_t flight_mode = 1;    // are we doing flight or menu navigation
    uint16_t conv_state = 0;
    pointer_sys_t flight;
    pointer_sys_t nav;
    vec2d stick;
    vec1d hat;
    bool trig_fire_R = 0, trig_fire_L = 0;

    // decide if we are hadling flight mode or menu navigation mode
    // default mode, based on scene
    if (gps.nav_mode) {
        flight_mode = 0;
    }

    // overwrite scene-based mode based on what button is pressed
    if (SDL_JoystickGetButton(m_gameController, gpc.button_spell - 1)) {
        flight_mode = 0;
    }

    if (SDL_GetModState() & KMOD_CTRL) {
        flight_mode = 0;
    }

    stick.x = gpe->axis_yaw;
    stick.y = gpe->axis_pitch;
    flight.x = gps.x;
    flight.y = gps.y;
    // flight yaw/pitch is always done via two axes
    conv_state |= gamepad_axis_flight_conv(&stick, &flight);

    // menu navigation
    nav.x = gps.x;
    nav.y = gps.y;

    if (gpc.hat_nav_conf & GAMEPAD_ITEM_ENABLED) {
        // menu navigation is done via a hat control
        hat.x = gpe->hat_nav;
        hat.x_conf = gpc.hat_nav_conf;
        conv_state |= gamepad_hat_nav_conv(&hat, &nav);
    } else if (gpc.axis_nav_ns_conf & gpc.axis_nav_ew_conf & GAMEPAD_ITEM_ENABLED) {
        // menu navigation is done via two axes
        stick.x = gpe->axis_nav_ns;
        stick.y = gpe->axis_nav_ew;
        conv_state |= gamepad_axis_nav_conv(&stick, &nav);
    }

    if (flight_mode) {
        gps.x = flight.x;
        gps.y = flight.y;
    } else {
        gps.x = nav.x;
        gps.y = nav.y;
    }

    if (gps.x < JOY_MIN_X) {
        gps.x = JOY_MIN_X;
    } else if (gps.x > gps.max_x) {
        gps.x = gps.max_x;
    }

    if (gps.y < JOY_MIN_Y) {
        gps.y = JOY_MIN_Y;
    } else if (gps.y > gps.max_y) {
        gps.y = gps.max_y;
    }

    // longitudinal/transversal movement 
    // (aka forward/back/strafe right/strafe left)
    if (gpc.hat_mov_conf & GAMEPAD_ITEM_ENABLED) {
        // if movement is done via a hat
        hat.x = gpe->hat_mov;
        hat.x_conf = gpc.hat_mov_conf;
        gamepad_hat_mov_conv(&hat);
    }

    if (((gpc.axis_long_conf & GAMEPAD_ITEM_ENABLED) || (gpc.axis_trans_conf & GAMEPAD_ITEM_ENABLED)) && (gps.scene_id != SCENE_SPELL_MENU)) {
        // if movement is done via two axes
        stick.x = gpe->axis_long;
        stick.x_conf = gpc.axis_long_conf;
        stick.y = gpe->axis_trans;
        gamepad_axis_mov_conv(&stick);
    }

    // trigger commands
    if (gpc.axis_fire_R_conf & GAMEPAD_ITEM_ENABLED) {
        gamepad_axis_bool_conv(gpe->axis_fire_R, &trig_fire_R);
        if (trig_fire_R != gps.last_trig_fire_R) {
            if (trig_fire_R) {
                button_state |= 0x8;
            } else {
                button_state |= 0x10;
            }
            gps.last_trig_fire_R = trig_fire_R;
        }
    }

    if (gpc.axis_fire_L_conf & GAMEPAD_ITEM_ENABLED) {
        gamepad_axis_bool_conv(gpe->axis_fire_L, &trig_fire_L);
        if (trig_fire_L != gps.last_trig_fire_L) {
            if (trig_fire_L) {
                button_state |= 0x2;
            } else {
                button_state |= 0x4;
            }
            gps.last_trig_fire_L = trig_fire_L;
        }
    }

    if (gpe->btn_pressed) {
        if (gpe->btn_pressed & ((uint64_t) 1 << gpc.button_fire_R)) {
            button_state |= 0x8;
        }
        if (gpe->btn_pressed & ((uint64_t) 1 << gpc.button_fire_L)) {
            button_state |= 0x2;
        }
        if (gps.scene_id != SCENE_FLIGHT && (gpe->btn_pressed & (1 << gpc.button_menu_select))) {
            button_state |= 0x2;
        }
        if (gps.scene_id != SCENE_FLIGHT_MENU && (gpe->btn_pressed & (1 << gpc.button_spell))) {
            setPress(true, GP_KEY_EMU_SPELL);
        }
        if (gpe->btn_pressed & ((uint64_t) 1 << gpc.button_minimap)) {
            setPress(true, GP_KEY_EMU_MINIMAP);
        }
        if (gpe->btn_pressed & ((uint64_t) 1 << gpc.button_pause_menu)) {
            setPress(true, GP_KEY_EMU_PAUSE);
        }
        if (gpe->btn_pressed & ((uint64_t) 1 << gpc.button_fwd)) {
            setPress(true, GP_KEY_EMU_UP);
        }
        if (gpe->btn_pressed & ((uint64_t) 1 << gpc.button_back)) {
            setPress(true, GP_KEY_EMU_DOWN);
        }
        if (gpe->btn_pressed & ((uint64_t) 1 << gpc.button_esc)) {
            setPress(true, GP_KEY_EMU_ESC);
        }
        if (gps.scene_id == SCENE_DEAD) {
            setPress(true, GP_KEY_EMU_SPACE);
        }
    }

    if (gpe->btn_released) {
        if (gpe->btn_released & ((uint64_t) 1 << gpc.button_fire_R)) {
            button_state |= 0x10;
        }
        if (gpe->btn_released & ((uint64_t) 1 << gpc.button_fire_L)) {
            button_state |= 0x4;
        }
        if (gps.scene_id != SCENE_FLIGHT && (gpe->btn_released & (1 << gpc.button_menu_select))) {
            button_state |= 0x4;
        }
        if (gpe->btn_released & ((uint64_t) 1 << gpc.button_spell)) {
            setPress(false, GP_KEY_EMU_SPELL);
        }
        if (gpe->btn_released & ((uint64_t) 1 << gpc.button_minimap)) {
            setPress(false, GP_KEY_EMU_MINIMAP);
        }
        if (gpe->btn_released & ((uint64_t) 1 << gpc.button_pause_menu)) {
            setPress(false, GP_KEY_EMU_PAUSE);
        }
        if (gpe->btn_released & ((uint64_t) 1 << gpc.button_fwd)) {
            setPress(false, GP_KEY_EMU_UP);
        }
        if (gpe->btn_released & ((uint64_t) 1 << gpc.button_back)) {
            setPress(false, GP_KEY_EMU_DOWN);
        }
        if (gpe->btn_released & ((uint64_t) 1 << gpc.button_esc)) {
            setPress(false, GP_KEY_EMU_ESC);
        }
        if (gps.scene_id == SCENE_DEAD) {
            setPress(false, GP_KEY_EMU_SPACE);
        }
    }

    if (gpe->btn_pressed || gpe->btn_released) {
        Logger->trace("joy pressed {}  released {}  mouse {}", gpe->btn_pressed, gpe->btn_released,
                      button_state);
    }

    if (button_state) {
        goto announce;
    }

    if (conv_state) {
        gps.dead_zone_announced = 0;
    } else {
        if (gps.dead_zone_announced) {
            // do NOT flood MouseEvents() on frames where the joystick is resting
            return;
        } else {
            gps.dead_zone_announced = 1;
        }
    }

 announce:

    MouseEvents(button_state & 0x7f, gps.x, gps.y);

    //Logger->info("gpc.axis_dead_zone not big enough fly ({},{}) nav ({},{}) conv_state {}", ge->axis_yaw, ge->axis_pitch, ge->axis_nav_ns, ge->axis_nav_ew, conv_state);
}

/// \brief once per frame read out all axes and hats, for perfect smoothness
/// \param gpe event data
void gamepad_poll_data(gamepad_event_t *gpe)
{

    if (!gps.initialized) {
        return;
    }

    // make sure to poll for the axis data on every single frame
    // otherwise we get janky movement since there is no event
    // if the joystick is held still outside the rest position

    if (gpc.axis_yaw_conf & GAMEPAD_ITEM_ENABLED) {
        gpe->axis_yaw = SDL_JoystickGetAxis(m_gameController, gpc.axis_yaw);
    }

    if (gpc.axis_pitch_conf & GAMEPAD_ITEM_ENABLED) {
        gpe->axis_pitch = SDL_JoystickGetAxis(m_gameController, gpc.axis_pitch);
    }

    if (gpc.axis_long_conf & GAMEPAD_ITEM_ENABLED) {
        gpe->axis_long = SDL_JoystickGetAxis(m_gameController, gpc.axis_long);
    }

    if (gpc.axis_trans_conf & GAMEPAD_ITEM_ENABLED) {
        gpe->axis_trans = SDL_JoystickGetAxis(m_gameController, gpc.axis_trans);
    }

    if (gpc.axis_nav_ns_conf & GAMEPAD_ITEM_ENABLED) {
        gpe->axis_nav_ns = SDL_JoystickGetAxis(m_gameController, gpc.axis_nav_ns);
    }

    if (gpc.axis_nav_ew_conf & GAMEPAD_ITEM_ENABLED) {
        gpe->axis_nav_ew = SDL_JoystickGetAxis(m_gameController, gpc.axis_nav_ew);
    }

    if (gpc.hat_nav_conf & GAMEPAD_ITEM_ENABLED) {
        gpe->hat_nav = SDL_JoystickGetHat(m_gameController, gpc.hat_nav);
    }

    if (gpc.hat_mov_conf & GAMEPAD_ITEM_ENABLED) {
        gpe->hat_mov = SDL_JoystickGetHat(m_gameController, gpc.hat_mov);
    }

    if (gpc.axis_fire_R_conf & GAMEPAD_ITEM_ENABLED) {
        gpe->axis_fire_R = SDL_JoystickGetAxis(m_gameController, gpc.axis_fire_R);
    }

    if (gpc.axis_fire_L_conf & GAMEPAD_ITEM_ENABLED) {
        gpe->axis_fire_L = SDL_JoystickGetAxis(m_gameController, gpc.axis_fire_L);
    }

    // call the event handler only once per frame not inside the SDL_PollEvent() loop
    // https://stackoverflow.com/questions/39376356/non-instantaneous-jerky-movement-using-sdl2-opengl
    gamepad_event_mgr(gpe);
}

void calc_inflection(void)
{
    gps.ip_xi_neg = (-32767 * gpc.inflection_x + 50) / 100;
    gps.ip_xi_pos = (32768 * gpc.inflection_x + 50) / 100;
    gps.ip_xo_neg = (gps.rest_x * gpc.inflection_y + 50) / 100;
    gps.ip_xo_pos = gps.max_x - ((gps.max_x - gps.rest_x) * gpc.inflection_y + 50) / 100;

    gps.ip_yi_neg = gps.ip_xi_neg;
    gps.ip_yi_pos = gps.ip_xi_pos;
    gps.ip_yo_neg = (gps.rest_y * gpc.inflection_y + 50) / 100;
    gps.ip_yo_pos = gps.max_y - ((gps.max_y - gps.rest_y) * gpc.inflection_y + 50) / 100;
}

/// \brief reconfigure gamepad maximum coverage and operating mode based on recode scene
/// \param scene_id one of SCENE_PREAMBLE_MENU, SCENE_FLIGHT, SCENE_FLIGHT_MENU
void set_scene(const uint8_t scene_id)
{
    gps.scene_id = scene_id;
    switch (scene_id) {
    case SCENE_PREAMBLE_MENU:
        gps.max_x = 640;
        gps.max_y = 480;
        gps.nav_mode = 1;
        break;
    case SCENE_FLIGHT:
        gps.max_x = gameResWidth;
        gps.max_y = gameResHeight;
        gps.nav_mode = 0;
#ifdef SOUND_OPENAL
        alsound_enable_scheduling();
#endif
        break;
    case SCENE_FLIGHT_MENU:
    case SCENE_SPELL_MENU:
        gps.max_x = gameResWidth;
        gps.max_y = gameResHeight;
        gps.nav_mode = 1;
        break;
    default:
        gps.max_x = gameResWidth;
        gps.max_y = gameResHeight;
        break;
    }
    Logger->trace("set scene {}, nav_mode {}", scene_id, gps.nav_mode);
    if (gpc.inflection_x && gpc.inflection_y) {
        calc_inflection();
    }
}

/// \brief set the x,y simulated mouse pointer coordinates of the joystick rest position
/// \param x coordinate
/// \param y coordinate
void joystick_set_env(const int32_t x, const int32_t y)
{
    //Logger->info("pointer rest at {},{} scene {}, window size {},{}", x, y, gps.scene_id, gps.max_x, gps.max_y);
    gps.rest_x = x;
    gps.rest_y = y;
    gps.x = x;
    gps.y = y;
    if (gpc.inflection_x && gpc.inflection_y) {
        calc_inflection();
    }
}

/// \brief unfinished load effects to be sent to the haptic subsystem
/// \return EXIT_FAILURE on error, EXIT_SUCCESS otherwise
int8_t haptic_load_effects(void)
{
    SDL_HapticEffect effect;
    int16_t max_effects = 0;

    if ((hs.cap & SDL_HAPTIC_SINE) == 0) {
        Logger->info("haptic sine not supported");
        return EXIT_FAILURE;
    }

    max_effects = SDL_HapticNumEffects(m_haptic);
    if (max_effects < 0) {
        Logger->info("SDL_HapticNumEffects() error {}", SDL_GetError());
    }

    // meteor effect
    SDL_memset(&effect, 0, sizeof(SDL_HapticEffect));
    effect.type = SDL_HAPTIC_SINE;
    effect.periodic.direction.type = SDL_HAPTIC_POLAR; // polar coordinates
    effect.periodic.direction.dir[0] = rand() % 36000; // force direction
    effect.periodic.period = 1000;      // time in ms
    effect.periodic.magnitude = 32000;  // out of 32767 strength
    effect.periodic.length = 300;       // time in ms
    effect.periodic.attack_length = 100;// interval until max strength
    effect.periodic.fade_length = 100;  // fade away interval
    hs.effect[GP_HAPTIC_METEOR] = SDL_HapticNewEffect(m_haptic, &effect);

    // quake effect
    SDL_memset(&effect, 0, sizeof(SDL_HapticEffect));
    effect.type = SDL_HAPTIC_SINE;
    effect.periodic.direction.type = SDL_HAPTIC_POLAR;  // polar coordinates
    effect.periodic.direction.dir[0] = rand() % 36000;  // force direction
    effect.periodic.period = 1000;       // time in ms
    effect.periodic.magnitude = 5000;    // carpet is floating, so suspension is great
    effect.periodic.length = 3000;       // time in ms
    effect.periodic.attack_length = 1000;// interval until max strength
    effect.periodic.fade_length = 1000;  // fade away interval
    hs.effect[GP_HAPTIC_QUAKE] = SDL_HapticNewEffect(m_haptic, &effect);

    // tornado effect
    SDL_memset(&effect, 0, sizeof(SDL_HapticEffect));   // 0 is safe default
    effect.type = SDL_HAPTIC_SINE;
    effect.periodic.direction.type = SDL_HAPTIC_POLAR;  // polar coordinates
    effect.periodic.direction.dir[0] = rand() % 36000;  // force direction
    effect.periodic.period = 1000;       // time in ms
    effect.periodic.magnitude = 20000;   // out of 32767 strength
    effect.periodic.length = 3200;       // time in ms
    effect.periodic.attack_length = 2000;// interval until max strength
    effect.periodic.fade_length = 1000;  // fade away interval
    hs.effect[GP_HAPTIC_TORNADO] = SDL_HapticNewEffect(m_haptic, &effect);

    return EXIT_SUCCESS;
}

/// \brief send sine-based effect to the haptic subsystem
/// \param effect_id  identifier
void haptic_run_effect(const int effect_id)
{
    if (!hs.enabled || ((hs.cap & SDL_HAPTIC_SINE) == 0)) {
        return;
    }
    //Logger->info("run_effect {}", effect_id);
    SDL_HapticRunEffect(m_haptic, effect_id, 1);
}

/// \brief send rumble effect to the haptic subsystem
/// \param strength defined in the [0-1.0] interval
/// \param length  effect duration in ms
void haptic_rumble_effect(const float strength, const uint32_t length)
{
    if ((!hs.enabled) || (!hs.rumble)) {
        return;
    }
    Logger->info("run_rumble {},{}", strength, length);
    SDL_HapticRumblePlay(m_haptic, strength, length);
}

/// \brief send rumble effect to the trigger buttons
/// \param strength_l TL effect level
/// \param strength_r TR effect level
/// \param length effect duration in ms
void haptic_rumble_triggers_effect(const uint16_t strength_l, const uint16_t strength_r,
                                   const uint32_t length)
{
    if ((!hs.enabled) || (!hs.rumble_trig)) {
        return;
    }
    SDL_JoystickRumbleTriggers(m_gameController, strength_l, strength_r, length);
}

