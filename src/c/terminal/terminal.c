#include "terminal.h"
#include "../common/common.h"
#include "../vga/vga.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static size_t terminal_viewport_top;
static size_t terminal_row;
static size_t terminal_last_row;
static size_t terminal_col;
static uint8_t terminal_color;
static uint16_t *terminal_viewport_buffer;
static uint16_t terminal_buffer[VGA_WIDTH * RECENT_ROWS];
static char terminal_input_buffer[TERMINAL_INPUT_BUFFER_SIZE] = {'\0'};
static size_t input_len;

static bool password_mode;
static bool input_mode;

static size_t get_row_written_length(size_t y) {
  for (int x = (int)VGA_WIDTH - 1; x >= 0; x--) {
    size_t index = y * VGA_WIDTH + (size_t)x;
    char c = (char)(terminal_buffer[index] & 0xFF);
    if (c != '\0') {
      return (size_t)(x + 1);
    }
  }
  return 0;
}

static void shift_history_up(void) {
  for (size_t y = 1; y < RECENT_ROWS; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      terminal_buffer[(y - 1) * VGA_WIDTH + x] =
          terminal_buffer[y * VGA_WIDTH + x];
    }
  }

  for (size_t x = 0; x < VGA_WIDTH; x++) {
    terminal_buffer[(RECENT_ROWS - 1) * VGA_WIDTH + x] =
        vga_entry('\0', terminal_color);
  }
}

static void terminal_refresh_screen(void) {
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      size_t history_index = (terminal_viewport_top + y) * VGA_WIDTH + x;
      size_t screen_index = y * VGA_WIDTH + x;
      terminal_viewport_buffer[screen_index] = terminal_buffer[history_index];
    }
  }
}

static void increase_row(void) {
  terminal_row++;

  if (terminal_row > terminal_last_row) {
    terminal_last_row = terminal_row;
  }

  if (terminal_row >= VGA_HEIGHT + terminal_viewport_top) {
    terminal_viewport_top = terminal_row - (VGA_HEIGHT - 1);
  }

  if (terminal_row >= RECENT_ROWS) {
    shift_history_up();
    terminal_row = RECENT_ROWS - 1;
    terminal_last_row = RECENT_ROWS - 1;
    terminal_viewport_top = RECENT_ROWS - VGA_HEIGHT;
  }
}

static void increase_col(void) {
  if (++terminal_col >= VGA_WIDTH) {
    increase_row();
    terminal_col = 0;
  }
}

static void decrease_row(void) {
  if (terminal_row > 0) {
    terminal_row--;

    if (terminal_row < terminal_viewport_top) {
      terminal_viewport_top = terminal_row;
    }
  }
}

static void decrease_col(void) {
  if (terminal_col > 0) {
    terminal_col--;
  } else {
    if (terminal_row > 0) {
      decrease_row();
      terminal_col = get_row_written_length(terminal_row);
    }
  }
}

static bool is_escape_sequence(char c) {
  return c == '\n' || c == '\r' || c == '\t' || c == '\b';
}

static void terminal_flush(void) {
  terminal_refresh_screen();
  vga_set_cursor(terminal_col, terminal_row - terminal_viewport_top);
}

static void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * VGA_WIDTH + x;
  terminal_buffer[index] = vga_entry(c, color);
}

static void jump_bottom(void) {
  terminal_row = terminal_last_row;
  terminal_col = get_row_written_length(terminal_row);
}

static void terminal_putchar_raw(char c) {
  if (!is_escape_sequence(c)) {
    terminal_putentryat(c, terminal_color, terminal_col, terminal_row);
    increase_col();
  } else if (c == '\n') {
    increase_row();
    terminal_col = 0;
  }
}

static void terminal_write(const char *data, size_t size) {
  for (size_t i = 0; i < size; i++) {
    terminal_putchar_raw(data[i]);
  }
}

static void reset_input(void) {
  kmemset((void *)terminal_input_buffer, 0, sizeof(terminal_input_buffer));
  input_len = 0;
}

static void terminal_putchar(char c) {
  terminal_putchar_raw(c);
  terminal_flush();
}

static void terminal_writestring(const char *data) {
  terminal_write(data, kstrlen(data));

  terminal_flush();
}

static void print_terminal_message() {
  const char *terminal_msg =
      "____  _____ _   _  ___  ____\n"
      "| __ )| ____| \\ | |/ _ \\/ ___|\n"
      "|  _ \\|  _| |  \\| | | | \\___ \\\n"
      "| |_) | |___| |\\  | |_| |___) |\n"
      "|____/|_____|_| \\_|\\___/|____/\n"
      "\n"
      "          B E N O S   O P E R A T I N G   S Y S T E M\n"
      "\n"
      "----------------------------------------------------------\n"
      " Welcome to BENOS\n"
      " A lightweight operating environment\n"
      "\n"
      " Kernel      : BENOS Core\n"
      " Architecture: x86_64\n"
      " Shell       : BENSH\n"
      " Status      : SYSTEM READY\n"
      "\n"
      "----------------------------------------------------------\n"
      "\n"
      "        \"Small system. Big ideas.\"\n"
      "\n"
      " Type 'help' to see available commands.\n"
      "\n";

  terminal_writestring(terminal_msg);
}

static void terminal_initialize(void) {
  terminal_viewport_buffer = (uint16_t *)VGA_MEMORY;
  terminal_viewport_top = 0;
  terminal_row = 0;
  terminal_last_row = 0;
  terminal_col = 0;
  input_len = 0;
  input_mode = false;
  terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

  for (size_t y = 0; y < RECENT_ROWS; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      terminal_buffer[y * VGA_WIDTH + x] = vga_entry('\0', terminal_color);
    }
  }

  terminal_refresh_screen();
  vga_enable_cursor(14, 15);
  terminal_flush();

  password_mode = false;
}

// --- PUBLIC TERMINAL INTERFACE ---

void terminal_setcolor(uint8_t color) { terminal_color = color; }

void terminal_arrow_handle(char d) {
  size_t line_limit = 0;

  switch (d) {
  case 'l':
    if (terminal_col == 0 && terminal_row > 0) {
      decrease_row();
      terminal_col = get_row_written_length(terminal_row);
    } else if (terminal_col > 0) {
      line_limit = get_row_written_length(terminal_row);
      if (terminal_col > line_limit) {
        terminal_col = line_limit;
      } else {
        terminal_col--;
      }
    }
    break;

  case 'r':
    line_limit = get_row_written_length(terminal_row);
    if (terminal_col < line_limit) {
      terminal_col++;
    } else if (terminal_col == line_limit && terminal_row + 1 < RECENT_ROWS) {
      if (get_row_written_length(terminal_row + 1) > 0) {
        increase_row();
        terminal_col = 0;
      }
    }
    break;

  case 'u':
    if (terminal_row > 0) {
      decrease_row();
      line_limit = get_row_written_length(terminal_row);
      if (line_limit == 0) {
        terminal_col = 0;
      } else if (terminal_col > line_limit) {
        terminal_col = line_limit;
      }
    }
    break;

  case 'd':
    if (terminal_row < terminal_last_row) {
      increase_row();

      line_limit = get_row_written_length(terminal_row);

      if (terminal_col > line_limit) {
        terminal_col = line_limit;
      }
    } else {
      terminal_col = get_row_written_length(terminal_row);
    }
    break;
  }

  terminal_flush();
}

void terminal_delete_last(void) {
  jump_bottom();
  if (terminal_row == 0 && terminal_col == 0)
    return;
  if (input_len == 0)
    return;

  decrease_col();

  terminal_putentryat('\0', terminal_color, terminal_col, terminal_row);
  terminal_input_buffer[--input_len] = '\0';
  terminal_flush();
}

void terminal_add_input(char c) {
  jump_bottom();
  if (input_len < sizeof(terminal_input_buffer) - 1 && c != '\n') {
    terminal_input_buffer[input_len++] = c;
  }

  if (!password_mode) {
    terminal_putchar(c);
  }
}

void end_input(void) {
  input_mode = false;
  jump_bottom();
  if (!password_mode) {
    terminal_writestring("\n");
    terminal_writestring(terminal_input_buffer);
    terminal_writestring("\n");
  }

  reset_input();
}

void terminal_log_hex(uint32_t hex) {
  char hex_digits[] = "0123456789abcdef";
  char buffer[9];
  uint32_t temp = hex;
  buffer[8] = '\0';

  terminal_writestring("0x");
  for (int i = 7; i >= 0; --i) {
    buffer[i] = hex_digits[temp & 0x0F];
    temp >>= 4;
  }

  terminal_writestring(buffer);
  terminal_putchar('\n');
}

void terminal_log_number(uint32_t number) {
  if (number == 0) {
    terminal_writestring("0\n");
    return;
  }

  uint32_t temp = number;
  while (temp > 0) {
    terminal_putchar(temp % 10);
    temp /= 10;
  }
  terminal_putchar('\n');
}

static char *itoa(int value, char *buffer) {
  char *ptr = buffer;
  char *ptr1 = buffer;
  char tmp_char;
  int tmp_value;

  // Handle negative numbers (only for base 10)
  int is_negative = 0;
  if (value < 0) {
    is_negative = 1;
    value = -value;
  }

  // Extract characters from right to left (modulus loop)
  do {
    tmp_value = value;
    value /= 10;
    *ptr++ = "0123456789"[tmp_value - value * 10];
  } while (value);

  // Append the negative sign if necessary
  if (is_negative) {
    *ptr++ = '-';
  }

  // Terminate the C-string with a null character
  *ptr = '\0';

  // The digits are currently backwards (e.g., 123 is stored as "321").
  // We must reverse the string in place to fix it.
  ptr--;
  while (ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr = *ptr1;
    *ptr1 = tmp_char;
    ptr1++;
    ptr--;
  }

  return buffer;
}

void terminal_log(const char *format_str, ...) {
  terminal_writestring("\nLOG::");
  char output_buffer[1024]; // Safe internal scratchpad memory
  size_t out_idx = 0;
  size_t max_len = sizeof(output_buffer) - 1; // Reserve room for '\0'

  // Initialize the variadic argument list pointer
  va_list args;
  va_start(args, format_str);

  for (size_t i = 0; format_str[i] != '\0' && out_idx < max_len; i++) {
    // If we encounter a lone '%' symbol, swap it with the next string argument
    if (format_str[i] == '%') {
      // Extract the next sequential string argument
      char *current_arg;
      if (format_str[i + 1] == 's') {
        current_arg = (char *)va_arg(args, const char *);
        ;
      } else if (format_str[i + 1] == 'n') {
        current_arg = itoa(va_arg(args, int), current_arg);
      } else {
        current_arg = "%";
      }

      // Safe fallback if a NULL pointer is accidentally passed
      if (current_arg == NULL) {
        current_arg = "(null)";
      }

      // Manually copy the argument characters into the temporary buffer
      for (size_t j = 0; current_arg[j] != '\0' && out_idx < max_len; j++) {
        output_buffer[out_idx++] = current_arg[j];
      }
      if (format_str[i + 1] == 'n' || format_str[i + 1] == 's')
        i++;
    } else {
      // Copy normal characters directly
      output_buffer[out_idx++] = format_str[i];
    }
  }

  // Clean up argument list state from the CPU stack
  va_end(args);

  // Explicitly set the null-terminator to seal the formatted C-string
  output_buffer[out_idx] = '\0';

  // Send the completed, formatted string to your system's write function
  terminal_writestring(output_buffer);
  terminal_writestring("\n");
}

void terminal_start(void) {
  terminal_initialize();

  print_terminal_message();
}
