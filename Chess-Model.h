#ifndef CHESS_MODEL_H
#define CHESS_MODEL_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
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
 * @brief struct used to represent a move made on a piece on the chess board
 */
struct move
{
    square from; // The tile from which the piece is moved
    square to; // The tile to which the piece is moved
};

/**
 * @brief This class is used to represent the state of the board
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
    /**
     * @brief The constructor for the class board. It creates the chess board and places all the chess
     * pieces in their starting positions
     */
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

    /**
     * @brief Checks if the move made is on a piece and if yes, it moves the piece to its new
     * position on the board
     * 
     * @param current_move is the move made by the player on the board
     * 
     * @return true if the move has been made successfully or false if the the move was being
     * made on an empty tile
     */
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


// Used to group all the SDL objects used and the actions performed on them
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
     * @return true if the textures have been loaded correctly into the array and false otherwise.
     */
    bool load_piece_textures();
};

/**
 * @brief This function checks if the user selected a piece or an empty tile. If the user has selected a
 * piece, it then checks if the move performed on the piece is a valid one according following chess rules.
 * 
 * @param the_board is the chess board object
 * @param current_move is the current move played by the player on the board
 * 
 * @return true if the move performed is a legal one and returns false if the user
 * selected an empty tile or performed an illegal move
 */
bool is_legal_move(const board &the_board, const move &current_move);

/**
 * @brief This function checks if the move made by the user is a valid one according to chess rules
 * 
 * @param the_board is the chess board object
 * @param current_move is the move performed on the rook
 * @param rank_displacement is the displacement of the rook in terms of ranks by the current move
 * @param file_displacement is the displacement of the rook in terms of files by the current move
 * 
 * @return true if the move performed on the rook is a legal one and returns false otherwise
 */
bool valid_rook_move(const board &the_board, const move &current_move, int rank_displacement, int file_displacement);

/**
 * @brief This function checks if the move made by the user on a bishop is a legal one
 * 
 * @param the_board is the chess board object
 * @param current_move is the current move played by the player on the bishop
 * @param rank_displacement is the displacement of the bishop in terms of ranks by the current move
 * @param file_displacement is the displacement of the bishop in terms of files by the current move
 * 
 * @return true if the move performed on the bishop is a legal one and returns false otherwise
 */
bool valid_bishop_move(const board &the_board, const move &current_move, int rank_displacement, int file_displacement);


/**
 * @brief This function checks if when a piece is moved, there is not any other piece in the middle of its path(not destination), as this
 * would make the move illegal. A piece cannot "jump" over another piece
 * 
 * @param the_board is the board object 
 * @param current_move is the move played by the player on the piece
 * @param single_step_rank is the rank displacement of the piece when it is moved one step closer to its destination, in direction
 * of its displacement
 * @param single_step_fileis the file displacement of the piece when it is moved one step closer to its destination, in direction
 * of its displacement
 */
bool check_for_obstruction(const board &the_board, const move &current_move, int single_step_rank, int single_step_file);

#endif