// fopen()
#define _CRT_SECURE_NO_WARNINGS 1

#include <stb/stb_image.h>

#include "renderer.h"
#include "common.h"
#include "input.h"
#include "immediate.h"

Renderer_Info renderer_info;

// Globals from snake.cpp
extern bool imgui_states[];
extern u32 game_state;

static unsigned int lighting_shader;
static unsigned int glyphs_shader;
static unsigned int rect_shader;
static unsigned int texture_shader;

static unsigned int square_vbo;
static unsigned int square_vao;
static unsigned int rect_vbo; // rect = Rectangle2D
static unsigned int rect_vao;
static unsigned int texture_vao;
static unsigned int texture_vbo;

static glm::mat4 projection;
static glm::mat4 text_projection;

static unsigned int square_projection_location;
static unsigned int square_model_location;
static unsigned int square_color_location;
static unsigned int rect_projection_location;
static unsigned int rect_color_location;
static unsigned int text_projection_location;
static unsigned int text_color_location;
static unsigned int texture_sampler_location;
static unsigned int texture_projection_location;

static unsigned int font_textures[128];

static char *read_file(const char *filepath) {
    FILE *file;
    char *data;
    
    int open_result = fopen_s(&file, filepath, "r"); // Open file in 'read' mode.
    if (open_result) {
        console_log("ERROR: Couldn't open '%s' shader file!\n", filepath);
        fclose(file);
    }

    // Get vertex shader file size.
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    // Allocate memory to contain the whole file.
    data = (char *) malloc(sizeof(char) * file_size);
    if (!data) {
        console_log("ERROR: Couldn't allocate %llu bytes of memory for '%s' shader file!\n", sizeof(char) * file_size, filepath);
        return NULL;
    }

    size_t bytes_read = fread(data, sizeof(char), file_size, file);
    if (bytes_read != file_size) {
        // bytes_read is always != file_size because file contains some additional metadata
        // apart from the actual data, so we just memset the tailing with zeros.
        memset(&data[bytes_read], 0, file_size - bytes_read); 
    }

    fclose(file);
    return data; 
}

static unsigned int load_shader(const char *vertex_shader_filepath, const char *fragment_shader_filepath) {
    ZoneScoped;
    
    FILE *file;
    const char *filepath;
    char *vertex_src = NULL;
    char *fragment_src = NULL;

    if (vertex_shader_filepath) {
        vertex_src = read_file(vertex_shader_filepath);
    }

    if (fragment_shader_filepath)
        fragment_src = read_file(fragment_shader_filepath);

    unsigned int program = glCreateProgram();
    unsigned int compiled_vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_src);
    unsigned int compiled_fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_src);

    glAttachShader(program, compiled_vertex_shader);
    glAttachShader(program, compiled_fragment_shader);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(compiled_vertex_shader);
    glDeleteShader(compiled_fragment_shader);

    if (program) {
        console_print("Shader '%s' loaded.\n", vertex_shader_filepath);
    } else {
        console_print("ERROR: Failed to load '%s' shader!\n", vertex_shader_filepath);
        assert(false);
    }

    free(vertex_src);
    free(fragment_src);
    return program;
}

void init_renderer() {
    ZoneScoped;

    auto io = GetIO();

    //
    // Init OpenGL related information.
    //
    renderer_info.gpu_vendor   = (const char *)glGetString(GL_VENDOR);
    renderer_info.gpu_renderer = (const char *)glGetString(GL_RENDERER);
    renderer_info.gl_version   = (const char *)glGetString(GL_VERSION);
    renderer_info.glsl_version = "#version 330";
    console_print("GPU Vendor: %s, GPU Renderer: %s\n", renderer_info.gpu_vendor, renderer_info.gpu_renderer);
    console_print("OpenGL/Driver version: %s\n", renderer_info.gl_version);
    console_print("GLSL: %s\n", renderer_info.glsl_version);

    //
    // Load shaders.
    //
    lighting_shader = load_shader("resources/shaders/lighting_vertex.glsl", "resources/shaders/lighting_fragment.glsl");
    glyphs_shader   = load_shader("resources/shaders/glyphs_vertex.glsl",   "resources/shaders/glyphs_fragment.glsl");
    rect_shader     = load_shader("resources/shaders/rect_vertex.glsl",     "resources/shaders/rect_fragment.glsl");
    texture_shader  = load_shader("resources/shaders/texture_vertex.glsl",  "resources/shaders/texture_fragment.glsl");

    //
    // Create 'Vertex Buffer' and 'Vertex Array' objects for square tiles.
    //
    glGenBuffers(1, &square_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, square_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SQUARE_VERTICES), SQUARE_VERTICES, GL_STATIC_DRAW);

    glGenVertexArrays(1, &square_vao);
    glBindVertexArray(square_vao);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
    glEnableVertexAttribArray(0);

    //
    // Create 'Vertex Buffer' and 'Vertex Array' objects for rectangles.
    //
    glGenBuffers(1, &rect_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, rect_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 2, NULL, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &rect_vao);
    glBindVertexArray(rect_vao);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
    glEnableVertexAttribArray(0);

    //
    // Create 'Vertex Buffer' and 'Vertex Array' objects for textures.
    //
    glGenBuffers(1, &texture_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, texture_vbo);
    glBufferData(
        GL_ARRAY_BUFFER, 
        (2 /* position */ + 3 /* color */ + 2 /* uv */) * 6 /* vertices */ * sizeof(float),
        NULL, 
        GL_DYNAMIC_DRAW
    );

    glGenVertexArrays(1, &texture_vao);
    glBindVertexArray(texture_vao);

    // Position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *) (5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Init uniforms location.
    square_projection_location  = glGetUniformLocation(lighting_shader, "projection");
    square_model_location       = glGetUniformLocation(lighting_shader, "model");
    square_color_location       = glGetUniformLocation(lighting_shader, "color");

    rect_projection_location    = glGetUniformLocation(rect_shader,     "projection");
    rect_color_location         = glGetUniformLocation(rect_shader,     "color");

    text_projection_location    = glGetUniformLocation(glyphs_shader,   "text_projection");
    text_color_location         = glGetUniformLocation(glyphs_shader,   "text_color");

    texture_sampler_location    = glGetUniformLocation(texture_shader,  "texture1");
    texture_projection_location = glGetUniformLocation(texture_shader,  "projection");

    // Init ImGui.
    IMGUI_CHECKVERSION();
    io->imgui_context = ImGui::CreateContext();
    if (io->imgui_context) {
        console_print("ImGui context created.\n");
    } else {
        console_print("Failed to create ImGui context!\n");
        assert(false);
    }
    ImGuiIO &imgui_io = ImGui::GetIO();

    // Disable windows states saving for now.
    imgui_io.IniFilename = NULL;
    imgui_io.WantSaveIniSettings = false;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(io->window, true);
    ImGui_ImplOpenGL3_Init(renderer_info.glsl_version);

    ImGui::SetCurrentContext(io->imgui_context);

    // Enable blend for drawing text.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(io->clear_color.r, io->clear_color.g, io->clear_color.b, io->clear_color.a);

    float w = io->window_size.width;
    float h = io->window_size.height;
    text_projection = glm::ortho(0.0f, w, 0.0f, h);
    projection = glm::ortho(-w, w, -h, h);

    int def_font_height = h / 24;
    io->font = load_font_from_file("resources/fonts/Roboto-Regular.ttf", 0, def_font_height);
    console_print("IO Font loaded (Roboto-Regular.ttf, %d)\n", def_font_height);
    imm_init();
}

bool in_window_frame(int x, int y, Rectangle2Di window) {
    return (x >= window.x && x <= window.x + window.width)
    && (y >= window.y && y <= window.y + window.height);
}

void set_viewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

//
// --- Shaders ---
//
static unsigned int compile_shader(unsigned int type, char *source) {
    ZoneScoped;

    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (!result) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*) malloc(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        console_print("OpenGL ERROR: Failed to compile '%s' shader!\n... %s\n", (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT", message);
        glDeleteShader(id);
        free(message);
        return 0;
    }

    return id;
}

static unsigned int create_shader(char *vertex_shader, char *fragment_shader) {
    ZoneScoped;
    
    unsigned int program = glCreateProgram();
    unsigned int compiled_vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader);
    unsigned int compiled_fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);

    glAttachShader(program, compiled_vertex_shader);
    glAttachShader(program, compiled_fragment_shader);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(compiled_vertex_shader);
    glDeleteShader(compiled_fragment_shader);

    return program;
}

//
// --- OpenGL Error handling ---
//
static void gl_clear_error() {
    while (glGetError() != GL_NO_ERROR);
}

static bool gl_log_call(const char *function, const char *file, int line) {
    while (GLenum error = glGetError()) {
        console_log("OpenGL ERROR: (%d): %s %s: %d\n", error, function, file, line);
        return false;
    }
    return true;
}

bool in_rect_frame(float x, float y, Rectangle2D rect) {
    return (x <= rect.x + rect.width && x >= rect.x - rect.width) 
    && (y <= rect.y + rect.height && y >= rect.y - rect.height);
}

bool in_rect_frame(int x, int y, Rectangle2Di rect) {
    return (x <= rect.x + rect.width && x >= rect.x - rect.width)
    && (y <= rect.y + rect.height && y >= rect.y - rect.height);
}

bool is_in_range(int value, int min_value, int max_value) {
    return (value >= min_value && value <= max_value);
}

inline Rectangle2D draw_text(Font *font, Screen_Text text, float scale /*= 1.0f*/) {
    return draw_text(font, text.text, text.x, text.y, scale, text.color, text.flags);
}

inline void draw_text_column(int amount, Rectangle2D *return_array, Font *font, Screen_Text text, float button_gap, float scale /*= 1.0f*/) {
    // draw_text_column(amount, return_array, font, screen
}

Rectangle2D draw_text(Font *font, const char *text, float x, float y, float scale, Vec3f color, u16 flags /*= 0*/) {
    ZoneScoped;

    glUseProgram(glyphs_shader);
    glUniformMatrix4fv(text_projection_location, 1, false, &text_projection[0][0]);
    glUniform3fv(text_color_location, 1, &color.x);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(font->vao);

    float original_x = x;

    // px = pixels
    unsigned int text_length_px = 0;
    unsigned int text_max_glyph_height_px = 0;
    int text_size = string_length(text);
    
    For (text_size) {
        Glyph glyph = font->glyphs[text[it]];
        text_length_px += (glyph.advance >> 6) * scale;
        if (glyph.size.y > text_max_glyph_height_px) {
            text_max_glyph_height_px = glyph.size.y;
        }
    }

    int half_width = text_length_px / 2;
    int half_height = text_max_glyph_height_px / 2;

    // Return text's X position.
    x = original_x;

    /*
    if (flags & ALIGN_CENTER_WIDTH) {
        x -= half_width;
    } else if (flags & ALIGN_RIGHT_WIDTH) {
        x -= half_width * 2;
    }
    */



    if (flags & ALIGN_LEFT) {

    } else if (flags & ALIGN_RIGHT) {
        ;x -= half_width * 2;
    } else /* ALIGN_CENTER_WIDTH */ {
        x -= half_width;
    }

    Rectangle2D rect;
    rect.x = x + half_width;
    rect.y = y + half_height;
    rect.width = half_width;
    rect.height = half_height;

    draw_text_glyphs(font, text, text_size, x, y, scale);
    
    return rect;
}

static void draw_text_glyphs(Font *font, const char *text, int text_size, float x, float y, float scale) {
    glBindBuffer(GL_ARRAY_BUFFER, font->vbo);

    For (text_size) {
        Glyph glyph = font->glyphs[text[it]];

        float xpos = x + glyph.bearing.x * scale;
        float ypos = y - (glyph.size.y - glyph.bearing.y) * scale;

        float w = glyph.size.x * scale;
        float h = glyph.size.y * scale;

        // Update VBO for each character.
        float vertices[6][4] = {
            { xpos,     ypos + h, 0.0f, 0.0f },
            { xpos,     ypos,     0.0f, 1.0f },
            { xpos + w, ypos,     1.0f, 1.0f },

            { xpos,     ypos + h, 0.0f, 0.0f },
            { xpos + w, ypos,     1.0f, 1.0f },
            { xpos + w, ypos + h, 1.0f, 0.0f }
        };

        // Render glyph texture over quad.
        glBindTexture(GL_TEXTURE_2D, glyph.texture_id);

        // Update content of VBO memory.
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 

        // Render quad.
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels).
        x += (glyph.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64).
    }
}

// no scale.
Screen_Text new_screen_text(const char *text, Font *font, float x, float y, Vec3f color /*= new_vec3f(0.0f)*/, u16 flags /*= 0*/) {
    Screen_Text result;
    result.text = text;
    result.x = x;
    result.y = y;
    result.color = color;
    result.flags = flags;
    result.width = 0; // Length in pixels.
    result.height = 0; // Max glyph height in pixels.
    
    int text_size = string_length(text);
    Glyph glyph;
    For (text_size) {
        glyph = font->glyphs[text[it]];
        result.width += (glyph.advance >> 6); // Should have been (... * scale).
        if (glyph.size.y > result.height) {
            result.height = glyph.size.y;
        }
    }

    return result;
}


// 'scale' doesn't work.
//
// Important Note: 'text_gap' is distance between previous and next text centers,
// not between their Rectangle2D faces.
void draw_text_column(int amount, Rectangle2D *return_array, Font *font, const char **text_array, float x, float y, float text_gap, float scale, Vec3f color, u16 flags /*= TEXT_ALIGN_ORIGIN*/) {
    ZoneScoped;
    
    glUseProgram(glyphs_shader);
    glUniformMatrix4fv(text_projection_location, 1, false, &text_projection[0][0]);
    glUniform3fv(text_color_location, 1, &color.x);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(font->vao);

    float original_x = x;

    // px = pixels
    unsigned int whole_text_max_length_px = 0;
    unsigned int whole_text_max_glyph_height_px = 0;
    const char *current_text;
    Glyph glyph;

    // @Robustness !!!
    // @malloc
    int *text_sizes = (int *)malloc(amount * sizeof(int));
    
    for (int text_num = 0; text_num < amount; text_num++) {
        current_text = text_array[text_num];
        text_sizes[text_num] = string_length(current_text);
        
        For (text_sizes[text_num]) {
            glyph = font->glyphs[current_text[it]];
            whole_text_max_length_px += (glyph.advance >> 6) * scale;
            if (glyph.size.y > whole_text_max_glyph_height_px) {
                whole_text_max_glyph_height_px = glyph.size.y;
            }
        }
    }
    
    for (int text_num = 0; text_num < amount; text_num++) {
        unsigned int current_text_length_px = 0;
        unsigned int current_text_max_glyph_height_px = 0;
        current_text = text_array[text_num];

        For (text_sizes[text_num]) {
            glyph = font->glyphs[current_text[it]];
            current_text_length_px += (glyph.advance >> 6) * scale;
            if (glyph.size.y > current_text_max_glyph_height_px) {
                current_text_max_glyph_height_px = glyph.size.y;
            }
        }

        int half_width;
        int half_height;

        if (flags & BUTTON_WIDTH_CONSTANT) {
            half_width = whole_text_max_length_px / 2;
        } else /*BUTTON_SIZE_TEXT_RELATIVE*/ {
            half_width = current_text_length_px / 2;
        }

        if (flags & BUTTON_HEIGHT_CONSTANT) {
            half_height = whole_text_max_glyph_height_px / 2;
        } else /*BUTTON_SIZE_TEXT_RELATIVE*/ {
            half_height = current_text_max_glyph_height_px / 2;
        }

        // Return text's X position.
        x = original_x;

        if (flags & ALIGN_CENTER_WIDTH) {
            x -= half_width;
        } else if (flags & ALIGN_RIGHT) {
            x -= half_width * 2;
            // x -= current_text_length_px;
        }

        // Set Rectangle2D's X position.
        Rectangle2D rect;
        rect.x = x + half_width;

        // Set Rectangle2D's Y position.
        if (text_num == 0) {
            rect.y = y;
            if (flags & COLUMN_ALIGN_CENTER_HEIGHT) {
            rect.y += amount/2 * (rect.height + text_gap);
            }
        } else {
            rect.y = return_array[text_num-1].y - return_array[text_num-1].height - text_gap;
        }

        // Set Rectangle2D's width and height.
        rect.width = half_width;
        rect.height = half_height;
        return_array[text_num] = rect;

        y = rect.y - half_height;

        draw_text_glyphs(font, current_text, text_sizes[text_num], x, y, scale);
    }
}

void renderer_draw(u32 game_state) {
    ZoneScoped;

    auto io = GetIO();

    io->frametime = glfwGetTime();
    io->frametime_delta = 1000.0f * (io->frametime - io->frametime_last);
    io->frametime_last = io->frametime;

    // Don't draw if window is minimized;
    if (io->window_size == new_vec2f(0)) {
        // glfwSwapBuffers(io->window);
        // glFlush();
        glfwPollEvents();
        return;
    }

    glClearColor(io->clear_color.r, io->clear_color.g, io->clear_color.b, io->clear_color.a);
    glClear(GL_COLOR_BUFFER_BIT);

    if (io->window_resized) {
        io->window_resized = false;

        // @Speed
        // Loading font from file every window resize is okay enough for now,
        // but later we really should store font in memory.
        // @MemoryLeak
        float w = io->window_size.width;
        float h = io->window_size.height;
        console_log("Window resized (%.fx%.f)\n", w, h);

        int io_def_font_height = h / 24;
        io->default_font_height = io_def_font_height;
        reload_font_from_file(&io->font, "resources/fonts/Roboto-Regular.ttf", 0, io_def_font_height);
        console_log("IO Font reloaded (%s, %d)\n", "Roboto-Regular.ttf", io_def_font_height);

        imm_update();

        text_projection = glm::ortho(0.0f, w, 0.0f, h);
        projection = glm::ortho(-w, w, -h, h);
    }

    if (game_state & TITLE_SCREEN) {
        if (game_state & SETTINGS_SCREEN) {
            draw_settings_screen();
        } else {
            draw_title_screen();
        }
    } else if (game_state & PLAY) {
        if (game_state & DEBUG) {
            // draw_all_tails_on_screen();
        } else {
            // draw_square_tiles();
        }

        if (game_state & PAUSE_SCREEN) {
            if (game_state & SETTINGS_SCREEN) {
                draw_settings_screen();
            } else {
                draw_pause_screen();
            }
        }
    }
    
    //
    // --- ImGui Render ---
    //
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    make_imgui_layout();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(io->window);
    glfwPollEvents();
    glFlush();
}

void renderer_free_resources() {
    ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplGlfw_Shutdown();

    glDeleteProgram(lighting_shader);
    glDeleteProgram(glyphs_shader);
}

Rectangle2D new_rect(float x, float y, float width, float height) {
    Rectangle2D rect = { x, y, width, height };
    return rect;
}

Rectangle2D new_rect(Vec2f position, Vec2f size) {
    Rectangle2D rect = { position.x, position.y, size.width, size.height };
    return rect;
}

void draw_triangle(Triangle2D tri, Vec3f color, int draw_mode /*= GL_TRIANGLES*/) {
    ZoneScoped;
    
    glUseProgram(rect_shader);
    glUniformMatrix4fv(rect_projection_location, 1, false, &text_projection[0][0]);
    glUniform3fv(rect_color_location, 1, &color.x);

    glBindVertexArray(rect_vao);
    
    float vertices[6] = {
        tri.x0, tri.y0,
        tri.x1, tri.y1,
        tri.x2, tri.y2
    };

    glBindBuffer(GL_ARRAY_BUFFER, rect_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glDrawArrays(draw_mode, 0, 3);
}

void draw_triangle2(Triangle2D tri, Vec3f color, int draw_mode /*= GL_TRIANGLES*/) {
    ZoneScoped;

    
}

void draw_rect(Rectangle2D rect, Vec3f color, int draw_mode /*= GL_TRIANGLES*/) {
    ZoneScoped;
    
    glUseProgram(rect_shader);
    glUniformMatrix4fv(rect_projection_location, 1, false, &text_projection[0][0]);
    glUniform3fv(rect_color_location, 1, &color.x);

    glBindVertexArray(rect_vao);

    float vertices[6][2] = {
        { rect.x - rect.width, rect.y - rect.height },
        { rect.x - rect.width, rect.y + rect.height },
        { rect.x + rect.width, rect.y + rect.height },

        { rect.x + rect.width, rect.y - rect.height },
        { rect.x - rect.width, rect.y - rect.height },
        { rect.x + rect.width, rect.y + rect.height }
    };

    glBindBuffer(GL_ARRAY_BUFFER, rect_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glDrawArrays(draw_mode, 0, 6);
}

void draw_rects(int amount, Rectangle2D *rects, Vec3f color, int draw_mode /*= GL_TRIANGLES*/) {
    ZoneScoped;

    glUseProgram(rect_shader);
    glUniformMatrix4fv(rect_projection_location, 1, false, &text_projection[0][0]);
    glUniform3fv(rect_color_location, 1, &color.x);

    glBindVertexArray(rect_vao);
    glBindBuffer(GL_ARRAY_BUFFER, rect_vbo);
    
    For (amount) {
        Rectangle2D rect = rects[it];

        float vertices[6][2] {
            { rect.x - rect.width, rect.y - rect.height },
            { rect.x - rect.width, rect.y + rect.height },
            { rect.x + rect.width, rect.y + rect.height },

            { rect.x + rect.width, rect.y - rect.height },
            { rect.x - rect.width, rect.y - rect.height },
            { rect.x + rect.width, rect.y + rect.height }
        };

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(draw_mode, 0, 6);
    }
}

void print_rect(Rectangle2D rect, const char *name) {
    console_log("Rectangle '%s':\n"
        " - x: %.f\n"
        " - y: %.f\n"
        " - width: %d\n"
        " - height: %d\n",
        name, rect.x, rect.y, rect.width, rect.height);
}

// 'gap' argument is referred to as gap between inside and outside rectangles of a checkbox.
Rectangle2D draw_checkbox(Checkbox box, int draw_mode /*= GL_TRIANGLES*/) {
    ZoneScoped;

    auto io = GetIO();

    // @Temp, make it as an actual argument.
    int gap = box.width/6;
    int gap_size = box.width/4;

    Rectangle2D outside_rect = new_rect(box.x, box.y, box.width, box.height);
    draw_rect(outside_rect, box.color, draw_mode);

    Rectangle2D inside_bg_rect = outside_rect; // bg = background
    inside_bg_rect.width -= gap;
    inside_bg_rect.height -= gap;

    Vec3f clear_color;
    clear_color = vec3f_from_vec4f(io->clear_color);
    draw_rect(inside_bg_rect, clear_color);

    if (box.checked) {
        Rectangle2D inside_rect = inside_bg_rect;
        inside_rect.width -= gap_size;
        inside_rect.height -= gap_size;
        draw_rect(inside_rect, box.color, draw_mode);
    }

    return outside_rect;
}

// Make 'Font' argument?
// @Incomplete: use drop.color !!!
Rectangle2D draw_dropdown(Dropdown drop, u16 flags /*= 0*/, int draw_mode /*= GL_TRIANGLES*/) {
    ZoneScoped;

    glUseProgram(rect_shader);
    glUniformMatrix4fv(rect_projection_location, 1, false, &text_projection[0][0]);
    glUniform3fv(rect_color_location, 1, &drop.bg_color.x);

    auto io = GetIO();

    // Draw text.
    Rectangle2D text_rect = draw_text(
        &io->font,
        &drop.items[drop.active_item],
        drop.x,
        drop.y - drop.height,
        1.0f,
        drop.text_color,
        ALIGN_CENTER
    );

    Rectangle2D drop_rect = new_rect(text_rect.x, text_rect.y, text_rect.width, text_rect.height);
    
    // Draw only for debug purposes.
    // draw_rect(drop_rect, bg_color, GL_LINE_STRIP);

    Rectangle2D select_rect;
    select_rect.width = drop_rect.height;
    select_rect.height = drop_rect.height;
    select_rect.x = (drop_rect.x + drop_rect.width) + select_rect.width;
    select_rect.y = drop_rect.y;

    draw_rect(select_rect, drop.text_color, draw_mode);

    float half_width = select_rect.width/2;
    float half_height = select_rect.height/2;

    Triangle2D select_tri;
    if (drop.opened) {
        // Make triangle "look down".
        select_tri.x0 = select_rect.x - half_width;
        select_tri.y0 = select_rect.y - half_height;
        select_tri.x1 = select_rect.x + half_width;
        select_tri.y1 = select_rect.y - half_height;
        select_tri.x2 = select_rect.x;
        select_tri.y2 = select_rect.y + half_height;
    } else {
        // Make triangle "look up".
        select_tri.x0 = select_rect.x - half_width;
        select_tri.y0 = select_rect.y + half_height;
        select_tri.x1 = select_rect.x + half_width;
        select_tri.y1 = select_rect.y + half_height;
        select_tri.x2 = select_rect.x;
        select_tri.y2 = select_rect.y - half_height;
    }

    draw_triangle(select_tri, drop.bg_color, draw_mode);

    Rectangle2D whole_dropdown_rect = drop_rect;
    whole_dropdown_rect.x += select_rect.width;
    whole_dropdown_rect.width += select_rect.width;
    
    return whole_dropdown_rect;
}

Triangle2D new_triangle(float x0, float y0, float x1, float y1, float x2, float y2) {
    Triangle2D result = { x0, y0, x1, y1, x2, y2 };
    return result;
};

// 'scale' doesn't work properly.
void draw_button_column(int amount, Rectangle2D *return_array, Font *font, const char **text_array, float x, float y, float button_gap, float scale, Vec3f text_color, Vec3f button_color, u16 button_flags /*= 0*/, u16 text_flags /*= 0*/) {
    ZoneScoped;
    
    float original_x = x;

    // Prepare uniforms.
    glUseProgram(rect_shader);
    glUniformMatrix4fv(rect_projection_location, 1, false, &text_projection[0][0]);
    glUniform3fv(rect_color_location, 1, &button_color.x);
    glUseProgram(glyphs_shader);
    glUniformMatrix4fv(text_projection_location, 1, false, &text_projection[0][0]);
    glUniform3fv(text_color_location, 1, &text_color.x);

    unsigned int max_text_length_in_pixels = 0;
    unsigned int max_text_max_glyph_height_in_pixels = 0;
    const char *button_text;
    Glyph glyph;
    
    for (int button = 0; button < amount; button++) {
        button_text = text_array[button];
        int text_size = string_length(button_text);
        For (text_size) {
            glyph = font->glyphs[button_text[it]];
            max_text_length_in_pixels += (glyph.advance >> 6) * scale;
            if (glyph.size.y > max_text_max_glyph_height_in_pixels) {
                max_text_max_glyph_height_in_pixels = glyph.size.y;
            }
        }
    }

    for (int button = 0; button < amount; button++) {
        unsigned int text_length_in_pixels = 0;
        unsigned int text_max_glyph_height_in_pixels = 0;
        button_text = text_array[button];
        int text_size = string_length(button_text);
        For (text_size) {
            glyph = font->glyphs[button_text[it]];
            text_length_in_pixels += (glyph.advance >> 6) * scale;
            if (glyph.size.y > text_max_glyph_height_in_pixels) {
                text_max_glyph_height_in_pixels = glyph.size.y;
            }
        }

        Rectangle2D rect;

        if (button_flags & BUTTON_WIDTH_CONSTANT) {
            rect.width = max_text_length_in_pixels / 2;
        } else /*BUTTON_TEXT_RELATIVE*/ {
            rect.width = text_length_in_pixels;
        }

        if (button_flags & BUTTON_HEIGHT_CONSTANT) {
            rect.height = max_text_max_glyph_height_in_pixels;
        } else /*BUTTON_HEIGHT_TEXT_RELATIVE*/ {
            rect.height = text_max_glyph_height_in_pixels;
        }

        if (button == 0) {
            rect.y = y;
            if (button_flags & COLUMN_ALIGN_CENTER_HEIGHT) {
                rect.y += amount/2 * (rect.height + button_gap);
            }
        } else {
            rect.y = return_array[button-1].y - return_array[button-1].height*2 - button_gap;
        }

        x = original_x;

        if (!(button_flags & ALIGN_CENTER_WIDTH)) {
            // x -= rect.width;
            x += rect.width;
        } else {
            // x += rect.width;
        }

        rect.x = x;
        return_array[button] = rect;
        y = rect.y;

        // Draw button
        draw_rect(rect, button_color);

        x -= text_length_in_pixels;

        if (text_flags & ALIGN_CENTER_WIDTH) {
            x += text_length_in_pixels / 2;
        }

        if (text_flags & ALIGN_CENTER_HEIGHT) {
            y -= rect.height / 3;
        }

        // Draw text
        glUseProgram(glyphs_shader);
        glBindVertexArray(font->vao);
        glActiveTexture(GL_TEXTURE0);

        draw_text_glyphs(font, button_text, text_size, x, y, scale);
    }
}

Rectangle2D draw_button(Font *font, const char *text, float x, float y, float scale, Vec3f text_color, Vec3f button_color, u16 flags /*= 0*/) {
    ZoneScoped;

    Glyph glyph;
    unsigned int text_length_in_pixels = 0;
    unsigned int text_max_glyph_height_in_pixels = 0;
    int text_size = string_length(text);
    For (text_size) {
        glyph = font->glyphs[text[it]];
        text_length_in_pixels += (glyph.advance >> 6) * scale;
        if (glyph.size.y > text_max_glyph_height_in_pixels) {
            text_max_glyph_height_in_pixels = glyph.size.y;
        }
    }

    Rectangle2D rect;
    rect.x = x;
    rect.y = y;
    rect.width = text_length_in_pixels;
    rect.height = text_max_glyph_height_in_pixels;

    if (flags & ALIGN_CENTER_WIDTH) {
        x -= text_length_in_pixels / 2;
    }

    if (flags & ALIGN_CENTER_HEIGHT) {
        y -= text_max_glyph_height_in_pixels / 2;
    }

    // Draw button.
    draw_rect(rect, button_color);

    // Draw text.
    //
    // @Copy from 'draw_text()'.
    // Activate corresponding render state.
    glUseProgram(glyphs_shader);
    glUniformMatrix4fv(text_projection_location, 1, false, &text_projection[0][0]);
    glUniform3fv(text_color_location, 1, &text_color.x);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(font->vao);

    // Iterate through all characters in text.
    draw_text_glyphs(font, text, text_size, x, y, scale);

    return rect;
}

Rectangle2D draw_button(Font *font, Button button) {
    ZoneScoped;

    const char *label = button.label;

    Glyph glyph;
    unsigned int text_length_in_pixels = 0;
    unsigned int text_max_glyph_height_in_pixels = 0;
    int text_size = string_length(label);
    For (text_size) {
        glyph = font->glyphs[label[it]];
        text_length_in_pixels += (glyph.advance >> 6) * button.scale;
        if (glyph.size.y > text_max_glyph_height_in_pixels) {
            text_max_glyph_height_in_pixels = glyph.size.y;
        }
    }

    float x = button.x;
    float y = button.y;

    Rectangle2D rect;
    rect.x = x;
    rect.y = y;
    if (button.size == new_vec2f(0.0f)) {
        rect.width = text_length_in_pixels;
        rect.height = text_max_glyph_height_in_pixels;
    } else {
        rect.width = button.size.width;
        rect.height = button.size.height;
    }

    // Draw button.
    draw_rect(rect, button.button_color);

    // Draw text.
    //
    // @Copy from 'draw_text()'.
    // Activate corresponding render state.
    glUseProgram(glyphs_shader);
    glUniformMatrix4fv(text_projection_location, 1, false, &text_projection[0][0]);
    glUniform3fv(text_color_location, 1, &button.label_color.x);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(font->vao);

    u16 flags = button.label_flags;
    
    // Label flags.
    if (flags & ALIGN_LEFT) {
        x -= text_length_in_pixels;
    } else if (flags & ALIGN_RIGHT) {
        
    } else /* ALIGN_CENTER_WIDTH */ {
        x -= text_length_in_pixels / 2;
    }

    if (flags & ALIGN_UP) {

    } else if (flags & ALIGN_DOWN) {
        y -= text_max_glyph_height_in_pixels;
    } else /* ALIGN_CENTER_HEIGHT */ {
        y -= text_max_glyph_height_in_pixels / 2;
    }

    // Iterate through all characters in text.
    draw_text_glyphs(font, label, text_size, x, y, button.scale);

    return rect;
}

Texture load_texture_from_file(const char *filepath) {
    ZoneScoped;

    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *image = stbi_load(filepath, &width, &height, &channels, 0);
    if (image) {
        glTexImage2D(
            /* target         */ GL_TEXTURE_2D,
            /* level          */ 0,
            /* internalFormat */ GL_RGB,
            /* width          */ width,
            /* height         */ height,
            /* border         */ 0,
            /* format         */ GL_RGB,
            /* type           */ GL_UNSIGNED_BYTE,
            /* data           */ image
        );
        glGenerateMipmap(GL_TEXTURE_2D);
        console_log("Texture '%s' has been successfully loaded.\n", filepath);
    } else {
        console_log("Failed to load Texture from file '%s'!\n", filepath);
    }
    stbi_image_free(image);

    Texture texture;
    texture.id = texture_id;
    texture.filepath = filepath;
    texture.width = width;
    texture.height = height;
    texture.channels = channels;
    return texture;
}

void draw_sprite(Rectangle2D rect, Texture texture) {
    ZoneScoped;

    glUseProgram(texture_shader);
    glUniformMatrix4fv(texture_projection_location, 1, false, &text_projection[0][0]);
    glUniform1i(texture_sampler_location, 0);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glBindVertexArray(texture_vao);

    float vertices[] = {
        // Position                                  // Color            // Texture coords
        rect.x - rect.width, rect.y - rect.height,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom left
        rect.x - rect.width, rect.y + rect.height,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f, // Top left
        rect.x + rect.width, rect.y + rect.height,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top right

        rect.x + rect.width, rect.y - rect.height,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom right
        rect.x - rect.width, rect.y - rect.height,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom left
        rect.x + rect.width, rect.y + rect.height,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f  // Top right
    };

    glBindBuffer(GL_ARRAY_BUFFER, texture_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void draw_sprite(float x, float y, Texture texture) {
    Rectangle2D rect = get_rect(x, y, texture.width, texture.height);
    draw_sprite(rect, texture);
}
