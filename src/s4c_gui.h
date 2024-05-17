// jgabaut @ github.com/jgabaut
// SPDX-License-Identifier: GPL-3.0-only
/*
    Copyright (C) 2023-2024  jgabaut

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef S4C_GUI_H_
#define S4C_GUI_H_
#include <stdlib.h>

/**
 * Function name to use in place of malloc.
 */
#ifndef S4C_GUI_MALLOC
#define S4C_GUI_MALLOC malloc
#endif // S4C_GUI_MALLOC

/**
 * Function name to use in place of calloc.
 */
#ifndef S4C_GUI_CALLOC
#define S4C_GUI_CALLOC calloc
#endif // S4C_GUI_CALLOC

#define S4C_GUI_MAJOR 0 /**< Represents current major release.*/
#define S4C_GUI_MINOR 0 /**< Represents current minor release.*/
#define S4C_GUI_PATCH 7 /**< Represents current patch release.*/

/* Defines current API version number from KLS_MAJOR, KLS_MINOR and KLS_PATCH.
 */
static const int S4C_GUI_API_VERSION_INT =
    (S4C_GUI_MAJOR * 1000000 + S4C_GUI_MINOR * 10000 + S4C_GUI_PATCH * 100);
/**< Represents current version with numeric format.*/

/**
 * Defines current API version string.
 */
static const char S4C_GUI_API_VERSION_STRING[] = "0.0.7-dev"; /**< Represents current version with MAJOR.MINOR.PATCH format.*/

/**
 * Returns current s4c_gui version as a string.
 */
const char *string_s4c_gui_version(void);

/**
 * Returns current s4c_gui version as an integer.
 */
int int_s4c_gui_version(void);

typedef void*(s4c_gui_malloc_func)(size_t size); /**< Used to select an allocation function.*/
typedef void*(s4c_gui_calloc_func)(size_t count, size_t size); /**< Used to select a counted allocation function.*/
typedef void(s4c_gui_free_func)(void* obj); /**< Used to select a free function.*/

extern s4c_gui_malloc_func* s4c_gui_inner_malloc;
extern s4c_gui_calloc_func* s4c_gui_inner_calloc;

#ifndef TEXT_FIELD_H_
#define TEXT_FIELD_H_

#ifdef _WIN32
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct TextField_s *TextField;

typedef void(TextField_Full_Handler)(TextField);

typedef bool(TextField_Linter)(TextField, const void*);

#define TEXTFIELD_DEFAULT_LINTERS_TOT 1
extern TextField_Linter* default_linters[TEXTFIELD_DEFAULT_LINTERS_TOT+1];

extern const void* default_linter_args[TEXTFIELD_DEFAULT_LINTERS_TOT+1];

void warn_TextField(TextField txt);
bool lint_TextField_not_empty(TextField txt, const void* unused);
bool lint_TextField_equals_cstr(TextField txt, const void* cstr);
bool lint_TextField_whitelist(TextField txt, const void* whitelist);
bool lint_TextField_digits_only(TextField txt);
bool lint_TextField_chars_only(TextField txt);
TextField new_TextField_(TextField_Full_Handler* full_buffer_handler, TextField_Linter** linters, size_t num_linters, const void** linter_args, size_t max_size, int height, int width, int start_x, int start_y, const char* prompt, s4c_gui_malloc_func* malloc_func, s4c_gui_calloc_func* calloc_func, s4c_gui_free_func* free_func);
TextField new_TextField_centered_(TextField_Full_Handler* full_buffer_handler, TextField_Linter** linters, size_t num_linters, const void** linter_args, size_t max_size, int height, int width, int bound_x, int bound_y, const char* prompt, s4c_gui_malloc_func* malloc_func, s4c_gui_calloc_func* calloc_func, s4c_gui_free_func* free_func);
bool lint_TextField(TextField txt_field);
TextField new_TextField(size_t max_size, int height, int width, int start_x, int start_y);
TextField new_TextField_centered(size_t max_size, int height, int width, int bound_x, int bound_y);
TextField new_TextField_linted(TextField_Linter** linters, size_t num_linters, const void** linter_args, size_t max_size, int height, int width, int start_x, int start_y);
TextField new_TextField_alloc(size_t max_size, int height, int width, int start_x, int start_y, s4c_gui_malloc_func* malloc_func, s4c_gui_calloc_func* calloc_func, s4c_gui_free_func* free_func);
void draw_TextField(TextField txt);
void clear_TextField(TextField txt);
void use_clean_TextField(TextField txt_field);
void free_TextField(TextField txt_field);
const char* get_TextField_value(TextField txt_field);
int get_TextField_len(TextField txt_field);
WINDOW* get_TextField_win(TextField txt_field);
#endif // TEXT_FIELD_H_

#ifndef TOGGLE_H_
#define TOGGLE_H_

#ifdef _WIN32
#include <ncursesw/ncurses.h>
#else
#include <ncurses.h>
#endif

#include <menu.h>
#include <stdlib.h>
#ifndef TEXT_FIELD_H_
#error "This should not happen. TEXT_FIELD_H_ is defined in this same file."
#include "text_field.h"
#endif // TEXT_FIELD_H_

typedef enum {
    BOOL_TOGGLE,
    MULTI_STATE_TOGGLE,
    TEXTFIELD_TOGGLE,
} ToggleType;

typedef const char* (ToggleMultiState_Formatter)(int current_state);

typedef struct ToggleMultiState {
    int current_state;
    int num_states;
} ToggleMultiState;

typedef union ToggleState {
    bool bool_state; // For BOOL_TOGGLE
    ToggleMultiState ts_state; // For MULTI_STATE_TOGGLE
    TextField txt_state; // For TEXTFIELD_TOGGLE
} ToggleState;

typedef struct {
    ToggleType type;
    ToggleState state;
    char *label;
    bool locked; // Toggle lock
    ToggleMultiState_Formatter* multistate_formatter;
} Toggle;

typedef struct ToggleMenu_Conf {
    int height;
    int width;
    int start_x;
    int start_y;
    bool boxed;
    int quit_key;
    int statewin_width;
    int statewin_height;
    int statewin_start_x;
    int statewin_start_y;
    bool statewin_boxed;
    const char* statewin_label;
    int key_up;
    int key_right;
    int key_down;
    int key_left;
} ToggleMenu_Conf;

typedef struct ToggleMenu {
    Toggle* toggles;
    int num_toggles;
    int height;
    int width;
    int start_x;
    int start_y;
    bool boxed;
    int quit_key;
    int statewin_width;
    int statewin_height;
    int statewin_start_x;
    int statewin_start_y;
    bool statewin_boxed;
    const char* statewin_label;
    int key_up;
    int key_right;
    int key_down;
    int key_left;
} ToggleMenu;

ToggleMenu new_ToggleMenu_(Toggle* toggles, int num_toggles, ToggleMenu_Conf conf);
ToggleMenu new_ToggleMenu(Toggle* toggles, int num_toggles);
void draw_ToggleMenu_states(WINDOW *win, ToggleMenu toggle_menu);
void handle_ToggleMenu(ToggleMenu toggle_menu);
void free_ToggleMenu(ToggleMenu toggle_menu);
#endif // TOGGLE_H_

#endif // S4C_GUI_H_
