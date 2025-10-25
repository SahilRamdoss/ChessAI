#include "Chess-Model.h"
#include <cmath>
#include <format>
#include <vector>
#include <stack>

using std::abs, std::find, std::vector, std::stack, std::max, std::min;

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

    // Initialising vectors
    this->white_pieces_remaining = {};
    this->black_pieces_remaining = {};
    this->move_history = {};

    // Initializing en_passant target
    this->en_passant_target = {-1, -1};
}

void board::set_en_passant_target(const square potential_en_passant_target)
{
    this->en_passant_target = potential_en_passant_target;
}

square board::get_en_passant_target() const
{
    return this->en_passant_target;
}

void board::reset_en_passant_target()
{
    this->en_passant_target = {-1, -1};
}

chess_piece board::get_piece_at(int rank, int file) const
{
    return this->chess_board[rank][file];
}

void board::set_piece_at(square tile, chess_piece piece)
{
    this->chess_board[tile.rank][tile.file] = piece;
}

void board::move_piece(const move &current_move)
{
    square from = current_move.from;
    square to = current_move.to;
    move_history_data move_data;
    chess_piece moving_piece = this->get_piece_at(from.rank, from.file);
    chess_piece captured_piece = this->get_piece_at(to.rank, to.file);

    // Updating the move history
    move_data.move_made = current_move;
    move_data.moved_piece = moving_piece;
    move_data.king_side_castle = false;
    move_data.queen_side_castle = false;
    move_data.en_passant_capture = false;
    move_data.en_passant_opportunity = false;
    move_data.en_passant_opportunity_square = {-1, -1};
    move_data.prev_white_king_moved = this->get_white_king_moved();
    move_data.prev_black_king_moved = this->get_black_king_moved();
    move_data.prev_white_rook_a_moved = this->get_white_rook_a_moved();
    move_data.prev_white_rook_h_moved = this->get_white_rook_h_moved();
    move_data.prev_black_rook_a_moved = this->get_black_rook_a_moved();
    move_data.prev_black_rook_h_moved = this->get_black_rook_h_moved();

    // Difference in rank of the piece position caused by the move
    int rank_difference = to.rank - from.rank;
    // Difference in file of the piece position caused by the move
    int file_difference = to.file - from.file;

    // Checking for castling move
    if (moving_piece.type == KING && abs(file_difference) == 2)
    {
        if (to.file == 6) // Kingside castling
        {
            move_data.king_side_castle = true;
            // Moving the h file rook for the king side castle
            this->chess_board[from.rank][5] = this->chess_board[from.rank][BOARD_SIZE - 1];
            this->chess_board[from.rank][BOARD_SIZE - 1] = {NONE, WHITE};
        }
        else if (to.file == 2) // Queen side castling
        {
            move_data.queen_side_castle = true;
            // Moving the a file rook for the queen side castle
            this->chess_board[from.rank][3] = this->chess_board[from.rank][0];
            this->chess_board[from.rank][0] = {NONE, WHITE};
        }
    }

    if (moving_piece.type == PAWN)
    {
        square potential_en_passant_target = this->get_en_passant_target();
        // Checking if it is a valid en passant capture attempt
        if (potential_en_passant_target.rank == to.rank && potential_en_passant_target.file == to.file && abs(file_difference) == 1 && this->chess_board[to.rank][to.file].type == NONE)
        {
            // The rank of the pawn which we are trying to capture by en passant
            int captured_pawn_rank;

            // Getting the rank of the pawn which we are trying to capture by en passant, based on the piece color
            if (moving_piece.color == WHITE)
            {
                captured_pawn_rank = to.rank + 1; // Add one as black pawns move in +1 steps
            }
            else
            {
                captured_pawn_rank = to.rank - 1; // Minus one as white pawns move in -1 steps
            }

            // Chess piece being captured
            chess_piece potential_pawn_capture = this->chess_board[captured_pawn_rank][to.file];

            // Must check if it is a pawn of the opposite color before capturing it
            if (potential_pawn_capture.type == PAWN && potential_pawn_capture.color != moving_piece.color)
            {
                captured_piece = this->chess_board[captured_pawn_rank][to.file];
                // Removing the captured pawn from the board
                this->chess_board[captured_pawn_rank][to.file] = {NONE, WHITE};
                // Setting en passant capture flag to true
                move_data.en_passant_capture = true;
            }
        }

        // If the pawn we have moved, has moved by 2 ranks, it is a potential en passant capture
        if (abs(rank_difference) == 2)
        {
            int en_passant_rank = (from.rank + to.rank) / 2;                        // Getting the rank of where we would capture it, if it is captured
            this->set_en_passant_target({en_passant_rank, from.file});              // Creating the en passant target
            move_data.en_passant_opportunity = true;                                // Creating en-passant capture opportunity for this move in move history
            move_data.en_passant_opportunity_square = {en_passant_rank, from.file}; // Getting the en passant target for this en passant opportunity
        }
    }

    // If we did not move a pawn by 2 ranks this move, it means there is no en passant target for the next move
    if (!(moving_piece.type == PAWN && abs(rank_difference) == 2))
    {
        this->reset_en_passant_target();
    }

    // Recording piece capture in move_data
    move_data.piece_captured = captured_piece;

    // Updating the vectors keeping track of the number of pieces on the board
    if (captured_piece.color != moving_piece.color && captured_piece.type != NONE)
    {
        if (captured_piece.color == WHITE)
        {
            // The data type of index_of_piece_to_remove will be generated dynamically
            auto index_of_piece_to_remove = find(this->white_pieces_remaining.begin(), this->white_pieces_remaining.end(), captured_piece.type);

            if (index_of_piece_to_remove != this->white_pieces_remaining.end())
            {
                this->white_pieces_remaining.erase(index_of_piece_to_remove);
            }
        }
        else
        {
            auto index_of_piece_to_remove = find(this->black_pieces_remaining.begin(), this->black_pieces_remaining.end(), captured_piece.type);

            if (index_of_piece_to_remove != this->black_pieces_remaining.end())
            {
                this->black_pieces_remaining.erase(index_of_piece_to_remove);
            }
        }
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
        }
        else
        {
            set_black_king_moved(true);
        }
    }
    else if (moving_piece.type == ROOK)
    {
        if (moving_piece.color == WHITE)
        {
            if (from.rank == BOARD_SIZE - 1 && from.file == 0)
            {
                set_white_rook_a_moved(true);
            }
            else if (from.rank == BOARD_SIZE - 1 && from.file == BOARD_SIZE - 1)
            {
                set_white_rook_h_moved(true);
            }
        }
        else
        {
            if (from.rank == 0 && from.file == 0)
            {
                set_black_rook_a_moved(true);
            }
            else if (from.rank == 0 && from.file == BOARD_SIZE - 1)
            {
                set_black_rook_h_moved(true);
            }
        }
    }

    // Saving move in move history
    this->move_history.push(move_data);
}

void board::unmove_piece()
{
    if (this->move_history.empty())
    {
        return;
    }

    // Get the data about the last move made
    move_history_data last_move_data = this->move_history.top();

    // Deleting the last move undone from move history
    move_history.pop();
    bool moves_history_empty = true;
    move_history_data previous_previous_move_data = {};

    // Getting the previous move
    if (!move_history.empty())
    {
        previous_previous_move_data = move_history.top();
        moves_history_empty = false;
    }

    // Get the last move made
    move last_move_made = last_move_data.move_made;

    // The moved piece will be the piece at the destination square on the board
    chess_piece moved_piece = last_move_data.moved_piece;

    // Get the captured piece from the move history data
    chess_piece captured_piece = last_move_data.piece_captured;

    // Restoring previous castling rights
    this->set_white_king_moved(last_move_data.prev_white_king_moved);
    this->set_black_king_moved(last_move_data.prev_black_king_moved);
    this->set_white_rook_a_moved(last_move_data.prev_white_rook_a_moved);
    this->set_white_rook_h_moved(last_move_data.prev_white_rook_h_moved);
    this->set_black_rook_a_moved(last_move_data.prev_black_rook_a_moved);
    this->set_black_rook_h_moved(last_move_data.prev_black_rook_h_moved);

    if (last_move_data.en_passant_capture)
    {
        // Move the moved piece back to its original position
        this->set_piece_at(last_move_made.from, moved_piece);
        // Since it is an enpassant pawn capture, the destination square should now be empty
        this->set_piece_at(last_move_made.to, {NONE, WHITE});

        // Used to locate rank of the pawn captured by enpasant
        int step_rank = 0;

        if (moved_piece.color == WHITE)
        {
            step_rank = 1;
            // Putting the captured pawn back to black pieces remaining if the moved piece is white
            this->black_pieces_remaining.push_back(PAWN);
        }
        else
        {
            step_rank = -1;
            // Putting the captured pawn back to white pieces remaining if the moved piece is black
            this->white_pieces_remaining.push_back(PAWN);
        }

        // Get the square where the pawn was captured by enpassant
        square en_passant_capture_square = {last_move_made.to.rank + step_rank, last_move_made.to.file};
        // Put the pawn that was captured by enpassant back on the board
        this->set_piece_at(en_passant_capture_square, {PAWN, (moved_piece.color == WHITE ? BLACK : WHITE)});
    }
    else if (last_move_data.king_side_castle)
    {
        // Re-positioning the king
        this->set_piece_at(last_move_made.from, moved_piece);
        this->set_piece_at(last_move_made.to, {NONE, WHITE});

        // Putting the rook as it was before the king side castle
        if (moved_piece.color == WHITE)
        {
            this->set_piece_at({BOARD_SIZE - 1, BOARD_SIZE - 1}, {ROOK, WHITE});
            this->set_piece_at({BOARD_SIZE - 1, last_move_made.to.file - 1}, {NONE, WHITE});
        }
        else
        {
            this->set_piece_at({0, BOARD_SIZE - 1}, {ROOK, BLACK});
            this->set_piece_at({0, last_move_made.to.file - 1}, {NONE, WHITE});
        }
    }
    else if (last_move_data.queen_side_castle)
    {
        // Re-positioning the king
        this->set_piece_at(last_move_made.from, moved_piece);
        this->set_piece_at(last_move_made.to, {NONE, WHITE});

        // Putting the rook as it was before the queen side castle
        if (moved_piece.color == WHITE)
        {
            this->set_piece_at({BOARD_SIZE - 1, 0}, {ROOK, WHITE});
            this->set_piece_at({BOARD_SIZE - 1, last_move_made.to.file + 1}, {NONE, WHITE});
        }
        else
        {
            this->set_piece_at({0, 0}, {ROOK, BLACK});
            this->set_piece_at({0, last_move_made.to.file + 1}, {NONE, WHITE});
        }
    }
    else
    {
        // Handling case of a quiet move or normal capture move //

        // Moving the moved piece back to it original position
        this->set_piece_at(last_move_made.from, moved_piece);
        // Set any piece captured back on the board
        this->set_piece_at(last_move_made.to, captured_piece);

        // Updating white_pieces_remaining and black_pieces_remaining accordingly
        if (captured_piece.type != NONE)
        {
            if (captured_piece.color == WHITE)
            {
                this->white_pieces_remaining.push_back(captured_piece.type);
            }
            else
            {
                this->black_pieces_remaining.push_back(captured_piece.type);
            }
        }
    }

    if (!moves_history_empty)
    {
        if (previous_previous_move_data.en_passant_opportunity)
        {
            // Recreating en passant target if the last moved was an enpassant but not enpassant capture was made
            this->set_en_passant_target(previous_previous_move_data.en_passant_opportunity_square);
        }
    }
    else
    {
        // Resetting enpassant target
        this->reset_en_passant_target();
    }
}

bool board::is_square_attacked(square tile, piece_color attacker_color) const
{
    move possible_attack_move;

    // Looping through every tile and if it contains a piece of our specified color,
    // check whether that piece can attack the given square. If there is at least one, return true/
    for (int rank = 0; rank < BOARD_SIZE; rank++)
    {
        for (int file = 0; file < BOARD_SIZE; file++)
        {
            chess_piece piece = chess_board[rank][file];

            if (piece.type != NONE && piece.color == attacker_color)
            {
                possible_attack_move.from = {rank, file};
                possible_attack_move.to = tile;

                if (is_legal_move(*this, possible_attack_move, true))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

square board::find_the_king(piece_color king_color) const
{
    // Loop through every tile till we come across our king of the given color
    for (int rank = 0; rank < BOARD_SIZE; rank++)
    {
        for (int file = 0; file < BOARD_SIZE; file++)
        {
            chess_piece piece = this->chess_board[rank][file];

            if (piece.type == KING && piece.color == king_color)
            {
                return {rank, file};
            }
        }
    }

    return {-1, -1};
}

bool board::king_in_check(piece_color king_color) const
{
    square king_square = this->find_the_king(king_color);
    piece_color opponent_color;

    // Error checking
    if (king_square.rank == -1 || king_square.file == -1)
    {
        return false;
    }

    if (king_color == WHITE)
    {
        opponent_color = BLACK;
    }
    else
    {
        opponent_color = WHITE;
    }

    // If king is being attacked, it is in check
    return is_square_attacked(king_square, opponent_color);
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

bool board::checkmate_or_stalemate(piece_color king_color, bool check_stalemate)
{

    // king_in_check XOR check_stalemate :)
    if (!this->king_in_check(king_color) && !check_stalemate)
    {
        return false;
    }
    else if (this->king_in_check(king_color) && check_stalemate)
    {
        return false;
    }

    // Loop through every tile on the board and whenever a piece is encountered,
    // check if it can move to any other tile. If there is at none which can, this
    // signifies either checkmate or stalemate, depending on which we are looking for

    vector<move> legal_moves = generate_legal_moves(*this, king_color);

    if (legal_moves.size() != 0)
    {
        return false;
    }

    return true;
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

bool valid_move_simulated(board &temp_board, const move current_move, bool valid_move)
{
    // If the move played on the piece is already detected to be an illegal one,
    // return false
    if (!valid_move)
    {
        return false;
    }

    // Try to play the move on a copy of the board
    temp_board.move_piece(current_move);

    chess_piece moving_piece = temp_board.get_piece_at(current_move.to.rank, current_move.to.file);

    // If when the move is played, the king of the same colour is in check, this means that this move cannot be played.
    // This pins a piece which is blocking an attack on the king and prevents the player from playing a
    // move that does not remove his king from check, if his king is in check
    if (temp_board.king_in_check(moving_piece.color))
    {
        return false;
    }

    return true;
}

bool is_legal_move(const board &the_board, const move &current_move, bool consider_attacks_only)
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

    // White pieces  move up while black pieces usually move down according to our board convention
    int direction = (piece.color == WHITE) ? -1 : 1;

    // Flag to know if the move played follows how pieces move according to chess rules
    bool legal_move_played = false;
    // Flag used to keep track if a castling attempt is illegal
    bool improper_castle = false;

    // Checking if move made is legal based on the type of the piece
    switch (piece.type)
    {
    case PAWN:
        if (!consider_attacks_only)
        {
            // If pawn moved by 1 rank in its allowed direction, it is acceptable
            if (file_displacement == 0 && rank_displacement == direction && target.type == NONE)
            {
                legal_move_played = true;
            }

            // If pawn tried to move by 2 ranks in its acceptable direction, check if the pawn has been moved since the start
            // of the game
            if (file_displacement == 0 && rank_displacement == 2 * direction && target.type == NONE)
            {
                int start_rank = (piece.color == WHITE) ? 6 : 1;

                // If the pawn is still at its start rank and there is nothing in its way, it can move by 2 ranks in its allowed direction
                if (current_move.from.rank == start_rank && the_board.get_piece_at(current_move.from.rank + direction, current_move.from.file).type == NONE)
                {
                    legal_move_played = true;
                }
            }
        }

        // Normal pawn capture. No need to check color of piece at destination square as this has already been handled
        // at the start of the function
        if (abs(file_displacement) == 1 && rank_displacement == direction && target.type != NONE)
        {
            legal_move_played = true;
        }

        // En passant pawn capture
        if (abs(file_displacement) == 1 && rank_displacement == direction && target.type == NONE)
        {
            square en_passant_target = the_board.get_en_passant_target();
            if (current_move.to.rank == en_passant_target.rank && current_move.to.file == en_passant_target.file)
            {
                chess_piece piece_captured = the_board.get_piece_at(current_move.to.rank - direction, current_move.to.file);
                if (piece_captured.type == PAWN && piece_captured.color != piece.color)
                {
                    legal_move_played = true;
                }
            }
        }
        break;
    case ROOK:
        legal_move_played = valid_rook_move(the_board, current_move, rank_displacement, file_displacement);
        break;
    case KNIGHT:
        legal_move_played = (abs(rank_displacement) == 2 && abs(file_displacement) == 1) || (abs(rank_displacement) == 1 && abs(file_displacement) == 2);
        break;
    case BISHOP:
        legal_move_played = valid_bishop_move(the_board, current_move, rank_displacement, file_displacement);
        break;
    case QUEEN:
        legal_move_played = (valid_rook_move(the_board, current_move, rank_displacement, file_displacement) || valid_bishop_move(the_board, current_move, rank_displacement, file_displacement));
        break;
    case KING:
        // If King tries to move by one square, it is acceptable. No need to check color of piece at destination
        // square as this has already been handled at the start of the function
        if (abs(rank_displacement) <= 1 && abs(file_displacement) <= 1)
        {
            legal_move_played = true;
            break;
        }

        if (!consider_attacks_only)
        {
            // Attempt at castling
            if (rank_displacement == 0 && abs(file_displacement) == 2)
            {
                int from_rank = current_move.from.rank;
                // Deciding if it is a king side castle or queen side castle depending on the sign of file_displacement
                bool king_side = (file_displacement == 2);

                // The king cannot castle if it is in check
                if (the_board.king_in_check(piece.color))
                {
                    legal_move_played = false;
                    break;
                }

                // Checking if any of the pieces involved in the castle have been moved from the start of the game
                if (piece.color == WHITE)
                {
                    if (the_board.get_white_king_moved() || (king_side && the_board.get_white_rook_h_moved()) || (!king_side && the_board.get_white_rook_a_moved()))
                    {
                        legal_move_played = false;
                        break;
                    }
                }
                else
                {
                    if (the_board.get_black_king_moved() || (king_side && the_board.get_black_rook_h_moved()) || (!king_side && the_board.get_black_rook_a_moved()))
                    {
                        legal_move_played = false;
                        break;
                    }
                }

                // Checking if there are any pieces blocking the castle.
                if (king_side)
                {
                    for (int file_number = current_move.from.file + 1; file_number < BOARD_SIZE - 1; file_number++)
                    {
                        if (the_board.get_piece_at(from_rank, file_number).type != NONE)
                        {
                            legal_move_played = false;
                            improper_castle = true;
                            break;
                        }
                    }

                    if (improper_castle)
                    {
                        break;
                    }
                    // if (the_board.get_piece_at(from_rank, 5).type != NONE || the_board.get_piece_at(from_rank, 6).type != NONE)
                    // {
                    //     legal_move_played = false;
                    //     break;
                    // }
                }
                else
                {
                    for (int file_number = 1; file_number < current_move.from.file; file_number++)
                    {
                        if (the_board.get_piece_at(from_rank, file_number).type != NONE)
                        {
                            legal_move_played = false;
                            improper_castle = true;
                            break;
                        }
                    }

                    if (improper_castle)
                    {
                        break;
                    }
                    // if (the_board.get_piece_at(from_rank, 1).type != NONE || the_board.get_piece_at(from_rank, 2).type != NONE || the_board.get_piece_at(from_rank, 3).type != NONE)
                    // {
                    //     legal_move_played = false;
                    //     break;
                    // }
                }

                // Preventing castling if there is any attack on the king if it castles
                bool square_attacked = false;
                int step = (king_side) ? 1 : -1;
                piece_color opponent_color = (piece.color == WHITE) ? BLACK : WHITE;

                for (int board_file = current_move.from.file; board_file != current_move.to.file + step; board_file += step)
                {
                    square temp_tile = {from_rank, board_file};

                    if (the_board.is_square_attacked(temp_tile, opponent_color))
                    {
                        square_attacked = true;
                        break;
                    }
                }

                // If there is an attack on the king when it castles, this is an invalid castle
                if (square_attacked)
                {
                    legal_move_played = false;
                    break;
                }

                legal_move_played = true;
                break;
            }
        }

        // King tried to move illegally
        legal_move_played = false;
        break;

    default:
        throw "Unhandled piece type";
        break;
    }

    if (consider_attacks_only)
    {
        return legal_move_played;
    }
    else
    {
        board temp_board = the_board;
        return valid_move_simulated(temp_board, current_move, legal_move_played);
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

int material_evaluation(const board &the_board)
{
    int evaluation = 0;
    piece_type type_of_piece;

    // Looping through each white piece type and adjusting the evaluation according to their score
    for (int index = 0; index < the_board.white_pieces_remaining.size(); index++)
    {
        type_of_piece = the_board.white_pieces_remaining[index];

        if (type_of_piece != NONE)
        {
            evaluation += PIECE_VALUE[type_of_piece];
        }
    }

    // Looping through each black piece type and adjusting the evaluation according to their score
    for (int index = 0; index < the_board.black_pieces_remaining.size(); index++)
    {
        type_of_piece = the_board.black_pieces_remaining[index];

        if (type_of_piece != NONE)
        {
            evaluation -= PIECE_VALUE[type_of_piece];
        }
    }

    return evaluation;
}

game_phase determine_game_phase(const board &the_board)
{
    int opening_phase = 0;
    int current_phase = 0;
    double phase_ratio = 0;

    piece_type type_of_piece;

    // Getting the opening game phase (no pieces lost)
    for (int piece_type = FIRST_TYPE; piece_type < LAST_TYPE; piece_type++)
    {
        opening_phase += OPENING_PIECES_COUNT[piece_type] * PIECE_PHASE_WEIGHT[piece_type];
    }

    // Determining the current game phase weight
    for (int index = 0; index < the_board.white_pieces_remaining.size(); index++)
    {
        type_of_piece = the_board.white_pieces_remaining[index];

        if (type_of_piece != NONE)
        {
            current_phase += PIECE_PHASE_WEIGHT[type_of_piece];
        }
    }

    for (int index = 0; index < the_board.black_pieces_remaining.size(); index++)
    {
        type_of_piece = the_board.black_pieces_remaining[index];

        if (type_of_piece != NONE)
        {
            current_phase += PIECE_PHASE_WEIGHT[type_of_piece];
        }
    }

    // Calculating phase ratio
    phase_ratio = (double)current_phase / opening_phase;

    // Determining game phase based on phase ratio
    if (phase_ratio > ((double)OPENING_PHASE_RATIO / 100.0))
    {
        return OPENING;
    }
    else if (phase_ratio > ((double)MIDDLE_GAME_PHASE_RATIO / 100.0))
    {
        return MIDDLE_GAME;
    }
    else
    {
        return ENDGAME;
    }
}

int positional_evaluation(const board &the_board)
{
    int evaluation = 0;
    int piece_positional_score = 0;
    chess_piece piece;

    // Determining in which phase of the game we are.
    game_phase stage_of_game = determine_game_phase(the_board);

    for (int rank = 0; rank < BOARD_SIZE; rank++)
    {
        for (int file = 0; file < BOARD_SIZE; file++)
        {
            piece = the_board.get_piece_at(rank, file);

            if (piece.type == NONE)
            {
                continue;
            }

            square piece_square = {rank, file};

            // Gettting the score from the relevant piece square table
            switch (piece.type)
            {
            case PAWN:
                if (stage_of_game == OPENING)
                {
                    piece_positional_score = pawn_piece_table_opening[(piece.color == WHITE ? rank : 7 - rank)][file];
                }
                else if (stage_of_game == MIDDLE_GAME)
                {
                    piece_positional_score = pawn_piece_table_middlegame[(piece.color == WHITE ? rank : 7 - rank)][file];
                }
                else
                {
                    piece_positional_score = pawn_piece_table_endgame[(piece.color == WHITE ? rank : 7 - rank)][file];
                }
                break;

            case KNIGHT:
                if (stage_of_game == OPENING)
                {
                    piece_positional_score = knight_piece_table_opening[(piece.color == WHITE ? rank : 7 - rank)][file];
                }
                else if (stage_of_game == MIDDLE_GAME)
                {
                    piece_positional_score = knight_piece_table_middlegame[(piece.color == WHITE ? rank : 7 - rank)][file];
                }
                else
                {
                    piece_positional_score = knight_piece_table_endgame[(piece.color == WHITE ? rank : 7 - rank)][file];
                }
                break;
            case BISHOP:
                if (stage_of_game == OPENING)
                {
                    piece_positional_score = bishop_piece_table_opening[(piece.color == WHITE ? rank : 7 - rank)][file];
                }
                else if (stage_of_game == MIDDLE_GAME)
                {
                    piece_positional_score = bishop_piece_table_middlegame[(piece.color == WHITE ? rank : 7 - rank)][file];
                }
                else
                {
                    piece_positional_score = bishop_piece_table_endgame[(piece.color == WHITE ? rank : 7 - rank)][file];
                }
                break;
            case ROOK:
                if (stage_of_game == OPENING)
                {
                    piece_positional_score = rook_piece_table_opening[(piece.color == WHITE ? rank : 7 - rank)][file];
                }
                else if (stage_of_game == MIDDLE_GAME)
                {
                    piece_positional_score = rook_piece_table_middlegame[(piece.color == WHITE ? rank : 7 - rank)][file];
                }
                else
                {
                    piece_positional_score = rook_piece_table_endgame[(piece.color == WHITE ? rank : 7 - rank)][file];
                }
                break;
            case QUEEN:
                piece_positional_score = queen_piece_table[(piece.color == WHITE ? rank : 7 - rank)][file];
                break;
            case KING:
                if (stage_of_game == OPENING)
                {
                    piece_positional_score = king_piece_table_opening[(piece.color == WHITE ? rank : 7 - rank)][file];
                }
                else if (stage_of_game == MIDDLE_GAME)
                {
                    piece_positional_score = king_piece_table_middlegame[(piece.color == WHITE ? rank : 7 - rank)][file];
                }
                else
                {
                    piece_positional_score = king_piece_table_endgame[(piece.color == WHITE ? rank : 7 - rank)][file];
                }
                break;
            default:
                piece_positional_score = 0;
                break;
            }

            evaluation += (piece.color == WHITE) ? piece_positional_score : -piece_positional_score;
        }
    }

    return evaluation;
}

int mobility_evaluation(board &the_board)
{
    chess_piece piece;
    move current_move = {{-1, -1}, {-1, -1}};
    vector<move> white_legal_moves = {};
    vector<move> black_legal_moves = {};
    int mobility_score = 0;

    // Getting the legal moves that black and white could play
    white_legal_moves = generate_legal_moves(the_board, WHITE);
    black_legal_moves = generate_legal_moves(the_board, BLACK);

    /////////////////////////////////////////////////////////////
    //           THINK OF USING LAMBDA FUNCTIONS               //
    /////////////////////////////////////////////////////////////

    // Calculating the mobility scores
    for (int index = 0; index < white_legal_moves.size(); index++)
    {
        current_move = white_legal_moves[index];
        piece = the_board.get_piece_at(current_move.from.rank, current_move.from.file);

        if (piece.type != NONE)
        {
            mobility_score += PIECE_MOBILITY_VALUE[piece.type];
        }
    }

    for (int index = 0; index < black_legal_moves.size(); index++)
    {
        current_move = black_legal_moves[index];
        piece = the_board.get_piece_at(current_move.from.rank, current_move.from.file);

        if (piece.type != NONE)
        {
            mobility_score -= PIECE_MOBILITY_VALUE[piece.type];
        }
    }

    return mobility_score;
}

int king_safety_evaluation(const board &the_board)
{
    square white_king_tile = the_board.find_the_king(WHITE);
    square black_king_tile = the_board.find_the_king(BLACK);

    // The number of pawns defending the white/black king
    int white_defenders_count = 0;
    int black_defenders_count = 0;

    int defender_rank_white;
    int defender_file_white;
    int defender_rank_black;
    int defender_file_black;

    chess_piece piece;

    for (int step_rank = -1; step_rank <= 1; step_rank++)
    {
        for (int step_file = -1; step_file <= 1; step_file++)
        {
            if (step_rank == 0 && step_file == 0)
            {
                continue;
            }

            defender_rank_white = white_king_tile.rank + step_rank;
            defender_file_white = white_king_tile.file + step_file;
            defender_rank_black = black_king_tile.rank + step_rank;
            defender_file_black = black_king_tile.file + step_file;

            if (defender_rank_white >= 0 && defender_rank_white < BOARD_SIZE && defender_file_white >= 0 && defender_file_white < BOARD_SIZE)
            {
                piece = the_board.get_piece_at(defender_rank_white, defender_file_white);

                if (piece.type == PAWN && piece.color == WHITE)
                {
                    white_defenders_count++;
                }
            }

            if (defender_rank_black >= 0 && defender_rank_black < BOARD_SIZE && defender_file_black >= 0 && defender_file_black < BOARD_SIZE)
            {
                piece = the_board.get_piece_at(defender_rank_black, defender_file_black);

                if (piece.type == PAWN && piece.color == BLACK)
                {
                    black_defenders_count++;
                }
            }
        }
    }

    return (white_defenders_count - black_defenders_count) * (int)PAWN_DEFENDER_SCORE;
}

int pawn_structure_evaluation(const board &the_board)
{
    int evaluation = 0;
    int white_pawns; // number of white pawns on a file
    int black_pawns; // number of black pawns on a file
    chess_piece piece;

    int white_pawns_on_file[BOARD_SIZE]; // Used to store the number of white pawns found on each file
    int black_pawns_on_file[BOARD_SIZE]; // Used to store the number of black pawns found on each file

    int isolated_white_pawns_count = 0; // Number of isolated white pawns
    int isolated_black_pawns_count = 0; // Number of isolated black pawns

    // Getting the number of white ppawns and black pawns on each file
    for (int file = 0; file < BOARD_SIZE; file++)
    {
        white_pawns = 0;
        black_pawns = 0;

        for (int rank = 0; rank < BOARD_SIZE; rank++)
        {
            piece = the_board.get_piece_at(rank, file);

            if (piece.type == PAWN)
            {
                if (piece.color == WHITE)
                {
                    white_pawns++;
                }
                else if (piece.color == BLACK)
                {
                    black_pawns++;
                }
            }
        }

        // Calculating the penalty for each file and adjusting the evaluation

        if (white_pawns > 1)
        {
            evaluation -= DOUBLE_PAWN_PENALTY * (white_pawns - 1);
        }

        if (black_pawns > 1)
        {
            evaluation += DOUBLE_PAWN_PENALTY * (black_pawns - 1);
        }

        white_pawns_on_file[file] = white_pawns;
        black_pawns_on_file[file] = black_pawns;
    }

    // Looking for isolated pawns and counting them
    for (int file = 0; file < BOARD_SIZE; file++)
    {
        if (file == 0)
        {
            if (white_pawns_on_file[file] >= 1 && white_pawns_on_file[file + 1] == 0)
            {
                isolated_white_pawns_count++;
            }

            if (black_pawns_on_file[file] >= 1 && black_pawns_on_file[file + 1] == 0)
            {
                isolated_black_pawns_count++;
            }
        }
        else if (file == BOARD_SIZE - 1)
        {
            if (white_pawns_on_file[file] >= 1 && white_pawns_on_file[file - 1] == 0)
            {
                isolated_white_pawns_count++;
            }

            if (black_pawns_on_file[file] >= 1 && black_pawns_on_file[file - 1] == 0)
            {
                isolated_black_pawns_count++;
            }
        }
        else
        {
            if (white_pawns_on_file[file] >= 1 && white_pawns_on_file[file - 1] == 0 && white_pawns_on_file[file + 1] == 0)
            {
                isolated_white_pawns_count++;
            }

            if (black_pawns_on_file[file] >= 1 && black_pawns_on_file[file - 1] == 0 && black_pawns_on_file[file + 1] == 0)
            {
                isolated_black_pawns_count++;
            }
        }
    }

    // Adjusting the evaluation for the isolated pawn penalty
    evaluation -= ISOLATED_PAWN_PENALTY * (isolated_white_pawns_count);
    evaluation += ISOLATED_PAWN_PENALTY * (isolated_black_pawns_count);

    return evaluation;
}

int center_control_evaluation(const board &the_board)
{
    int evaluation = 0;
    chess_piece piece;
    // Since a chess board will always have an even number of
    // ranks and files, this formula will work
    int middle_square = BOARD_SIZE / 2;

    for (int rank = middle_square - 1; rank <= middle_square; rank++)
    {
        for (int file = middle_square - 1; file <= middle_square; file++)
        {
            piece = the_board.get_piece_at(rank, file);

            if (piece.type == NONE)
            {
                continue;
            }

            int score = CENTER_CONTROL_BONUS[piece.type];

            // Adjusting the evaluation based on piece color
            evaluation += (piece.color == WHITE ? score : -score);
        }
    }

    return evaluation;
}

int evaluate_board(board &the_board, const int &depth)
{
    // If white is checkmated, this is ideal for minimizing player
    if (the_board.checkmate_or_stalemate(WHITE, false))
    {
        return -100000 + depth; // We add depth so that the AI chooses shortest path to checkmate
    }

    // If black is checkmated, this is ideal for maximizing player
    if (the_board.checkmate_or_stalemate(BLACK, false))
    {
        return 100000 - depth; // We substract depth so that the AI chooses shortest path to checkmate
    }

    // Checking for stalemate
    if (the_board.checkmate_or_stalemate(WHITE, true) || the_board.checkmate_or_stalemate(BLACK, true))
    {
        return 0;
    }

    int material_score = 0;
    int positional_score = 0;
    int mobility_score = 0;
    int king_safety_score = 0;
    int pawn_structure_score = 0;
    int center_control_score = 0;

    material_score = material_evaluation(the_board);
    positional_score = positional_evaluation(the_board);
    // mobility_score = mobility_evaluation(the_board);
    // king_safety_score = king_safety_evaluation(the_board);
    // pawn_structure_score = pawn_structure_evaluation(the_board);
    // center_control_score = center_control_evaluation(the_board);

    // We will implement dynamic weights depending on the game phase :)))
    double final_heuristic_value = ((int)MATERIAL_EVALUATION_WEIGHT_PERCENTAGE / 100.0) * material_score +
                                   ((int)POSITIONAL_EVALUATION_WEIGHT_PERCENTAGE / 100.0) * positional_score +
                                   ((int)MOBILITY_EVALUATION_WEIGHT_PERCENTAGE / 100.0) * mobility_score +
                                   ((int)KING_SAFETY_EVALUATION_WEIGHT_PERCENTAGE / 100.0) * king_safety_score +
                                   ((int)PAWN_STRUCTURE_EVALUATION_WEIGHT_PERCENTAGE / 100.0) * pawn_structure_score +
                                   ((int)CENTER_CONTROL_EVALUATION_WEIGHT_PERCENTAGE / 100.0) * center_control_score;

    return (int)final_heuristic_value;
}

vector<square> possible_bishop_moves(const square &start_square, const int max_displacement)
{
    vector<square> bishop_possible_destination_squares = {};
    int bishop_destination_rank = -1;
    int bishop_destination_file = -1;
    square bishop_destination_square;

    for (int step = 1; step <= max_displacement; step++)
    {
        for (int step_rank = -step; step_rank <= step; step_rank += 2 * (abs(step)))
        {
            bishop_destination_rank = start_square.rank + step_rank;

            if (bishop_destination_rank < 0 || bishop_destination_rank >= BOARD_SIZE)
            {
                continue;
            }
            for (int step_file = -step; step_file <= step; step_file += 2 * (abs(step)))
            {
                bishop_destination_file = start_square.file + step_file;

                if (bishop_destination_file < 0 || bishop_destination_file >= BOARD_SIZE)
                {
                    continue;
                }

                bishop_destination_square = {bishop_destination_rank, bishop_destination_file};
                bishop_possible_destination_squares.push_back(bishop_destination_square);
            }
        }
    }

    return bishop_possible_destination_squares;
}

vector<square> possible_rook_moves(const square &start_square, const int max_displacement)
{
    vector<square> rook_possible_destination_squares = {};
    int rook_destination_rank = -1;
    int rook_destination_file = -1;
    square rook_destination_square;

    for (int step = -max_displacement; step <= max_displacement; step++)
    {
        rook_destination_rank = start_square.rank + step;
        rook_destination_file = start_square.file;
        rook_destination_square = {rook_destination_rank, rook_destination_file};

        if ((rook_destination_file >= 0 && rook_destination_file < BOARD_SIZE) && (rook_destination_rank >= 0 && rook_destination_rank < BOARD_SIZE))
        {
            rook_possible_destination_squares.push_back(rook_destination_square);
        }

        rook_destination_rank = start_square.rank;
        rook_destination_file = start_square.file + step;
        rook_destination_square = {rook_destination_rank, rook_destination_file};

        if ((rook_destination_file >= 0 && rook_destination_file < BOARD_SIZE) && (rook_destination_rank >= 0 && rook_destination_rank < BOARD_SIZE))
        {
            rook_possible_destination_squares.push_back(rook_destination_square);
        }
    }

    return rook_possible_destination_squares;
}

vector<square> generate_possible_destination_squares(const square &start_square, const chess_piece &piece)
{
    vector<square> possible_destination_squares = {};
    vector<square> rook_possible_destination_squares = {};
    vector<square> bishop_possible_destination_squares = {};
    int destination_rank = -1;
    int destination_file = -1;
    square destination_square = {destination_rank, destination_file};
    int start_step = 0;
    int end_step = 0;

    switch (piece.type)
    {
    case PAWN:
        // Upperbound and lowerbound of loop depends on the piece color
        if (piece.color == WHITE)
        {
            start_step = -2;
            end_step = -1;
        }
        else
        {
            start_step = 1;
            end_step = 2;
        }

        for (int step_rank = start_step; step_rank <= end_step; step_rank++)
        {
            // Move the pawn in terms of rank
            destination_rank = start_square.rank + step_rank;
            // If it goes outside the board, ignore this move
            if (destination_rank < 0 || destination_rank >= BOARD_SIZE)
            {
                continue;
            }

            if (abs(step_rank) == 2)
            {
                destination_file = start_square.file;
                destination_square.rank = destination_rank;
                destination_square.file = destination_file;
                possible_destination_squares.push_back(destination_square);
            }
            else
            {
                for (int step_file = -1; step_file <= 1; step_file++)
                {
                    destination_file = start_square.file + step_file;
                    if (destination_file >= 0 && destination_file < BOARD_SIZE)
                    {
                        destination_square.rank = destination_rank;
                        destination_square.file = destination_file;
                        possible_destination_squares.push_back(destination_square);
                    }
                }
            }
        }
        break;

    case KNIGHT:
        for (int step_rank = -2; step_rank <= 2; step_rank++)
        {
            if (step_rank == 0)
            {
                continue;
            }

            destination_rank = start_square.rank + step_rank;

            if (destination_rank < 0 || destination_rank >= BOARD_SIZE)
            {
                continue;
            }

            for (int step_file = -2; step_file <= 2; step_file++)
            {
                if ((abs(step_rank) == 1 && abs(step_file) == 2) || (abs(step_rank) == 2 && abs(step_file) == 1))
                {
                    destination_file = start_square.file + step_file;

                    if (destination_file < 0 || destination_file >= BOARD_SIZE)
                    {
                        continue;
                    }

                    destination_square = {destination_rank, destination_file};
                    possible_destination_squares.push_back(destination_square);
                }
            }
        }
        break;
    case BISHOP:
        bishop_possible_destination_squares = possible_bishop_moves(start_square, BOARD_SIZE);
        possible_destination_squares.insert(possible_destination_squares.end(), bishop_possible_destination_squares.begin(), bishop_possible_destination_squares.end());
        break;
    case ROOK:
        rook_possible_destination_squares = possible_rook_moves(start_square, BOARD_SIZE - 1);
        possible_destination_squares.insert(possible_destination_squares.end(), rook_possible_destination_squares.begin(), rook_possible_destination_squares.end());
        break;
    case QUEEN:
        rook_possible_destination_squares = possible_rook_moves(start_square, BOARD_SIZE - 1);
        possible_destination_squares.insert(possible_destination_squares.end(), rook_possible_destination_squares.begin(), rook_possible_destination_squares.end());
        bishop_possible_destination_squares = possible_bishop_moves(start_square, BOARD_SIZE);
        possible_destination_squares.insert(possible_destination_squares.end(), bishop_possible_destination_squares.begin(), bishop_possible_destination_squares.end());
        break;
    case KING:
        // Castling
        for (int step = -2; step <= 2; step += 4)
        {
            destination_file = start_square.file + step;

            if (destination_file < 0 || destination_file >= BOARD_SIZE)
            {
                continue;
            }

            destination_rank = start_square.rank;
            destination_square = {destination_rank, destination_file};

            possible_destination_squares.push_back(destination_square);
        }

        // Normal king moves
        rook_possible_destination_squares = possible_rook_moves(start_square, 1);
        possible_destination_squares.insert(possible_destination_squares.end(), rook_possible_destination_squares.begin(), rook_possible_destination_squares.end());
        bishop_possible_destination_squares = possible_bishop_moves(start_square, 1);
        possible_destination_squares.insert(possible_destination_squares.end(), bishop_possible_destination_squares.begin(), bishop_possible_destination_squares.end());
        break;
    default:
        break;
    }

    return possible_destination_squares;
}

vector<move> generate_legal_moves(board &the_board, piece_color player_color)
{
    chess_piece piece;
    vector<move> legal_moves = {};
    vector<move> check_moves = {};
    vector<move> capture_moves = {};
    vector<move> quiet_moves = {};
    vector<square> possible_piece_destinations = {};
    square start_square = {-1, -1};
    square end_square = {-1, -1};

    move try_move = {{-1, -1}, {-1, -1}};

    for (int start_rank = 0; start_rank < BOARD_SIZE; start_rank++)
    {
        for (int start_file = 0; start_file < BOARD_SIZE; start_file++)
        {
            piece = the_board.get_piece_at(start_rank, start_file);

            if (piece.type == NONE || piece.color != player_color)
            {
                continue;
            }

            start_square.rank = start_rank;
            start_square.file = start_file;

            possible_piece_destinations = generate_possible_destination_squares(start_square, piece);

            for (int index = 0; index < possible_piece_destinations.size(); index++)
            {
                end_square = possible_piece_destinations[index];
                try_move = {start_square, end_square};

                if (is_legal_move(the_board, try_move, false))
                {
                    chess_piece target_piece = the_board.get_piece_at(end_square.rank, end_square.file);
                    // Checking if it is a capture move
                    if (target_piece.type != NONE || (piece.type == PAWN && abs(end_square.file - start_square.file) == 1))
                    {
                        capture_moves.push_back(try_move);
                        continue;
                    }

                    // Simulating the legal move on the board
                    the_board.move_piece(try_move);

                    if (the_board.king_in_check(player_color == WHITE ? BLACK : WHITE))
                    {
                        check_moves.push_back(try_move);
                    }
                    else
                    {
                        quiet_moves.push_back(try_move);
                    }

                    // Returning the board to its original state
                    the_board.unmove_piece();
                }
            }
        }
    }

    // Adding the capture moves to legal moves
    legal_moves.insert(legal_moves.end(), capture_moves.begin(), capture_moves.end());
    // Adding the check moves to legal moves
    legal_moves.insert(legal_moves.end(), check_moves.begin(), check_moves.end());
    // Adding the quiet moves to legal moves
    legal_moves.insert(legal_moves.end(), quiet_moves.begin(), quiet_moves.end());

    return legal_moves;
}

bool automatic_white_pawn_promotion(board &the_board, const chess_piece &moved_piece, const move &move_made)
{
    bool white_pawn_promoted = false;
    // Automatically promoting to a queen if pawn reaches promotion square
    if (moved_piece.type == PAWN && move_made.to.rank == 0)
    {
        the_board.set_piece_at(move_made.to, {QUEEN, WHITE});
        white_pawn_promoted = true;

        // Removing the promoted pawn from white pieces remaining
        auto index_of_pawn_to_remove = find(the_board.white_pieces_remaining.begin(), the_board.white_pieces_remaining.end(), PAWN);
        if (index_of_pawn_to_remove != the_board.white_pieces_remaining.end())
        {
            the_board.white_pieces_remaining.erase(index_of_pawn_to_remove);
        }

        // Adding a white queen to white pieces remaining
        the_board.white_pieces_remaining.push_back(QUEEN);
    }

    return white_pawn_promoted;
}

void undo_white_pawn_promotion(board &the_board, const move &move_made)
{
    the_board.set_piece_at(move_made.to, {PAWN, WHITE});

    // Removing the promoted pawn from white pieces remaining
    auto index_of_queen_to_remove = find(the_board.white_pieces_remaining.begin(), the_board.white_pieces_remaining.end(), QUEEN);
    if (index_of_queen_to_remove != the_board.white_pieces_remaining.end())
    {
        the_board.white_pieces_remaining.erase(index_of_queen_to_remove);
    }

    // Adding a white pawn to white pieces remaining
    the_board.white_pieces_remaining.push_back(PAWN);
}

bool automatic_black_pawn_promotion(board &the_board, const chess_piece &moved_piece, const move &move_made)
{
    bool black_pawn_promoted = false;
    // Automatically promoting to a queen if pawn reaches promotion square
    if (moved_piece.type == PAWN && move_made.to.rank == BOARD_SIZE - 1)
    {
        the_board.set_piece_at(move_made.to, {QUEEN, BLACK});
        black_pawn_promoted = true;

        // Removing the promoted pawn from black pieces remaining
        auto index_of_pawn_to_remove = find(the_board.black_pieces_remaining.begin(), the_board.black_pieces_remaining.end(), PAWN);
        if (index_of_pawn_to_remove != the_board.black_pieces_remaining.end())
        {
            the_board.black_pieces_remaining.erase(index_of_pawn_to_remove);
        }

        // Adding a black queen to black pieces remaining
        the_board.black_pieces_remaining.push_back(QUEEN);
    }

    return black_pawn_promoted;
}

void undo_black_pawn_promotion(board &the_board, const move &move_made)
{
    the_board.set_piece_at(move_made.to, {PAWN, BLACK});

    // Removing the promoted pawn from black pieces remaining
    auto index_of_queen_to_remove = find(the_board.black_pieces_remaining.begin(), the_board.black_pieces_remaining.end(), QUEEN);
    if (index_of_queen_to_remove != the_board.black_pieces_remaining.end())
    {
        the_board.black_pieces_remaining.erase(index_of_queen_to_remove);
    }

    // Adding a black pawn to black pieces remaining
    the_board.black_pieces_remaining.push_back(PAWN);
}

bool promote_pawn_to_queen(board &the_board, const chess_piece &moved_piece, const move &move_made, const piece_color player_color)
{
    if (player_color == WHITE)
    {
        return automatic_white_pawn_promotion(the_board, moved_piece, move_made);
    }
    else
    {
        return automatic_black_pawn_promotion(the_board, moved_piece, move_made);
    }
}

void unpromote_pawn_from_queen(board &the_board, const move &move_made, const piece_color player_color)
{
    if (player_color == WHITE)
    {
        undo_white_pawn_promotion(the_board, move_made);
    }
    else
    {
        undo_black_pawn_promotion(the_board, move_made);
    }
}
