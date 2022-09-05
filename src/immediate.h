#ifndef PAWN_IMMEDIATE_H
#define PAWN_IMMEDIATE_H

#include "common.h"
#include "font.h"

struct Immediate {
    bool frame_active;
    Rectangle2D frame;
    int default_font_height; // Default font height = io.default_font_height / 3
    Font font;
    float scale;
    Vec4f margin;
    Vec2f cursor;

    Vec3f button_color;
    Vec3f button_label_color;
    float button_gap;
    u16 button_flags;
    u16 button_label_flags;

    Vec3f text_color;
    u16 text_flags;
};

/*
  Immediate mode interface

  Usage example:

void func() {
    imm_default_interface_set_font(&roboto);
    imm_default_interface_add_flags(TEXT_ALIGN_RIGHT);

    Rectangle rect;
    imm_begin_frame(rect);

    Font roboto;

    imm_text("Resolution:");
    imm_text("Window Mode:");
    imm_text("VSync:");

    imm_end_interface();

    rect; // new rect
    imm_begin_interface(rect);

    imm_combobox("2560x1440", "1920x1080", "1280x720");
    imm_combobox("Fullscreen", "Windowed");
    imm_checkbox(NULL, screen.vsync);
}

*/

//
// --- Functions ---
//
Immediate *GetImmediate();
void imm_init();
void imm_update();
void imm_set_font(Font font);
void imm_begin_frame(Rectangle2D frame);
void imm_end_frame();
void imm_text(const char *formatted_text, ...);
void imm_plain_text(const char *text, u64 text_size = NULL);
bool imm_checkbox(const char *label, bool *value);
bool imm_button(const char *label, bool *out_hovered = NULL, bool *out_held = NULL, Vec2f size = new_vec2f(0, 0));
inline bool imm_button(const char *label, Vec2f size) { return imm_button(label, NULL, NULL, size); };
bool imm_transparent_button(const char *label, Vec2f size = new_vec2f(0, 0));
bool imm_combobox(const char *label, int *current_item, const char *items, int items_count, int popup_max_height = -1);

bool get_button_state(Rectangle2D bounding_box, bool *out_hovered, bool *out_held);

#endif /* PAWN_IMMEDIATE_H */
