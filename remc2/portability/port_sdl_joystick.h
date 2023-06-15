#pragma once
#ifndef PORT_SDL_JOYSTICK_H
#define PORT_SDL_JOYSTICK_H

///< possible scenes
#define  SCENE_PREAMBLE_MENU  0x1
#define         SCENE_FLIGHT  0x2
#define    SCENE_FLIGHT_MENU  0x3
#define     SCENE_SPELL_MENU  0x4
#define           SCENE_DEAD  0x5

///< gamepad_event_t flags
#define      GP_BTN_RELEASED  0x40
#define       GP_BTN_PRESSED  0x80

///< haptic effect array index
#define     GP_HAPTIC_METEOR  0x0
#define      GP_HAPTIC_QUAKE  0x1
#define    GP_HAPTIC_TORNADO  0x2
#define GP_HAPTIC_EFFECT_CNT  0x3  ///< update this one to last effect count+1 !

struct gamepad_event {
    int16_t axis_yaw;
    int16_t axis_pitch;
    int16_t axis_long;
    int16_t axis_trans;
    int16_t axis_nav_ns;
    int16_t axis_nav_ew;
    int16_t axis_fire_R;
    int16_t axis_fire_L;
    uint8_t hat_nav;
    uint8_t hat_mov;
    uint64_t btn_released;      ///< bitwise OR of every released button
    uint64_t btn_pressed;       ///< bitwise OR of every pressed button
    uint8_t flag;               ///< bitwise OR of every parameter that was updated
};
typedef struct gamepad_event gamepad_event_t;

void set_scene(const uint8_t scene_id);
void joystick_set_env(const int32_t x, const int32_t y);
void gamepad_sdl_init(void);
void gamepad_sdl_close(void);
void gamepad_init(const int gameResWidth, const int gameResHeight);
void gamepad_poll_data(gamepad_event_t * gpe);

void haptic_run_effect(const int effect_id);
void haptic_rumble_effect(const float strength, const uint32_t length);
void haptic_rumble_triggers_effect(const uint16_t strength_l, const uint16_t strength_r,
                                   const uint32_t length);

void controller_imgui(bool *p_open);

#endif                          //PORT_SDL_JOYSTICK
