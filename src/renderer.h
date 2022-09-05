#ifndef PAWN_RENDERER_H
#define PAWN_RENDERER_H

#define GLEW_STATIC
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "pawn.h"
#include "font.h"

//
// --- Structs ---
//
struct Shader {
    unsigned int id;
    const char *vertex_shader;
    const char *fragment_shader;
};

struct Shader_Source {
    const char *vertex_shader;
    const char *fragment_shader;
};

struct Vertex_Buffer {
    unsigned int id;
    unsigned int size;
    const void *data;
};

struct Index_Buffer {
    unsigned int id;
    unsigned int amount;
    const unsigned int *data;
};

struct Renderer_Info {
    const char *gpu_vendor;
    const char *gpu_renderer;
    const char *gl_version;
    const char *glsl_version;
};

struct Screen_Text {
    float x;
    float y;
    int width; // Length in pixels.
    int height; // Max glyph height in pixels.
    const char *text;
    Font *font;
    Vec3f color;
    u16 flags;
};

struct Texture {
    unsigned int id;
    const char *filepath;
    int width;
    int height;
    int channels;
};

struct Sprite {
    Texture texture;
};

struct Draw_Command {
    u32 vertex_offset;
    u32 index_offset;
    u32 elements;
};

// typedef u32 Color_RGBA8;

struct Draw_Vertex_Batch {
    unsigned int shader;
    Vec2f position;
    Vec3f color;
};

struct Draw_Texture_Vertex {
    Vec2f position;
    Vec3f color;
    Vec2f uv;
};

struct Draw_Vertex {
    Vec2f position;
    Vec2f uv;
    Vec3f color;
};

struct Draw_List {
    Array<Draw_Command>  commands;
    Array<u32>           indices;
    Array<Draw_Vertex>   vertices;
    u64                  draw_flags;
};

static Draw_List g_draw_list;

//
// --- Enums ---
//
enum Align_Flag {
    ALIGN_CENTER_WIDTH = 0,
    ALIGN_CENTER_HEIGHT = 0,
    ALIGN_CENTER = ALIGN_CENTER_WIDTH | ALIGN_CENTER_HEIGHT,

    ALIGN_LEFT = (1 << 0),
    ALIGN_UP = (1 << 1),

    ALIGN_RIGHT = (1 << 2),
    ALIGN_DOWN = (1 << 3),

    BUTTON_SIZE_TEXT_RELATIVE = 0,

    BUTTON_WIDTH_CONSTANT = (1 << 4),
    BUTTON_HEIGHT_CONSTANT = (1 << 5),
    BUTTON_SIZE_CONSTANT = BUTTON_WIDTH_CONSTANT | BUTTON_HEIGHT_CONSTANT,

    COLUMN_ALIGN_CENTER_HEIGHT = (1 << 6)
};

//
// --- Functions ---
//
void init_renderer();
void renderer_free_resources();
void renderer_draw(u32 game_state);

/*inline*/ Rectangle2D draw_text(Font *font, Screen_Text text, float scale = 1.0f);
Rectangle2D draw_text(Font *font, const char *text, float x, float y, float scale, Vec3f color, u16 flags = 0);
Rectangle2D draw_text2(Font *font, const char *text, float x, float y, float scale, Vec3f color, u16 flags = 0);
void draw_text_column(int amount, Rectangle2D *return_array, Font *font, const char **text_array, float x, float y, float text_gap, float scale, Vec3f color, u16 flags = 0);
/*inline*/ void draw_text_column(int amount, Rectangle2D *return_array, Font *font, Screen_Text text, float button_gap, float scale = 1.0f);
Rectangle2D draw_button(Font *font, const char *text, float x, float y, float scale, Vec3f text_color, Vec3f button_color, u16 flags = 0);
Rectangle2D draw_button(Font *font, Button button);
void draw_button_column(int amount, Rectangle2D *return_array, Font *font, const char **text_array, float x, float  y, float button_gap, float scale, Vec3f text_color, Vec3f button_color, u16 button_flags = 0, u16 text_flags = 0);
void draw_rect(Rectangle2D rect, Vec3f color, int draw_mode = GL_TRIANGLES);
void draw_triangle(Triangle2D tri, Vec3f color, int draw_mode = GL_TRIANGLES);
void draw_rects(int amount, Rectangle2D *rects, Vec3f color, int draw_mode = GL_TRIANGLES);
Screen_Text new_screen_text(const char *text, Font *font, float x, float y, Vec3f color = new_vec3f(1.0f), u16 flags = 0);
Rectangle2D draw_dropdown(Dropdown drop, u16 flags = 0, int draw_mode = GL_TRIANGLES);
Rectangle2D draw_checkbox(Checkbox box, int draw_mode = GL_TRIANGLES);
void set_viewport(int x, int y, int width, int height);
bool in_rect_frame(float x, float y, Rectangle2D rect);
bool in_rect_frame(int x, int y, Rectangle2Di rect);
bool is_in_range(int value, int min_value, int max_value);
bool in_window_frame(int x, int y, Rectangle2Di window);
void print_rect(Rectangle2D rect, const char *name);

Rectangle2D new_rect(float x, float y, float width, float height);
Rectangle2D new_rect(Vec2f position, Vec2f size);
Triangle2D new_triangle(float x0, float y0, float x1, float y1, float x2, float y2);

Texture load_texture_from_file(const char *filepath);
void draw_sprite(Rectangle2D rect, Texture texture);
void draw_sprite(float x, float y, Texture texture);

// Internal functions
static void draw_text_glyphs(Font *font, const char *text, int text_size, float x, float y, float scale);
static char *read_file(const char *filepath);

// Shaders
static unsigned int load_shader(const char *vertex_shader_filepath, const char *fragment_shader_filepath);
static unsigned int compile_shader(unsigned int type, char *source);
static unsigned int create_shader(char *vertex_shader, char *fragment_shader);
static void gl_clear_error();
static bool gl_log_call(const char *function, const char *file, int line);

#endif /* PAWN_RENDERER_H */
