#include "keyboard.h"
#include "../../terminal/terminal.h"
#include <stdbool.h>
#include <stdint.h>

#define KEYBOARD_BUFFER_SIZE 256

static KeyboardEvent keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static uint32_t buffer_head = 0;
static uint32_t buffer_tail = 0;

static bool shift = false;
static bool caps_lock = false;
static bool ctrl = false;
static bool alt = false;
static bool released = false;

static keyboard_callback_t activate_callback = NULL;

static const char shift_nums[10] = {
    ')',
    '!',
    '@',
    '#',
    '$',
    '%',
    '^',
    '&',
    '*',
    '('
};

static bool is_printable(KeyboardEvent ev) {
  return (ev.code >= KEY_A && ev.code <= KEY_Z) ||             /* Letters */
         (ev.code >= KEY_0 && ev.code <= KEY_9) ||             /* Numbers */
         (ev.code >= KEY_GRAVE && ev.code <= KEY_SEMICOLON) || /* Symbols */
         ev.code == KEY_SPACE || (ev.code == KEY_ENTER && ev.shift);
}

static void buffer_push_event(KeyboardEvent ev) {
  uint32_t next_head = (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;

  if (next_head == buffer_tail) {
    return;
  }

  keyboard_buffer[next_head] = ev;
  buffer_head = next_head;
}

static bool is_arrow(KEYBOARD_CODE code) {
  return code == KEY_UP || code == KEY_DOWN || code == KEY_LEFT ||
         code == KEY_RIGHT;
}

static void update_shift(uint8_t sc) {
  switch (sc) {
  case 0x2A:
  case 0x36:
    shift = true;
    break;

  case 0xAA:
  case 0xB6:
    shift = false;
    break;
  }
}

static void handle_arrow(KEYBOARD_CODE code) {
  switch (code) {
  case KEY_UP:
    terminal_arrow_handle('u');
    break;
  case KEY_DOWN:
    terminal_arrow_handle('d');
    break;
  case KEY_LEFT:
    terminal_arrow_handle('l');
    break;
  case KEY_RIGHT:
    terminal_arrow_handle('r');
    break;
  default:
    break;
  }
}

static KEYBOARD_CODE keyboard_translate_scancode(uint8_t scancode) {
  static bool extended = false;

  // Extended key prefix
  if (scancode == 0xE0) {
    extended = true;
    return KEY_NONE;
  }

  // Remove release bit
  released = scancode & 0x80;
  scancode &= 0x7F;

  KEYBOARD_CODE key = KEY_NONE;

  if (!extended) {
    switch (scancode) {
    /* Letters */
    case 0x1E:
      key = KEY_A;
      break;
    case 0x30:
      key = KEY_B;
      break;
    case 0x2E:
      key = KEY_C;
      break;
    case 0x20:
      key = KEY_D;
      break;
    case 0x12:
      key = KEY_E;
      break;
    case 0x21:
      key = KEY_F;
      break;
    case 0x22:
      key = KEY_G;
      break;
    case 0x23:
      key = KEY_H;
      break;
    case 0x17:
      key = KEY_I;
      break;
    case 0x24:
      key = KEY_J;
      break;
    case 0x25:
      key = KEY_K;
      break;
    case 0x26:
      key = KEY_L;
      break;
    case 0x32:
      key = KEY_M;
      break;
    case 0x31:
      key = KEY_N;
      break;
    case 0x18:
      key = KEY_O;
      break;
    case 0x19:
      key = KEY_P;
      break;
    case 0x10:
      key = KEY_Q;
      break;
    case 0x13:
      key = KEY_R;
      break;
    case 0x1F:
      key = KEY_S;
      break;
    case 0x14:
      key = KEY_T;
      break;
    case 0x16:
      key = KEY_U;
      break;
    case 0x2F:
      key = KEY_V;
      break;
    case 0x11:
      key = KEY_W;
      break;
    case 0x2D:
      key = KEY_X;
      break;
    case 0x15:
      key = KEY_Y;
      break;
    case 0x2C:
      key = KEY_Z;
      break;

    /* Numbers */
    case 0x0B:
      key = KEY_0;
      break;
    case 0x02:
      key = KEY_1;
      break;
    case 0x03:
      key = KEY_2;
      break;
    case 0x04:
      key = KEY_3;
      break;
    case 0x05:
      key = KEY_4;
      break;
    case 0x06:
      key = KEY_5;
      break;
    case 0x07:
      key = KEY_6;
      break;
    case 0x08:
      key = KEY_7;
      break;
    case 0x09:
      key = KEY_8;
      break;
    case 0x0A:
      key = KEY_9;
      break;

    /* Symbols */
    case 0x29:
      key = KEY_GRAVE;
      break;
    case 0x0C:
      key = KEY_MINUS;
      break;
    case 0x0D:
      key = KEY_EQUALS;
      break;
    case 0x1A:
      key = KEY_LEFT_BRACKET;
      break;
    case 0x1B:
      key = KEY_RIGHT_BRACKET;
      break;
    case 0x2B:
      key = KEY_BACKSLASH;
      break;
    case 0x27:
      key = KEY_SEMICOLON;
      break;
    case 0x28:
      key = KEY_APOSTROPHE;
      break;
    case 0x33:
      key = KEY_COMMA;
      break;
    case 0x34:
      key = KEY_PERIOD;
      break;
    case 0x35:
      key = KEY_SLASH;
      break;

    /* Controls */
    case 0x01:
      key = KEY_ESCAPE;
      break;
    case 0x39:
      key = KEY_SPACE;
      break;
    case 0x0F:
      key = KEY_TAB;
      break;
    case 0x1C:
      key = KEY_ENTER;
      break;
    case 0x0E:
      key = KEY_BACKSPACE;
      break;

    /* Locks */
    case 0x3A:
      key = KEY_CAPS_LOCK;
      break;
    case 0x45:
      key = KEY_NUM_LOCK;
      break;
    case 0x46:
      key = KEY_SCROLL_LOCK;
      break;

    /* Modifiers */
    case 0x2A:
      key = KEY_LEFT_SHIFT;
      break;
    case 0x36:
      key = KEY_RIGHT_SHIFT;
      break;
    case 0x1D:
      key = KEY_LEFT_CTRL;
      break;
    case 0x38:
      key = KEY_LEFT_ALT;
      break;

    /* Function keys */
    case 0x3B:
      key = KEY_F1;
      break;
    case 0x3C:
      key = KEY_F2;
      break;
    case 0x3D:
      key = KEY_F3;
      break;
    case 0x3E:
      key = KEY_F4;
      break;
    case 0x3F:
      key = KEY_F5;
      break;
    case 0x40:
      key = KEY_F6;
      break;
    case 0x41:
      key = KEY_F7;
      break;
    case 0x42:
      key = KEY_F8;
      break;
    case 0x43:
      key = KEY_F9;
      break;
    case 0x44:
      key = KEY_F10;
      break;
    case 0x57:
      key = KEY_F11;
      break;
    case 0x58:
      key = KEY_F12;
      break;

    /* Keypad */
    case 0x52:
      key = KEY_KP_0;
      break;
    case 0x4F:
      key = KEY_KP_1;
      break;
    case 0x50:
      key = KEY_KP_2;
      break;
    case 0x51:
      key = KEY_KP_3;
      break;
    case 0x4B:
      key = KEY_KP_4;
      break;
    case 0x4C:
      key = KEY_KP_5;
      break;
    case 0x4D:
      key = KEY_KP_6;
      break;
    case 0x47:
      key = KEY_KP_7;
      break;
    case 0x48:
      key = KEY_KP_8;
      break;
    case 0x49:
      key = KEY_KP_9;
      break;
    case 0x53:
      key = KEY_KP_DECIMAL;
      break;
    case 0x37:
      key = KEY_KP_MULTIPLY;
      break;
    case 0x4A:
      key = KEY_KP_SUBTRACT;
      break;
    case 0x4E:
      key = KEY_KP_ADD;
      break;

    default:
      break;
    }
  } else {
    switch (scancode) {
    case 0x48:
      key = KEY_UP;
      break;
    case 0x50:
      key = KEY_DOWN;
      break;
    case 0x4B:
      key = KEY_LEFT;
      break;
    case 0x4D:
      key = KEY_RIGHT;
      break;

    case 0x47:
      key = KEY_HOME;
      break;
    case 0x4F:
      key = KEY_END;
      break;
    case 0x49:
      key = KEY_PAGE_UP;
      break;
    case 0x51:
      key = KEY_PAGE_DOWN;
      break;
    case 0x52:
      key = KEY_INSERT;
      break;
    case 0x53:
      key = KEY_DELETE;
      break;

    case 0x1C:
      key = KEY_KP_ENTER;
      break;
    case 0x35:
      key = KEY_KP_DIVIDE;
      break;

    case 0x1D:
      key = KEY_RIGHT_CTRL;
      break;
    case 0x38:
      key = KEY_RIGHT_ALT;
      break;

    case 0x5B:
      key = KEY_LEFT_GUI;
      break;
    case 0x5C:
      key = KEY_RIGHT_GUI;
      break;
    case 0x5D:
      key = KEY_MENU;
      break;

    default:
      break;
    }

    extended = false;
  }

  return key;
}

/**
INTERFACE CONTRACT FUNCTIONS
*/
static bool keyboard_init(void) {
  buffer_head = 0;
  buffer_tail = 0;

  return true;
}

static void keyboard_handle_interrupt(void) {
  uint8_t scancode = inb(KEYBOARD_PORT);

  KeyboardEvent ev = {0};
  ev.code = keyboard_translate_scancode(scancode);
  ev.press = !released;


  if (ev.code == KEY_LEFT_SHIFT || ev.code == KEY_RIGHT_SHIFT) {
    shift = ev.press;
  } else if (ev.code == KEY_LEFT_CTRL || ev.code == KEY_RIGHT_CTRL) {
    ctrl = ev.press;
  } else if (ev.code == KEY_LEFT_ALT || ev.code == KEY_RIGHT_ALT) {
    alt = ev.press;
  } else if (ev.code == KEY_CAPS_LOCK && ev.press) {
    caps_lock = !caps_lock;
  }

  ev.shift = shift;
  ev.ctrl = ctrl;
  ev.alt = alt;
  ev.caps_lock = caps_lock;

  if (ev.code != KEY_NONE) {
    buffer_push_event(ev);

    if (activate_callback != NULL) {
      activate_callback(ev);
    }
  }
}

driver_t keyboard_driver = {.name = "PS/2 Keyboard Driver",
                            .type = DRIVER_KEYBOARD,
                            .init = keyboard_init,
                            .handle_interrupt = keyboard_handle_interrupt,
                            .disable = NULL};

void keyboard_set_callback(keyboard_callback_t cb) {
  activate_callback = cb;
}

bool keyboard_has_event(void) {
  return buffer_head != buffer_tail;
}

KeyboardEvent keyboard_get_event(void) {
  if (!keyboard_has_event()) {
    KeyboardEvent empty = {.code = KEY_NONE};
    return empty;
  }

  KeyboardEvent ev = keyboard_buffer[buffer_tail];
  buffer_tail = (buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
  return ev;
}

char keyboard_translate(KeyboardEvent ev) {
  if (!is_printable(ev) || ev.code == KEY_NONE)
    return '\0';

  if (ev.code == KEY_SPACE)
    return ' ';
  if (ev.code == KEY_ENTER)
    return '\n';
  if (ev.code == KEY_TAB)
    return '\t';
  if (ev.code == KEY_BACKSPACE)
    return '\b';

  KEYBOARD_CODE code = ev.code;
  bool upper = ev.shift ^ ev.caps_lock; // XOR: either shift or caps, not both
  /* Letters */
  if (code >= KEY_A && code <= KEY_Z) {
    char c = 'a' + (code - KEY_A);
    return upper ? c - 'a' + 'A' : c;
  }

  /* Top-row numbers */
  if (code >= KEY_0 && code <= KEY_9) {
    char c = '0' + (code - KEY_0);
    return ev.shift ? shift_nums[code - KEY_0] : c;
  }

  switch (code) {
  /* Symbols (unshifted / shifted) */
  case KEY_GRAVE:
    return ev.shift ? '~' : '`';
  case KEY_MINUS:
    return ev.shift ? '_' : '-';
  case KEY_EQUALS:
    return ev.shift ? '+' : '=';
  case KEY_LEFT_BRACKET:
    return ev.shift ? '{' : '[';
  case KEY_RIGHT_BRACKET:
    return ev.shift ? '}' : ']';
  case KEY_BACKSLASH:
    return ev.shift ? '|' : '\\';
  case KEY_SEMICOLON:
    return ev.shift ? ':' : ';';
  case KEY_APOSTROPHE:
    return ev.shift ? '"' : '\'';
  case KEY_COMMA:
    return ev.shift ? '<' : ',';
  case KEY_PERIOD:
    return ev.shift ? '>' : '.';
  case KEY_SLASH:
    return ev.shift ? '?' : '/';

  default:
    return '\0'; /* not printable */
  }
}
