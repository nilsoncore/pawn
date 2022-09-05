// time()
#include <time.h>

// glm::perspective, glm::lookAt
#include <GLM/gtc/matrix_transform.hpp>

// Sleep()
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#include <Windows.h>
#undef max
#undef min

// 'pawn.h' is already included in 'renderer.h'
#include "pawn.h"
#include "renderer.h"
#include "immediate.h"
#include "input.h"
#include "array.h"

//
// --- Global variables ---
//
u32 game_state;
bool imgui_states[] = { true, false, false, false, true };

extern Renderer_Info renderer_info;

static Texture g_game_image;

int main(int arguments_count, char **arguments) {
    ZoneScoped;

    test_allocators();
    // array_test();

    srand(time(NULL)); // Init random number generator seed

    init_input_system();
    init_renderer();

    game_state = TITLE_SCREEN;

    g_game_image = load_texture_from_file("resources/textures/pawn.png");

    auto io = GetIO();

    //
    // Main game loop
    //
    while (!glfwWindowShouldClose(io->window)) {
        // Sleep for 100 milliseconds so we don't overload CPU
        // while game is minimized.
        if (io->window_size == new_vec2f(0)) {
            Sleep(100);
        }

        // process_input(window);
        process_input();

        renderer_draw(game_state);
    }

    game_exit();

    exit(EXIT_SUCCESS);
}

void draw_debug_info() {
    auto io = GetIO();
    
    float window_width = io->window_size.width;
    float window_height = io->window_size.height;
    float frame_padding = window_height / 24;
    Rectangle2D imm_frame = new_rect(window_width/2, window_height/2, window_width/2 - frame_padding, window_height/2 - frame_padding);
    // Rectangle2D imm_frame = new_rect(window_width/2, window_height/2, window_width, window_height);
    
    auto imm = GetImmediate();
    
    // imm->flags |= TEXT_ALIGN_CENTER;
    // imm->flags = TEXT_ALIGN_CENTER;
    
    imm_begin_frame(imm_frame);
    draw_rect(imm_frame, new_vec3f(0.0f, 1.0f, 0.0f), GL_LINE_STRIP);
    imm_text("Window size: %.fx%.f", window_width, window_height);
    imm_text("IO font size: %d", io->font.height);
    imm_text("Immediate font size: %d", imm->font.height);
    imm_text("Frametime: %.3f", io->frametime_delta);
    imm_text("FPS: %.f", 1000.0f / io->frametime_delta);
    imm_text("Cursor: (%.f, %.f)", io->cursor_position.x, io->cursor_position.y);
    bool button_hovered, button_held;
    bool button_pressed = imm_button("Button", &button_hovered, &button_held);
    imm_text("Button pressed: %s", (button_pressed) ? "true" : "false");
    imm_text("Button hovered: %s", (button_hovered) ? "true" : "false");
    imm_text("Button held: %s", (button_held) ? "true" : "false");
    imm_text("Left mouse down: %s", (io->mouse_down[0]) ? "true" : "false");
    imm_text("Left mouse released: %s", (io->mouse_released[0]) ? "true" : "false");
    imm_end_frame();
}

void draw_title_screen() {
    ZoneScoped;

    auto io = GetIO();

    auto width = io->window_size.width;
    auto height = io->window_size.height;
    auto gap = height/32;

    auto frame = get_rect(width/4, height/2, width/4, height/4);
    draw_rect(frame, new_vec3f(1.0f, 0.0f, 0.0f), GL_LINE_STRIP);

    auto imm = GetImmediate();
    
    imm_begin_frame(frame);
    Vec2f button_size = new_vec2f(height/16, height/96);
    bool continue_pressed = imm_button("Continue", button_size);
    if (continue_pressed) {
        console_log("Continue button pressed.\n");
    }

    bool new_game_pressed = imm_button("New Game", button_size);
    if (new_game_pressed) {
        console_log("New Game button pressed.\n");
    }

    bool settings_pressed = imm_button("Settings", button_size);
    if (settings_pressed) {
        console_log("Settings button pressed.\n");
    }

    bool quit_pressed = imm_button("Quit", button_size);
    if (quit_pressed) {
        console_log("Quit button pressed.\n");
    }

    imm_end_frame();

    auto texture_rect = get_rect(width/2, height/2, width/16, height/16);
    draw_sprite(texture_rect, g_game_image);
}

void draw_pause_screen() {
    ZoneScoped;

    static int settings_draw = 0;
    if (settings_draw == 0) {
        console_log("Trying to draw not yet implemented 'draw_pause_screen()'!\n");
        settings_draw++;
    }
}

void draw_settings_screen() {
    ZoneScoped;

    auto io = GetIO();

    int width = (float)io->window_size.width;
    int height = (float)io->window_size.height;

    int x = width/2;
    int y = height/2;
    float button_gap = height/32;
    float center_line_gap = button_gap/4;
    Rectangle2D menu_frame = new_rect(x, y, width/4, height/2 - button_gap);
    Vec3f button_color = new_vec3f(1.0f, 0.0f, 0.0f);
    Vec3f text_color = new_vec3f(0.9f, 0.9f, 0.9f);
    const char *text[] = { "Resolution:", "Window Mode:", "VSync:" };
    float gap_between_options = height/32;

    draw_rect(menu_frame, text_color, GL_LINE_STRIP);

    Rectangle2D text_rects[3];
    x = menu_frame.x - center_line_gap;
    y = menu_frame.y + menu_frame.height - button_gap;
    draw_text_column(3, &text_rects[0], &io->font, &text[0], x, y, button_gap, 1.0f, text_color, ALIGN_RIGHT);

    Screen_Text scr_text = new_screen_text(text[0], &io->font, x, y, text_color, ALIGN_RIGHT);
    // Rectangle2D scr_text_rect = draw_text(&roboto, scr_text);
    // draw_rect(scr_text_rect, text_color, GL_LINE_STRIP);

    For (3 /*rects*/) {
        // draw_rect(text_rects[it], text_color, GL_LINE_STRIP);
    }

    // Options part.
    const char *res_items[] = { "2560x1440", "1920x1080", "1280x720" };
    
    Dropdown res; // res = resolution
    res.x = menu_frame.x + center_line_gap;
    res.y = text_rects[0].y;
    res.width = menu_frame.width/4;
    res.height = text_rects[0].height;
    res.items = &res_items[0][0];
    res.items_count = 3;
    res.active_item = 0;
    res.bg_color = vec3f_from_vec4f(io->clear_color);
    res.text_color = new_vec3f(1.0f, 0.0f, 0.0f);
    res.opened = true;

    Rectangle2D res_rect = draw_dropdown(res);
    // draw_rect(res_rect, text_color, GL_LINE_STRIP);

    const char *window_mode_items[] = { "Fullscreen", "Windowed", "Windowed (Borderless)" };

    Dropdown window_mode;
    window_mode.x = menu_frame.x + center_line_gap;
    window_mode.y = text_rects[1].y;
    window_mode.width = menu_frame.width/4;
    window_mode.height = text_rects[1].height;
    window_mode.items = &window_mode_items[0][0];
    window_mode.items_count = 3;
    window_mode.active_item = 0;
    window_mode.bg_color = vec3f_from_vec4f(io->clear_color);
    window_mode.text_color = new_vec3f(1.0f, 0.0f, 0.0f);
    window_mode.opened = false;

    Rectangle2D window_mode_rect = draw_dropdown(window_mode);
    // draw_rect(window_mode_rect, text_color, GL_LINE_STRIP);

    Checkbox vsync;
    vsync.width = text_rects[2].height;
    // 'height' is the same as 'width' because it is a square and it is a union.
    vsync.x = menu_frame.x + center_line_gap + vsync.width;
    vsync.y = text_rects[2].y;
    vsync.color = new_vec3f(1.0f, 0.0f, 0.0f);
    vsync.checked = true;

    Rectangle2D vsync_rect = draw_checkbox(vsync);
    // draw_rect(vsync_rect, text_color, GL_LINE_STRIP);
}

void make_imgui_layout() {
    ZoneScoped;

    if (imgui_states[DRAW_DEMO_WINDOW]) {
        ImGui::ShowDemoWindow(&imgui_states[DRAW_DEMO_WINDOW]);
    }

    if (imgui_states[DRAW_CONSTANTS_WINDOW]) {
        ImGui::Begin("Constants");
        ImGui::End();
    }

    if (imgui_states[DRAW_GLOBALS_WINDOW]) {
        // ...
    }

    if (imgui_states[DRAW_EDIT_WINDOW]) {
        auto io = GetIO();
        ImGuiIO &imgui_io = ImGui::GetIO();

        ImGui::Begin("Scene");
        ImGui::Checkbox("Demo Window", &imgui_states[DRAW_DEMO_WINDOW]);
        ImGui::Checkbox("Constants Window", &imgui_states[DRAW_CONSTANTS_WINDOW]);
        ImGui::Checkbox("Globals Window", &imgui_states[DRAW_GLOBALS_WINDOW]);
        ImGui::ColorEdit3("Clear color", &io->clear_color.r);
        ImGui::NewLine();
        ImGui::Text("GPU Vendor: %s", renderer_info.gpu_vendor);
        ImGui::Text("GPU Renderer: %s", renderer_info.gpu_renderer);
        ImGui::Text("OpenGL/Driver version: %s", renderer_info.gl_version);
        ImGui::Text("GLSL: %s", renderer_info.glsl_version);
        ImGui::Text("ImGui Frametime: %.3f ms/frame (%.1f FPS)", 1000.0f / imgui_io.Framerate, imgui_io.Framerate);
        ImGui::Text("Frametime: %.3f ms/frame (%.1f FPS)", io->frametime_delta, 1000.0f / io->frametime_delta);
        ImGui::End();
    }

    if (imgui_states[DRAW_INPUT_WINDOW]) {
        auto io = GetIO();

        ImGui::Begin("Input");
        ImGui::Text("Pressed keys:");

        For (512 /*keys*/) {
            auto key_down = io->keys_down[it];
            if (key_down) {
                ImGui::Text("Key '%c'", (char)it);
            }
        }

        ImGui::End();
    }
}

void print_game_state(u32 game_state) {
    auto s = game_state;
    console_log("Current game state: { PLAY: %d, DEBUG: %d, TITLE_SCREEN: %d, PAUSE_SCREEN: %d, SETTINGS_SCREEN: %d }\n", s & PLAY, s & DEBUG, s & TITLE_SCREEN, s & PAUSE_SCREEN, s & SETTINGS_SCREEN);
}

void game_exit() {
    ZoneScoped;

    static int game_exit = 0;
    if (game_exit == 0) {
        console_log("Trying to exit game, but 'game_exit' is not yet implemented!\n");
        game_exit++;
    }
}

Board create_board(Allocator *allocator, s32 rows, s32 columns, s32 player_count) {
    Board board;

    board.rows = rows;
    board.columns = columns;
    // board.squares = new_array<Board_Square>(allocator, (s64)(rows * columns));
    board.squares = new_array<Board_Square>(allocator, rows * columns);

    // board.player_count = player_count;
    board.players = new_array<Board_Player>(allocator, player_count);
    board.player_turn = 0;

    return board;
}
