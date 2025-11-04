#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Chess-Model.h"
#include <iostream>
#include <format>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

using std::max, std::min, std::this_thread::sleep_for, std::chrono::seconds;

/**
 * @brief draws the board and adds the pieces
 *
 * @param app_object is the chess app
 * @param game_object is the game object
 */
void draw_board(const SDLStructures &app_object, const game &game_object)
{
    // Getting the chess board
    board board_object = game_object.game_board;

    for (int rank = 0; rank < BOARD_SIZE; rank++)
    {
        for (int file = 0; file < BOARD_SIZE; file++)
        {
            // Checking if we should draw a light or dark square on the bord
            bool dark = (rank + file) % 2;
            if (dark)
            {
                SDL_SetRenderDrawColor(app_object.renderer, 196, 135, 135, 255); // dark square
            }
            else
            {
                SDL_SetRenderDrawColor(app_object.renderer, 235, 215, 215, 255); // light square
            }

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

/**
 * @brief This function displays the promotion block and allows the user to select
 * the piece to promote a pawn to
 * 
 * @param app_object is the chess app object
 * @param game_object is the game_object
 * @param color is the color of the pawn being promoted
 * 
 * @return the type of the chess piece the pawn must be promoted to
 */
piece_type select_promotion_piece(const SDLStructures &app_object, const game &game_object, piece_color color)
{
    bool piece_selected = false; // Used to keep track whether a piece has been selected or not
    bool quit = false;           // Flag to know if user wants to quit the game
    SDL_Event event;             // Used to store an SDL event in the SDL event queue

    float x_top_left_corner = float(PROMOTION_BLOCK_X_TOP_LEFT * TILE_SIZE);                                                               // x-coordinate of top-left corner of promotion block
    float y_top_left_corner = float(((color == WHITE) ? PROMOTION_BLOCK_Y_TOP_LEFT_WHITE : PROMOTION_BLOCK_Y_TOP_LEFT_BLACK) * TILE_SIZE); // y-coordinate of top-left corner of promotion block
    float block_width = float(TILE_SIZE * PROMOTION_BLOCK_WIDTH_TILE);                                                                     // Width of the promotion block
    float block_height = float(TILE_SIZE);                                                                                                 // Height of promotion block

    //
    /**
     * @brief draw the promotion block
     *
     * Use of lambda function to create this sub-code which is needed inside the select_promotion_piece procedure.
     * This lambda function heavily depends on many of the parameters and variables used in the select_promotion_piece.
     * Hence to simplify the code, we make use of the lambda function instead of a helper function
     * We capture all outside variables as reference
     */
    auto display_promotion_block = [&]()
    {
        int colorId = color;
        int typeId = KNIGHT;

        // Drawing the promotion block
        SDL_FRect block = {x_top_left_corner, y_top_left_corner, block_width, block_height};
        SDL_SetRenderDrawColor(app_object.renderer, 209, 202, 202, 255);
        SDL_RenderFillRect(app_object.renderer, &block);

        // Drawing the sub-block for each piece that can be selected
        for (int x_coor = PROMOTION_BLOCK_X_TOP_LEFT; x_coor < PROMOTION_BLOCK_X_TOP_LEFT + PROMOTION_BLOCK_WIDTH_TILE; x_coor++)
        {
            SDL_FRect dst = {float(x_coor * TILE_SIZE), y_top_left_corner, float(TILE_SIZE), float(TILE_SIZE)};
            SDL_RenderTexture(app_object.renderer, app_object.piece_textures[colorId][typeId], nullptr, &dst);
            typeId++;
        }
    };

    // Prevent any chess moves until a piece has been selected for promotion
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

    // In case of an unexpected error, we just return NONE
    return NONE;
}


/**
 * @brief This procedure checks if a pawn has reached a promotion square and if yes, it allows the user to select the piece to
 * promotoe the pawn to and does the promotion. If it is the AI playing, the pawn will automatically be promoted to a Queen.
 * 
 * @param app_structure is the chess app
 * @param current_game is the game object
 * @param piece is the piece just moved in the current move
 * @param current_move is the move just played on the board
 * @param AI is a flag used to know whether it is a human player or the AI trying to promote a piece
 * 
 */
void check_for_promotion(const SDLStructures &app_structure, game &current_game, const chess_piece &piece, const move &current_move, bool AI)
{
    piece_type promoted_type = NONE;
    if (piece.type == PAWN)
    {
        if ((piece.color == WHITE && current_move.to.rank == 0) || (piece.color == BLACK && current_move.to.rank == BOARD_SIZE - 1))
        {
            if (AI)
            {
                // AI always promotes to a queen
                promoted_type = QUEEN;
            }
            else
            {
                // Create block for user to choose piece and allow him to choose the piece
                promoted_type = select_promotion_piece(app_structure, current_game, piece.color);
            }
            // Replacing the pawn by the promoted piece
            current_game.game_board.set_piece_at(current_move.to, {promoted_type, piece.color});
        }
    }
}


/**
 * @brief The procedure allows the AI to make a move on the board
 * 
 * @param app_structure is the chess app
 * @param current_game is the current chess game being played
 */
void AI_move(const SDLStructures &app_structure, game &current_game)
{
    // Getting the color of the pieces moved by the AI
    piece_color AI_color = current_game.active_player;

    int depth = 4; // Depth applied to minimax algorithm. The value of depth MUST BE EVEN for better performance
    move best_move; // Best move calculated by AI
    chess_piece moving_piece; // Piece being moved by AI

    // Checking if the AI is making the first move in the game
    if (current_game.number_of_moves_played == 0)
    {
        // Generate a random move as it is pointless to call minimax in this case
        // Or generate an opening? NOT SURE. FURTHER LOGIC TO BE IMPLEMENTED LATER
    }

    // Finding the best move the AI can play
    best_move = find_best_move(current_game.game_board, depth, AI_color);

    // Get the moving piece
    moving_piece = current_game.game_board.get_piece_at(best_move.from.rank, best_move.from.file);

    // AI moves the piece.
    current_game.game_board.move_piece(best_move);

    // Checking if a pawn must be promoted and promote it to queen if yes.
    check_for_promotion(app_structure, current_game, moving_piece, best_move, true);

    // Switching to the other player's turn
    current_game.active_player = (current_game.active_player == WHITE) ? BLACK : WHITE;

    // Incrementing the number of moves played
    current_game.number_of_moves_played++;
}

/**
 * @brief This function is called to check if there is a checkmate or stalemate on the board
 * 
 * @param current_game is the current chess game object
 * 
 * @return true if there is a checkmate or stalemate. Returns false otherwise.
 */
bool end_the_game(game &current_game)
{
    // Checking if we have reached checkmate or stalemate
    if (current_game.game_board.checkmate_or_stalemate(WHITE, false))
    {
        current_game.outcome = BLACK_WIN;

        SDL_Log("Black WIN");
        return true;
    }
    else if (current_game.game_board.checkmate_or_stalemate(BLACK, false))
    {
        current_game.outcome = WHITE_WIN;

        SDL_Log("White WIN");
        return true;
    }
    else if (current_game.game_board.checkmate_or_stalemate(WHITE, true) || current_game.game_board.checkmate_or_stalemate(WHITE, true))
    {
        current_game.outcome = DRAW;

        SDL_Log("Draw");
        return true;
    }

    return false;
}

/**
 * @brief This function loads the texture for the banner to be displayed 
 * upon the end of the game
 * 
 * @param app_structure is the chess app object
 * @param outcome is the game outcome (WHITE WIN, BLACK WIN, DRAW)
 * 
 * @return true if texture is loaded correctly and false otherwise.
 */
bool load_banner_texture(SDLStructures &app_structure, game_outcome &outcome)
{
    string file_path; // File path of where asset needed is located.

    switch (outcome)
    {
    case WHITE_WIN:
        file_path = "assets/banners/White_Wins.png";
        break;
    case BLACK_WIN:
        file_path = "assets/banners/Black_Wins.png";
        break;
    case DRAW:
        file_path = "assets/banners/Draw.png";
        break;
    default:
        return false;
        break;
    }

    // Convert to c string as SDL does not accept string arguments
    const char *path = file_path.c_str();

    // Creating the image surface
    SDL_Surface *surf = IMG_Load(path);

    // Checking if surface has been created successfully
    if (!surf)
    {
        SDL_Log("Failed to load %s: %s", path, SDL_GetError());
        return false;
    }

    // Creating the banner texture from the surface
    app_structure.banner_texture = SDL_CreateTextureFromSurface(app_structure.renderer, surf);

    SDL_DestroySurface(surf);

    // Checking if the texture has been created successfully
    if (!app_structure.banner_texture)
    {
        SDL_Log("Failed to create texture for %s: %s", path, SDL_GetError());
        return false;
    }

    return true;
}

/**
 * @brief This procedure displays a banner showing the game outcome
 * 
 * @param app_structure is the chess app object
 * @param game_object is the chess game object
 */
void display_winner(SDLStructures &app_structure, const game &game_object)
{
    // Getting the outcome of the chess game
    game_outcome outcome = game_object.outcome;

    // Clearing the buffer
    SDL_SetRenderDrawColor(app_structure.renderer, 0, 0, 0, 255);
    SDL_RenderClear(app_structure.renderer);

    // Initializing position of banner
    float x_top_left_corner = float(PROMOTION_BLOCK_X_TOP_LEFT * TILE_SIZE);
    float y_top_left_corner = float(PROMOTION_BLOCK_Y_TOP_LEFT_BLACK * TILE_SIZE);
    float block_width = float(TILE_SIZE * PROMOTION_BLOCK_WIDTH_TILE);
    float block_height = float(2 * TILE_SIZE);

    // Creating the banner container
    SDL_FRect block = {x_top_left_corner, y_top_left_corner, block_width, block_height};
    SDL_SetRenderDrawColor(app_structure.renderer, 66, 63, 63, 255);
    SDL_RenderFillRect(app_structure.renderer, &block);

    if (!load_banner_texture(app_structure, outcome))
    {
        SDL_Log("Failed to load the banner textures");
        return;
    }

    float text_width;
    float text_height;

    // Get the banner dimensions
    SDL_GetTextureSize(app_structure.banner_texture, &text_width, &text_height);

    // Centering the banner texture inside the block
    SDL_FRect dst = {x_top_left_corner + (block_width - text_width) / 2.0f,y_top_left_corner + (block_height - text_height) / 2.0f,text_width,text_height};

    // Draw the banner texture
    SDL_RenderTexture(app_structure.renderer, app_structure.banner_texture, NULL, &dst);

    // Present the updated frame
    SDL_RenderPresent(app_structure.renderer);
}

int main(int argc, char *argv[])
{
    SDLStructures app_structure; // The SDL APP
    game current_game;           // The chess game

    // Initialising the fields of the game struct
    current_game.active_player = WHITE;
    current_game.outcome = UNDETERMINED;

    move current_move = {{-1, -1}, {-1, -1}}; // Move made by the current player on the board
    bool piece_selected = false;              // Flag used to keep track if it is the first click on the board or the second click
    chess_piece piece;

    bool game_ended = false; // Used to know when a checkmate is reached
    bool running = true; // Flag used to know when user quites the app
    SDL_Event event;     // Used to store an SDL event in the SDL event queue

    bool AI_active = true;// Flag to know if the AI is playing or not. implemented as a variable here 
                            // due to possible program extension later

    piece_color AI_color = BLACK; // Color of chess pieces played by AI

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

            // If AI is playing, AI will move a piece after the player has played his move
            if (AI_active && current_game.active_player == AI_color)
            {
                // AI plays move
                AI_move(app_structure, current_game);
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

                        // Checks if a pawn must be promoted and allows for promotion if yes
                        check_for_promotion(app_structure, current_game, piece, current_move, false);

                        // Switch to the other player's turn.
                        current_game.active_player = (current_game.active_player == WHITE) ? BLACK : WHITE;

                        // Incrementing number of moves played
                        current_game.number_of_moves_played++;
                    }

                    // Setting the flag to false and erasing the move made
                    piece_selected = false;
                    current_move = {{-1, -1}, {-1, -1}};
                }
            }

            // Checking if checkmate or stalemate has been reached
            game_ended = end_the_game(current_game);

            // Ending the game
            if (game_ended)
            {
                running = false;

                // Repeated code here to correct a bug concerning displaying the banner
                SDL_SetRenderDrawColor(app_structure.renderer, 0, 0, 0, 255);
                SDL_RenderClear(app_structure.renderer);

                draw_board(app_structure, current_game);

                SDL_RenderPresent(app_structure.renderer);

                sleep_for(seconds(1));

                // Display banner of winner
                display_winner(app_structure, current_game);
            }
        }

        SDL_SetRenderDrawColor(app_structure.renderer, 0, 0, 0, 255);
        SDL_RenderClear(app_structure.renderer);

        // Drawing the chess board
        draw_board(app_structure, current_game);

        SDL_RenderPresent(app_structure.renderer);

        // If the game ended, display the banner
        if (running == false)
        {
            // Display banner of winner
            display_winner(app_structure, current_game);
            sleep_for(seconds(2));
        }
    }

    // Clean up textures
    for (int color_index = 0; color_index < 2; color_index++)
    {
        for (int type_index = 0; type_index < 6; type_index++)
        {
            SDL_DestroyTexture(app_structure.piece_textures[color_index][type_index]);
        }
    }

    // Destroy the banner texture
    SDL_DestroyTexture(app_structure.banner_texture);

    // Clean up the renderer and window, then quit SDL3
    SDL_DestroyRenderer(app_structure.renderer);
    SDL_DestroyWindow(app_structure.window);
    SDL_Quit();
}

