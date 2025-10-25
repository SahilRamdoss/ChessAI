#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "Chess-Model.h"
#include <iostream>
#include <format>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

using std::max, std::min;

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int minimax(const SDLStructures &app_structure, const game &current_game, board &the_board, int depth, int alpha, int beta, bool maximizing_player)
{
    if (depth == 0 || the_board.checkmate_or_stalemate(maximizing_player? WHITE : BLACK, false) || the_board.checkmate_or_stalemate(maximizing_player? WHITE : BLACK, true))
    {
        int eval = evaluate_board(the_board, depth);
        SDL_Log("Eval at depth %d is %d", depth, eval);
        return eval;
    }

    vector<move> legal_moves = generate_legal_moves(the_board, (maximizing_player ? WHITE : BLACK));

    if (legal_moves.size() == 0)
    {
        return evaluate_board(the_board, depth);
    }
    bool white_pawn_promoted = false;
    bool black_pawn_promoted = false;

    if (maximizing_player)
    {
        int max_evaluation = -1000000;

        for (int index = 0; index < legal_moves.size(); index++)
        {
            move move_made = legal_moves[index];
            chess_piece moved_piece = the_board.get_piece_at(move_made.from.rank, move_made.from.file);
            the_board.move_piece(move_made);

            // Checking if there should be a pawn promotion and doing it if yes
            white_pawn_promoted = automatic_white_pawn_promotion(the_board, moved_piece, move_made);

            // SDL_SetRenderDrawColor(app_structure.renderer, 0, 0, 0, 255);
            // SDL_RenderClear(app_structure.renderer);

            // // Drawing the chess board
            // draw_board(app_structure, current_game);

            // SDL_RenderPresent(app_structure.renderer);
            // std::this_thread::sleep_for(std::chrono::seconds(2));

            // Evaluating the board state
            int evaluation = minimax(app_structure, current_game, the_board, depth - 1, alpha, beta, false);

            // Undoing the pawn promotion
            if (white_pawn_promoted)
            {
                undo_white_pawn_promotion(the_board, move_made);
                white_pawn_promoted = false;
            }

            // Undoing the move
            the_board.unmove_piece();

            max_evaluation = max(max_evaluation, evaluation);
            alpha = max(alpha, evaluation);

            if (beta <= alpha)
            {
                break;
            }
        }

        return max_evaluation;
    }
    else
    {
        int min_evaluation = 1000000;
        for (int index = 0; index < legal_moves.size(); index++)
        {
            move move_made = legal_moves[index];
            chess_piece moved_piece = the_board.get_piece_at(move_made.from.rank, move_made.from.file);

            // Moving the chess piece
            the_board.move_piece(move_made);

            // Checking if there should be a pawn promotion and doing it if yes
            black_pawn_promoted = automatic_black_pawn_promotion(the_board, moved_piece, move_made);

            // SDL_SetRenderDrawColor(app_structure.renderer, 0, 0, 0, 255);
            // SDL_RenderClear(app_structure.renderer);

            // // Drawing the chess board
            // draw_board(app_structure, current_game);

            // SDL_RenderPresent(app_structure.renderer);
            // std::this_thread::sleep_for(std::chrono::seconds(2));

            // Evaluating the board state
            int evaluation = minimax(app_structure, current_game, the_board, depth - 1, alpha, beta, true);

            // Undoing the pawn promotion
            if (black_pawn_promoted)
            {
                undo_black_pawn_promotion(the_board, move_made);
                black_pawn_promoted = false;
            }

            // Undoing the move
            the_board.unmove_piece();

            min_evaluation = min(min_evaluation, evaluation);
            beta = min(beta, evaluation);

            if (beta <= alpha)
            {
                break;
            }
        }

        return min_evaluation;
    }
}

move find_best_move(const SDLStructures &app_structure, game &current_game, board &the_board, int depth, piece_color player_color)
{
    vector<move> possible_legal_moves = generate_legal_moves(the_board, player_color);
    move best_move = {{-1, -1}, {-1, -1}};
    int evaluation;
    int alpha = -1000000;
    int beta = 1000000;

    // If there are no legal moves to play, just return best_move to signify either
    // checkmate or stalemate
    if (possible_legal_moves.size() == 0)
    {
        return best_move;
    }

    // Initialize best_value to a very high integer for white
    // and a very low integer for black
    int best_value = (player_color == WHITE ? -1000000 : 1000000);

    for (int index = 0; index < possible_legal_moves.size(); index++)
    {
        move move_made = possible_legal_moves[index];
        chess_piece moved_piece = the_board.get_piece_at(move_made.from.rank, move_made.from.file);
        bool pawn_promoted = false;

        // Making the move on the board
        the_board.move_piece(move_made);

        // Check if the moved piece is a pawn and if it lands on a promotion square. If yes,
        // promote it to a queen
        pawn_promoted = promote_pawn_to_queen(the_board, moved_piece, move_made, player_color);

        // SDL_SetRenderDrawColor(app_structure.renderer, 0, 0, 0, 255);
        // SDL_RenderClear(app_structure.renderer);

        // // Drawing the chess board
        // draw_board(app_structure, current_game);

        // SDL_RenderPresent(app_structure.renderer);
        // std::this_thread::sleep_for(std::chrono::seconds(2));

        // Evaluating the board state
        evaluation = minimax(app_structure, current_game, the_board, depth - 1, alpha, beta, (player_color == WHITE ? false : true));

        // Adjusting alpha and/or at top level
        if (player_color == WHITE)
        {
            alpha = max(alpha, evaluation);
        }
        else
        {
            beta = min(beta, evaluation);
        }

        // Undoing the pawn promotion if it was promoted
        if (pawn_promoted)
        {
            unpromote_pawn_from_queen(the_board, move_made, player_color);
        }

        // Undoing the move
        the_board.unmove_piece();

        // Adjusting the best_value and best move that can be made
        if ((player_color == WHITE && evaluation > best_value) || (player_color == BLACK && evaluation < best_value))
        {
            best_value = evaluation;
            best_move = move_made;
        }
    }

    SDL_Log("Best evaluation is : %d", best_value);

    return best_move;
}

void AI_move(const SDLStructures &app_structure, game &current_game)
{
    piece_color AI_color = current_game.active_player;
    // The value of depth MUST BE EVEN for better performance
    int depth = 4;
    move best_move;
    chess_piece moving_piece;

    // Checking if the AI is making the first move in the game
    if (current_game.number_of_moves_played == 0)
    {
        // Generate a random move as it is pointless to call minimax in this case
        // Or generate an opening?
    }

    // Finding the best move the AI can play
    best_move = find_best_move(app_structure, current_game, current_game.game_board, depth, AI_color);

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

    bool AI_active = true;        // Flag to know if the AI is playing or not
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