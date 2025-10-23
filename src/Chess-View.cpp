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
void draw_board(const SDLStructures &app_object, const game &game_object)
{
    board board_object = game_object.game_board;

    for (int rank = 0; rank < BOARD_SIZE; rank++)
    {
        for (int file = 0; file < BOARD_SIZE; file++)
        {
            // Checking if we should draw a light or dark square on the bord
            bool dark = (rank + file) % 2;
            if (dark)
                SDL_SetRenderDrawColor(app_object.renderer, 196, 135, 135, 255); // dark square
            else
                SDL_SetRenderDrawColor(app_object.renderer, 235, 215, 215, 255); // light square

            // Creating a tile for the chesss board
            SDL_FRect tile = {float(file * TILE_SIZE), float(rank * TILE_SIZE), float(TILE_SIZE), float(TILE_SIZE)};
            SDL_RenderFillRect(app_object.renderer, &tile);

            // Draw the chess piece if present
            chess_piece piece = board_object.get_piece_at(rank, file);
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

piece_type select_promotion_piece(const SDLStructures &app_object, const game &game_object, piece_color color)
{
    bool piece_selected = false; // Used to keep track whether a piece has been selected or not
    bool quit = false; // Flag to know if user wants to quit the game
    SDL_Event event; // Used to store an SDL event in the SDL event queue

    float x_top_left_corner = float(PROMOTION_BLOCK_X_TOP_LEFT * TILE_SIZE); // x-coordinate of top-left corner of promotion block
    float y_top_left_corner = float(((color == WHITE) ? PROMOTION_BLOCK_Y_TOP_LEFT_WHITE : PROMOTION_BLOCK_Y_TOP_LEFT_BLACK) * TILE_SIZE);// y-coordinate of top-left corner of promotion block
    float block_width = float(TILE_SIZE * PROMOTION_BLOCK_WIDTH_TILE); // Width of the promotion block
    float block_height = float(TILE_SIZE); // Height of promotion block

    // 
    /**
     * @brief draw the promotion block
     * 
     * Use of lambda function to create this sub-code which is needed inside the select_promotion_piece procedure.
     * This lambda function heavily depends on many of the parameters and variables used in the select_promotion_piece.
     * Hence to simplify the code, we make use of the lambda function
     * We capture all outside variables as reference
     */
    auto display_promotion_block = [&]()
    {
        int colorId = color;
        int typeId = KNIGHT;
        SDL_FRect block = {x_top_left_corner, y_top_left_corner, block_width, block_height};
        SDL_SetRenderDrawColor(app_object.renderer, 209, 202, 202, 255);
        SDL_RenderFillRect(app_object.renderer, &block);

        for (int x_coor = PROMOTION_BLOCK_X_TOP_LEFT; x_coor < PROMOTION_BLOCK_X_TOP_LEFT + PROMOTION_BLOCK_WIDTH_TILE; x_coor++)
        {
            SDL_FRect dst = {float(x_coor * TILE_SIZE), y_top_left_corner, float(TILE_SIZE), float(TILE_SIZE)};
            SDL_RenderTexture(app_object.renderer, app_object.piece_textures[colorId][typeId], nullptr, &dst);
            typeId++;
        }
    };

    while (!piece_selected)
    {
        while (SDL_PollEvent(&event))
        {
            // For safety in case an issue happens when selecting the piece
            if (event.type == SDL_EVENT_QUIT)
            {
                quit = true;
                break;
            }

            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                int mouse_x = event.button.x;
                int mouse_y = event.button.y;

                // Checking if mouse click is made inside the promotion block
                if (mouse_x < x_top_left_corner || mouse_x > (x_top_left_corner + block_width) || mouse_y < y_top_left_corner || mouse_y > (y_top_left_corner + TILE_SIZE))
                {
                    continue;
                }
                else
                {
                    // Get the piece selected based on order of the tile selected
                    int tile_number = ((mouse_x - x_top_left_corner) / TILE_SIZE) + 1;
                    return (piece_type)tile_number;
                }
            }
        }

        // If quit button is pressed, exit the game
        if (quit)
        {
            break;
        }

        // Re-drawing the board and the block every frame
        draw_board(app_object, game_object);
        display_promotion_block();
        SDL_RenderPresent(app_object.renderer);
    }

    return NONE;
}

int main(int argc, char *argv[])
{
    SDLStructures app_structure;              // The SDL APP
    game current_game;                        // The chess game

    // Initialising the fields of the game struct
    current_game.active_player = WHITE;
    current_game.outcome = UNDETERMINED;
    
    move current_move = {{-1, -1}, {-1, -1}}; // Move made by the current player on the board
    bool piece_selected = false;              // Flag used to keep track if it is the first click on the board or the second click
    chess_piece piece;
    bool game_ended = false; // Used to know when a checkmate is reached
    piece_type promoted_type = NONE;

    bool running = true; // Flag used to know when user quites the app
    SDL_Event event;     // Used to store an SDL event in the SDL event queue

    // Initialising SDL3
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Failed to init SDL: %s", SDL_GetError());
        return 1; // Indicating to the OS that an error has occured
    }

    // Create the window and renderer
    if (!SDL_CreateWindowAndRenderer("Chess Board", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &app_structure.window, &app_structure.renderer))
    {
        SDL_Log("Failed to create window/renderer: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Try loading the textures
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

            // Handling a mouse click event
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT && !game_ended)
            {
                // Get window coordinates of where screen was clicked
                int mouse_x = event.button.x;
                int mouse_y = event.button.y;

                // Calculate the file and rank which correlate to the position where the chess board was clicked
                int file = mouse_x / TILE_SIZE;
                int rank = mouse_y / TILE_SIZE;

                // Checking if it is the first click or the second click
                if (!piece_selected)
                {
                    // Get the tile where first click is made
                    current_move.from = {rank, file};
                    // Get the piece being moved
                    piece = current_game.game_board.get_piece_at(rank, file);
                    // Setting flag to true to indicate the first click has been made
                    piece_selected = true;
                }
                else
                {
                    // Get the tile where second click is made
                    current_move.to = {rank, file};

                    // Checks if the click corresponded to moving a piece and checks if the move made is allowed
                    if (is_legal_move(current_game.game_board, current_move, false) && piece.color == current_game.active_player)
                    {
                        // Tries moving the piece to the new position
                        current_game.game_board.move_piece(current_move);

                        // Checks if a pawn must be promoted.
                        // To be seperated into the model later!!!!!!!
                        if (piece.type == PAWN)
                        {
                            if ((piece.color == WHITE && current_move.to.rank == 0) || (piece.color == BLACK && current_move.to.rank == BOARD_SIZE - 1))
                            {
                                // Create block for user to choose piece and allow him to choose the piece
                                promoted_type = select_promotion_piece(app_structure, current_game, piece.color);
                                // Replacing the pawn by the promoted piece
                                current_game.game_board.set_piece_at(current_move.to, {promoted_type, piece.color});
                            }
                        }
                        // Switch to the other player's turn.
                        current_game.active_player = (current_game.active_player == WHITE) ? BLACK : WHITE;

                        // Checking if we have reached checkmate or stalemate
                        if (current_game.game_board.checkmate_or_stalemate(WHITE, false))
                        {
                            game_ended = true;
                            current_game.outcome = BLACK_WIN;
                        }
                        else if (current_game.game_board.checkmate_or_stalemate(BLACK, false))
                        {
                            game_ended = true;
                            current_game.outcome = WHITE_WIN;
                        }else if (current_game.game_board.checkmate_or_stalemate(WHITE, true) || current_game.game_board.checkmate_or_stalemate(WHITE, true))
                        {
                            game_ended = true;
                            current_game.outcome = DRAW;
                        }
                    }

                    // Setting the flag to false and erasing the move made
                    piece_selected = false;
                    current_move = {{-1, -1}, {-1, -1}};
                }
            }

            // Ending the game
            if (game_ended)
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
    // Clean up the renderer and window
    SDL_DestroyRenderer(app_structure.renderer);
    SDL_DestroyWindow(app_structure.window);
    SDL_Quit();
}