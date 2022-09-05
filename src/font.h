#ifndef PAWN_FONT_H
#define PAWN_FONT_H

// Font rendering.
#include <ft2build.h>
#include FT_FREETYPE_H

struct Glyph {
    unsigned int texture_id; // unnecessary?
    Vec2i size;
    Vec2i bearing;
    unsigned int advance;
};

struct Font {
    int width = 0;
    int height = 0;
    unsigned int vao = 0;
    unsigned int vbo = 0;
    Glyph glyphs[128]; // For now it's just ASCII.
    unsigned int textures[128];
    int max_glyph_length_px = 0;
    int max_glyph_height_px = 0;
};

Font load_font_from_file(const char *filepath, int pixel_width, int pixel_height);
void reload_font_from_file(Font *font, const char *filepath, int pixel_width, int pixel_height);
Vec2i calc_text_size(const char *text, Font *font, int length = 0, float scale = 1.0f);

static void load_font_chars(Font *font, FT_Face face);

#endif /* PAWN_FONT_H */
