#include "pawn.h"
#include "input.h"
#include "font.h"
#include "renderer.h"

// @MemoryLeak
Font load_font_from_file(const char *filepath, int pixel_width, int pixel_height) {
    ZoneScoped;

    Font font;
    font.width = pixel_width;
    font.height = pixel_height;

    // Generate vertex buffer
    glGenBuffers(1, &font.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, font.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    // Generate vertex array
    glGenVertexArrays(1, &font.vao);
    glBindVertexArray(font.vao);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    FT_Library freetype;
    if (FT_Init_FreeType(&freetype)) {
        console_log("FreeType ERROR: Couldn't initialize FreeType library!\n");
    }

    FT_Face face;
    if (FT_New_Face(freetype, filepath, 0, &face)) {
        console_log("FreeType ERROR: Couldn't load font file '%s'!\n", filepath);
        font.max_glyph_length_px = -1;
        font.max_glyph_height_px = -1;
        return font;
    }

    FT_Set_Pixel_Sizes(face, font.width, font.height);

    // Disable byte-alignment restriction.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Generate new textures since we are creating new font instance.
    // We don't need to generate in 'reload_font' though,
    // because we just reuse already generated textures.
    glGenTextures(128, &font.textures[0]);

    load_font_chars(&font, face);

    FT_Done_Face(face);
    FT_Done_FreeType(freetype);

    return font;
}

void reload_font_from_file(Font *font, const char *filepath, int pixel_width, int pixel_height) {
    ZoneScoped;

    font->width = pixel_width;
    font->height = pixel_height;

    // Gerenate and bind vertex buffer and vertex array
    // if they are not generated yet. 
    if (font->vbo < 1) { // If vertex buffer is generated and binded, its 'id' should be > 0
        glGenBuffers(1, &font->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, font->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    }

    if (font->vao < 1) { // If vertex array is generated and binded, its 'id' should be > 0
        glGenVertexArrays(1, &font->vao);
        glBindVertexArray(font->vao);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
        glEnableVertexAttribArray(0);
    }

    FT_Library freetype;
    if (FT_Init_FreeType(&freetype)) {
        console_log("FreeType ERROR: Couldn't initialize FreeType library!\n");
    }

    FT_Face face;
    if (FT_New_Face(freetype, filepath, 0, &face)) {
        console_log("FreeType ERROR: Couldn't load font file '%s'!\n", filepath);
        font->max_glyph_length_px = -1;
        font->max_glyph_height_px = -1;
        return;
    }

    FT_Set_Pixel_Sizes(face, font->width, font->height);

    // Disable byte-alignment restriction.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    load_font_chars(font, face);

    FT_Done_Face(face);
    FT_Done_FreeType(freetype);
}

Vec2i calc_text_size(const char *text, Font *font, int length /*= 0*/, float scale /*= 1.0f*/) {
    ZoneScoped;

    if (length <= 0) {
        length = string_length(text);
    }

    Vec2i result = new_vec2i(0);

    For (length) {
        Glyph glyph = font->glyphs[text[it]];
        result.width += (glyph.advance >> 6) * scale;
        if (glyph.size.y > result.height) {
            result.height = glyph.size.y;
        }
    }

    return result;
}

static void load_font_chars(Font *font, FT_Face face) {
    int glyph_length_px;
    int glyph_height_px;
    int max_glyph_length_px = 0;
    int max_glyph_height_px = 0;

    for (unsigned char c = 0; c < 128; c++) {
        // Load character glyph.
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            console_log("FreeType ERROR: Failed to load a Glyph for char '%c'!\n", c);
            continue;
        }

        // Generate texture.
        // unsigned int texture;
        // glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, font->textures[c]);
        auto bitmap = face->glyph->bitmap;
        glTexImage2D(
            /* target         */ GL_TEXTURE_2D,
            /* level          */ 0,
            /* internalFormat */ GL_RED,
            /* width          */ bitmap.width,
            /* height         */ bitmap.rows,
            /* border         */ 0,
            /* format         */ GL_RED,
            /* type           */ GL_UNSIGNED_BYTE,
            /* data           */ bitmap.buffer
        );

        // Set texture options.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Now store character for later use.
        Glyph glyph = {
            font->textures[c],
            new_vec2i(bitmap.width, bitmap.rows),
            new_vec2i(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        font->glyphs[c] = glyph;

        // Evaluate max glyph parameters.
        glyph_length_px = (glyph.advance >> 6);
        if (glyph_length_px > max_glyph_length_px) {
            max_glyph_length_px = glyph_length_px;
        }

        glyph_height_px = glyph.size.y;
        if (glyph_height_px > max_glyph_height_px) {
            max_glyph_height_px = glyph_height_px;
        }
    }

    font->max_glyph_length_px = max_glyph_length_px;
    font->max_glyph_height_px = max_glyph_height_px;
}
