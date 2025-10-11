#ifndef CHESS_MODEL_H
#define CHESS_MODEL_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <format>
#include <vector>
#include <string>

using std::vector, std::string;
const int TILE_SIZE = 80;                             // Size of each square on the board
const int BOARD_SIZE = 8;                             // The number of files/ranks on the chess board
constexpr int WINDOW_WIDTH = BOARD_SIZE * TILE_SIZE;  // Getting the width of the window
constexpr int WINDOW_HEIGHT = BOARD_SIZE * TILE_SIZE; // Getting the height of the window


/**
 * @brief enum used to represent the type of the chess piece
 */
enum piece_type
{
    FIRST_TYPE,
    PAWN = FIRST_TYPE,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    LAST_TYPE,
    NONE
};

/**
 * @brief enum used to represent the color of chess pieces but also used to distinguish
 * whose turn is it to play
 */
enum piece_color
{
    FIRST_COLOR,
    WHITE = FIRST_COLOR,
    BLACK,
    LAST_COLOR
};

/**
 * @brief The struct is used to represent each chess piece
 */
struct chess_piece
{
    piece_type type;   // The piece type
    piece_color color; // The piece color
};

/**
 * @brief struct used to represent a square on the chess board
 */
struct square
{
    int rank; // Represents a row on the chess board (1-8)
    int file; // Represents a column on the chess board(1-8)
};

/**
 * @brief struct used to represent a move made on the chess board
 */
struct move
{
    square from;
    square to;
};

/**
 * @brief This struct stores the state of the board
 */
class board
{
private:
    chess_piece chess_board[BOARD_SIZE][BOARD_SIZE]; // The state of the board
    vector<piece_type> white_pieces_remaining;       // White pieces reamining on the board
    vector<piece_type> black_pieces_remaining;       // Black pieces remaining on the board
    vector<piece_type> white_pieces_lost;            // White pieces removed from the board
    vector<piece_type> black_pieces_lost;            // Black pieces removed from the board

public:
    board();

    /**
     * @brief method used to get the chess piece at this location on the board
     *
     * @param rank is the rank on the board on which the piece is found
     * @param file is the file on the board on which the piece is found
     *
     * @return the chess piece data
     */
    chess_piece get_piece_at(int rank, int file) const;

    bool move_piece(move current_move);
};

/**
 * @brief This struct is used to store the details for a particular game
 */
struct game
{
    board game_board;          // Used to store the state of the board
    piece_color active_player; // Used to store the colour of the current player
    int outcome;               // Used to store who won the game. (-1 for black, 0 for draw, 1 for white)
};

class SDLStructures
{
public:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *piece_textures[2][6];

public:
    /**
     * @brief This function returns the file path of where the png image of
     * the chess piece pass is found
     *
     * @param color is the color of the chess piece
     * @param type is the type of the chess piece(Pawn, Knight, etc...)
     *
     * @return the path
     */
    string get_piece_file_name(piece_color color, piece_type type);

    /**
     * @brief This procedure load all textures for our chess pieces into piece_textures array
     *
     * @return true if the textures have been loaded correctly into the array
     */
    bool load_piece_textures();
};

bool is_legal_move(const board &the_board, const move &current_move);

bool valid_rook_move(const board &the_board, const move &current_move, int rank_displacement, int file_displacement);

bool valid_bishop_move(const board &the_board, const move &current_move, int rank_displacement, int file_displacement);

bool check_for_obstruction(const board &the_board, const move &current_move, int single_step_rank, int single_step_file);

#endif