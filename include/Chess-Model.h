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
const int PROMOTION_BLOCK_X_TOP_LEFT = 2;             // Rank number of the top left corner of the promotion block
const int PROMOTION_BLOCK_Y_TOP_LEFT_WHITE = 4;       // File number of the top left corner of the promotion block
                                                      // for white pieces
const int PROMOTION_BLOCK_Y_TOP_LEFT_BLACK = 3;       // File number of the top left corner of the promotion block
                                                      // for black pieces
const int PROMOTION_BLOCK_WIDTH_TILE = 4;             // The width of the promotion block in terms of tiles.


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
 * @brief enum used to represent the outcome of the game.
 */
enum game_outcome
{
    WHITE_WIN,
    BLACK_WIN,
    DRAW,
    UNDETERMINED
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
    square to;   // The tile to which the piece is moved
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

    square en_passant_target = {-1, -1}; // When a pawn moves 2 squares, it is a
                                         // potential target for en passant capture

    bool white_king_moved = false; // Flag used to keeo track if the white king moved since the beginning of the game
    bool black_king_moved = false; // Flag used to keeo track if the black king moved since the beginning of the game

    bool white_rook_a_moved = false; // Flag used to keeo track if the white rook on the a file moved since the beginning of the game
    bool white_rook_h_moved = false; // Flag used to keeo track if the white rook on the h file moved since the beginning of the game
    bool black_rook_a_moved = false; // Flag used to keeo track if the black rook on the a file moved since the beginning of the game
    bool black_rook_h_moved = false; // Flag used to keeo track if the black rook on the h file moved since the beginning of the game

public:
    /**
     * @brief The constructor for the class board. It creates the chess board and places all the chess
     * pieces in their starting positions
     */
    board();

    /**
     * @brief When a piece moves by 2 places, we store it as a potential en passant target
     * for one move
     *
     * @param target is the chess pawn that has been moved by 2 squares
     */
    void set_en_passant_target(const square potential_target_pawn);

    /**
     * @brief This function returns the potential en passant target
     *
     * @return the en passant potential target
     */
    square get_en_passant_target() const;

    /**
     * @brief This procedure resets the en passant target to nothing on the board
     */
    void reset_en_passant_target();

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
     * @brief method used to change the piece on the board at a particular tile
     * 
     * @param rank is the rank of the tile
     * @param file is the file of the tile
     * @param piece is the chess piece to place on that tile
     * 
     * @return;
     */
    void set_piece_at(int rank, int file, chess_piece piece);

    /**
     * @brief It moves the piece to its new
     * position on the board
     *
     * @param current_move is the move made by the player on the board. It has already been
     * confirmed to be a legal move
     */
    void move_piece(const move &current_move);

    /**
     * @brief function returns whether a piece on the board can attack the given tile
     * 
     * @param tile is the tile to check if there is an attack on
     * @param attacker_color is the color of the piece which should be attacking
     * 
     * @return true if the tile can be attacked or false otherwise.
     */
    bool is_square_attacked(square tile, piece_color attacker_color);

    /**
     * @brief This function returns the square/tile on which the king of the given color is found
     * 
     * @param king_color is the color of the king we are searching for
     * 
     * @return the square on which that king is found
     */
    square find_the_king(piece_color king_color);

    /**
     * @brief This function checks if the king of the mentioned color is in check
     * 
     * @param king_color is the color of the king which we are verifying if it is in check
     * 
     * @return true if the king of the mentioned color is in check or false otherwise
     */
    bool king_in_check(piece_color king_color);

    // Setters and getters for each of the boolean attributes of the class. For the setters
    // the parameter truth is simply a boolean value used to set the corresponding attribute of the class
    // board to that truth value. The method names are self-explanatory in terms of  which attributes they act on
    void set_white_king_moved(bool truth);

    bool get_white_king_moved() const;

    void set_black_king_moved(bool truth);

    bool get_black_king_moved() const;

    void set_white_rook_a_moved(bool truth);

    bool get_white_rook_a_moved() const;

    void set_white_rook_h_moved(bool truth);

    bool get_white_rook_h_moved() const;

    void set_black_rook_a_moved(bool truth);

    bool get_black_rook_a_moved() const;

    void set_black_rook_h_moved(bool truth);

    bool get_black_rook_h_moved() const;
};

/**
 * @brief This struct is used to store the details for a particular game
 */
class game
{
    public:
    board game_board;          // Used to store the state of the board
    piece_color active_player; // Used to store the colour of the current player
    game_outcome outcome;      // Used to store who won the game.

    public: 
    // Constructor for board class
    game();

    /**
     * @brief The method checks if there is a checkmate on the board or not
     * 
     * @param king_color is the color of the king which is potentially checkmated
     * 
     * @return true if there is a check mate and false otherwise
     */
    bool checkmate(piece_color king_color);
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
 * @param consider_attacks_only is a flag used to indicate whether we should 
 * consider only the attacking behaviour of pieces
 *
 * @return true if the move performed is a legal one and returns false if the user
 * selected an empty tile or performed an illegal move
 */
bool is_legal_move(board &the_board, const move &current_move, bool consider_attacks_only);

/**
 * @brief This function checks if the move made by the user on a rook is a valid one according to chess rules
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

/**
 * @brief This function returns true if the user played a valid move or false if the user either tried to move a piece incorrect or 
 * tried playing an invalid move when his king is in check
 * 
 * @param the_board is the chess board object
 * @param current_move is the moved being played on the board
 * @param valid_move is a flag used to indicate if the piece is being moved according to the 
 * rules of its type or not
 * 
 * @return true if the piece is moved correctly and the move is a valid one if the king is in check, or returns false otherwise
 */
bool valid_move_simulated(board &the_board, const move current_move, bool valid_move);

#endif