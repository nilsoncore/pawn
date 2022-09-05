#include "immediate.h"
#include "input.h"
#include "renderer.h"

static Immediate imm;

//
// --- Functions ---
//
void imm_init() {
    auto io = GetIO();

    imm.frame_active = false;
    // imm.frame = get_rect(0.0f, 0.0f, 0.0f, 0.0f);
    imm.default_font_height = io->font.height / 3;
    imm.font = load_font_from_file("resources/fonts/Roboto-Regular.ttf", 0, imm.default_font_height);
    console_print("Immediate Font loaded (Roboto-Regular.ttf, %d)\n", imm.default_font_height);
    imm.scale = 1.0f;
    imm.margin = new_vec4f(0.0f);
    imm.cursor = new_vec2f(0.0f);

    imm.button_color = new_vec3f(1.0f, 0.0f, 0.0f);
    imm.button_label_color = new_vec3f(1.0f);
    imm.button_gap = io->window_size.height/256;
    imm.button_flags = ALIGN_LEFT;
    imm.button_label_flags = ALIGN_CENTER;

    imm.text_color = new_vec3f(1.0f);
    imm.text_flags = ALIGN_LEFT;
}

void imm_update() {
    auto io = GetIO();

    imm.button_gap = io->window_size.height/256;
    console_log("imm.button_gap: %f\n", imm.button_gap);
    imm.default_font_height = io->font.height / 3;
    reload_font_from_file(&imm.font, "resources/fonts/Roboto-Regular.ttf", 0, imm.default_font_height);
    console_log("Immediate Font reloaded (%s, %d)\n", "Roboto-Regular.ttf", imm.default_font_height);
}

Immediate *GetImmediate() {
    return &imm;
}

void imm_set_font(Font font) {
    imm.font = font;
}

void imm_begin_frame(Rectangle2D frame) {
    ZoneScoped;

    imm.frame = frame;
    imm.frame_active = true;

    imm.cursor.x = frame.x - frame.width;
    imm.cursor.y = frame.y + frame.height;
}

void imm_end_frame() {
    ZoneScoped;

    imm.frame_active = false;
}

void imm_text(const char *formatted_text, ...) {
    ZoneScoped;

    float x = imm.cursor.x + imm.margin.left;
    float y = imm.cursor.y - imm.font.height;

    char text[256];
    va_list args;
    va_start(args, formatted_text);
    int written = vsnprintf(text, 256, formatted_text, args);

    if (written > 256) {
        console_print("WARNING: Immediate text call exceeded 256 character limit! The rest is trimmed. (format was: \"%s\")\n", formatted_text);
    }

    va_end(args);

    draw_text(&imm.font, text, x, y, 1.0f, imm.text_color, imm.text_flags);

    // Advance cursor
    imm.cursor.y -= imm.font.height;
}

void imm_plain_text(const char *text, u64 text_size /*= NULL*/) {
    ZoneScoped;

    float x = imm.cursor.x + imm.margin.left;
    float y = imm.cursor.y - imm.font.height;

    draw_text(&imm.font, text, x, y, 1.0f, imm.text_color);

    // Advance cursor
    imm.cursor.y -= imm.font.height;
}

bool imm_checkbox(const char *label, bool *value) {
    assert(false && "Not implemented yet");
    return true;
}

// Move to other place
bool get_button_state(Rectangle2D bounding_box, bool *out_hovered, bool *out_held) {
    ZoneScoped;

    auto io = GetIO();

    Vec2f cursor = io->cursor_position;
    bool hovered = in_rect_frame(cursor.x, cursor.y, bounding_box);
    bool held = false;
    bool pressed = false;

    // Left mouse button has index '0'.
    if (hovered) {
        if (io->mouse_down[0]) {
            held = true;
        } else if (io->mouse_released[0]) {
            pressed = true;
        }
    }

    if (out_hovered) *out_hovered = hovered;
    if (out_held) *out_held = held;
    return pressed;
}

bool imm_button(const char *label, bool *out_hovered /*= NULL*/, bool *out_held /*= NULL*/, Vec2f size /*= new_vec2f(0, 0)*/) {
    ZoneScoped;

    Vec2i label_size = calc_text_size(label, &imm.font);
    // Vec2i button_padding = new_vec2i(10, 10);
    if (size.width == 0.0f) {
        size.width = label_size.width;   // + button_padding.x;
    }

    if (size.height == 0.0f) {
        size.height = label_size.height; // + button_padding.y;
    }

    Button button;
    // button.x = bounding_box.x;
    button.y = imm.cursor.y - size.height - imm.button_gap;
    button.size = size;
    button.scale = imm.scale;
    button.button_color = imm.button_color;
    button.button_flags = imm.button_flags;

    if (button.button_flags & ALIGN_RIGHT) {
        button.x = imm.frame.x + imm.frame.width - size.width;
    } else if (button.button_flags & ALIGN_LEFT) {
        button.x = imm.cursor.x + size.width;
    } else /* ALIGN_CENTER_HEIGHT */ {
        button.x = imm.frame.x;
    }

    Rectangle2D bounding_box = new_rect(
        button.x,
        button.y,
        size.width,
        size.height
    );

    bool hovered, held;
    bool pressed = get_button_state(bounding_box, &hovered, &held);

    button.label = label;
    button.label_color = imm.button_label_color;
    button.label_flags = imm.button_label_flags;

    draw_button(&imm.font, button);

    // Advance cursor
    imm.cursor.y -= bounding_box.height + bounding_box.height + imm.button_gap;

    if (out_hovered) *out_hovered = hovered;
    if (out_held) *out_held = held;

    return pressed;
}

bool imm_transparent_button(const char *label, Vec2f size /*= new_vec2f(0, 0)*/) {
    assert(false && "Not implemented yet");
    return true;
}

bool imm_combobox(const char *label, int *current_item, const char *items, int items_count, int popup_max_height /*= -1*/) {
    assert(false && "Not implemented yet");
    return true;
}
