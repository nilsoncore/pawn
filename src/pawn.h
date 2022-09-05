#ifndef PAWN_PAWN_H
#define PAWN_PAWN_H

#include "common.h"

//
// --- Constants ---
//
const float SQUARE_VERTICES[] = {
    // Top-left triangle
    -0.5f,  0.5f,
     0.5f,  0.5f,
    -0.5f, -0.5f,
    // Bottom-right triangle
     0.5f,  0.5f,
     0.5f, -0.5f,
    -0.5f, -0.5f
};

const float TEXTURE_VERTICES[] {
     // Position          // Color            // Texture coordinates
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top left 
};

const int TEXTURE_INDICES[] = {
    0, 1, 3, // First triangle
    1, 2, 3  // Seconds triangle
};

//
// --- Structs ---
//
struct Frametime {
    float last = 0.0f;
    float current = 0.0f;
    float delta = 0.0f;
};

enum Piece_Kind {
    EMPTY = 0,
    KING = 1,
    QUEEN = 2,
    ROOK = 3,
    BISHOP = 4,
    KNIGHT = 5,
    PAWN = 6
};

struct Piece {
    int player_id;
    Piece_Kind kind;
    // Texture *texture;
};

struct Board_Square {
    int row;
    int column;
    Piece *piece;
};

const int PLAYER_NAME_LENGTH = 128;

struct Player {
    s64 id;
    char name[PLAYER_NAME_LENGTH];
    s32 elo;

    float average_time_per_move;
};

struct Board_Player {
    Player player;
    
    s32 id;
    s64 moves_made;
    float average_time_per_move;
    float move_time_left;
    float total_time_left;
};

const int BOARD_WIDTH = 8;
const int BOARD_HEIGHT = 8;

struct Board {
    s32 rows;
    s32 columns;
    // Board_Square squares[BOARD_WIDTH][BOARD_HEIGHT];
    Array<Board_Square> squares;
    
    // s32 player_count;
    // Board_Player *players; // Array<Board_Player> players[player_count]
    Array<Board_Player> players;
    s32 player_turn;
};

struct Button {
    float x;
    float y;
    Vec2f size;
    float scale;
    Vec3f button_color;
    u16 button_flags;

    const char *label;
    Vec3f label_color;
    u16 label_flags;
};

struct Dropdown {
    float x;
    float y;
    int width;
    int height;
    int items_count;
    int active_item;
    const char *items;
    Vec3f bg_color;
    Vec3f text_color;
    bool opened;
};

struct Checkbox {
    float x;
    float y;
    union {
        int width;
        int height;
    };
    Vec3f color;
    bool *value_ptr;
    bool checked;
};

//
// --- Enums ---
//
enum Imgui_State {
    DRAW_EDIT_WINDOW = 0,
    DRAW_DEMO_WINDOW = 1,
    DRAW_CONSTANTS_WINDOW = 2,
    DRAW_GLOBALS_WINDOW = 3,
    DRAW_INPUT_WINDOW = 4
};

enum Game_State {
    PLAY = (1 << 0),
    DEBUG = (1 << 1),
    TITLE_SCREEN = (1 << 2),
    PAUSE_SCREEN = (1 << 3),
    SETTINGS_SCREEN = (1 << 4),
};

//
// --- Functions ---
//
void draw_title_screen();
void draw_settings_screen();
void draw_pause_screen();

void make_imgui_layout();
void print_game_state(u32 game_state);
void game_exit();

Board create_board(Allocator *allocator, s32 rows, s32 columns, s32 player_count);

#endif /* PAWN_PAWN_H */
