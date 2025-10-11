#include "Chess-Model.h"
#include <cmath>

using std::abs;

bool is_legal_move(const board &the_board, const move &current_move)
{
    chess_piece piece = the_board.get_piece_at(current_move.from.rank, current_move.from.file);

    // User selected an empty tile
    if (piece.type == NONE)
    {
        return false;
    }

    int rank_displacement = current_move.to.rank - current_move.from.rank;
    int file_displacement = current_move.to.file - current_move.from.file;

    if (rank_displacement == 0 && file_displacement == 0)
    {
        return false;
    }

    chess_piece target = the_board.get_piece_at(current_move.to.rank, current_move.to.file);

    if (target.type != NONE && target.color == piece.color)
    {
        return false;
    }

    int direction = (piece.color == WHITE) ? -1 : 1;

    switch (piece.type)
    {
    case PAWN:

        if (file_displacement == 0 && rank_displacement == direction && target.type == NONE)
        {
            return true;
        }

        if (file_displacement == 0 && rank_displacement == 2 * direction && target.type == NONE)
        {
            int start_rank = (piece.color == WHITE) ? 6 : 1;

            if (current_move.from.rank == start_rank && the_board.get_piece_at(current_move.from.rank + direction, current_move.from.file).type == NONE)
            {
                return true;
            }
        }

        if (abs(file_displacement) == 1 && rank_displacement == direction && target.type != NONE && target.color != piece.color)
        {
            return true;
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
        return (abs(rank_displacement) <= 1 && abs(file_displacement) <= 1);
    default:
        throw "Unhandled piece type";
    }
}

bool valid_rook_move(const board &the_board, const move &current_move, int rank_displacement, int file_displacement)
{
    if (rank_displacement != 0 && file_displacement != 0)
    {
        return false;
    }
    int single_step_rank;
    int single_step_file;

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
    if (abs(rank_displacement) != abs(file_displacement))
    {
        return false;
    }

    int single_step_rank = (rank_displacement > 0) ? 1 : -1;
    int single_step_file = (file_displacement > 0) ? 1 : -1;

    return check_for_obstruction(the_board, current_move, single_step_rank, single_step_file);
}

bool check_for_obstruction(const board &the_board, const move &current_move, int single_step_rank, int single_step_file)
{
    int new_rank = current_move.from.rank + single_step_rank;
    int new_file = current_move.from.file + single_step_file;

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