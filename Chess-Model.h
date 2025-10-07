#include <string>
#include <vector>


using std::vector, std::string;

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
 * @brief struct to represent the color of the current player
 */

 enum PLAYER_COLOR
 {
    WHITE,
    BLACK
 };

 /**
  * @brief Enum to represent the values of the different chess pieces
  * 
  */
 enum CHESS_PIECES
 {
    PAWN = 1,
    KNIGHT = 3,
    BISHOP = 3,
    ROOK = 5,
    QUEEN = 9
 };

/**
 * @brief Struct entity to represent the state of the board
 */
struct board{
    string board_state;
    vector<string> white_pieces_lost;
    vector<string> black_pieces_lost;
    vector<string> white_pieces_remaining;
    vector<string> black_pieces_remaining;
};

/**
 * @brief Struct entity to represent the game being played
 * 
 */
struct game{
    board game_board; // Used to store the state of the board state for the game
    bool white_to_move; // To keep track of the current player's turn
    int outcome; // To store the outcome of the game
};

