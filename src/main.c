#include "s4c_gui.h"

int textfield_main(void)
{
    // Initialize ncurses
    initscr();
    cbreak(); // Disable line buffering
    noecho(); // Don't echo input to screen

    // Define the dimensions and position of the window
    int height = 5;
    int width = 30;

    TextField_Linter* my_linters[2] = {
        &lint_TextField_not_empty,
        &lint_TextField_equals_cstr,
    };

    const void* linter_args[2] = {
        NULL,
        "ciao",
    };

    size_t n_linters = 2;
    size_t max_size = 10;

    TextField txt_field = new_TextField_centered_(&warn_TextField, my_linters, n_linters, linter_args, max_size, height, width, COLS, LINES, s4c_gui_malloc, s4c_gui_calloc, NULL);

    use_clean_TextField(txt_field);

    // Print the input back to the screen
    mvprintw(LINES/2 +1, (COLS - strlen("You entered: ")) / 2, "You entered: %s", get_TextField_value(txt_field));
    mvprintw(LINES/2 +2, (COLS - strlen("len: ")) / 2, "len: %i", get_TextField_len(txt_field));
    mvprintw(LINES/2 +4, (COLS - strlen("Lint result: ")) / 2, "Lint result: %s", (lint_TextField(txt_field) ? "pass" : "fail"));
    refresh();

    // Wait for user input before exiting
    getch();

    free_TextField(txt_field);
    endwin();

    return 0;
}

int togglemenu_main(void)
{
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // Define the dimensions and position of the textfield window
    int height = 5;
    int width = 30;
    int start_y = (LINES/2) +3;
    int start_x = 2;

    size_t txt_max_size_1 = 10;
    size_t txt_max_size_2 = 15;


    // Define menu options and their toggle states
    Toggle toggles[] = {
        {BOOL_TOGGLE, (ToggleState){.bool_state = true}, "[] Light(U)", false},
        {MULTI_STATE_TOGGLE, (ToggleState){.ts_state.current_state = 0, .ts_state.num_states = MAX_STATES}, "<Volume>(L)", true},
        {BOOL_TOGGLE, (ToggleState){.bool_state = false}, "[] Root (L)", true},
        {MULTI_STATE_TOGGLE, (ToggleState){.ts_state.current_state = 0, .ts_state.num_states = MAX_STATES}, "<Frequency> (U)", false},
        {TEXTFIELD_TOGGLE, (ToggleState){.txt_state = new_TextField(txt_max_size_1, height, width, start_y, start_x)}, "Token-> (L)", true},
        {TEXTFIELD_TOGGLE, (ToggleState){.txt_state = new_TextField(txt_max_size_2, height, width, start_y, start_x)}, "Name-> (U)", false},
    };
    int num_toggles = sizeof(toggles) / sizeof(toggles[0]);
    ToggleMenu toggle_menu = new_ToggleMenu(toggles, num_toggles);
    handle_ToggleMenu(toggle_menu);

    endwin(); // End ncurses
    free_ToggleMenu(toggle_menu);
    return 0;
}

int main(int argc, char** argv)
{
    if (argc > 1) {
        return togglemenu_main();
    } else {
        return textfield_main();
    }
}
