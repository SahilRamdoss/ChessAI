#include "Chess-Model.h"
#include <cmath>
#include <iostream>
#include <format>

using std::abs;

board::board()
{
    // Creating an empty chess board
    for (int rank = 0; rank < BOARD_SIZE; rank++)
    {
        for (int file = 0; file < BOARD_SIZE; file++)
        {
            this->chess_board[rank][file] = {NONE, WHITE};
        }
    }

    // Placing the pawns
    for (int file = 0; file < BOARD_SIZE; file++)
    {
        this->chess_board[BOARD_SIZE - 2][file] = {PAWN, WHITE};
        this->white_pieces_remaining.push_back(PAWN);
        this->chess_board[1][file] = {PAWN, BLACK};
        this->black_pieces_remaining.push_back(PAWN);
    }

    // Placing the other pieces
    this->chess_board[BOARD_SIZE - 1][0] = this->chess_board[BOARD_SIZE - 1][7] = {ROOK, WHITE};
    this->white_pieces_remaining.push_back(ROOK);
    this->white_pieces_remaining.push_back(ROOK);
    this->chess_board[BOARD_SIZE - 1][1] = this->chess_board[BOARD_SIZE - 1][6] = {KNIGHT, WHITE};
    this->white_pieces_remaining.push_back(KNIGHT);
    this->white_pieces_remaining.push_back(KNIGHT);
    this->chess_board[BOARD_SIZE - 1][2] = this->chess_board[BOARD_SIZE - 1][5] = {BISHOP, WHITE};
    this->white_pieces_remaining.push_back(BISHOP);
    this->white_pieces_remaining.push_back(BISHOP);
    this->chess_board[BOARD_SIZE - 1][3] = {QUEEN, WHITE};
    this->white_pieces_remaining.push_back(QUEEN);
    this->chess_board[BOARD_SIZE - 1][4] = {KING, WHITE};
    this->white_pieces_remaining.push_back(KING);

    this->chess_board[0][0] = this->chess_board[0][7] = {ROOK, BLACK};
    this->black_pieces_remaining.push_back(ROOK);
    this->black_pieces_remaining.push_back(ROOK);
    this->chess_board[0][1] = this->chess_board[0][6] = {KNIGHT, BLACK};
    this->black_pieces_remaining.push_back(KNIGHT);
    this->black_pieces_remaining.push_back(KNIGHT);
    this->chess_board[0][2] = this->chess_board[0][5] = {BISHOP, BLACK};
    this->black_pieces_remaining.push_back(BISHOP);
    this->black_pieces_remaining.push_back(BISHOP);
    this->chess_board[0][3] = {QUEEN, BLACK};
    this->black_pieces_remaining.push_back(QUEEN);
    this->chess_board[0][4] = {KING, BLACK};
    this->black_pieces_remaining.push_back(KING);
}

void board::set_en_passant_target(const square potential_target_pawn)
{
    this->en_passant_target = potential_target_pawn;
}

square board::get_en_passant_target() const
{
    return this->en_passant_target;
}

void board::reset_en_passant_target()
{
    this->en_passant_target = {-1,-1};
}

chess_piece board::get_piece_at(int rank, int file) const
{
    return this->chess_board[rank][file];
}

bool board::move_piece(move current_move)
{
    square from = current_move.from;
    square to = current_move.to;
    chess_piece moving_piece = this->chess_board[from.rank][from.file];

    // If an empty tile has been selected we ignore it
    if (moving_piece.type == NONE)
    {
        return false;
    }

    // Difference in rank of the piece position caused by the move
    int rank_difference = to.rank - from.rank;
    // Difference in file of the piece position caused by the move
    int file_difference = to.file - from.file;

    // Checking for castling move
    if (moving_piece.type == KING && abs(file_difference) == 2 )
    {
        if (to.file == 6) // Kingside castling
        {
            this->chess_board[from.rank][5] = this->chess_board[from.rank][BOARD_SIZE-1];
            this->chess_board[from.rank][BOARD_SIZE-1] = {NONE, WHITE};
        }
        else if (to.file == 2) // Queen side castling
        {
            this->chess_board[from.rank][3] = this->chess_board[from.rank][0];
            this->chess_board[from.rank][0] = {NONE,WHITE};
        }
    }

    // checking for en passant pawn capture
    if (moving_piece.type == PAWN)
    {
        square potential_en_passant_target = this->get_en_passant_target();
        // Checking if it is a valid en passant attempt
        if (potential_en_passant_target.rank == to.rank && potential_en_passant_target.file == to.file && abs(file_difference) == 1 && this->chess_board[to.rank][to.file].type == NONE)
        {
            // The rank of the pawn which we are trying to capture by en passant
            int captured_pawn_rank;

            // Getting the rank of the pawn which we are trying to capture by en passant, based on the piece color
            if (moving_piece.color == WHITE){
                captured_pawn_rank = to.rank + 1; // Add one as black pawns move in +1 steps
            }else{
                captured_pawn_rank = to.rank - 1; // Minus one as white pawns move in -1 steps
            }

            // Chess piece being captured
            chess_piece captured_piece = this->chess_board[captured_pawn_rank][to.file];
            
            // Must check if it is a pawn of the opposite color before capturing it
            if (captured_piece.type == PAWN && captured_piece.color != moving_piece.color)
            {
                this->chess_board[captured_pawn_rank][to.file] = {NONE,WHITE};
            }
        }

        // If the pawn we have moved, has moved by 2 ranks, it is a potential en passant capture
        if (abs(rank_difference) == 2)
        {
            int en_passant_rank = (from.rank + to.rank) / 2; // Getting the rank of where we would capture it, if it is captured
            this->set_en_passant_target({en_passant_rank,from.file}); // Creating the en passant target
        }
    }else{
        // Resetting the en passant target to nothing if we did move a pawn
        this->reset_en_passant_target();
    }

    // If we did not move a pawn by 2 ranks this move, it means there is no en passant target for the next move
    if (!(moving_piece.type == PAWN && abs(rank_difference) == 2))
    {
        this->reset_en_passant_target();
    }

    // Moving the piece to its new destination
    this->chess_board[to.rank][to.file] = moving_piece;
    this->chess_board[from.rank][from.file] = {NONE, WHITE};

    // Keeping track of  whether the pieces which affect the ability of castling have been moved yet since 
    // the start of the game.
    if (moving_piece.type == KING)
    {
        if (moving_piece.color == WHITE)
        {
            set_white_king_moved(true);
        }else{
            set_black_king_moved(true);
        }
    }else if (moving_piece.type == ROOK)
    {
        if (moving_piece.color == WHITE)
        {
            if (from.rank == BOARD_SIZE-1 && from.file == 0)
            {
                set_white_rook_a_moved(true);
            }else if (from.rank == BOARD_SIZE-1 && from.file == BOARD_SIZE-1)
            {
                set_white_rook_h_moved(true);
            }
        }else{
            if (from.rank == 0 && from.file == 0)
            {
                set_black_rook_a_moved(true);
            }else if (from.rank == 0 && from.file == 7)
            {
                set_black_rook_h_moved(true);
            }
        }
    }

    return true;
}

void board::set_white_king_moved(bool truth)
{
    this->white_king_moved = truth;
}

bool board::get_white_king_moved() const
{
    return this->white_king_moved;
}

void board::set_black_king_moved(bool truth)
{
    this->black_king_moved = truth;
}

bool board::get_black_king_moved() const
{
    return this->black_king_moved;
}

void board::set_white_rook_a_moved(bool truth)
{
    this->white_rook_a_moved = truth;
}

bool board::get_white_rook_a_moved() const
{
    return this->white_rook_a_moved;
}

void board::set_white_rook_h_moved(bool truth)
{
    this->white_rook_h_moved = truth;
}

bool board::get_white_rook_h_moved() const
{
    return this->white_rook_h_moved;
}

void board::set_black_rook_a_moved(bool truth)
{
    this->black_rook_a_moved = truth;
}

bool board::get_black_rook_a_moved() const
{
    return this->black_rook_a_moved;
}

void board::set_black_rook_h_moved(bool truth)
{
    this->black_rook_h_moved = truth;
}

bool board::get_black_rook_h_moved() const
{
    return this->black_rook_h_moved;
}

string SDLStructures::get_piece_file_name(piece_color color, piece_type type)
{
    // Exiting the method if an empty tile has been passed
    if (type == NONE)
    {
        return "";
    }

    // Use of ternary operator here. Getting the color of the piece as a string
    string colorStr = (color == WHITE) ? "White" : "Black";
    string typeStr = "";
    string path = "";

    // DO NOT CHANGE THE STRING AS IT FOLLOWS THE NAMING CONVENTIONS I USED FOR MY PNG FILES
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

bool SDLStructures::load_piece_textures()
{
    // Getting the texture of every piece.
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

            this->piece_textures[piece_color_value][piece_type_value] = SDL_CreateTextureFromSurface(this->renderer, surf);

            // Since we already stored the surface in our array, we destroy it
            SDL_DestroySurface(surf);
            // Checks for nullptr, as this means we failed to create the texture
            if (!this->piece_textures[piece_color_value][piece_type_value])
            {
                SDL_Log("Failed to create texture for %s: %s", path, SDL_GetError());
                return false;
            }
        }
    }
    return true;
}

bool is_legal_move(board &the_board, const move &current_move)
{
    // Getting the chess piece selected
    chess_piece piece = the_board.get_piece_at(current_move.from.rank, current_move.from.file);

    // User selected an empty tile
    if (piece.type == NONE)
    {
        return false;
    }

    // Calculating the displacement of the piece by the move made in terms of the rank displacement and file displacement
    int rank_displacement = current_move.to.rank - current_move.from.rank;
    int file_displacement = current_move.to.file - current_move.from.file;

    // When user has double clicked on a piece, we just ignore it
    if (rank_displacement == 0 && file_displacement == 0)
    {
        return false;
    }

    // Getting the piece at the destination of the move made
    chess_piece target = the_board.get_piece_at(current_move.to.rank, current_move.to.file);

    // If the target tile contains a piece of the same color, this is an illegal move
    // as we cannot capture our own pieces
    if (target.type != NONE && target.color == piece.color)
    {
        return false;
    }

    // White pieces usually move up while black pieces usually move down
    int direction = (piece.color == WHITE) ? -1 : 1;

    // Checking if move made is legal based on the type of the piece
    switch (piece.type)
    {
    case PAWN:
        // If pawn moved by 1 rank in its allowed direction, it is acceptable
        if (file_displacement == 0 && rank_displacement == direction && target.type == NONE)
        {
            return true;
        }

        // If pawn tried to move by 2 ranks in its acceptable direction, check if the pawn has been moved since the start
        // of the game
        if (file_displacement == 0 && rank_displacement == 2 * direction && target.type == NONE)
        {
            int start_rank = (piece.color == WHITE) ? 6 : 1;

            // If the pawn is still at its start rank and there is nothing in its way, it can move by 2 ranks in its allowed direction
            if (current_move.from.rank == start_rank && the_board.get_piece_at(current_move.from.rank + direction, current_move.from.file).type == NONE)
            {
                return true;
            }
        }

        // Normal pawn capture
        if (abs(file_displacement) == 1 && rank_displacement == direction && target.type != NONE && target.color != piece.color)
        {
            return true;
        }

        // En passant pawn capture
        if (abs(file_displacement) == 1 && rank_displacement == direction && target.type == NONE)
        {
            square en_passant_target = the_board.get_en_passant_target();
            if (current_move.to.rank == en_passant_target.rank && current_move.to.file == en_passant_target.file)
            {
                chess_piece piece_captured = the_board.get_piece_at((piece.color == WHITE)? current_move.to.rank + 1 : current_move.to.rank -1, current_move.to.file);
                if (piece_captured.type == PAWN && piece_captured.color != piece.color)
                {
                    return true;
                }
            }
        }

        return false;
    case ROOK:
        return valid_rook_move(the_board, current_move, rank_displacement, file_displacement);
    case KNIGHT:
        return (abs(rank_displacement) == 2 && abs(file_displacement) == 1) || (abs(rank_displacement) == 1 && abs(file_displacement) == 2);
    case BISHOP:
        return valid_bishop_move(the_board, current_move, rank_displacement, file_displacement);
    case QUEEN:
        return (valid_rook_move(the_board, current_move, rank_displacement, file_displacement) || valid_bishop_move(the_board, current_move, rank_displacement, file_displacement));
    case KING:
        // If King tries to move by one square, it is acceptable
        if (abs(rank_displacement) <= 1 && abs(file_displacement) <= 1)
        {
            return true;
        }

        // Attempt at castling
        if (rank_displacement == 0 && abs(file_displacement) == 2)
        {
            int from_rank = current_move.from.rank;
            // Deciding if it is a king side castle or queen side castle depending on the sign of file_displacement
            bool king_side = (file_displacement == 2);

            // Checking if any of the pieces involved in the castle have been moved from the start of the game
            if (piece.color == WHITE)
            {
                if (the_board.get_white_king_moved() || (king_side && the_board.get_white_rook_h_moved()) || (!king_side && the_board.get_white_rook_a_moved()))
                {
                    return false;
                }
            }else{
                if (the_board.get_black_king_moved() || (king_side && the_board.get_black_rook_h_moved()) || (!king_side && the_board.get_black_rook_a_moved()))
                {
                    return false;
                }
            }

            // Checking if there are any pieces blocking the castle.
            if (king_side)
            {
                if (the_board.get_piece_at(from_rank,5).type != NONE || the_board.get_piece_at(from_rank,6).type != NONE)
                {
                    return false;
                }
            }else{
                if (the_board.get_piece_at(from_rank,1).type != NONE || the_board.get_piece_at(from_rank,2).type != NONE || the_board.get_piece_at(from_rank,3).type != NONE)
                {
                    return false;
                }
            }

            return true;
        }

        return false;

    default:
        throw "Unhandled piece type";
    }
}

bool valid_rook_move(const board &the_board, const move &current_move, int rank_displacement, int file_displacement)
{
    // A rook cannot move diagonally
    if (rank_displacement != 0 && file_displacement != 0)
    {
        return false;
    }
    int single_step_rank;
    int single_step_file;

    // Getting the step directions
    if (rank_displacement == 0)
    {
        single_step_rank = 0;
    }
    else if (rank_displacement > 0)
    {
        single_step_rank = 1;
    }
    else
    {
        single_step_rank = -1;
    }

    if (file_displacement == 0)
    {
        single_step_file = 0;
    }
    else if (file_displacement > 0)
    {
        single_step_file = 1;
    }
    else
    {
        single_step_file = -1;
    }

    return check_for_obstruction(the_board, current_move, single_step_rank, single_step_file);
}

bool valid_bishop_move(const board &the_board, const move &current_move, int rank_displacement, int file_displacement)
{
    // The path moved by a bishop must always be the hypotenuse of an isosceles triangle
    if (abs(rank_displacement) != abs(file_displacement))
    {
        return false;
    }

    // Getting the step directions
    int single_step_rank = (rank_displacement > 0) ? 1 : -1;
    int single_step_file = (file_displacement > 0) ? 1 : -1;

    return check_for_obstruction(the_board, current_move, single_step_rank, single_step_file);
}

bool check_for_obstruction(const board &the_board, const move &current_move, int single_step_rank, int single_step_file)
{
    int new_rank = current_move.from.rank + single_step_rank;
    int new_file = current_move.from.file + single_step_file;

    // We just move the piece step by step in the direction of its final displacement and check each time
    // if it meets another piece in the middle of its path before reaching its destination, because if it does, it means
    // that the move is illegal, as it consists of jumping over a piece.
    while (new_rank != current_move.to.rank || new_file != current_move.to.file)
    {
        if (the_board.get_piece_at(new_rank, new_file).type != NONE)
        {
            return false;
        }

        new_rank += single_step_rank;
        new_file += single_step_file;
    }

    return true;
}
