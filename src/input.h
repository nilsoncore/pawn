#ifndef PAWN_INPUT_H
#define PAWN_INPUT_H

#define GLEW_STATIC
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include "pawn.h"
#include "renderer.h"

struct ImGuiContext;

//
// --- Structs ---
//
struct Snake_IO {
    // [General]
    GLFWwindow *window;                  // GLFW window, which we get input from.
    GLFWmonitor *monitor;
    GLFWmonitor *last_windowed_monitor;
    ImGuiContext *imgui_context;
    bool window_focused;                 // Whether the window is focused or not.
    Vec2f window_position;               // Position of the window on in virtual space, in pixels.
    Vec2f window_size;                   // Size of window, in pixels.
    Rectangle2Di windowed_frame;
    bool window_resized;                 // Whether the window was resized or not in previous frame.
    bool window_fullscreen;              // Is window in windowed or fullscreen mode.
    float frametime;                     // Time elapsed since last frame, in seconds.
    float frametime_last;                // Time elapsed since last frame (in previous frame), in seconds.
    float frametime_delta;               // Difference between current and previous frames' elapsed time, in seconds.
    Font font;                           // Font, which bitmap will be used to draw symbols.
    int default_font_height;             // Default font height = window_height / 24.
    float font_global_scale;             // Global scale of the font.
    Vec4f clear_color;                   // Clear color, which will be used to fill emptiness on the screen.

    // [Cursor]
    bool draw_cursor;                    // Whether draw cursor on screen or not.
    bool cursor_in_window;               // Whether is cursor inside a window or not.
    Vec2f cursor_position;               // Cursor position, in pixels. Vec2f(F32_MIN, F32_MIN) if mouse is unavailable (not inside window frame, etc.)
    Vec2f cursor_position_prev;          // Previous cursor position.
    float cursor_sensitivity;            // Sensitivity of the cursor.
    Vec2f cursor_delta;                  // Cursor position delta.

    // [Mouse]
    // We capture only 3 mouse buttons: left, right, and middle.
    float mouse_double_click_time;       // Time for a double-click, in seconds.
    float mouse_double_click_max_dist;   // Distance threshold to stay in to a validate a double-click, in pixels.
    // float mouse_drag_threshold;          // Distance threshold before considering we are dragging.
    Vec2f mouse_clicked_pos[3];          // Position of a cursor at the time of clicking.
    double mouse_clicked_time[3];        // Time of last tick (used to figure out double-click)
    bool mouse_clicked[3];               // Mouse button went from !Down to Down
    bool mouse_doubleclicked[3];         // Has mouse button been double-clicked?
    bool mouse_released[3];              // Mouse button went from Down to !Down
    bool mouse_down[3];                  // Mouse buttons: 0=left, 1=right, 2=middle.
    bool mouse_down_was_doubleclick[3];  // Track if button down was a double-click.
    float mouse_down_duration[3];        // Duration the mouse button has been down (0.0f == just clicked).
    float mouse_down_duration_prev[3];   // Previous time the mouse button has been down.
    float mouse_wheel;                   // Mouse wheel Vertical.
    float mouse_wheel_horizontal;        // Mouse wheel Horizontal.

    // [Keys]
    float key_repeat_delay;              // When holding a key/button, time before it starts repeating, in seconds (for buttons in Repeat mode, etc.).
    float key_repeat_rate;               // When holding a key/button, rate at which it repeats, in seconds.
    u32 key_mods;                        // Key mods flags (Ctrl/Shift/Alt/Super), updated every frame.
    bool keys_down[512];                 // Keyboard keys that are pressed.
    bool keys_down_duration[512];        // Duration the keyboard key has been down (0.0f == just pressed).
    bool keys_down_duration_prev[512];   // Previous duration the key has been down.
};

//
// --- Enums ---
//
/*
enum KB_Key { // Keyboard key.
    TAB,
    LEFT_ARROW,
    RIGHT_ARROW,
    UP_ARROW,
    DOWN_ARROW,
    PAGE_UP,
    PAGE_DOWN,
    HOME,
    END,
    INSERT,
    DELETE,
    BACKSPACE,
    SPACE,
    ENTER,
    ESCAPE,
    KEYPAD_ENTER,
    A, // For text edit
    C,
    V,
    X,
    Y,
    Z,
    COUNT
};
*/

//
// --- Functions ---
//
void init_input_system();
Snake_IO *GetIO();
void process_input();
void input_free_resources();
GLFWwindow *create_window(int width, int height, const char *title, bool fullscreen);
GLFWmonitor *get_last_windowed_monitor();
void resize_window(int width, int height);
void destroy_window(GLFWwindow *window);
void window_switch_fullscreen();
void set_current_context(GLFWwindow *window);
char *float_to_time(float time);
char *float_to_time_with_millis(float time);
void console_print(const char *format, ...);
void console_log(const char *format, ...);

//
// --- Internal [GLFW] ---
//
static void init_glfw();
static void GLFW_update_callbacks(GLFWwindow *window);
static void GLFW_cursor_pos_callback(GLFWwindow *window, double new_cursor_x, double new_cursor_y);
static void GLFW_mouse_button_callback(GLFWwindow *window, int button, int action, int flags);
static void GLFW_cursor_enter_window_callback(GLFWwindow *window, int entered);
static void GLFW_scroll_callback(GLFWwindow *window, double offset_x, double offset_y);
static void GLFW_single_key_press_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
static void GLFW_window_focus_callback(GLFWwindow *window, int focused);
static void GLFW_window_refresh_callback(GLFWwindow *window);
static void GLFW_window_close_callback(GLFWwindow *window);
// static void GLFW_window_resize_callback(GLFWwindow *window);
static void GLFW_framebuffer_resize_callback(GLFWwindow *window, int new_width, int new_height);
static void GLFW_error_callback(int error_code, const char *description);

#endif /* PAWN_INPUT_H */
