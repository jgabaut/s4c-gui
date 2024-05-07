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
#include "s4c_gui.h"

const char *string_s4c_gui_version(void)
{
    return S4C_GUI_API_VERSION_STRING;
}

int int_s4c_gui_version(void)
{
    return S4C_GUI_API_VERSION_INT;
}

s4c_gui_malloc_func* s4c_gui_malloc = &malloc;
s4c_gui_calloc_func* s4c_gui_calloc = &calloc;

#ifndef TEXT_FIELD_H_
#error "This should not happen. TEXT_FIELD_H_ is defined in s4c_gui.h"
#include "text_field.h"
#endif // TEXT_FIELD_H_

struct TextField_s {
    WINDOW* win;
    int height;
    int width;
    int start_x;
    int start_y;
    char* buffer;
    int length;
    int max_length;
    TextField_Full_Handler* handler;
    TextField_Linter** linters;
    size_t num_linters;
    const void** linter_args;
    s4c_gui_malloc_func* malloc_func;
    s4c_gui_calloc_func* calloc_func;
    s4c_gui_free_func* free_func;
};

TextField_Linter* default_linters[TEXTFIELD_DEFAULT_LINTERS_TOT+1] = {
    &lint_TextField_not_empty,
};

const void* default_linter_args[TEXTFIELD_DEFAULT_LINTERS_TOT+1] = {
    NULL,
};

TextField new_TextField_(TextField_Full_Handler* full_buffer_handler, TextField_Linter** linters, size_t num_linters, const void** linter_args, size_t max_size, int height, int width, int start_x, int start_y, s4c_gui_malloc_func* malloc_func, s4c_gui_calloc_func* calloc_func, s4c_gui_free_func* free_func)
{
    assert(height>=0);
    assert(width>=0);
    assert(start_x>=0);
    assert(start_x>=0);
    TextField res = NULL;
    if (malloc_func != NULL) {
        res = malloc_func(sizeof(struct TextField_s));
        res->malloc_func = malloc_func;
        if (free_func != NULL) {
            res->free_func = free_func;
        }
    } else {
        res->free_func = free;
        res->malloc_func = s4c_gui_malloc;
        res = s4c_gui_malloc(sizeof(struct TextField_s));
    }
    res->buffer = calloc(max_size+1, sizeof(char));
    memset(res->buffer, 0, max_size);
    res->height = height;
    res->width = width;
    res->start_x = start_x;
    res->start_y = start_y;
    res->win = newwin(height, width, start_y, start_x);
    res->length = 0;
    res->max_length = max_size;
    res->handler = full_buffer_handler;
    res->num_linters = num_linters;
    if (linters != NULL && num_linters > 0) {
        // Alloc memory for the linter callbacks
        if (calloc_func != NULL) {
            res->linters = calloc_func(num_linters, sizeof(TextField_Linter*));
            res->calloc_func = calloc_func;
            if (free_func != NULL) {
                res->free_func = free_func;
            }
        } else {
            res->calloc_func = s4c_gui_calloc;
            res->linters = s4c_gui_calloc(num_linters, sizeof(TextField_Linter*));
        }
        res->linter_args = res->calloc_func(num_linters, sizeof(void*));
        for (size_t i=0; i < num_linters; i++) {
            if (linters[i] != NULL) {
                res->linters[i] = linters[i];
                res->linter_args[i] = linter_args[i];
            }
        }
    }
    return res;
}

TextField new_TextField_centered_(TextField_Full_Handler* full_buffer_handler, TextField_Linter** linters, size_t num_linters, const void** linter_args, size_t max_size, int height, int width, int bound_x, int bound_y, s4c_gui_malloc_func* malloc_func, s4c_gui_calloc_func* calloc_func, s4c_gui_free_func* free_func)
{
    int start_y = (bound_y - height) / 2;
    int start_x = (bound_x - width) / 2;
    return new_TextField_(full_buffer_handler, linters, num_linters, linter_args, max_size, height, width, start_x, start_y, malloc_func, calloc_func, free_func);
}

TextField new_TextField(size_t max_size, int height, int width, int start_x, int start_y)
{
    return new_TextField_(&warn_TextField, default_linters, TEXTFIELD_DEFAULT_LINTERS_TOT, default_linter_args, max_size, height, width, start_x, start_y, malloc, calloc, NULL);
}

TextField new_TextField_centered(size_t max_size, int height, int width, int bound_x, int bound_y)
{
    return new_TextField_centered_(&warn_TextField, default_linters, TEXTFIELD_DEFAULT_LINTERS_TOT, default_linter_args, max_size, height, width, bound_x, bound_y, malloc, calloc, NULL);
}

TextField new_TextField_linted(TextField_Linter** linters, size_t num_linters, const void** linter_args, size_t max_size, int height, int width, int start_x, int start_y)
{
    return new_TextField_(&warn_TextField, linters, num_linters, linter_args, max_size, height, width, start_x, start_y, malloc, calloc, NULL);
}

TextField new_TextField_alloc(size_t max_size, int height, int width, int start_x, int start_y, s4c_gui_malloc_func* malloc_func, s4c_gui_calloc_func* calloc_func, s4c_gui_free_func* free_func)
{
    return new_TextField_(&warn_TextField, default_linters, TEXTFIELD_DEFAULT_LINTERS_TOT, default_linter_args, max_size, height, width, start_x, start_y, malloc_func, calloc_func, free_func);
}

void free_TextField(TextField txt_field)
{
    assert(txt_field!=NULL);
    // Clean up
    delwin(txt_field->win);
    if (txt_field->malloc_func == malloc && txt_field->calloc_func == calloc) {
        if (txt_field->linters != NULL) {
            free(txt_field->linters);
            free(txt_field->linter_args);
        }
        free(txt_field->buffer);
        free(txt_field);
    } else {
        if (txt_field->free_func != NULL) {
            // TODO: Pseudo-free?
            txt_field->free_func(txt_field);
        }
        // Do nothing
    }
}

const char* get_TextField_value(TextField txt_field)
{
    assert(txt_field!=NULL);
    return txt_field->buffer;
}

bool lint_TextField(TextField txt_field)
{
    assert(txt_field!=NULL);
    bool res = true;
    TextField_Linter* linter_func = NULL;
    for (size_t i=0; res == true && i < txt_field->num_linters; i++) {
        linter_func = txt_field->linters[i];
        if (linter_func != NULL) {
            // NULL func being found don't affect the result
            res = linter_func(txt_field, txt_field->linter_args[i]);
        }
    }
    return res;
}

int get_TextField_len(TextField txt_field)
{
    assert(txt_field!=NULL);
    return txt_field->length;
}

WINDOW* get_TextField_win(TextField txt_field)
{
    assert(txt_field!=NULL);
    return txt_field->win;
}

void draw_TextField(TextField txt)
{
    assert(txt!=NULL);
    // Draw a box around the window
    box(txt->win, 0, 0);
    wrefresh(txt->win);
}

void clear_TextField(TextField txt)
{
    assert(txt!=NULL);
    // Zero buffer and length
    memset(txt->buffer, 0, txt->max_length+1);
    txt->length = 0;
}

void warn_TextField(TextField txt)
{
    assert(txt!=NULL);
    WINDOW* win = get_TextField_win(txt);
    // Buffer is full and user passed one more char. Discard it.
    wclear(win);
    box(win,0,0);
    mvwprintw(win, 1, 1, "%s", "Input is full.");
    mvwprintw(win, 2, 1, "%s", "Press Enter or Backspace.");
    wrefresh(win);
    napms(500);
    wclear(win);
    box(win,0,0);
    mvwprintw(win, 1, 1, "%s", get_TextField_value(txt));
    wrefresh(win);

}

bool lint_TextField_not_empty(TextField txt, const void* unused)
{
    (void) unused;
    if (txt==NULL) return false;
    return txt->length>0;
}

bool lint_TextField_equals_cstr(TextField txt, const void* cstr)
{
    // Please, pass a null-terminated string.
    const char* str = (const char*) cstr;

    return (strcmp(get_TextField_value(txt), str) == 0);
}


bool lint_TextField_whitelist(TextField txt, const void* whitelist)
{
    // Please, pass a null-terminated string.
    const char* whitelist_cstr = (const char*) whitelist;
    size_t whitelist_size = strlen(whitelist);
    if (txt==NULL || whitelist == NULL) return false;
    bool res = true;
    for (size_t i=0; res == true && i < txt->length; i++) {
        for (size_t j=0; res == true && j < whitelist_size; j++) {
            res = (txt->buffer[i] != whitelist_cstr[j]);
        }
    }
    return res;
}

bool lint_TextField_char_range(TextField txt, int min, int max)
{
    if (txt==NULL) return false;
    if (min < 0 || max > CHAR_MAX) return false;
    bool res = true;
    char ch = -1;
    for (size_t i=0; res == true && i < txt->length; i++) {
        ch = txt->buffer[i];
        if (ch == '\0') {
            if (i != txt->length) {
                //TODO: mismatch on len?
            }
            break;
        }
        res = (ch >= min && ch > max);
    }
    return res;
}

bool lint_TextField_digits_only(TextField txt)
{
    if(txt==NULL) return false;
    return lint_TextField_char_range(txt, '0', '9');
}

bool lint_TextField_printable_only(TextField txt)
{
    if(txt==NULL) return false;
    return lint_TextField_char_range(txt, ' ', '~');
}

static void get_userText(TextField txt_field)
{
    assert(txt_field!=NULL);
    char* buffer = txt_field->buffer;
    int* length = &(txt_field->length);
    WINDOW* win = txt_field->win;
    assert(win!=NULL);

    const int max_length = txt_field->max_length;

    const int input_start_x = 1;
    // Move the cursor to the input field position
    wmove(win, 1, input_start_x);

    // Get input from the user
    int ch;
    while ((ch = wgetch(win)) != '\n') {
        // Check for backspace
        if (ch == KEY_BACKSPACE || ch == '\b' || ch == 127) {
            if (*length > 0) {
                // Erase the character
                mvwaddch(win, 1, *length, ' ');
                wrefresh(win);
                // Move cursor back
                wmove(win, 1, *length);
                buffer[(*length)] = '\0';
                (*length)--;
            }
        } else {
            if (*length < max_length) {
                // Echo the character
                waddch(win, ch);
                wrefresh(win);
                // Add it to the buffer
                buffer[(*length)++] = ch;
            } else {
                // Buffer is full
                if (txt_field->handler != NULL) {
                    txt_field->handler(txt_field);
                }
            }
        }
    }
}

void use_clean_TextField(TextField txt_field)
{
    assert(txt_field!=NULL);
    clear_TextField(txt_field);

    draw_TextField(txt_field);

    get_userText(txt_field);
    wclear(txt_field->win);
    wrefresh(txt_field->win);
}
// }
// TEXT_FIELD_H_


#ifndef TOGGLE_H_
#error "This should not happen. TOGGLE_H_ is defined in s4c_gui.h"
#include "toggle.h"
#endif // TOGGLE_H_

static const int TOGGLE_MENU_DEFAULT_QUIT_KEY = KEY_F(1);

static void cycle_toggle_state(Toggle *toggle)
{
    toggle->state.ts_state.current_state = (toggle->state.ts_state.current_state + 1) % toggle->state.ts_state.num_states;
}

ToggleMenu new_ToggleMenu_(Toggle* toggles, int num_toggles, ToggleMenu_Conf conf)
{
    size_t widest_label_size = 0;
    for (size_t i=0; i < num_toggles; i++) {
        size_t curr_size = strlen(toggles[i].label);
        if (curr_size > widest_label_size) widest_label_size = curr_size;
    }
    return (ToggleMenu) {
        .toggles = toggles,
        .num_toggles = num_toggles,
        .height = num_toggles+1,
        .width = widest_label_size+1,
        .start_x = conf.start_x,
        .start_y = conf.start_y,
        .boxed = conf.boxed,
        .quit_key = ((conf.quit_key < 0) ? TOGGLE_MENU_DEFAULT_QUIT_KEY : conf.quit_key),
        .statewin_height = conf.statewin_height,
        .statewin_width = conf.statewin_width,
        .statewin_start_x = conf.statewin_start_x,
        .statewin_start_y = conf.statewin_start_y,
        .statewin_boxed = conf.statewin_boxed,
        .statewin_label = conf.statewin_label,
    };
}

static const ToggleMenu_Conf TOGGLE_MENU_DEFAULT_CONF = {
    .start_x = 0,
    .start_y = 0,
    .boxed = true,
    .quit_key = TOGGLE_MENU_DEFAULT_QUIT_KEY,
};

ToggleMenu new_ToggleMenu(Toggle* toggles, int num_toggles)
{
    return new_ToggleMenu_(toggles, num_toggles, TOGGLE_MENU_DEFAULT_CONF);
}

void free_ToggleMenu(ToggleMenu toggle_menu)
{
    for (size_t i=0; i<toggle_menu.num_toggles; i++) {
        switch (toggle_menu.toggles[i].type) {
        case BOOL_TOGGLE:
        case MULTI_STATE_TOGGLE: {
            break;
        }
        case TEXTFIELD_TOGGLE: {
            free_TextField(toggle_menu.toggles[i].state.txt_state);
            break;
        }
        default: {
            //Unexpected
            assert(false);
            break;
        }
        }
    }
}

void draw_ToggleMenu_states(WINDOW *win, ToggleMenu toggle_menu)
{

    Toggle* toggles = toggle_menu.toggles;
    int num_toggles = toggle_menu.num_toggles;
    // Clear the window
    wclear(win);

    if (toggle_menu.statewin_boxed) box(win, 0, 0);
    if (toggle_menu.statewin_label != NULL) mvwprintw(win, 0, 1, toggle_menu.statewin_label);

    // Print toggle states
    for (int i = 0; i < num_toggles; i++) {
        // Print toggle label
        mvwprintw(win, i + 1, 1, "%s:", toggles[i].label);

        // Print toggle state
        if (toggles[i].type == BOOL_TOGGLE) {
            mvwprintw(win, i + 1, 20, "[%s]", toggles[i].state.bool_state ? "ON" : "OFF");
        } else if (toggles[i].type == MULTI_STATE_TOGGLE) {
            mvwprintw(win, i + 1, 20, "[%d/%d]", toggles[i].state.ts_state.current_state + 1, toggles[i].state.ts_state.num_states);
        } else if (toggles[i].type == TEXTFIELD_TOGGLE) {
            mvwprintw(win, i + 1, 20, "%s", get_TextField_value(toggles[i].state.txt_state));
        }

        // Print lock indicator
        if (toggles[i].locked) {
            mvwprintw(win, i + 1, 30, "(LOCKED)");
        }
    }

    wrefresh(win);
}

void handle_ToggleMenu(ToggleMenu toggle_menu)
{
    bool try_display_state = false;
    WINDOW* state_win = NULL;
    if (toggle_menu.statewin_width > 0 && toggle_menu.statewin_height > 0) {
        try_display_state = true;
        // Create a window for toggle states
        state_win = newwin(toggle_menu.statewin_height, toggle_menu.statewin_width, toggle_menu.statewin_start_y, toggle_menu.statewin_start_x);
        if (toggle_menu.statewin_boxed) box(state_win, 0, 0);
        if (toggle_menu.statewin_label != NULL) mvwprintw(state_win, 0, 1, toggle_menu.statewin_label);
        wrefresh(state_win);
    }

    Toggle* toggles = toggle_menu.toggles;
    int num_toggles = toggle_menu.num_toggles;

    // Create MENU for toggles
    ITEM **toggle_items = (ITEM **)calloc(num_toggles + 1, sizeof(ITEM *));
    MENU *nc_menu;
    for (int i = 0; i < num_toggles; i++) {
        toggle_items[i] = new_item(toggles[i].label, "");
        if (toggles[i].type == MULTI_STATE_TOGGLE && !toggles[i].locked) {
            // Allow cycling through states for MULTI_STATE_TOGGLE toggles
            set_item_userptr(toggle_items[i], &toggles[i]);
        }
        if (toggles[i].type == BOOL_TOGGLE && !toggles[i].locked) {
            // Allow switching on bool toggle
            set_item_userptr(toggle_items[i], &toggles[i]);
        }
        if (toggles[i].type == TEXTFIELD_TOGGLE && !toggles[i].locked) {
            // Allow changing textfield toggle
            set_item_userptr(toggle_items[i], &toggles[i]);
        }
    }
    toggle_items[num_toggles] = NULL;
    nc_menu = new_menu(toggle_items);

    if (try_display_state) draw_ToggleMenu_states(state_win, toggle_menu);

    // Create a window for the MENU
    WINDOW *menu_win = newwin(toggle_menu.height, toggle_menu.width, toggle_menu.start_y, toggle_menu.start_x); //LINES/2, COLS / 2, 0, 0);
    keypad(menu_win, TRUE);
    set_menu_win(nc_menu, menu_win);
    set_menu_sub(nc_menu, derwin(menu_win, toggle_menu.height -2, toggle_menu.width -2, toggle_menu.start_y +1, toggle_menu.start_x+1)); //LINES/2) - 2, COLS / 2 - 2, 1, 1));
    set_menu_mark(nc_menu, "");
    if (toggle_menu.boxed) box(menu_win,0,0);
    post_menu(nc_menu);
    wrefresh(menu_win);

    // Main loop
    int c;
    while ((c = wgetch(menu_win)) != toggle_menu.quit_key) {
        switch (c) {
        case KEY_DOWN: {
            int res = menu_driver(nc_menu, REQ_DOWN_ITEM);
            if (res == E_REQUEST_DENIED) {
                res = menu_driver(nc_menu, REQ_FIRST_ITEM);
            }
        }
        break;
        case KEY_UP: {
            int res = menu_driver(nc_menu, REQ_UP_ITEM);
            if (res == E_REQUEST_DENIED) {
                res = menu_driver(nc_menu, REQ_LAST_ITEM);
            }
        }
        break;
        case KEY_RIGHT:
            // Cycle through states for selected item
            if (current_item(nc_menu)) {
                Toggle *toggle = (Toggle *)item_userptr(current_item(nc_menu));
                if (toggle && toggle->type == MULTI_STATE_TOGGLE && !toggle->locked) {
                    cycle_toggle_state(toggle);
                    if (try_display_state) draw_ToggleMenu_states(state_win, toggle_menu);
                }
            }
            break;
        case KEY_LEFT:
            // Cycle through states for selected item
            if (current_item(nc_menu)) {
                Toggle *toggle = (Toggle *)item_userptr(current_item(nc_menu));
                if (toggle && toggle->type == MULTI_STATE_TOGGLE && !toggle->locked) {
                    cycle_toggle_state(toggle);
                    if (try_display_state) draw_ToggleMenu_states(state_win, toggle_menu);
                }
            }
            break;
        case '\n':
            // Toggle state for selected BOOL_TOGGLE item
            if (current_item(nc_menu)) {
                Toggle *toggle = (Toggle *)item_userptr(current_item(nc_menu));
                if (toggle && toggle->type == BOOL_TOGGLE && !toggle->locked) {
                    toggle->state.bool_state = !toggle->state.bool_state;
                    draw_ToggleMenu_states(state_win, toggle_menu);
                } else if (toggle && toggle->type == TEXTFIELD_TOGGLE && !toggle->locked) {
                    use_clean_TextField(toggle->state.txt_state);
                    if (try_display_state) draw_ToggleMenu_states(state_win, toggle_menu);
                }
            }
            break;
        }
    }

    // Clean up
    unpost_menu(nc_menu);
    free_menu(nc_menu);
    for (int i = 0; i < num_toggles; i++) {
        free_item(toggle_items[i]);
    }
    free(toggle_items);
    delwin(menu_win);
    if (try_display_state) delwin(state_win);
}
// }
// TOGGLE_H_
