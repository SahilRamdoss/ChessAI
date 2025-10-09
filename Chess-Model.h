#include <string>
#include <vector>


using std::vector, std::string;
const int TILE_SIZE = 80; // Size of each square on the board
const int BOARD_SIZE = 8; // The number of files/ranks on the chess board
constexpr int WINDOW_WIDTH = BOARD_SIZE * TILE_SIZE; // Getting the width of the window
constexpr int WINDOW_HEIGHT = BOARD_SIZE * TILE_SIZE; // Getting the height of the window

//...................................................................
//.MMMMMM...MMMMMM...OOOOOOO.....DDDDDDDDD....EEEEEEEEEEE.LLLL.......
//.MMMMMM...MMMMMM..OOOOOOOOOO...DDDDDDDDDD...EEEEEEEEEEE.LLLL.......
//.MMMMMM...MMMMMM.OOOOOOOOOOOO..DDDDDDDDDDD..EEEEEEEEEEE.LLLL.......
//.MMMMMMM.MMMMMMM.OOOOO..OOOOO..DDDD...DDDD..EEEE........LLLL.......
//.MMMMMMM.MMMMMMMOOOOO....OOOOO.DDDD....DDDD.EEEE........LLLL.......
//.MMMMMMM.MMMMMMMOOOO......OOOO.DDDD....DDDD.EEEEEEEEEE..LLLL.......
//.MMMMMMMMMMMMMMMOOOO......OOOO.DDDD....DDDD.EEEEEEEEEE..LLLL.......
//.MMMMMMMMMMMMMMMOOOO......OOOO.DDDD....DDDD.EEEEEEEEEE..LLLL.......
//.MMMMMMMMMMMMMMMOOOOO....OOOOO.DDDD....DDDD.EEEE........LLLL.......
//.MMMM.MMMMM.MMMM.OOOOO..OOOOO..DDDD...DDDDD.EEEE........LLLL.......
//.MMMM.MMMMM.MMMM.OOOOOOOOOOOO..DDDDDDDDDDD..EEEEEEEEEEE.LLLLLLLLL..
//.MMMM.MMMMM.MMMM..OOOOOOOOOO...DDDDDDDDDD...EEEEEEEEEEE.LLLLLLLLL..
//.MMMM.MMMMM.MMMM....OOOOOO.....DDDDDDDDD....EEEEEEEEEEE.LLLLLLLLL..
//...................................................................

/**
 * @brief enum used to represent the type of the chess piece
 */
enum piece_type
{
    NONE = 0,
    PAWN = 1,
    KNIGHT = 3,
    BISHOP = 4,
    ROOK = 5,
    QUEEN = 9,
    KING = -1
};

/**
 * @brief enum used to represent the color of chess pieces but also used to distinguish
 * whose turn is it to play
 */
enum piece_color
{
    BLACK = 0,
    WHITE = 1
};

/**
 * @brief The struct is used to represent each chess piece
 */
struct chess_piece
{
    piece_type type; // The piece type
    piece_color color; // The piece color
};


/**
 * @brief This struct stores the state of the board
 */
struct board{
    chess_piece board[BOARD_SIZE][BOARD_SIZE]; // The state of the board
    vector <chess_piece> white_pieces_remaining; // White pieces reamining on the board
    vector <chess_piece> black_pieces_remaining; // Black pieces remaining on the board
    vector <chess_piece> white_pieces_lost; // White pieces removed from the board
    vector <chess_piece> black_pieces_lost; // Black pieces removed from the board
};

/**
 * @brief This struct is used to store the details for a particular game
 */
struct game{
    board game_board; // Used to store the state of the board
    piece_color active_player; // Used to store the colour of the current player
    int outcome; // Used to store who won the game. (-1 for black, 0 for draw, 1 for white)
};

/**
 * @brief struct used to represent a square on the chess board
 */
struct square{
    int rank; // Represents a row on the chess board (1-8)
    int file; // Represents a column on the chess board(1-8)
};

/**
 * @brief struct used to represent a move made on the chess board
 */
struct move{
    square from;
    square to;
};



