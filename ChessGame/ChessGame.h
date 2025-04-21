#include <iostream> // Include standard input/output stream for console I/O
#include <vector> // Include vector for dynamic array handling (used for the board and captures)
#include <string> // Include string for handling text-based input and data
#include <fstream> // Include fstream for file input/output (used to save/load game state)
#include <thread> // Include thread for adding delay
#include <chrono> // Include chrono to specify duration of delay

const int BOARD_SIZE = 8;  // Setting the board size to 8x8 for a standard chess game
const char EMPTY = '.'; // Constants for representing an empty square on the chessboard

// Constants for representing white chess pieces(which is used to define the constant value: once the value is assigned it cannot be changed during the execution)
const char WHITE_KING = 'K';  // 'K' stands for the white king
const char WHITE_QUEEN = 'Q'; // 'Q' stands for the white queen
const char WHITE_ROOK = 'R'; // 'R' stands for the white rook
const char WHITE_BISHOP = 'B'; // 'B' stands for the white bishop
const char WHITE_KNIGHT = 'N';// 'N' stands for the white knight
const char WHITE_PAWN = 'P'; // 'P' stands for the white pawn

// Constants for representing black chess pieces
const char BLACK_KING = 'k'; // 'k' stands for the black king
const char BLACK_QUEEN = 'q'; // 'q' stands for the black queen
const char BLACK_ROOK = 'r'; // 'r' stands for the black rook
const char BLACK_BISHOP = 'b'; // 'b' stands for the black bishop
const char BLACK_KNIGHT = 'n'; // 'n' stands for the black knight
const char BLACK_PAWN = 'p'; // 'p' stands for the black pawn

// Struct to represent a chess piece with its symbol, color, and movement status
struct ChessPiece {
    char symbol; // Character representing the piece (e.g., 'K' for white king)
    bool isWhite; // Storing if the piece is white (true) or black (false)
    bool hasMoved; // Used to track if a piece has moved (important for castling, etc.)
    ChessPiece(char s = EMPTY, bool color = true); // Constructor to initialize a ChessPiece with a symbol and color  // Default piece is empty, white color
};

class ChessBoard { // Defining the ChessBoard class to manage the board and game logic
private:
    std::vector<std::vector<ChessPiece>> board; // 2D vector representing the board
    std::pair<int, int> lastMoveFrom, lastMoveTo; // Stores last move positions
    bool whiteCanCastleKingside, whiteCanCastleQueenside; // Castling flags for white
    bool blackCanCastleKingside, blackCanCastleQueenside; // Castling flags for black
    int halfMoveClock; // For draw rules like 50-move rule
    int fullMoveNumber; // Move counter (used in PGN and game state)

    bool isValidPosition(int row, int col) const; // Helper functions for move validation and other game rules
    std::pair<int, int> findKing(bool isWhite) const;  // Checking if a position is on the board
    bool isPathClear(int fromRow, int fromCol, int toRow, int toCol) const; // Checking if a path is clear for sliding pieces
    bool canPieceAttack(const ChessPiece& piece, int fromRow, int fromCol, int toRow, int toCol) const; // Checking if a piece can attack a square
    bool isValidMove(const ChessPiece& piece, int fromRow, int fromCol, int toRow, int toCol); // Checking if a move is valid

public:
    ChessBoard(); // Constructor for initializing the board
    void resetBoard();  // Function to reset the board to the starting position
    void displayBoard(const std::vector<char>& whiteCaptures, const std::vector<char>& blackCaptures) const;   // Function to display the board on the console, including captured pieces
    bool isKingInCheck(bool isWhite) const; // Checking if a king is in check
    bool movePiece(std::string move, bool isWhiteTurn, char promotion,
                  std::vector<char>& whiteCaptures, std::vector<char>& blackCaptures); // Moving a piece on the board, including special moves like promotions and castling
    bool isCheckmate(bool isWhite); // Checking if the current game state is checkmate
    bool isStalemate(bool isWhite); // Checking if the current game state is stalemate
    bool isDraw() const; // Checking if the game is a draw (including stalemate and 50-move rule)
    bool hasLegalMoves(bool isWhite);  // Checking if the current player has any legal moves left
    void saveGame(const std::string& filename); // Saving the current game state to a file
    void loadGame(const std::string& filename);  // Loading a previously saved game from a file
};

class ChessGame { // Defining the ChessGame class to manage the game flow (turns, rules, etc.)
private:
    ChessBoard board; // A ChessBoard object to manage the board state
    bool whiteTurn; // Boolean to keep track of whose turn it is (true for white, false for black)
    std::vector<char> whiteCaptures; // Vector to store white's captured pieces
    std::vector<char> blackCaptures; // Vector to store black's captured pieces
    int checkCount;  // Counter for how many times a player has been in check

public:
    ChessGame();  // Constructor to initialize the game
    void displayWelcomeMessage(); // Displaying the welcome message when starting the game
    void displayRules(); // Displaying the game rules for the player
    void start();  // Starting and managing the game loop
    void saveGame(const std::string& filename); // Saving the current game state
    void loadGame(const std::string& filename); // Loading a saved game state
};

// Defining a global function to display the main menu options
void displayMenu(); // Function prototype for the menu, definition to be provided elsewhere

// ChessGame.cpp
// Implementation of the chess game logic.
