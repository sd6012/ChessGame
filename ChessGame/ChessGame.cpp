#include "ChessGame.h"      // Header file for the chess game, defining pieces, board, and game classes.
using namespace std;  // Allowing the use of standard library features without prefixing 'std::'.

ChessPiece::ChessPiece(char s, bool color) : symbol(s), isWhite(color), hasMoved(false) {}  // Initializing a piece with its symbol, color, and unmoved status.

ChessBoard::ChessBoard() { // Constructor for ChessBoard, initializing the board to an 8x8 grid and calling resetBoard to set up the initial state.
    board.resize(BOARD_SIZE, vector<ChessPiece>(BOARD_SIZE, ChessPiece(EMPTY))); // Resizing the board to 8x8 and filling it with empty ChessPiece objects.
    resetBoard(); // Calling resetBoard function to set up the initial state of the board.
}

void ChessBoard::resetBoard() { // Setting up the initial configuration of pieces on the chessboard.
    lastMoveFrom = lastMoveTo = {-1, -1}; // Resetting last move positions, castle rights, and move clocks.

    // Setting up the castling rights for both white and black.
    whiteCanCastleKingside = whiteCanCastleQueenside = true;
    blackCanCastleKingside = blackCanCastleQueenside = true;

    // Initializing the half-move clock and full-move number to 0 and 1 respectively.
    halfMoveClock = 0;
    fullMoveNumber = 1;

    // Placing the black pieces on the board, setting row 0.
    board[0] = {ChessPiece(BLACK_ROOK, false), ChessPiece(BLACK_KNIGHT, false), ChessPiece(BLACK_BISHOP, false),
               ChessPiece(BLACK_QUEEN, false), ChessPiece(BLACK_KING, false), ChessPiece(BLACK_BISHOP, false),
               ChessPiece(BLACK_KNIGHT, false), ChessPiece(BLACK_ROOK, false)};
    for (int i = 0; i < BOARD_SIZE; i++) board[1][i] = ChessPiece(BLACK_PAWN, false); // Placing the black pawns on row 1.
    for (int row = 2; row < 6; row++) fill(board[row].begin(), board[row].end(), ChessPiece(EMPTY)); // Filling rows 2 to 5 with empty chess pieces, indicating empty squares.

    // Placing the white pieces on the last row (row 7).
    board[7] = {ChessPiece(WHITE_ROOK, true), ChessPiece(WHITE_KNIGHT, true), ChessPiece(WHITE_BISHOP, true),
               ChessPiece(WHITE_QUEEN, true), ChessPiece(WHITE_KING, true), ChessPiece(WHITE_BISHOP, true),
               ChessPiece(WHITE_KNIGHT, true), ChessPiece(WHITE_ROOK, true)};
    for (int i = 0; i < BOARD_SIZE; i++) board[6][i] = ChessPiece(WHITE_PAWN, true);  // Placing the white pawns on row 6.
}

// Function to check if the given position (row, col) is valid on the chessboard.
bool ChessBoard::isValidPosition(int row, int col) const {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;// Checking if the row and column values are within the bounds of the 8x8 board.
}

pair<int, int> ChessBoard::findKing(bool isWhite) const { // Function to find the position of the king for the specified color (white or black).
    for (int row = 0; row < BOARD_SIZE; row++) { // Looping through the entire board to find the king.
        for (int col = 0; col < BOARD_SIZE; col++) {
            if (board[row][col].symbol == (isWhite ? WHITE_KING : BLACK_KING)) { // If the piece is the king of the specified color, return its position.
                return {row, col}; // Returning the position (row, col) of the king
            }
        }
    }
    return {-1, -1}; // If no king is found, return an invalid position (-1, -1).
}

// Function to check if the path between two positions is clear (i.e., no pieces are blocking the path).
bool ChessBoard::isPathClear(int fromRow, int fromCol, int toRow, int toCol) const {

 // Calculating the steps required to move row-wise and column-wise towards the target position.
    int rowStep = (toRow > fromRow) ? 1 : (toRow < fromRow) ? -1 : 0;
    int colStep = (toCol > fromCol) ? 1 : (toCol < fromCol) ? -1 : 0;

    // Starting the check from the square next to the starting position.
    int row = fromRow + rowStep;
    int col = fromCol + colStep;

    while (row != toRow || col != toCol) { // Checking each square along the path towards the target position.
        if (!isValidPosition(row, col) || board[row][col].symbol != EMPTY) return false; // If the square is outside the board or occupied by another piece, return false (path is blocked).
        row += rowStep; // Moving to the next row along the path.
        col += colStep; // Moving to the next column along the path.
    }
    return true;  // If no obstacles are found, the path is clear, return true.
}

// Function to check if a piece can attack a square at the given destination position.
bool ChessBoard::canPieceAttack(const ChessPiece& piece, int fromRow, int fromCol, int toRow, int toCol) const {

    // Calculating the difference in row and column between the start and end positions.
    int rowDiff = toRow - fromRow;
    int colDiff = toCol - fromCol;
    int absRowDiff = abs(rowDiff); // Getting the absolute difference in rows.
    int absColDiff = abs(colDiff); // Getting the absolute difference in columns.

    switch (piece.symbol) {  // Using a switch statement to handle different types of chess pieces.
        case WHITE_PAWN:
             // White pawns attack one square diagonally forward.
            return rowDiff == -1 && absColDiff == 1 && board[toRow][toCol].symbol != EMPTY && !board[toRow][toCol].isWhite;
        case BLACK_PAWN:
            // Black pawns attack one square diagonally forward.
            return rowDiff == 1 && absColDiff == 1 && board[toRow][toCol].symbol != EMPTY && board[toRow][toCol].isWhite;
        case WHITE_KING: case BLACK_KING:
            // Kings can attack one square in any direction (vertically, horizontally, or diagonally).
            return absRowDiff <= 1 && absColDiff <= 1;
        case WHITE_QUEEN: case BLACK_QUEEN:
            // Queens can attack like both a rook and a bishop.
            return (absRowDiff == absColDiff || fromRow == toRow || fromCol == toCol) &&
                   isPathClear(fromRow, fromCol, toRow, toCol);
        case WHITE_ROOK: case BLACK_ROOK:
            // Rooks can attack in straight lines (either rows or columns).
            return (fromRow == toRow || fromCol == toCol) &&
                   isPathClear(fromRow, fromCol, toRow, toCol);
        case WHITE_BISHOP: case BLACK_BISHOP:
            // Bishops can attack diagonally.
            return absRowDiff == absColDiff &&
                   isPathClear(fromRow, fromCol, toRow, toCol);
        case WHITE_KNIGHT: case BLACK_KNIGHT:
            // Knights attack in an "L" shape: either two squares in one direction and one square in another direction, or vice versa.
            return (absRowDiff == 2 && absColDiff == 1) || (absRowDiff == 1 && absColDiff == 2);
        default:
            return false; // If the piece type is not recognized, it cannot attack.
    }
}

// Function to determine whether a specific move is valid for a given chess piece.
// Returns 'true' if the move is valid according to chess rules, otherwise 'false'.
bool ChessBoard::isValidMove(const ChessPiece& piece, int fromRow, int fromCol, int toRow, int toCol) {
    if (!isValidPosition(fromRow, fromCol) || !isValidPosition(toRow, toCol)) return false; // Checking if the source and destination positions are within the bounds of the chessboard.
    if (fromRow == toRow && fromCol == toCol) return false; // Returning false if the piece is trying to move to the same square it's already on.

    int rowDiff = toRow - fromRow; // Calculating the vertical movement (row difference).
    int colDiff = toCol - fromCol;  // Calculating the horizontal movement (column difference).
    int absRowDiff = abs(rowDiff); // Getting the absolute value of row difference.
    int absColDiff = abs(colDiff); // Getting the absolute value of column difference.

    // If the destination square is occupied by a piece of the same color, the move is invalid.
    if (board[toRow][toCol].symbol != EMPTY && board[toRow][toCol].isWhite == piece.isWhite) {
        return false;
    }

    switch (piece.symbol) { // Switch block to handle different types of pieces and their movement rules
        case WHITE_PAWN:
            // White pawn: forward move (one or two squares) if path is clear.
            if (fromCol == toCol && rowDiff < 0 && board[toRow][toCol].symbol == EMPTY) {
                if (rowDiff == -1) return true; // Move one square forward.
                if (rowDiff == -2 && fromRow == 6 && board[fromRow - 1][fromCol].symbol == EMPTY) return true; // Initial two-square move.
            }
             // White pawn: capturing diagonally or en passant.
            if (rowDiff == -1 && absColDiff == 1) {
                if (board[toRow][toCol].symbol != EMPTY && !board[toRow][toCol].isWhite) return true; // Normal diagonal capture.
                // En passant capture condition.
                if (toRow == 5 && lastMoveTo == make_pair(6, toCol) &&
                    lastMoveFrom == make_pair(4, toCol) &&
                    board[6][toCol].symbol == BLACK_PAWN) return true;
            }
            return false; // If none of the valid pawn conditions are met.
        case BLACK_PAWN:
            // If none of the valid pawn conditions are met.
            if (fromCol == toCol && rowDiff > 0 && board[toRow][toCol].symbol == EMPTY) {
                if (rowDiff == 1) return true; // Move one square forward.
                if (rowDiff == 2 && fromRow == 1 && board[fromRow + 1][fromCol].symbol == EMPTY) return true; // Initial two-square move.
            }
            // Black pawn: capturing diagonally or en passant.
            if (rowDiff == 1 && absColDiff == 1) {
                if (board[toRow][toCol].symbol != EMPTY && board[toRow][toCol].isWhite) return true; // Normal diagonal capture.
                // En passant capture condition.
                if (toRow == 2 && lastMoveTo == make_pair(1, toCol) &&
                    lastMoveFrom == make_pair(3, toCol) &&
                    board[1][toCol].symbol == WHITE_PAWN) return true;
            }
            return false; // If none of the valid pawn conditions are met.
        case WHITE_KING: case BLACK_KING:
             // Standard king move: one square in any direction.
            if (absRowDiff <= 1 && absColDiff <= 1) return true;

            // Checking for castling: king has not moved and moves two squares horizontally.
            if (!piece.hasMoved && fromRow == toRow && absColDiff == 2) {
                    // Determine if it's kingside (right) or queenside (left) castling.
                bool kingside = colDiff > 0;
                int rookCol = kingside ? 7 : 0;
             // References to the castling availability flags.
                bool& canCastleKingside = piece.isWhite ? whiteCanCastleKingside : blackCanCastleKingside;
                bool& canCastleQueenside = piece.isWhite ? whiteCanCastleQueenside : blackCanCastleQueenside;

                if ((kingside && !canCastleKingside) || (!kingside && !canCastleQueenside)) return false;   // Check if castling is allowed for the king's side.
                  // Check if castling is allowed for the king's side.
                if (board[fromRow][rookCol].symbol != (piece.isWhite ? WHITE_ROOK : BLACK_ROOK) ||
                    board[fromRow][rookCol].hasMoved || isKingInCheck(piece.isWhite)) return false;

                      // Check that all squares between king and rook are empty
                int step = kingside ? 1 : -1;
                for (int col = fromCol + step; col != rookCol; col += step) {
                    if (board[fromRow][col].symbol != EMPTY) return false;
                }

             // Temporarily simulate the king's move across the squares to ensure it does not pass through check.
                ChessPiece tempKing = board[fromRow][fromCol];
                for (int i = 1; i <= 2; i++) {
                    board[fromRow][fromCol] = ChessPiece(EMPTY);  // Remove king from current position.
                    board[fromRow][fromCol + i * step] = tempKing; // Place king in new position.
                    if (isKingInCheck(piece.isWhite)) { // If king is in check, castling is invalid.
                        board[fromRow][fromCol] = tempKing;  // Revert position.
                        board[fromRow][fromCol + i * step] = ChessPiece(EMPTY);
                        return false;
                    }
                    board[fromRow][fromCol + i * step] = ChessPiece(EMPTY); // Clean up simulated move.
                }
                board[fromRow][fromCol] = tempKing; // Restore king to original position.
                return true; // Castling is a valid move.
            }
            return false;  // King move is invalid.
             // For other pieces like queen, rook, bishop, knight — use the generic attack logic.
        default:
            return canPieceAttack(piece, fromRow, fromCol, toRow, toCol); // Delegate to piece-specific attack logic.
    }
}

void ChessBoard::displayBoard(const vector<char>& whiteCaptures, const vector<char>& blackCaptures) const {
    cout << "\033[38;5;117m+-----------------+\033[0m\n"; // Print top border with columns.
    cout << "\033[38;5;117m|  a b c d e f g h|\033[0m\n"; // Show column labels (a-h).
    cout << "\033[38;5;117m+-----------------+\033[0m\n"; // Print top border.
    for (int row = 0; row < BOARD_SIZE; row++) { // Loop through rows.
        cout << 8 - row << "|"; // Print row number (8 to 1).
        for (int col = 0; col < BOARD_SIZE; col++) { // Loop through columns.
            cout << " " << board[row][col].symbol; // Print piece symbol.
        }
        cout << " |" << 8 - row << "\n"; // Close row with number.
    }
    cout << "\033[38;5;216m+-----------------+\033[0m\n"; // Print bottom border.
    cout << "\033[38;5;216m|  a b c d e f g h|\033[0m\n"; // Show column labels again.
    cout << "\033[38;5;216m+-----------------+\033[0m\n"; // Print bottom border.
    cout << "\nHalf-moves: " << halfMoveClock << " Full moves: " << fullMoveNumber << "\n"; // Show move counters.
    cout << "\033[38;5;216mWhite captured: \033[0m";  // Label white captures.
    if (whiteCaptures.empty()) cout << "None"; // Show "None" if no captures.
    else for (char piece : whiteCaptures) cout << piece << " ";  // List white captures.
    cout << "\n\033[38;5;117mBlack captured: \033[0m"; // Label black captures.
    if (blackCaptures.empty()) cout << "None"; // Show "None" if no captures.
    else for (char piece : blackCaptures) cout << piece << " "; // List black captures.
    cout << "\n\n";
}

bool ChessBoard::isKingInCheck(bool isWhite) const { // Function to check if the current player's king is under attack (in check).
    pair<int, int> kingPos = findKing(isWhite); // Find the position of the king for the specified color.
    if (kingPos.first == -1) return false; // If the king is not found (should not happen in a valid game), return false.

    for (int row = 0; row < BOARD_SIZE; row++) { // Check all squares for opponent pieces.
        for (int col = 0; col < BOARD_SIZE; col++) { // Loop through columns.
            ChessPiece piece = board[row][col]; // Get the piece at the current square.
            if (piece.symbol != EMPTY && piece.isWhite != isWhite) { // Skip empty or friendly pieces.
                if (canPieceAttack(piece, row, col, kingPos.first, kingPos.second)) { // Check if piece can attack king.
                    return true; // If any opposing piece can attack the king, it's in check.
                }
            }
        }
    }
    return false; // If no enemy piece can attack the king, it's not in check.
}

// Executes a move on the chessboard, handling captures, castling, en passant, and pawn promotion.
bool ChessBoard::movePiece(string move, bool isWhiteTurn, char promotion, vector<char>& whiteCaptures, vector<char>& blackCaptures) {
    if (move.length() != 5 || move[2] != ' ') return false; // Check if move string is exactly 5 characters and has a space in the middle (e.g., "e2 e4")

    int fromCol = move[0] - 'a'; // Convert column letter to 0-based index (e.g., 'a' = 0, 'b' = 1)
    int fromRow = 8 - (move[1] - '0'); // Convert row number to 0-based index (e.g., '1' = row 7, '8' = row 0)
    int toCol = move[3] - 'a'; // Convert destination column letter to index
    int toRow = 8 - (move[4] - '0'); // Convert destination row number to index

    if (!isValidPosition(fromRow, fromCol) || !isValidPosition(toRow, toCol)) return false;  // Check if source and destination positions are valid board coordinates

    ChessPiece piece = board[fromRow][fromCol]; // Retrieve the piece located at the source square
    if (piece.symbol == EMPTY || piece.isWhite != isWhiteTurn) return false;  // Check if there is a piece and if it belongs to the player whose turn it is

    if (!isValidMove(piece, fromRow, fromCol, toRow, toCol)) return false; // Check if the move is valid for the selected piece

    bool isCapture = board[toRow][toCol].symbol != EMPTY; // Check if destination square has a piece (normal capture)
    bool isPawnMove = piece.symbol == WHITE_PAWN || piece.symbol == BLACK_PAWN; // Check if the piece being moved is a pawn
    bool isCastling = (piece.symbol == WHITE_KING || piece.symbol == BLACK_KING) && abs(toCol - fromCol) == 2; // Detect castling by checking if a king is moving two columns
    bool isEnPassant = isPawnMove && abs(toCol - fromCol) == 1 && board[toRow][toCol].symbol == EMPTY &&
                      ((piece.isWhite && toRow == 5) || (!piece.isWhite && toRow == 2)); // Check for en passant: pawn moves diagonally to an empty square on the correct rank

    ChessPiece originalPiece = board[fromRow][fromCol]; // Save the original pieces and positions in case we need to undo the move
    ChessPiece originalToPiece = board[toRow][toCol]; // Store the piece at the destination position (could be EMPTY or opponent's).
    pair<int, int> originalLastMoveFrom = lastMoveFrom; // Save the previous move's starting position.
    pair<int, int> originalLastMoveTo = lastMoveTo; // Save the previous move's ending position.
    ChessPiece originalRook;  // For castling: track the rook and its movement
    ChessPiece enPassantCaptured; // For en passant: store the captured pawn
    int rookFromCol = 0, rookToCol = 0; // Initialize rook columns for castling logic


    bool kingInCheckBefore = isKingInCheck(isWhiteTurn); // Check if king is in check before the move

    board[fromRow][fromCol] = ChessPiece(EMPTY); // Temporarily remove piece from original position
    board[toRow][toCol] = ChessPiece(piece.symbol, piece.isWhite); // Place the moving piece on the target square
    board[toRow][toCol].hasMoved = true; // Mark piece as moved (affects castling/pawn logic)
    lastMoveFrom = {fromRow, fromCol}; // Record last move's starting position
    lastMoveTo = {toRow, toCol}; // Record last move's ending position

    if (isCastling) { // If move is a castling
        rookFromCol = (toCol > fromCol) ? 7 : 0; // Determine rook's original column
        rookToCol = (toCol > fromCol) ? fromCol + 1 : fromCol - 1; // Determine rook's new column
        originalRook = board[fromRow][rookFromCol]; // Save the rook's original state
        board[fromRow][rookToCol] = board[fromRow][rookFromCol];  // Move the rook
        board[fromRow][rookFromCol] = ChessPiece(EMPTY);  // Clear original rook position
        board[fromRow][rookToCol].hasMoved = true; // Mark rook as moved
    }

    if (isEnPassant) { // If move is an en passant capture
        int capturedRow = toRow + (piece.isWhite ? 1 : -1);  // Calculate captured pawn's row
        enPassantCaptured = board[capturedRow][toCol]; // Save the captured pawn
        board[capturedRow][toCol] = ChessPiece(EMPTY); // Remove the pawn
        isCapture = true; // Mark as a capture
    }

    if (isPawnMove && (toRow == 0 || toRow == 7)) { // If pawn reaches last rank
        char promoted = piece.isWhite ? toupper(promotion) : tolower(promotion); // Convert promotion piece case
        if (promoted != 'Q' && promoted != 'R' && promoted != 'B' && promoted != 'N') promoted = 'Q'; // Default to Queen
        board[toRow][toCol].symbol = promoted; // Promote pawn
    }

    bool kingInCheckAfter = isKingInCheck(isWhiteTurn); // Check if king is in check after move
    bool invalidMove = (kingInCheckBefore && kingInCheckAfter) || (!kingInCheckBefore && kingInCheckAfter);  // Determine if move is invalid

    board[fromRow][fromCol] = originalPiece; // Undo move: restore source piece
    board[toRow][toCol] = originalToPiece; // Undo move: restore destination piece
    lastMoveFrom = originalLastMoveFrom; // Restore last move from
    lastMoveTo = originalLastMoveTo; // Restore last move to

    if (isCastling) { // Undo castling if simulated
        board[fromRow][rookFromCol] = originalRook;  // Restore rook
        board[fromRow][rookToCol] = ChessPiece(EMPTY); // Clear moved rook position
    }
    if (isEnPassant) {  // Undo en passant if simulated
        int capturedRow = toRow + (piece.isWhite ? 1 : -1); // Calculate captured pawn row
        board[capturedRow][toCol] = enPassantCaptured; // Restore captured pawn
    }

    if (invalidMove) return false;  // If move leaves king in check, it's invalid

    if (isCapture) { // Record the capture
        (isWhiteTurn ? whiteCaptures : blackCaptures).push_back(originalToPiece.symbol);
    }

    board[fromRow][fromCol] = ChessPiece(EMPTY); // Apply final move: clear source
    board[toRow][toCol] = ChessPiece(piece.symbol, piece.isWhite); // Apply final move: move piece
    board[toRow][toCol].hasMoved = true; // Mark moved
    lastMoveFrom = {fromRow, fromCol};  // Update move tracking
    lastMoveTo = {toRow, toCol};

    if (isCastling) { // Finalize castling
        rookFromCol = (toCol > fromCol) ? 7 : 0;  // Determine rook's original column
        rookToCol = (toCol > fromCol) ? fromCol + 1 : fromCol - 1; // Determine rook's new column
        board[fromRow][rookToCol] = board[fromRow][rookFromCol]; // Move rook
        board[fromRow][rookFromCol] = ChessPiece(EMPTY); // Clear rook's original square
        board[fromRow][rookToCol].hasMoved = true; // Mark rook as moved
        if (piece.isWhite) {  // Disable castling rights for White
            whiteCanCastleKingside = whiteCanCastleQueenside = false;
        } else { // Disable castling rights for Black
            blackCanCastleKingside = blackCanCastleQueenside = false;
        }
    }

    if (isEnPassant) {  // Remove pawn if en passant capture was valid
        int capturedRow = toRow + (piece.isWhite ? 1 : -1); // Get captured pawn's row.
        board[capturedRow][toCol] = ChessPiece(EMPTY); // Remove captured pawn.
    }

    if (isPawnMove && (toRow == 0 || toRow == 7)) { // Promote pawn again in final board state
        char promoted = piece.isWhite ? toupper(promotion) : tolower(promotion);  // Get promotion piece.
        if (promoted != 'Q' && promoted != 'R' && promoted != 'B' && promoted != 'N') promoted = 'Q'; // Default to queen if invalid.
        board[toRow][toCol].symbol = promoted; // Set promoted piece.
    }

    if (piece.symbol == WHITE_ROOK && !piece.hasMoved) { // Update castling rights if white rook moved
        if (fromCol == 0) whiteCanCastleQueenside = false; // Disable queenside castling.
        else if (fromCol == 7) whiteCanCastleKingside = false; // Disable kingside castling.
    }
    if (piece.symbol == BLACK_ROOK && !piece.hasMoved) { // Update castling rights if white rook moved
        if (fromCol == 0) blackCanCastleQueenside = false; // Disable queenside castling.
        else if (fromCol == 7) blackCanCastleKingside = false; // Disable kingside castling.
    }
    if (piece.symbol == WHITE_KING) whiteCanCastleKingside = whiteCanCastleQueenside = false; // Disable white king's castling rights
    if (piece.symbol == BLACK_KING) blackCanCastleKingside = blackCanCastleQueenside = false; // Disable black king's castling rights

    halfMoveClock = (isCapture || isPawnMove) ? 0 : halfMoveClock + 1; // Reset or increment half-move clock
    if (!isWhiteTurn) fullMoveNumber++; // Increment full move number after Black’s move
    return true; // Move was successfully completed
}

bool ChessBoard::isCheckmate(bool isWhite) {
    if (!isKingInCheck(isWhite)) return false; // If king is not in check, it's not checkmate
    return !hasLegalMoves(isWhite); // If king is in check and no legal moves, it's checkmate
}

bool ChessBoard::isStalemate(bool isWhite) {
    if (isKingInCheck(isWhite)) return false;  // If king is in check, it's not stalemate
    return !hasLegalMoves(isWhite); // If not in check and no legal moves, it's stalemate
}

bool ChessBoard::isDraw() const {
    return halfMoveClock >= 50; // Draw if 50 half-moves passed with no pawn move or capture
}

bool ChessBoard::hasLegalMoves(bool isWhite) {
    for (int fromRow = 0; fromRow < BOARD_SIZE; fromRow++) { // Loop through all rows
        for (int fromCol = 0; fromCol < BOARD_SIZE; fromCol++) { // Loop through all columns
            ChessPiece piece = board[fromRow][fromCol]; // Get the piece at current position
            if (piece.symbol != EMPTY && piece.isWhite == isWhite) { // Check if piece belongs to current player
                for (int toRow = 0; toRow < BOARD_SIZE; toRow++) { // Try moving to all rows
                    for (int toCol = 0; toCol < BOARD_SIZE; toCol++) {  // Try moving to all columns
                        if (isValidMove(piece, fromRow, fromCol, toRow, toCol)) { // Check if move is valid
                            ChessPiece originalPiece = board[fromRow][fromCol]; // Save original piece
                            ChessPiece originalToPiece = board[toRow][toCol];  // Save target square piece
                            pair<int, int> originalLastMoveFrom = lastMoveFrom; // Save last move from
                            pair<int, int> originalLastMoveTo = lastMoveTo; // Save last move to

                            board[toRow][toCol] = ChessPiece(piece.symbol, piece.isWhite); // Simulate move to target
                            board[toRow][toCol].hasMoved = true; // Mark as moved
                            board[fromRow][fromCol] = ChessPiece(EMPTY); // Clear source square
                            lastMoveFrom = {fromRow, fromCol}; // Update move from
                            lastMoveTo = {toRow, toCol}; // Update move to

                            bool stillInCheck = isKingInCheck(isWhite); // Check if king is still in check

                            board[fromRow][fromCol] = originalPiece; // Undo move: restore source
                            board[toRow][toCol] = originalToPiece; // Undo move: restore target
                            lastMoveFrom = originalLastMoveFrom; // Restore last move from
                            lastMoveTo = originalLastMoveTo; // Restore last move to

                            if (!stillInCheck) return true; // If king is safe after move, legal move exists
                        }
                    }
                }
            }
        }
    }
    return false; // No legal move found, return false
}

void ChessBoard::saveGame(const string& filename) {
    ofstream file(filename); // Open file for writing
    if (file.is_open()) {  // Check if file is open
        file << (whiteCanCastleKingside ? "1" : "0") << " " // Save white kingside castling right
             << (whiteCanCastleQueenside ? "1" : "0") << " " // Save white queenside castling right
             << (blackCanCastleKingside ? "1" : "0") << " " // Save black kingside castling right
             << (blackCanCastleQueenside ? "1" : "0") << " " // Save black queenside castling right
             << halfMoveClock << " " << fullMoveNumber << endl; // Save move clocks
        for (int row = 0; row < BOARD_SIZE; row++) { // Loop through all rows
            for (int col = 0; col < BOARD_SIZE; col++) { // Loop through all columns
                file << board[row][col].symbol;  // Write piece symbol to file
            }
            file << endl; // Newline after each row
        }
        file.close(); // Close the file
    }
}

void ChessBoard::loadGame(const string& filename) {
    ifstream file(filename); // Open file for reading
    if (file.is_open()) { // Check if file opened successfully
        file >> whiteCanCastleKingside >> whiteCanCastleQueenside // Load castling rights
             >> blackCanCastleKingside >> blackCanCastleQueenside
             >> halfMoveClock >> fullMoveNumber; // Load move clocks
        file.ignore(); // Ignore the newline character after last integer
        for (int row = 0; row < BOARD_SIZE; row++) { // Loop through rows
            string line;
            if (getline(file, line) && line.length() >= BOARD_SIZE) { // Read line of piece symbols
                for (int col = 0; col < BOARD_SIZE; col++) { // Loop through columns
                    board[row][col] = ChessPiece(line[col], line[col] >= 'A' && line[col] <= 'Z'); // Rebuild board with correct piece and color
                }
            }
        }
        file.close(); // Close the file
    }
}

ChessGame::ChessGame() : whiteTurn(true), checkCount(0) { // Initialize game: white's turn and check count 0
    whiteCaptures.clear(); // Clear white's capture list
    blackCaptures.clear(); // Clear black's capture list
}

// This function displays a stylized welcome message and instructions to the user.
// It pauses briefly to give players a moment before starting the game.
void ChessGame::displayWelcomeMessage() {
    cout << "\n\n\033[38;5;183m*     Welcome to Chess Master     *\033[0m\n"; // Display welcome title with color
    cout << "\033[38;5;183m***********************************************\033[0m\n"; // Decorative line
    cout << "\033[38;5;30m      Enter moves as 'e2 e4'       \033[0m\n"; // Move input example
    cout << "\033[38;5;30m      Promotion: 'd7 d8=Q'         \033[0m\n"; // Promotion example
    cout << "\033[38;5;30m    Castling: 'e1 g1' or 'e8 g8'   \033[0m\n"; // Castling example
    cout << "\033[38;5;30m      Type 'exit' to end game      \033[0m\n"; // Exit instruction
    cout << "\033[38;5;30m      White moves first            \033[0m\n"; // First move info
    cout << "\033[38;5;183m************************************************\033[0m\n"; // Decorative line
    this_thread::sleep_for(chrono::seconds(1)); // Pause for 1 second
    cout << "\n"; // New line
}

// This function displays the chess rules in a formatted and colorful way.
// It waits for the user to press Enter before returning to the menu.
// It helps new players understand how to play the game.
void ChessGame::displayRules() {
    cout << "\n\n\033[38;5;30m*--------------------------------------------*\033[0m\n"; // Rules header border
    cout << "\033[48;5;30m|                 Chess Rules                |\033[0m\n"; // Title bar
    cout << "\033[38;5;30m*--------------------------------------------*\033[0m\n";  // Decorative line
    cout << "\033[38;5;30m|                1. White moves first        |\033[0m\n"; // Rule 1
    cout << "\033[38;5;30m|                                            |\033[0m\n";  // Spacer
    cout << "\033[38;5;30m|            2. Game ends after 8 checks     |\033[0m\n"; // Rule 2
    cout << "\033[38;5;30m|                                            |\033[0m\n"; // Spacer
    cout << "\033[38;5;30m|----------3. Pieces move as follows:--------|\033[0m\n"; // Rule 3 header
    cout << "\033[38;5;30m|         *   King: 1 square any dir         |\033[0m\n"; // King move
    cout << "\033[38;5;30m|         -   Queen: Any dir, any dist       |\033[0m\n"; // Queen move
    cout << "\033[38;5;30m|         *   Rook: Horz/vert any dist       |\033[0m\n"; // Rook move
    cout << "\033[38;5;30m|         -   Bishop: Diag any dist          |\033[0m\n"; // Bishop move
    cout << "\033[38;5;30m|         *   Knight: L-shape (2x1)          |\033[0m\n"; // Knight move
    cout << "\033[38;5;30m|         -   Pawn: 1 forward, 2 start       |\033[0m\n";  // Pawn move
    cout << "\033[38;5;30m|                                            |\033[0m\n"; // Spacer
    cout << "\033[38;5;30m| 4. Capture by landing on opponent's piece  |\033[0m\n"; // Rule 4
    cout << "\033[38;5;30m|                                            |\033[0m\n"; // Spacer
    cout << "\033[38;5;30m|           5. Special moves:                |\033[0m\n"; // Rule 5 header
    cout << "\033[38;5;30m|                                            |\033[0m\n"; // Spacer
    cout << "\033[38;5;30m|       - Castling (King+Rook)               |\033[0m\n"; // Castling
    cout << "\033[38;5;30m|       - En passant (Pawn)                  |\033[0m\n"; // En passant
    cout << "\033[38;5;30m|       - Promotion (Pawn)                   |\033[0m\n";  // Promotion
    cout << "\033[38;5;30m*--------------------------------------------*\033[0m\n";  // Bottom border
    cout << "\n\033[45mPress Enter to return to menu...\033[0m"; // Prompt to return
    cin.get(); // Wait for user input
}

void ChessGame::start() {
    displayWelcomeMessage(); // Show welcome message
    string move; // User input for move
    while (true) {
        board.displayBoard(whiteCaptures, blackCaptures);  // Display board with captured pieces
        cout << (whiteTurn ? "\033[38;5;216m White" : "\033[38;5;117mBlack") << "'s turn. Enter move: \033[0m"; // Show whose turn
        getline(cin, move);  // Get move from user

        if (move == "exit") {  // If user types exit
            cout << "\033[31m                +-----------------+\033[0m\n";
            cout << "\033[31m                |    Game Over!   |\033[0m\n";
            cout << "\033[31m                +-----------------+\033[0m\n";
            break;  // End loop
        }

        char promotion = 'Q'; // Default promotion piece
        if (move.length() == 7 && move[5] == '=') { // Check for promotion input
            promotion = move[6]; // Get promoted piece
            move = move.substr(0, 5); // Trim move string
        }

        if (board.movePiece(move, whiteTurn, promotion, whiteCaptures, blackCaptures)) { // If move is valid
            bool opponentInCheck = board.isKingInCheck(!whiteTurn); // Check if opponent is in check
            if (opponentInCheck) {
                checkCount++; // Increment check count
                board.displayBoard(whiteCaptures, blackCaptures); // Show board
                cout << "\033[31m+-----------------------------------------------------+\033[0m\n";
                cout << "\033[31m|****************          Check!       **************|\033[0m\n";
                cout << "\033[31m+-----------------------------------------------------+\033[0m\n";

                if (checkCount >= 4) { // If 4 checks occurred
                    cout << "\033[38;5;30m\n####################################################################+\033[0m\n";
                    cout << "\033[38;5;30m|                       Game Over!                                   |\033[0m\n";
                    cout << "\033[38;5;30m|                    " << (whiteTurn ? "Black" : "White") << "\033[38;5;30m wins after second check!                  |\033[0m\n";
                    cout << "\033[38;5;30m+####################################################################+\033[0m\n";
                    break; // End game
                }
            }
            whiteTurn = !whiteTurn; // Switch turn
        } else {
            board.displayBoard(whiteCaptures, blackCaptures); // Re-display board
            cout << "\033[31m|*******************  Invalid move!  **********************|\033[0m\n"; // Invalid move message
        }
    }
}

void ChessGame::saveGame(const string& filename) {
    board.saveGame(filename); // Save board state
    cout << "\033[38;5;183m+-----------------+\033[0m\n";
    cout << "\033[45m|   Game saved!   \033[0m|\n";
    cout << "\033[38;5;183m+-----------------+\033[0m\n";
}

void ChessGame::loadGame(const string& filename) {
    board.loadGame(filename);  // Load board state
    cout << "\033[38;5;183m+-----------------+\033[0m\n";
    cout << "\033[45m|   Game loaded!  |\033[0m\n";
    cout << "\033[38;5;183m+-----------------+\033[0m\n";
}

void displayMenu() {
    cout << "\033[38;5;183m\n*-------------------------*\033[0m\n"; // Menu border
    cout << "\033[38;5;183m|      Chess Master       |\033[0m\n"; // Title
    cout << "\033[38;5;183m*-------------------------*\033[0m\n"; // Border
    cout << "\033[45m|     1. Start Game      |\033[0m\n"; // Option 1
    cout << "\033[45m|     2. Save Game       |\033[0m\n"; // Option 2
    cout << "\033[45m|     3. Load Game       |\033[0m\n"; // Option 3
    cout << "\033[45m|     4. View Rules      |\033[0m\n"; // Option 4
    cout << "\033[45m|     5. Exit            |\033[0m\n"; // Option 5
    cout << "\n\033[38;5;183mEnter choice: \033[0m"; // Prompt
}

int main() {
    ChessGame game; // Create game object
    int choice; // Store menu choice
    while (true) { // Main loop
        displayMenu(); // Show menu
        cin >> choice; // Get choice
        cin.ignore(); // Ignore leftover newline

        switch (choice) { // Check user input for selected menu option
            case 1: game.start(); break;  // Start game // Exit the case
            case 2: {  // Save game
                string filename; // Variable to store the filename
                cout << "Enter filename to save: ";  // Prompt the user for filename
                getline(cin, filename);  // Get the filename input
                game.saveGame(filename); // Call the save function with the provided filename
                break; // Exit the case
            }
            case 3: { // Load game
                string filename; // Variable to store the filename
                cout << "Enter filename to load: "; // Prompt the user for the filename
                getline(cin, filename);  // Get the filename input
                game.loadGame(filename);  // Call the load function with the provided filename
                break;   // Exit the case
            }
            case 4: game.displayRules(); break; // Show the chess rules   // Exit the case
            case 5: // Exit the game
                cout << "\n\033[38;5;183m*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*\033[0m\n";  // Display exit message
                cout << "\033[45m|     Thank you for playing the Chess! HAVE A NICE DAY      |\033[0m\n"; // Thank the player for playing
                cout << "\033[38;5;183m*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*\033[0m\n"; // End of the exit message
                return 0; // Exit the program
            default: // In case of an invalid menu option
                cout << "\033[38;5;30m    +-----------------+\033[0m\n"; // Display error message
                cout << "\033[38;5;30m    | Invalid choice! |\033[0m\n"; // Inform the user about invalid choice
                cout << "\033[38;5;30m    +-----------------+\033[0m\n"; // End of error message
        }
    }
    return 0; // End of the program, return from the main function
}
