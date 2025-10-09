#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include "Chess-Model.h"
#include <iostream>
#include <vector>
#include <string>

using std::vector;
using std::string;

// Global Variables needed to be used with the SDL_Window
// This is the defining convention used in the SDL3 package. So 
// even if I know that using global variables is poor programming practice
// I prefer not going against the conventions used in the package so that all works
// as intended.
static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;

// We'll store textures for all pieces: [color][type] using the array below.
// The first index represents the color of the piece:
// 0 = WHITE, 1 = BLACK
// The second index represents the color of the piece:
// 0 = PAWN, 1 = KNIGHT, 2 = BISHOP, 3 = ROOK, 4 = QUEEN, 5 = KING
static SDL_Texture* piece_textures[2][6];

// === UTILS ===

// Get the filename of a piece's image based on color and type
const char* getPieceFilename(piece_color color, piece_type type) {
    if (type == NONE) return nullptr;

    const char* colorStr = (color == WHITE) ? "White" : "Black";
    const char* typeStr = nullptr;

    switch (type) {
        case PAWN:   typeStr = "Pawn"; break;
        case KNIGHT: typeStr = "Knight"; break;
        case BISHOP: typeStr = "Bishop"; break;
        case ROOK:   typeStr = "Rook"; break;
        case QUEEN:  typeStr = "Queen"; break;
        case KING:   typeStr = "King"; break;
        default: return nullptr;
    }

    static char path[128];
    snprintf(path, sizeof(path), "assets/pieces/%s_%s.png", colorStr, typeStr);
    return path;
}

// Load all textures into piece_textures array
bool loadPieceTextures() {
    for (int c = 0; c < 2; ++c) {
        for (int t = 0; t < 6; ++t) {
            piece_type type;
            switch(t) {
                case 0: type = PAWN; break;
                case 1: type = KNIGHT; break;
                case 2: type = BISHOP; break;
                case 3: type = ROOK; break;
                case 4: type = QUEEN; break;
                case 5: type = KING; break;
            }

            const char* path = getPieceFilename(c == 0 ? WHITE : BLACK, type);
            SDL_Surface* surf = IMG_Load(path); // SDL3_image
            if (!surf) {
                SDL_Log("Failed to load %s: %s", path, SDL_GetError());
                return false;
            }

            piece_textures[c][t] = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_DestroySurface(surf); // SDL3
            if (!piece_textures[c][t]) {
                SDL_Log("Failed to create texture for %s: %s", path, SDL_GetError());
                return false;
            }
        }
    }
    return true;
}

// Initialize the board with starting pieces
void initialize_board(board& b) {
    // Clear all squares
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            b.board[r][c] = { NONE, WHITE }; // type NONE, color default WHITE

    // Place pawns
    for (int c = 0; c < 8; ++c) {
        b.board[6][c] = { PAWN, WHITE };
        b.board[1][c] = { PAWN, BLACK };
    }

    // Place major pieces
    b.board[7][0] = b.board[7][7] = { ROOK, WHITE };
    b.board[7][1] = b.board[7][6] = { KNIGHT, WHITE };
    b.board[7][2] = b.board[7][5] = { BISHOP, WHITE };
    b.board[7][3] = { QUEEN, WHITE };
    b.board[7][4] = { KING, WHITE };

    b.board[0][0] = b.board[0][7] = { ROOK, BLACK };
    b.board[0][1] = b.board[0][6] = { KNIGHT, BLACK };
    b.board[0][2] = b.board[0][5] = { BISHOP, BLACK };
    b.board[0][3] = { QUEEN, BLACK };
    b.board[0][4] = { KING, BLACK };
}

// Draw the board and pieces
void drawBoard(const board& b) {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            // Draw tile color (green/brown)
            bool dark = (row + col) % 2;
            if (dark)
                SDL_SetRenderDrawColor(renderer, 118, 150, 86, 255); // dark square
            else
                SDL_SetRenderDrawColor(renderer, 238, 238, 210, 255); // light square

            SDL_FRect tile = { float(col * TILE_SIZE), float(row * TILE_SIZE), float(TILE_SIZE), float(TILE_SIZE) };
            SDL_RenderFillRect(renderer, &tile);

            // Draw piece if present
            chess_piece piece = b.board[row][col];
            if (piece.type != NONE) {
                int colorIdx = (piece.color == WHITE) ? 0 : 1;
                int typeIdx;
                switch(piece.type) {
                    case PAWN: typeIdx = 0; break;
                    case KNIGHT: typeIdx = 1; break;
                    case BISHOP: typeIdx = 2; break;
                    case ROOK: typeIdx = 3; break;
                    case QUEEN: typeIdx = 4; break;
                    case KING: typeIdx = 5; break;
                    default: typeIdx = 0;
                }

                SDL_FRect dst = { float(col * TILE_SIZE), float(row * TILE_SIZE), float(TILE_SIZE), float(TILE_SIZE) };
                SDL_RenderTexture(renderer, piece_textures[colorIdx][typeIdx], nullptr, &dst);
            }
        }
    }
}

// === SDL CALLBACKS ===
static game current_game;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Failed to init SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Chess Board", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Failed to create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Initialize board and load textures
    initialize_board(current_game.game_board);

    if (!loadPieceTextures()) {
        SDL_Log("Failed to load chess piece textures.");
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    drawBoard(current_game.game_board);

    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    // Destroy piece textures
    for (int c = 0; c < 2; ++c)
        for (int t = 0; t < 6; ++t)
            if (piece_textures[c][t])
                SDL_DestroyTexture(piece_textures[c][t]);
}
