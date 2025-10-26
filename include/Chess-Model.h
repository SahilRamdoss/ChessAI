#ifndef CHESS_MODEL_H
#define CHESS_MODEL_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <vector>
#include <string>
#include <stack>

using std::vector, std::string, std::stack;

const int TILE_SIZE = 80;                             // Size of each tile on the chess board
const int BOARD_SIZE = 8;                             // The number of files/ranks on the chess board
constexpr int WINDOW_WIDTH = BOARD_SIZE * TILE_SIZE;  // Getting the width of the window
constexpr int WINDOW_HEIGHT = BOARD_SIZE * TILE_SIZE; // Getting the height of the window
const int PROMOTION_BLOCK_X_TOP_LEFT = 2;             // File number of the top left corner of the promotion block
const int PROMOTION_BLOCK_Y_TOP_LEFT_WHITE = 4;       // Rank number of the top left corner of the promotion block
                                                      // for white pieces
const int PROMOTION_BLOCK_Y_TOP_LEFT_BLACK = 3;       // Rank number of the top left corner of the promotion block
                                                      // for black pieces
const int PROMOTION_BLOCK_WIDTH_TILE = 4;             // The width of the promotion block in terms of tiles.
const int PAWN_DEFENDER_SCORE = 20;                   // Score of a pawn defender in king_safety_evaluation
const int DOUBLE_PAWN_PENALTY = 15;                   // Penalty for each double pawn in pawn structure evaluation
const int ISOLATED_PAWN_PENALTY = 25;                 // Penalty for each isolated pawn in pawn structure evaluation
const int TYPE_OF_PIECE_COUNT = 6;                    // Number of different types of pieces on Chess board
const int PIECE_VALUE[TYPE_OF_PIECE_COUNT] = {100,320,330,500,900,20000}; // Piece values for PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
const int PIECE_PHASE_WEIGHT[TYPE_OF_PIECE_COUNT] = {0,1,1,2,4,0};        // How much each piece contributes to the game phase
const int OPENING_PIECES_COUNT[TYPE_OF_PIECE_COUNT] = {16,4,4,4,2,2};      // Number of each piece type at the start of the game
const int PIECE_MOBILITY_VALUE[TYPE_OF_PIECE_COUNT] = {0,4,3,2,1,0};      // Mobility values of each piece type
const int CENTER_CONTROL_BONUS[TYPE_OF_PIECE_COUNT] = {10,20,20,5,30,0};  // Center control bonus score for each piece type

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* PIECE SQUARE TABLES */

/* PAWN */

// Piece square table for pawn for opening game
const int pawn_piece_table_opening[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {5, 10, 10, -20, -20, 10, 10, 5},
    {5, -5, -10, 0, 0, -10, -5, 5},
    {0, 0, 0, 20, 20, 0, 0, 0},
    {5, 5, 10, 25, 25, 10, 5, 5},
    {10, 10, 20, 30, 30, 20, 10, 10},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

// Piece square table for pawn for middle game
const int pawn_piece_table_middlegame[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {5, 10, 10, -10, -10, 10, 10, 5},
    {5, -5, -10, 0, 0, -10, -5, 5},
    {0, 0, 0, 20, 20, 0, 0, 0},
    {5, 5, 10, 25, 25, 10, 5, 5},
    {10, 10, 20, 30, 30, 20, 10, 10},
    {40, 40, 40, 45, 45, 40, 40, 40},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

// Piece square table for pawn for endgame
const int pawn_piece_table_endgame[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 10, 10, 0, 0, 0},
    {0, 0, 10, 20, 20, 10, 0, 0},
    {5, 10, 10, 20, 20, 10, 10, 5},
    {10, 10, 20, 30, 30, 20, 10, 10},
    {20, 20, 30, 40, 40, 30, 20, 20},
    {40, 40, 50, 60, 60, 50, 40, 40},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

/* KNIGHT */

// Piece square table for knight for opening
const int knight_piece_table_opening[8][8] = {
    {-50, -40, -30, -30, -30, -30, -40, -50},
    {-40, -20, 0, 0, 0, 0, -20, -40},
    {-30, 0, 10, 15, 15, 10, 0, -30},
    {-30, 5, 15, 20, 20, 15, 5, -30},
    {-30, 0, 15, 20, 20, 15, 0, -30},
    {-30, 5, 10, 15, 15, 10, 5, -30},
    {-40, -20, 0, 5, 5, 0, -20, -40},
    {-50, -40, -30, -30, -30, -30, -40, -50}
};

// Piece square table for Knight for middle game
const int knight_piece_table_middlegame[8][8] = {
    {-50, -30, -20, -20, -20, -20, -30, -50},
    {-30, -10, 0, 0, 0, 0, -10, -30},
    {-20, 0, 10, 15, 15, 10, 0, -20},
    {-20, 5, 15, 20, 20, 15, 5, -20},
    {-20, 0, 15, 20, 20, 15, 0, -20},
    {-20, 5, 10, 15, 15, 10, 5, -20},
    {-30, -10, 0, 5, 5, 0, -10, -30},
    {-50, -30, -20, -20, -20, -20, -30, -50}
};

// Piece square table for Knight for endgame
const int knight_piece_table_endgame[8][8] = {
    {-50, -30, -20, -10, -10, -20, -30, -50},
    {-30, -10, 0, 0, 0, 0, -10, -30},
    {-20, 0, 10, 15, 15, 10, 0, -20},
    {-10, 5, 15, 20, 20, 15, 5, -10},
    {-10, 0, 15, 20, 20, 15, 0, -10},
    {-20, 5, 10, 15, 15, 10, 5, -20},
    {-30, -10, 0, 5, 5, 0, -10, -30},
    {-50, -30, -20, -10, -10, -20, -30, -50}
};

/* BISHOP */

// Piece square table for bishop for opening
const int bishop_piece_table_opening[8][8] = {
    {-20, -10, -10, -10, -10, -10, -10, -20},
    {-10, 0, 0, 0, 0, 0, 0, -10},
    {-10, 0, 5, 10, 10, 5, 0, -10},
    {-10, 5, 5, 10, 10, 5, 5, -10},
    {-10, 0, 10, 10, 10, 10, 0, -10},
    {-10, 10, 10, 10, 10, 10, 10, -10},
    {-10, 5, 0, 0, 0, 0, 5, -10},
    {-20, -10, -10, -10, -10, -10, -10, -20}
};

// Piece square table for bishop for middle game
const int bishop_piece_table_middlegame[8][8] = {
    {-20, -10, -10, -10, -10, -10, -10, -20},
    {-10, 0, 0, 0, 0, 0, 0, -10},
    {-10, 0, 5, 10, 10, 5, 0, -10},
    {-10, 5, 5, 10, 10, 5, 5, -10},
    {-10, 0, 10, 10, 10, 10, 0, -10},
    {-10, 10, 10, 10, 10, 10, 10, -10},
    {-10, 0, 0, 0, 0, 0, 0, -10},
    {-20, -10, -10, -10, -10, -10, -10, -20}
};

// Piece square table for bishop for endgame
const int bishop_piece_table_endgame[8][8] = {
    {-10, -10, -10, -10, -10, -10, -10, -10},
    {-10, 0, 0, 0, 0, 0, 0, -10},
    {-10, 0, 10, 10, 10, 10, 0, -10},
    {-10, 5, 10, 15, 15, 10, 5, -10},
    {-10, 0, 10, 15, 15, 10, 0, -10},
    {-10, 5, 10, 10, 10, 10, 5, -10},
    {-10, 0, 0, 0, 0, 0, 0, -10},
    {-10, -10, -10, -10, -10, -10, -10, -10}
};

/* ROOK */

// Piece square table for rook for opening
const int rook_piece_table_opening[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {5, 10, 10, 10, 10, 10, 10, 5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {0, 0, 0, 5, 5, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

// Piece square table for rook for middlegame
const int rook_piece_table_middlegame[8][8] = {
    {0, 0, 0, 5, 5, 0, 0, 0},
    {5, 10, 10, 10, 10, 10, 10, 5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {-5, 0, 0, 0, 0, 0, 0, -5},
    {0, 0, 0, 5, 5, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};


// Piece square table for rook for endgame
const int rook_piece_table_endgame[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 5, 10, 10, 10, 10, 5, 0},
    {-5, 0, 0, 5, 5, 0, 0, -5},
    {-5, 0, 0, 5, 5, 0, 0, -5},
    {-5, 0, 0, 5, 5, 0, 0, -5},
    {-5, 0, 0, 5, 5, 0, 0, -5},
    {0, 5, 10, 10, 10, 10, 5, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

/* QUEEN */

// For the queen, only 1 piece square table is needed
const int queen_piece_table[8][8] = {
    {-20, -10, -10, -5, -5, -10, -10, -20},
    {-10, 0, 0, 0, 0, 0, 0, -10},
    {-10, 0, 5, 5, 5, 5, 0, -10},
    {-5, 0, 5, 5, 5, 5, 0, -5},
    {0, 0, 5, 5, 5, 5, 0, -5},
    {-10, 5, 5, 5, 5, 5, 0, -10},
    {-10, 0, 5, 0, 0, 0, 0, -10},
    {-20, -10, -10, -5, -5, -10, -10, -20}
};

/* KING */

// Piece square table for king for opening
const int king_piece_table_opening[8][8] = {
    {20, 30, 10, 0, 0, 10, 30, 20},
    {20, 20, 0, 0, 0, 0, 20, 20},
    {-10, -20, -20, -20, -20, -20, -20, -10},
    {-20, -30, -30, -40, -40, -30, -30, -20},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30}
};

// Piece square table for king for middlegame
const int king_piece_table_middlegame[8][8] = {
    {20, 30, 10, 0, 0, 10, 30, 20},
    {20, 20, 0, 0, 0, 0, 20, 20},
    {-10, -20, -20, -20, -20, -20, -20, -10},
    {-20, -30, -30, -40, -40, -30, -30, -20},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30}
};

// Piece square table for king for endgame
const int king_piece_table_endgame[8][8] = {
    {-50, -40, -30, -20, -20, -30, -40, -50},
    {-30, -20, -10, 0, 0, -10, -20, -30},
    {-30, -10, 20, 30, 30, 20, -10, -30},
    {-30, -10, 30, 40, 40, 30, -10, -30},
    {-30, -10, 30, 40, 40, 30, -10, -30},
    {-30, -10, 20, 30, 30, 20, -10, -30},
    {-30, -30, 0, 0, 0, 0, -30, -30},
    {-50, -40, -30, -20, -20, -30, -40, -50}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum heuristic_weights_percentage
{
    MATERIAL_EVALUATION_WEIGHT_PERCENTAGE = 100,
    POSITIONAL_EVALUATION_WEIGHT_PERCENTAGE = 20,
    MOBILITY_EVALUATION_WEIGHT_PERCENTAGE = 5,
    KING_SAFETY_EVALUATION_WEIGHT_PERCENTAGE = 20,
    PAWN_STRUCTURE_EVALUATION_WEIGHT_PERCENTAGE = 15,
    CENTER_CONTROL_EVALUATION_WEIGHT_PERCENTAGE = 10
};

/**
 * @brief Enum used to keep track of the current phase of the game
 */
enum game_phase
{
    OPENING,
    MIDDLE_GAME,
    ENDGAME
};

/**
 * @brief the enum represents the phase ratio for each game phase * 100. It is used to determine in which
 * game phase we are
 */
enum game_phase_ratio
{
    OPENING_PHASE_RATIO = 55,
    MIDDLE_GAME_PHASE_RATIO = 25
};

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
 * @brief enum used to represent the material value of each type of chess piece. The values assigned are the
 * standard values used in chess engines.
 */
enum piece_value
{
    PAWN_VALUE = 100,
    KNIGHT_VALUE = 320,
    BISHOP_VALUE = 330,
    ROOK_VALUE = 500,
    QUEEN_VALUE = 900,
    KING_VALUE = 20000,
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
 * @brief struct used to keep track of which pieces were captured in the game
 * in chronological order and some data of the pieces and game state
 */
struct move_history_data
{
    chess_piece piece_captured;    // The piece captured
    move move_made;                // The move_made
    chess_piece moved_piece;       // The piece moved

    bool king_side_castle;         // If a castle was made, was it king side castle
    bool queen_side_castle;        // If a castle was made, was it queen side castle

    bool en_passant_capture;       // Is it an en-passant capture?
    bool en_passant_opportunity;   // If pawn moved 2 places, we set it to true
    square en_passant_opportunity_square;// Potential target for en passant capture

    bool prev_white_king_moved; // Has the white king moved since the start of the game for that move
    bool prev_black_king_moved; // Has the black king moved since the start of the game for that move
    bool prev_white_rook_a_moved; // Has the a file white rook moved since the start of the game for that move
    bool prev_white_rook_h_moved; // Has the h file white rook moved since the start of the game for that move
    bool prev_black_rook_a_moved; // Has the a file black rook moved since the start of the game for that move
    bool prev_black_rook_h_moved; // Has the h file black rook moved since the start of the game for that move
};

/**
 * @brief This class is used to represent the state of the board
 */
class board
{
private:
    chess_piece chess_board[BOARD_SIZE][BOARD_SIZE]; // The state of the board
    square en_passant_target;                        // When a pawn moves 2 squares, it is a
                                                     // potential target for en passant capture

    bool white_king_moved = false; // Flag used to keeo track if the white king moved since the beginning of the game
    bool black_king_moved = false; // Flag used to keeo track if the black king moved since the beginning of the game

    bool white_rook_a_moved = false; // Flag used to keeo track if the white rook on the a file moved since the beginning of the game
    bool white_rook_h_moved = false; // Flag used to keeo track if the white rook on the h file moved since the beginning of the game
    bool black_rook_a_moved = false; // Flag used to keeo track if the black rook on the a file moved since the beginning of the game
    bool black_rook_h_moved = false; // Flag used to keeo track if the black rook on the h file moved since the beginning of the game

public:
    vector<piece_type> white_pieces_remaining; // Used to keep track of all the white pieces remaining on the board
    vector<piece_type> black_pieces_remaining; // Used to keep track of all the black pieces remaining on the board
    stack<move_history_data> move_history;     // Used to store data about the moves played and the pieces captured if any.
                                               // Operated as a LIFO structure
    /**
     * @brief The constructor for the class board. It creates the chess board and places all the chess
     * pieces in their starting positions. It also initializes the other attributes of the class
     */
    board();

    /**
     * @brief When a pawn moves by 2 ranks, we store the square behind it as a potential en passant target
     * for one move
     *
     * @param potential_en_passant_target is the square behind the chess pawn which
     * just moved by 2 ranks.
     */
    void set_en_passant_target(const square potential_en_passant_target);

    /**
     * @brief This function returns the potential en passant target
     *
     * @return the potential en passant target
     */
    square get_en_passant_target() const;

    /**
     * @brief This procedure resets the en passant target to nothing on the board
     */
    void reset_en_passant_target();

    /**
     * @brief method used to get the chess piece at this location on the board. We pass in a rank 
     * and a file instead of a square object as this method is used a lot in for loops.
     *
     * @param rank is the rank on the board on which the piece is found
     * @param file is the file on the board on which the piece is found
     *
     * @return the chess piece data
     */
    chess_piece get_piece_at(int rank, int file) const;

    /**
     * @brief method used to place/change the piece on the board at a particular tile
     *
     * @param tile is the tile on which to place the chess piece
     * @param piece is the chess piece to place on that tile
     *
     * @return void
     */
    void set_piece_at(square tile, chess_piece piece);

    /**
     * @brief It moves the piece to its new position on the board. The legality of the move
     * must already have been checked before calling this method.
     *
     * @param current_move is the move made by the player on the board. It has already been
     * confirmed to be a legal move
     * 
     * @return void
     */
    void move_piece(const move &current_move);

    /**
     * @brief It undoes the last move made in the move history, excluding pawn promotions
     */
    void unmove_piece();

    /**
     * @brief function returns whether a piece on the board can attack the given tile
     *
     * @param tile is the tile to check if there is an attack on
     * @param attacker_color is the color of the piece which can possibly attack the tile
     *
     * @return true if the tile can be attacked or false otherwise.
     */
    bool is_square_attacked(square tile, piece_color attacker_color) const;

    /**
     * @brief This function returns the square/tile on which the king of the given color is found
     *
     * @param king_color is the color of the king we are searching for
     *
     * @return the square on which that king is found
     */
    square find_the_king(piece_color king_color) const;

    /**
     * @brief This function checks if the king of the mentioned color is in check
     *
     * @param king_color is the color of the king which we are verifying if it is in check
     *
     * @return true if the king of the mentioned color is in check or false otherwise
     */
    bool king_in_check(piece_color king_color) const;

    /**
     * @brief The method checks if there is a checkmate or stalemate on the board
     *
     * @param king_color is the color of the king which is potentially checkmated
     * @param check_stalemate is a flag used to indicate whether we want the function to check 
     * for checkmate or stalemate
     *
     * @return true if there is a checkmate and false otherwise
     */
    bool checkmate_or_stalemate(piece_color king_color, bool check_stalemate);

    // Setters and getters for each of the boolean attributes of the class used to keep track of castling rights. 
    // For the setters, the parameter truth is simply a boolean value used to set the corresponding attribute of the class
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
struct game
{
    board game_board;          // Used to store the state of the board
    piece_color active_player; // Used to store the colour of the current player
    game_outcome outcome;      // Used to store who won the game.
    int number_of_moves_played; // The number of moves played since the start of the game
};

// Used to group all the SDL objects used and the actions performed on them
class SDLStructures
{
public:
    SDL_Window *window = nullptr; // The window
    SDL_Renderer *renderer = nullptr; // The renderer
    SDL_Texture *piece_textures[2][6]; // The textures for the chess pieces
    SDL_Texture *banner_texture; // The texture for the end of game banner

public:
    /**
     * @brief This function returns the file path of where the png image of
     * the chess piece passed is found
     *
     * @param color is the color of the chess piece
     * @param type is the type of the chess piece(Pawn, Knight, etc...)
     *
     * @return the path
     */
    string get_piece_file_name(piece_color color, piece_type type);

    /**
     * @brief This procedure loads all textures for our chess pieces into piece_textures array
     *
     * @return true if the textures have been loaded correctly into the array and false otherwise.
     */
    bool load_piece_textures();
};

/**
 * @brief This function checks if the user selected a piece or an empty tile. If the user has selected a
 * piece, it then checks if the move performed on the piece is a valid one according following all chess rules.
 *
 * @param the_board is the chess board object
 * @param current_move is the move played by the player on the board
 * @param consider_attacks_only is a flag used to indicate whether we should
 * consider only the attacking behaviour of pieces
 *
 * @return true if the move performed is a legal one and returns false if the user
 * selected an empty tile or performed an illegal move
 */
bool is_legal_move(const board &the_board, const move &current_move, bool consider_attacks_only);

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
 * would make the move illegal. A piece cannot "jump" over another piece, except for the knight
 *
 * @param the_board is the board object
 * @param current_move is the move played by the player on the piece
 * @param single_step_rank is the rank displacement of the piece when it is moved one step closer to its destination, in direction
 * of its displacement
 * @param single_step_file is the file displacement of the piece when it is moved one step closer to its destination, in direction
 * of its displacement
 */
bool check_for_obstruction(const board &the_board, const move &current_move, int single_step_rank, int single_step_file);

/**
 * @brief This function returns true if the user played a valid move and returns false if the user either tried to move a piece incorrectly or
 * tried playing an invalid move when his king is in check. This function is mainly used to ensure the player does not play
 * a move that does not ignore the fact that his king is in check
 *
 * @param the_board is the chess board object
 * @param current_move is the moved being played on the board
 * @param valid_move is a flag used to indicate if the piece is being moved according to the
 * rules of its type or not
 *
 * @return true if the piece is moved correctly and the move is a valid one if the king is in check, or returns false otherwise
 */
bool valid_move_simulated(board &the_board, const move current_move, bool valid_move);

/**
 * @brief The function returns the evaluation based on the number of pieces each player has. White is always the
 * maximizing player while Black is always the minimizing player. Hence, a positive value means WHITE is being favoured
 * for this criteria while a negative value means that BLACK is being favoured for this criteria.
 *
 * @param the_board is the current state of the chess board
 *
 * @return the material evaluation for the current state of the chess board
 */
int material_evaluation(const board &the_board);

/**
 * @brief The function takes the board and analyses it. Based on the analysis, it
 * decides whether we are in the opening, the middle game or the endgame.
 *
 * @param the_board is the state of the board
 *
 * @return the game phase
 */
game_phase determine_game_phase(const board &the_board);

/**
 * @brief The function returns the evaluation based on the positions of the different pieces
 * at the different stages of the game. It uses piece tables to generate the evaluation
 *
 * @param the_board is the state of the board
 *
 * @return the evaluation
 */
int positional_evaluation(const board &the_board);

/**
 * @brief The function provides an evaluation based on how freely the white and black pieces can move
 *
 * @param the_board is the state of the board
 *
 * @return the evaluation
 */
int mobility_evaluation(const board &the_board);

/**
 * @brief The function returns an evaluation based on how many pawns are surrounding the king to protect it
 *
 * @param the_board is the board state
 *
 * @return the evaluation.
 */
int king_safety_evaluation(const board &the_board);

/**
 * @brief The function checks the structure of the pawn by checking for double pawns or isolated pawns, then makes an evaluation
 * based on this.
 *
 * @param the_board is the board state
 *
 * @return the evaluation
 */
int pawn_structure_evaluation(const board &the_board);

/**
 * @brief The function checks who controls the center and returns an evaluation based on that.
 *
 * @param the_board is the state of the board
 *
 * @return the evaluation
 */
int center_control_evaluation(const board &the_board);

/**
 * @brief The function takes the board state and evaluates the position to see who has
 * an advantage
 *
 * @param the_board is the state of the board
 *
 * @return the evaluation
 */
int evaluate_board(board &the_board, const int &depth);

/**
 * @brief The function returns a vector containing all the possible squares 
 * a rook found on the start_square can move to, ignoring any obstructions and whether the
 * king is in check.
 * 
 * @param start_square is the square on which the piece to be moved is found
 * @param max_displacement is the maximum displacement that the piece can have on the board,
 * in terms of tiles
 * 
 * @return a vector containing all those possible destination squares
 */
vector<square> possible_rook_moves(const square &start_square, const int max_displacement);

/**
 * @brief The function returns a vector containing all the possible squares a bishop
 * found on the start_square can move to, ignoring any obstructions in its way and whether the player's 
 * king is in check.
 * 
 * @param start_square is the square on which the bishop to be moved is found
 * @param max_displacement is the maximum displacement that the piece can have on the board,
 * in terms of tiles
 * 
 * @return a vector containing all those possible destination squares
 */
vector<square> possible_bishop_moves(const square &start_square, const int max_displacement);

/**
 * @brief The function returns a vector containing all the possible squares a queen
 * found on the start_square can move to, ignoring any obstructions and whether the
 * king is in check.
 * 
 * @param start_square is the square on which the piece to be moved is found
 * @param max_displacement is the maximum displacement that the piece can have on the board,
 * in terms of tiles
 * 
 * @return a vector containing all those possible destination squares
 */
vector<square> possible_queen_moves(const square &start_square, const int max_displacement);

/**
 * @brief The function generates all the possible destination squares a particular piece on the board can move to
 * following only the piece's moving behaviour and ignoring all other chess rules
 *
 * @param start_square is the square on which the piece is initially found
 * @param piece is the chess_piece found on that square
 *
 * @return a vector containing all those possible destination squares
 */
vector<square> generate_possible_destination_squares(const square &start_square, const chess_piece &piece);

/**
 * @brief This function takes in the board state and the player color. It then returns all the legal moves that the player
 * can play on the board state
 *
 * @param the_board is the state of the board
 * @param player_color is the color of the pieces of the current player's turn
 *
 * @return the vector containing all the legal moves that the player can play on this board state
 */
vector<move> generate_legal_moves(board &the_board, piece_color player_color);

/**
 * @brief This function checks if a white pawn reached a promotion square and if it did., it automatically
 * promotes the pawn to a white queen on that square. This promotion is not recorded in the move history as it is 
 * only temporary
 * 
 * @param the_board is the board object
 * @param moved_piece is the chess piece moved
 * @param move_made is the move made on the chess piece
 * 
 * @return true if a pawn has been promoted to a queen or false otherwise
 */
bool automatic_white_pawn_promotion(board &the_board,const chess_piece &moved_piece,const move &move_made);

/**
 * @brief This procedure undoes the promotion of a white pawn to a white queen. It does not move the unpromoted pawn
 * nor is this recorded in move history as it is temporary.
 */
void undo_white_pawn_promotion(board &the_board, const move &move_made);

/**
 * @brief This function checks if a black pawn reached a promotion square and if it did., it automatically
 * promotes the pawn to a black queen on that square. This promotion is not recorded in the move history as it is 
 * only temporary
 * 
 * @param the_board is the board object
 * @param moved_piece is the chess piece moved
 * @param move_made is the move made on the chess piece
 * 
 * @return true if a pawn has been promoted to a queen or false otherwise
 */
bool automatic_black_pawn_promotion(board &the_board,const chess_piece &moved_piece,const move &move_made);

/**
 * @brief This procedure undoes the promotion of a black pawn to a black queen. It does not move the unpromoted pawn
 * nor is this recorded in move history as it is temporary.
 */
void undo_black_pawn_promotion(board &the_board, const move &move_made);

/**
 * @brief The minimax algorithm returns the evaluation for a move played on the board
 * by calling the evaluation functions
 *
 * @param the_board is the board state
 * @param depth is how deep the minimax algorithm should look in the tree of possibilitties
 * @param alpha is the alpha value used for alpha-beta pruning
 * @param beta is the beta value used for alpha-beta pruning
 * @param maximizing_player is a boolean value used to state if it is WHITE'turn (true)
 * or BLACK's turn(false)
 *
 * @return the evaluation for the move played
 *
 */
int minimax(const board &the_board, int depth, int alpha, int beta, bool maximizing_player);

/**
 * @brief This function automatically promotes a pawn to a queen if that pawn has reached a promotion square.
 * It does not affect the position of any of the pieces. The move is not recorded in move_history as it is temporary.
 * 
 * @param the_board is the board object
 * @param moved_piece is the piece which was moved
 * @param move_made is the move made on the moved piece
 * @param player_color is the color of the chess pieces of the current player's turn
 * 
 * @return true if a pawn has been promoted and false otherwise.
 */
bool promote_pawn_to_queen(board &the_board, const chess_piece &moved_piece, const move &move_made, const piece_color player_color);

/**
 * @brief The function undoes a pawn to queen promotion. This is not recorded in the move history as it is only
 * temporary.
 * 
 * @param the_board is the board object
 * @param move_made is the move made on the moved piece
 * @param player_color is the color of the chess pieces of the current player's turn
 * 
 * @return void
 */
void unpromote_pawn_from_queen(board &the_board, const move &move_made, const piece_color player_color);

/**
 * @brief This function is the entry point for the AI program. It uses the minimax algorithm to find and 
 * return the best move the current player can play.
 * 
 * @param the_board is the board object
 * @param depth is how deep in the tree of possibilities the minimax algorithm will dive
 * @param player_color is the color of the chess pieces of the player for whom the AI has
 * to find the best move
 * 
 * @return the best move that can be played
 */
move find_best_move(board &the_board, int depth, piece_color player_color);

#endif