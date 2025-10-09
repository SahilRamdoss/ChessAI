#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Chess-Model.h"
#include <iostream>
#include <format>
#include <vector>
#include <string>

/**
 * @brief draws the board and adds the pieces
 * 
 * @param app_object is the chess app
 * @param game_object is the game object
 */
void draw_board(const SDLStructures &app_object, const game game_object)
{
    board board_object = game_object.game_board;

    for (int rank = 0; rank < BOARD_SIZE; rank++)
    {
        for (int file = 0; file < BOARD_SIZE; file++)
        {
            // Checking if we should draw a light or dark square
            bool dark = (rank + file) % 2;
            if (dark)
                SDL_SetRenderDrawColor(app_object.renderer, 118, 150, 86, 255); // dark square
            else
                SDL_SetRenderDrawColor(app_object.renderer, 238, 238, 210, 255); // light square

            // Creating a tile
            SDL_FRect tile = {float(file * TILE_SIZE), float(rank * TILE_SIZE), float(TILE_SIZE), float(TILE_SIZE)};
            SDL_RenderFillRect(app_object.renderer, &tile);

            // Draw piece if present
            chess_piece piece = board_object.get_piece_at(rank,file);
            if (piece.type != NONE)
            {
                int colorId = (piece.color == WHITE) ? WHITE : BLACK;
                int typeId = piece.type;
                SDL_FRect dst = {float(file * TILE_SIZE), float(rank * TILE_SIZE), float(TILE_SIZE), float(TILE_SIZE)};
                SDL_RenderTexture(app_object.renderer, app_object.piece_textures[colorId][typeId], nullptr, &dst);
            }
        }
    }
}


int main(int argc, char *argv[])
{
    SDLStructures app_structure;
    game current_game;


    bool running = true;
    SDL_Event event;

    // Initialising SDL3
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Failed to init SDL: %s", SDL_GetError());
        return 1; // Indicating to the OS that an error has occured
    }

    if (!SDL_CreateWindowAndRenderer("Chess Board", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &app_structure.window, &app_structure.renderer))
    {
        SDL_Log("Failed to create window/renderer: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    if (!app_structure.load_piece_textures())
    {
        SDL_Log("Failed to load piece textures.");
        SDL_DestroyRenderer(app_structure.renderer);
        SDL_DestroyWindow(app_structure.window);
        SDL_Quit();
        return 1;
    }

    //  Keep running until the quit event is not detected
    while (running)
    {
        // Loops over all events in SDL events queue
        // We then handle each event in the while loop
        while (SDL_PollEvent(&event))
        {
            // Handling the quit event
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(app_structure.renderer, 0, 0, 0, 255);
        SDL_RenderClear(app_structure.renderer);

        // Drawing the chess board
        draw_board(app_structure, current_game);

        SDL_RenderPresent(app_structure.renderer);
    }

    // Clean up textures
    for (int color_index = 0; color_index < 2; color_index++)
    {
        for (int type_index = 0; type_index < 6; type_index++)
        {
            SDL_DestroyTexture(app_structure.piece_textures[color_index][type_index]);
        }
    }

    SDL_DestroyRenderer(app_structure.renderer);
    SDL_DestroyWindow(app_structure.window);
    SDL_Quit();
}