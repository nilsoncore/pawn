// sprintf()
#define _CRT_SECURE_NO_WARNINGS 1

#include "input.h"
#include "renderer.h"

//
// --- Globals ---
//
extern u32 game_state;
extern bool imgui_states[];

static Snake_IO io;

static bool glfw_mouse_just_pressed[3] = {0};

//
// --- Functions ---
//
void init_input_system() {

    init_glfw();

    // Initialise Snake IO.
    // [General]
    // io.window - Set by renderer.
    // io.window_focused - Set by renderer.
    // io.window_pos - Set by renderer.
    // io.window_size - Set by renderer.
    // io.window_resized - Set by renderer.
    io.window_fullscreen = false;
    io.frametime = 0.0f;
    io.frametime_last = 0.0f;
    io.frametime_delta = 0.0f;
    // io.font = load_font_from_file("resources/fonts/Roboto-Regular.ttf", 0, screen.height/24); // Renderer function call
    // io.default_font_height - Set by renderer. 
    io.font_global_scale = 1.0f;
    io.clear_color = new_vec4f(0.1f, 0.1f, 0.1f, 1.0f); // RGBA

    // [Cursor]
    io.draw_cursor = true;
    io.cursor_in_window = true;
    io.cursor_position = new_vec2f(io.window_size.width/2, io.window_size.height/2);
    io.cursor_position_prev = new_vec2f(0.0f, 0.0f);
    io.cursor_sensitivity = 0.1f;
    io.cursor_delta = new_vec2f(0.0f, 0.0f);

    // [Mouse]
    io.mouse_double_click_time = 0.300f;
    io.mouse_double_click_max_dist = 6.0f;
    mem_zero(&io.mouse_clicked_pos[0],          sizeof(io.mouse_clicked_pos)          );
    mem_zero(&io.mouse_clicked_time[0],         sizeof(io.mouse_clicked_time)         );
    mem_zero(&io.mouse_clicked[0],              sizeof(io.mouse_clicked)              );
    mem_zero(&io.mouse_doubleclicked[0],        sizeof(io.mouse_doubleclicked)        );
    mem_zero(&io.mouse_released[0],             sizeof(io.mouse_released)             );
    mem_zero(&io.mouse_down[0],                 sizeof(io.mouse_down)                 );
    mem_zero(&io.mouse_down_was_doubleclick[0], sizeof(io.mouse_down_was_doubleclick) );
    mem_zero(&io.mouse_down_duration[0],        sizeof(io.mouse_down_duration)        );
    mem_zero(&io.mouse_down_duration_prev[0],   sizeof(io.mouse_down_duration_prev)   );
    io.mouse_wheel = 0.0f;

    // [Keys]
    io.key_repeat_delay = 0.250f;
    io.key_repeat_rate = 0.050f;
    io.key_mods = 0;
    mem_zero(&io.keys_down[0],            sizeof(io.keys_down)               );
    mem_zero(&io.keys_down_duration,      sizeof(io.keys_down_duration)      );
    mem_zero(&io.keys_down_duration_prev, sizeof(io.keys_down_duration_prev) );
}

void process_input() {
    For (3 /*mouse buttons*/) {
        bool mouse_down_last_frame = io.mouse_down[it];
        io.mouse_down[it] = glfw_mouse_just_pressed[it] || glfwGetMouseButton(io.window, it) != GLFW_RELEASE;
        glfw_mouse_just_pressed[it] = false;

        bool mouse_released = mouse_down_last_frame && !io.mouse_down[it];
        io.mouse_released[it] = mouse_released;
    }
}

GLFWwindow *create_window(int width, int height, const char *title, bool fullscreen) {
    if (io.window != NULL) {
        glfwDestroyWindow(io.window);
    }

    GLFWmonitor *monitor = (fullscreen) ? glfwGetPrimaryMonitor() : NULL;
    auto window = glfwCreateWindow(width, height, title, monitor, NULL);
    if (window) {
        console_print("Window created (%dx%d, %s)\n", width, height, (monitor) ? "Fullscreen" : "Windowed");
    } else {
        console_print("ERROR: Failed to create window! (resolution: %dx%d, fullscreen: %d, monitor: %p, backend: GLFW)\n", width, height, fullscreen, monitor);
    }

    glfwMakeContextCurrent(window);
    GLFW_update_callbacks(window);

    io.window = window;
    io.window_fullscreen = fullscreen;
    io.window_size.width = width;
    io.window_size.height = height;

    return window;
}

GLFWmonitor *get_last_windowed_monitor() {
    int monitors_count;
    auto monitors = glfwGetMonitors(&monitors_count);

    Rectangle2Di window_rect;
    glfwGetWindowPos(io.window, &window_rect.x, &window_rect.y);
    glfwGetWindowSize(io.window, &window_rect.width, &window_rect.height);
    Rectangle2Di monitor_rect;
    int window_center_x = window_rect.x + window_rect.width/2;
    int window_center_y = window_rect.y + window_rect.height/2;
    For (monitors_count) {
        glfwGetMonitorWorkarea(monitors[it], &monitor_rect.x, &monitor_rect.y, &monitor_rect.width, &monitor_rect.height);

        if (in_window_frame(window_center_x, window_center_y, monitor_rect)) {
            io.last_windowed_monitor = monitors[it];
            console_log("[DEBUG] - Last windowed monitor index: %d\n", it);
            break;
        }
    }

    return io.last_windowed_monitor;
}

void resize_window(int width, int height) {
    io.window_size.width = width;
    io.window_size.height = height;
    set_viewport(0, 0, width, height);
    if (!io.window_resized) io.window_resized = !io.window_resized;
}

void set_current_context(GLFWwindow *window) {
    glfwMakeContextCurrent(window);
    GLFW_update_callbacks(window);
    ImGui::SetCurrentContext(io.imgui_context);
    console_log("Set new window context (%p)\n", window);
}

void window_switch_fullscreen() {
    auto window = io.window;

    auto primary_monitor = glfwGetPrimaryMonitor();
    auto videomode = glfwGetVideoMode(primary_monitor);
    io.window_fullscreen = !io.window_fullscreen;

    if (io.window_fullscreen) {
        // If we enable fullscreen mode, then we save window position,
        // set screen resolution to max monitor resolution, and
        // open window in fullscreen mode on primary monitor.
        // Max FPS clamps to refresh rate of the current videomode of the monitor.
        auto last_windowed_monitor = get_last_windowed_monitor();

        glfwGetWindowPos(window, &io.windowed_frame.x, &io.windowed_frame.y);
        glfwGetWindowSize(window, &io.windowed_frame.width, &io.windowed_frame.height);
        io.window_size.width = videomode->width;
        io.window_size.height = videomode->height;
        glfwSetWindowMonitor(window, last_windowed_monitor, 0, 0, videomode->width, videomode->height, videomode->refreshRate);
    } else {
        // If we disable fullscreen mode, then we reset screen resolution
        // to saved settings from windowed mode and open window in windowed mode.
        // Max FPS clamps to refresh rate of the current videomode of the monitor.
        int new_width = io.windowed_frame.width;
        int new_height = io.windowed_frame.height;
        io.window_size.width = new_width;
        io.window_size.height = new_height;
        glfwSetWindowMonitor(window, NULL, io.windowed_frame.x, io.windowed_frame.y, new_width, new_height, 0);
    }
    console_log("Switched window fullscreen mode (%s)\n", (io.window_fullscreen) ? "Fullscreen" : "Windowed");
}

void destroy_window(GLFWwindow *window) {
    glfwDestroyWindow(window);
    console_log("Window destroyed.\n");
}

Snake_IO *GetIO() {
    return &io;
}

void input_free_resources() {
    // Free all allocated resources.
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(io.imgui_context);
    glfwTerminate();
}

char *float_to_time(float time) {
    // H - Hours
    // M - Minutes
    // S - Seconds
    // [HH:MM:SS]
    // [00:00:00]
    char formatted_time[10+1];
    int seconds = (int)time;
    int minutes = seconds / 60;
    int hours = minutes / 60;
    seconds = seconds % 60;
    snprintf(&formatted_time[0], 10+1, "[%.2d:%.2d:%.2d]", hours, minutes, seconds);
    return &formatted_time[0];
}

char *float_to_time_with_millis(float time) {
    // H - Hours
    // M - Minutes
    // S - Seconds
    // L - Milliseconds
    // [HH:MM:SS.LLL]
    // [00:00:00.000]
    char formatted_time[15+1];
    float mul_time = time * 1000.0f;
    int millis = (int)mul_time;
    int seconds = millis / 1000;
    int minutes = seconds / 60;
    int hours = minutes / 60;
    millis = millis % 1000;
    snprintf(&formatted_time[0], 15+1, "[%.2d:%.2d:%.2d.%.3d]", hours, minutes, seconds, millis);
    return &formatted_time[0];
}

void console_log(const char *format, ...) {
    ZoneScoped;

    va_list args;
    va_start(args, format);

    int seconds = (int)io.frametime;
    int minutes = seconds / 60;
    int hours = minutes / 60;
    seconds = seconds % 60;

    char text[256];
    int written = vsnprintf(text, 256, format, args);
    if (written > 256) {
        char larger_text[2048];
        int larger_written = vsnprintf(larger_text, 2048, format, args);

        if (larger_written > 2048) {
            printf("[%.2d:%.2d:%.2d]: WARNING: Console print exceeds 2048 character limit! The rest is trimmed. (format was: \"%s\")\n", hours, minutes, seconds, format);
        } else {
            printf("[%.2d:%.2d:%.2d]: %s", hours, minutes, seconds, larger_text);
        }

    } else {
        printf("[%.2d:%.2d:%.2d]: %s", hours, minutes, seconds, text);
    }

    va_end(args);
}

void console_print(const char *format, ...) {
    ZoneScoped;

    va_list args;
    va_start(args, format);

    char text[256];
    int written = vsnprintf(text, 256, format, args);
    if (written > 256) {
        char larger_text[2048];
        int larger_written = vsnprintf(larger_text, 2048, format, args);

        if (larger_written > 2048) {
            int seconds = (int)io.frametime;
            int minutes = seconds / 60;
            int hours = minutes / 60;
            seconds = seconds % 60;

            printf("[%.2d:%.2d:%.2d]: WARNING: Console print exceeds 2048 character limit! The rest is trimmed. (format was: \"%s\"\n", hours, minutes, seconds, format);
        } else {
            printf("%s", larger_text);
        }

    } else {
        printf("%s", text);
    }

    va_end(args);
}

//
// --- GLFW ---
//
void init_glfw() {
    ZoneScoped;

    glfwSetErrorCallback(GLFW_error_callback);

    if (glfwInit()) {
        console_print("GLFW initialized.\n");
    } else {
        console_print("ERROR: Failed to initialize GLFW!\n");
        assert(false && "Failed to initialize GLFW");
        exit(EXIT_FAILURE);
    }

    // OpenGL 3.3, Core profile.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context.
    // Put  glfwGetPrimaryMonitor()  after the window's name to make it fullscreen.
    // ... "name", glfwGetPrimaryMonitor(), NULL);
    // GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    auto monitor = glfwGetPrimaryMonitor();
    auto videomode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, videomode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, videomode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, videomode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, videomode->refreshRate);

    Rectangle2Di window_rect;
    glfwGetMonitorWorkarea(monitor, &window_rect.x, &window_rect.y, &window_rect.width, &window_rect.height);
    
    io.window_position.x = window_rect.x;
    io.window_position.y = window_rect.y;
    io.window_size.width = window_rect.width;
    io.window_size.height = window_rect.height;

    auto window = create_window(io.window_size.width, io.window_size.height, "snake", io.window_fullscreen);

    if (glewInit() == GLEW_OK) {
        console_print("GLEW initialized.\n");
    } else {
        console_print("ERROR: Failed to initialize GLEW!\n");
        assert(false && "Failed to initialize GLEW");
        exit(EXIT_FAILURE);
    }
}

void GLFW_update_callbacks(GLFWwindow *window) {
    glfwSetCursorPosCallback(window, GLFW_cursor_pos_callback);
    glfwSetMouseButtonCallback(window, GLFW_mouse_button_callback);
    glfwSetCursorEnterCallback(window, GLFW_cursor_enter_window_callback);
    glfwSetScrollCallback(window, GLFW_scroll_callback);

    // Callback function for processing single press.
    // Keys which are being listened continuously are
    // in process_input().
    glfwSetKeyCallback(window, GLFW_single_key_press_callback);

    glfwSetWindowFocusCallback(window, GLFW_window_focus_callback);
    glfwSetWindowRefreshCallback(window, GLFW_window_refresh_callback);
    glfwSetWindowCloseCallback(window, GLFW_window_close_callback);
//    glfwSetWindowSizeCallback(window, GLFW_window_resize_callback);
    glfwSetFramebufferSizeCallback(window, GLFW_framebuffer_resize_callback);
}

void GLFW_cursor_pos_callback(GLFWwindow *window, double new_cursor_x, double new_cursor_y) {
    io.cursor_position.x = new_cursor_x;
    // We invert the Y-coordinate value since OpenGL's screen origin is the upper-left corner,
    // but we want the bottom-left one.
    io.cursor_position.y = abs(new_cursor_y - io.window_size.height); // screen_size.y == screen height.
}

void GLFW_mouse_button_callback(GLFWwindow *window, int button, int action, int flags) {
    // We currently handle only these button values:
    // 0 - Left (GLFW_MOUSE_BUTTON_LEFT)
    // 1 - Right (GLFW_MOUSE_BUTTON_RIGHT)
    // 2 - Middle (GLFW_MOUSE_BUTTON_MIDDLE)
    if (button < 0 || button > 2) return;

    if (action == GLFW_PRESS) {
        glfw_mouse_just_pressed[button] = true;
    }

/*
    if (action == GLFW_PRESS) {

        glfw_mouse_just_pressed[button] = true;

        io.mouse_clicked[button] = true;
        io.mouse_clicked_time[button] = glfwGetTime();
        io.mouse_down[button] = true;
        io.mouse_released[button] = false;
        io.mouse_down_duration_prev[button] = io.mouse_down_duration[button];
        io.mouse_down_duration[button] = 0.0f;
        io.mouse_clicked_pos[button] = io.cursor_position;

    } else if (action == GLFW_REPEAT) {

        io.mouse_clicked[button] = false;

    } else if (action == GLFW_RELEASE) {

        io.mouse_clicked[button] = false;
        io.mouse_down[button] = false;
        io.mouse_released[button] = true;
        io.mouse_down_duration_prev[button] = io.mouse_down_duration[button];
        io.mouse_down_duration[button] = 0.0f;
    }
    */
}

void GLFW_cursor_enter_window_callback(GLFWwindow *window, int entered) {
    io.cursor_in_window = entered;
}

void GLFW_scroll_callback(GLFWwindow *window, double offset_x, double offset_y) {
    io.mouse_wheel += (float)offset_y;
    io.mouse_wheel_horizontal += (float)offset_x;
}

void GLFW_single_key_press_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS)
        io.keys_down[key] = true;
    if (action == GLFW_RELEASE)
        io.keys_down[key] = false;
}

void GLFW_window_focus_callback(GLFWwindow *window, int focused) {
    io.window_focused = focused;
}

void GLFW_window_refresh_callback(GLFWwindow *window) {
    // renderer_draw(game_state);
    // glfwSwapBuffers(window);
}

void GLFW_window_close_callback(GLFWwindow *window) {
    // ...
}

/*
void GLFW_window_resize_callback(GLFWwindow *window) {
    // ...
}
*/

void GLFW_framebuffer_resize_callback(GLFWwindow *window, int new_width, int new_height) {
    resize_window(new_width, new_height);
}

void GLFW_error_callback(int error_code, const char *description) {
    console_print("GLFW ERROR: %s (Error code: %d)\n", description, error_code);
}
