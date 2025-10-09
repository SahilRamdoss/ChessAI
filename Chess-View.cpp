#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include "Chess-Model.h"
#include <iostream>
#include <format>
#include <vector>
#include <string>

using std::string;
using std::vector;

// Global Variables needed to be used with the SDL_Window
// This is the defining convention used in the SDL3 package. So
// even if I know that using global variables is poor programming practice
// I prefer not going against the conventions used in the package so that all works
// as intended.
static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;

// We'll store textures for all pieces: [color][type] using the array below.
// The first index represents the color of the piece:
// 0 = WHITE, 1 = BLACK
// The second index represents the color of the piece:
// 0 = PAWN, 1 = KNIGHT, 2 = BISHOP, 3 = ROOK, 4 = QUEEN, 5 = KING
static SDL_Texture *piece_textures[2][6];

// Our game that we have created and initialises the board automatically. We MUST DECLARE
// THIS GLOBALLY AS WE HAVE NO OTHER PLACE WHERE WE CAN DECLARE THIS LOCALLY IN OUR CODE correctly
static game current_game;

/**
 * @brief This function returns the file path of where the png image of
 * the chess piece pass is found
 *
 * @param color is the color of the chess piece
 * @param type is the type of the chess piece(Pawn, Knight, etc...)
 *
 * @return the path
 */
string get_piece_file_name(piece_color color, piece_type type)
{
    if (type == NONE)
    {
        return "";
    }

    // Use of ternary operator here. Getting the color of the piece as a string
    string colorStr = (color == WHITE) ? "White" : "Black";
    string typeStr = "";
    string path = "";

    // DO NOT CHANGE THE STRING AS IT FOLLOWS THE NAMING CONVENTIONS I USED FOR MY FILES
    // Getting the type of the piece as a string
    switch (type)
    {
    case PAWN:
        typeStr = "Pawn";
        break;
    case KNIGHT:
        typeStr = "Knight";
        break;
    case BISHOP:
        typeStr = "Bishop";
        break;
    case ROOK:
        typeStr = "Rook";
        break;
    case QUEEN:
        typeStr = "Queen";
        break;
    case KING:
        typeStr = "King";
        break;
    default:
        return "";
    }

    // Creating the file path where the png image for the chess piece if found
    path = format("assets/pieces/{}_{}.png", colorStr, typeStr);
    return path;
}

/**
 * @brief This procedure load all textures for our chess pieces into piece_textures array
 * 
 * @return true if the textures have been loaded correctly into the array
 */
bool load_piece_textures()
{
    for (int piece_color_value = FIRST_COLOR; piece_color_value < LAST_COLOR; piece_color_value++)
    {
        for (int piece_type_value = FIRST_TYPE; piece_type_value < LAST_TYPE; piece_type_value++)
        {
            // Getting the color of the current piece
            piece_color color_of_piece = static_cast<piece_color>(piece_color_value);
            // Getting the type of the current piece
            piece_type type_of_piece = static_cast<piece_type>(piece_type_value);

            // Getting the file path of where the png image for that chess piece is found
            string file_path = get_piece_file_name(color_of_piece, type_of_piece);

            // Converting the string to a C style string as SDL works with const char *
            //, not with string data type
            const char *path = file_path.c_str();

            // Initailizing the surface for the image so that we can manipulate it
            SDL_Surface *surf = IMG_Load(path);

            // Checks if surf is nullptr, because if it is it means we failed to load the image
            if (!surf)
            {
                // Print the error message to the console
                SDL_Log("Failed to load %s: %s", path, SDL_GetError());
                return false;
            }

            piece_textures[piece_color_value][piece_type_value] = SDL_CreateTextureFromSurface(renderer, surf);

            // Since we already stored the surface in our array, we destroy it
            SDL_DestroySurface(surf);
            // Checks for nullptr, as this means we failed to create the texture
            if (!piece_textures[piece_color_value][piece_type_value])
            {
                SDL_Log("Failed to create texture for %s: %s", path, SDL_GetError());
                return false;
            }
        }
    }
    return true;
}

/**
 * @brief draws the board and adds the pieces
 * 
 * @param board_object is the chess board being drawn
 */
void draw_board(const board &board_object)
{
    for (int rank = 0; rank < BOARD_SIZE; rank++)
    {
        for (int file = 0; file < BOARD_SIZE; file++)
        {
            // Checking if we should draw a light or dark square
            bool dark = (rank + file) % 2;
            if (dark)
                SDL_SetRenderDrawColor(renderer, 118, 150, 86, 255); // dark square
            else
                SDL_SetRenderDrawColor(renderer, 238, 238, 210, 255); // light square

            // Creating a tile
            SDL_FRect tile = {float(file * TILE_SIZE), float(rank * TILE_SIZE), float(TILE_SIZE), float(TILE_SIZE)};
            SDL_RenderFillRect(renderer, &tile);

            // Draw piece if present
            chess_piece piece = board_object.get_piece_at(rank,file);
            if (piece.type != NONE)
            {
                int colorId = (piece.color == WHITE) ? WHITE : BLACK;
                int typeId;
                switch (piece.type)
                {
                case PAWN:
                    typeId = PAWN;
                    break;
                case KNIGHT:
                    typeId = KNIGHT;
                    break;
                case BISHOP:
                    typeId = BISHOP;
                    break;
                case ROOK:
                    typeId = ROOK;
                    break;
                case QUEEN:
                    typeId = QUEEN;
                    break;
                case KING:
                    typeId = KING;
                    break;
                default:
                    typeId = 0;
                }

                SDL_FRect dst = {float(file * TILE_SIZE), float(rank * TILE_SIZE), float(TILE_SIZE), float(TILE_SIZE)};
                SDL_RenderTexture(renderer, piece_textures[colorId][typeId], nullptr, &dst);
            }
        }
    }
}




//...........................................................................................................................................................
//...SSSSSSS....DDDDDDDDD....LLLL..............CCCCCCC.......AAAAA.....LLLL.......LLLL.......BBBBBBBBBB......AAAAA.......CCCCCCC....KKKK...KKKKK..SSSSSSS....
//..SSSSSSSSS...DDDDDDDDDD...LLLL.............CCCCCCCCC......AAAAA.....LLLL.......LLLL.......BBBBBBBBBBB.....AAAAA......CCCCCCCCC...KKKK..KKKKK..SSSSSSSSS...
//..SSSSSSSSSS..DDDDDDDDDDD..LLLL............CCCCCCCCCCC....AAAAAA.....LLLL.......LLLL.......BBBBBBBBBBB....AAAAAA.....CCCCCCCCCCC..KKKK.KKKKK...SSSSSSSSSS..
//.SSSSS..SSSS..DDDD...DDDD..LLLL............CCCC...CCCCC...AAAAAAA....LLLL.......LLLL.......BBBB...BBBB....AAAAAAA....CCCC...CCCCC.KKKKKKKKK...SSSSS..SSSS..
//.SSSSS........DDDD....DDDD.LLLL........... CCC.....CCC...AAAAAAAA....LLLL.......LLLL.......BBBB...BBBB...AAAAAAAA...CCCC.....CCC..KKKKKKKK....SSSSS........
//..SSSSSSS.....DDDD....DDDD.LLLL........... CCC...........AAAAAAAA....LLLL.......LLLL.......BBBBBBBBBBB...AAAAAAAA...CCCC..........KKKKKKKK.....SSSSSSS.....
//...SSSSSSSSS..DDDD....DDDD.LLLL........... CCC...........AAAA.AAAA...LLLL.......LLLL.......BBBBBBBBBB....AAAA.AAAA..CCCC..........KKKKKKKK......SSSSSSSSS..
//.....SSSSSSS..DDDD....DDDD.LLLL........... CCC..........AAAAAAAAAA...LLLL.......LLLL.......BBBBBBBBBBB..AAAAAAAAAA..CCCC..........KKKKKKKKK.......SSSSSSS..
//........SSSSS.DDDD....DDDD.LLLL........... CCC.....CCC..AAAAAAAAAAA..LLLL.......LLLL.......BBBB....BBBB.AAAAAAAAAAA.CCCC.....CCC..KKKK.KKKKK.........SSSS..
//.SSSS....SSSS.DDDD...DDDDD.LLLL............CCCC...CCCCC.AAAAAAAAAAA..LLLL.......LLLL.......BBBB....BBBB.AAAAAAAAAAA..CCCC...CCCCC.KKKK..KKKK..SSSS....SSS..
//.SSSSSSSSSSSS.DDDDDDDDDDD..LLLLLLLLLL......CCCCCCCCCCC.AAAA....AAAA..LLLLLLLLLL.LLLLLLLLLL.BBBBBBBBBBBBAAAA....AAAA..CCCCCCCCCCC..KKKK..KKKKK.SSSSSSSSSSS..
//..SSSSSSSSSS..DDDDDDDDDD...LLLLLLLLLL.......CCCCCCCCCC.AAAA.....AAAA.LLLLLLLLLL.LLLLLLLLLL.BBBBBBBBBBB.AAAA.....AAAA..CCCCCCCCCC..KKKK...KKKKK.SSSSSSSSSS..
//...SSSSSSSS...DDDDDDDDD....LLLLLLLLLL........CCCCCCC..CAAAA.....AAAA.LLLLLLLLLL.LLLLLLLLLL.BBBBBBBBBB.BAAAA.....AAAA...CCCCCCC....KKKK...KKKKK..SSSSSSSS...
//...........................................................................................................................................................

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    // Initialising SDL3
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Failed to init SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Creating the window
    if (!SDL_CreateWindowAndRenderer("Chess Board", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        SDL_Log("Failed to create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!load_piece_textures())
    {
        SDL_Log("Failed to load chess piece textures.");
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    draw_board(current_game.game_board);

    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    // Destroy piece textures
    for (int c = 0; c < 2; ++c)
        for (int t = 0; t < 6; ++t)
            if (piece_textures[c][t])
                SDL_DestroyTexture(piece_textures[c][t]);
}
