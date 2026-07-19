#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../driver/driver.h"
#include "../../common/common.h"
#include <stdbool.h>
#include <stdint.h>

extern driver_t keyboard_driver;

typedef enum {
    KEY_NONE = 0,

    /* Letters */
    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,

    /* Numbers */
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,

    /* Symbols */
    KEY_GRAVE,          // `
    KEY_MINUS,          // -
    KEY_EQUALS,         // =
    KEY_LEFT_BRACKET,   // [
    KEY_RIGHT_BRACKET,  // ]
    KEY_BACKSLASH,      /* \ */
    KEY_APOSTROPHE,     // '
    KEY_COMMA,          // ,
    KEY_PERIOD,         // .
    KEY_SLASH,          // /
    KEY_SEMICOLON,      // ;


    /* Whitespace */
    KEY_SPACE,
    KEY_TAB,
    KEY_ENTER,
    KEY_BACKSPACE,

    /* Locks */
    KEY_CAPS_LOCK,
    KEY_NUM_LOCK,
    KEY_SCROLL_LOCK,

    /* Modifiers */
    KEY_LEFT_SHIFT,
    KEY_RIGHT_SHIFT,
    KEY_LEFT_CTRL,
    KEY_RIGHT_CTRL,
    KEY_LEFT_ALT,
    KEY_RIGHT_ALT,
    KEY_LEFT_GUI,        // Windows / Command
    KEY_RIGHT_GUI,
    KEY_MENU,

    /* Navigation */
    KEY_INSERT,
    KEY_DELETE,
    KEY_HOME,
    KEY_END,
    KEY_PAGE_UP,
    KEY_PAGE_DOWN,

    /* Arrows */
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,

    /* Function keys */
    KEY_ESCAPE,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    KEY_F13,
    KEY_F14,
    KEY_F15,
    KEY_F16,
    KEY_F17,
    KEY_F18,
    KEY_F19,
    KEY_F20,
    KEY_F21,
    KEY_F22,
    KEY_F23,
    KEY_F24,

    /* Print/System */
    KEY_PRINT_SCREEN,
    KEY_PAUSE,

    /* Keypad */
    KEY_KP_0,
    KEY_KP_1,
    KEY_KP_2,
    KEY_KP_3,
    KEY_KP_4,
    KEY_KP_5,
    KEY_KP_6,
    KEY_KP_7,
    KEY_KP_8,
    KEY_KP_9,

    KEY_KP_DECIMAL,
    KEY_KP_DIVIDE,
    KEY_KP_MULTIPLY,
    KEY_KP_SUBTRACT,
    KEY_KP_ADD,
    KEY_KP_ENTER,
    KEY_KP_EQUALS,

    /* Multimedia */
    KEY_VOLUME_UP,
    KEY_VOLUME_DOWN,
    KEY_VOLUME_MUTE,

    KEY_MEDIA_NEXT,
    KEY_MEDIA_PREVIOUS,
    KEY_MEDIA_STOP,
    KEY_MEDIA_PLAY_PAUSE,

    /* Browser */
    KEY_BROWSER_BACK,
    KEY_BROWSER_FORWARD,
    KEY_BROWSER_REFRESH,
    KEY_BROWSER_STOP,
    KEY_BROWSER_SEARCH,
    KEY_BROWSER_FAVORITES,
    KEY_BROWSER_HOME,

    /* Applications */
    KEY_CALCULATOR,
    KEY_MAIL,
    KEY_MY_COMPUTER,

    /* Power */
    KEY_POWER,
    KEY_SLEEP,
    KEY_WAKE,

    KEY_COUNT
} KEYBOARD_CODE;

typedef struct {
    KEYBOARD_CODE code;
    bool press;
    bool shift;
    bool ctrl;
    bool alt;
    bool caps_lock;
} KeyboardEvent;
//     ')',
//     '!',
//     '@',
//     '#',
//     '$',
//     '%',
//     '^',
//     '&',
//     '*',
//     '('
// };

typedef void (*keyboard_callback_t)(KeyboardEvent ev);

KeyboardEvent keyboard_input(void);
void keyboard_handle_event(KeyboardEvent ev);

// The public interface for your future shell application to read user input
void keyboard_set_callback(keyboard_callback_t cb);
bool keyboard_has_event(void);
KeyboardEvent keyboard_get_event(void);
char keyboard_translate(KeyboardEvent ev);

#endif
