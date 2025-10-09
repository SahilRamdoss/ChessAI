#include <string>
#include <vector>

using std::vector, std::string;
const int TILE_SIZE = 80;                             // Size of each square on the board
const int BOARD_SIZE = 8;                             // The number of files/ranks on the chess board
constexpr int WINDOW_WIDTH = BOARD_SIZE * TILE_SIZE;  // Getting the width of the window
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
 * @brief This struct stores the state of the board
 */
class board
{
    private:
    chess_piece chess_board[BOARD_SIZE][BOARD_SIZE]; // The state of the board
    vector<piece_type> white_pieces_remaining;      // White pieces reamining on the board
    vector<piece_type> black_pieces_remaining;      // Black pieces remaining on the board
    vector<piece_type> white_pieces_lost;           // White pieces removed from the board
    vector<piece_type> black_pieces_lost;           // Black pieces removed from the board

    public:
    board()
    {
        // Creating the chess board
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
        this->chess_board[BOARD_SIZE-1][0] = this->chess_board[BOARD_SIZE-1][7] = {ROOK, WHITE};
        this->white_pieces_remaining.push_back(ROOK);
        this->white_pieces_remaining.push_back(ROOK);
        this->chess_board[BOARD_SIZE-1][1] = this->chess_board[BOARD_SIZE-1][6] = {KNIGHT, WHITE};
        this->white_pieces_remaining.push_back(KNIGHT);
        this->white_pieces_remaining.push_back(KNIGHT);
        this->chess_board[BOARD_SIZE-1][2] = this->chess_board[BOARD_SIZE-1][5] = {BISHOP, WHITE};
        this->white_pieces_remaining.push_back(BISHOP);
        this->white_pieces_remaining.push_back(BISHOP);
        this->chess_board[BOARD_SIZE-1][3] = {QUEEN, WHITE};
        this->white_pieces_remaining.push_back(QUEEN);
        this->chess_board[BOARD_SIZE-1][4] = {KING, WHITE};
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

    /**
     * @brief method used to get the chess piece at this location on the board
     * 
     * @param rank is the rank on the board on which the piece is found
     * @param file is the file on the board on which the piece is found
     * 
     * @return the chess piece data
     */
    chess_piece get_piece_at(int rank, int file) const
    {
        return this->chess_board[rank][file];
    }
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
 * @brief This function returns the file path of where the png image of
 * the chess piece pass is found
 *
 * @param color is the color of the chess piece
 * @param type is the type of the chess piece(Pawn, Knight, etc...)
 *
 * @return the path
 */
string get_piece_file_name(piece_color color, piece_type type);
