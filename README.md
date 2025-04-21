#  Chess Master

## Project Title
Chess Master

## Project Description

Chess Master is a console-based C++ application built using Code::Blocks, designed for two-player chess gameplay. It follows traditional chess rules such as castling, en passant, and pawn promotion, while also introducing a unique custom rule: the game concludes after four checks, awarding victory to the player who delivered the fourth. The application manages the chessboard using 2D arrays, processes player inputs via algebraic notation, maintains game state for continuity, and uses basic collision detection to handle piece interactions. Its key features include:

a. Standard Chess Rules: Supports all piece movements, captures, and special moves (castling, en passant, pawn promotion).
b. Custom Rule: Game ends after two checks, with the delivering player declared the winner.
c. Save/Load Functionality: Persists and resumes game states via text files, storing board configuration, castling availability, and move counters.
d. User Interface: Menu-driven navigation with clear prompts, error messages, and a colorful 8x8 board display showing captured pieces and move counters (half-moves for 50-move rule, full moves).
e. Rules Display: In-game explanation of chess rules and special moves.
f. The program uses a modular class-based design (ChessBoard, ChessGame, ChessPiece structure) without inheritance, relying on composition for simplicity and maintainability.

## Installation

### Prepare Files:

a. Place ChessGame.h, ChessGame.cpp, and README.md(this file) in a project directory.

### To Create Project:

a. Open Code::Blocks.
b. Go to File > New > Project > Console Application > Go.
c. Select C++, then click Next.
d. Enter project name (e.g., ChessMaster) and choose the project directory.
e. Ensure GNU GCC Compiler is selected, then click Finish.

### To Add Source Files:

a. In the Management pane, right-click the project name.
b. Select Add files... and add ChessGame.h and ChessGame.cpp.
c. Verify files appear under Headers and Sources.

### Configure Compiler:

a. Go to Settings > Compiler... > Compiler Flags.
b. Check -std=c++11 to enable C++11 features.
c. Click OK.

### To Build The Code:

a. Click Build > Build (or Ctrl+F9).
b. Check the Build Log for errors. Ensure both files are included and C++11 is enabled.

### To Run The Code:

a. Click Build > Run (or Ctrl+F10) or Build > Build and Run (F9).
b. The console opens, displaying the main menu.

#### Note: Use a terminal with ANSI color support (e.g: Windows Terminal, Linux/macOS terminals) for proper color rendering. The default Code::Blocks console (Windows CMD) may not display colors correctly.

## Limitations

1. No AI opponent; designed for two human players.
2. Custom four-check rule often overrides standard checkmate/stalemate endings.
3. No support for advanced draw rules (e.g., threefold repetition, insufficient material).
4. Requires ANSI color support for proper console formatting.
5. File I/O assumes valid file formats for loading (no error messages for invalid files).

## Troubleshooting

a. No Colors: 
Use Windows Terminal or a Linux/macOS terminal. Configure Code::Blocks to use an external terminal (Settings > Environment > Terminal to launch console programs).

b. Build Errors:
Ensure ChessGame.h and ChessGame.cpp are added to the project and -std=c++11 is enabled (Project > Build Options > Compiler Flags).

c. Invalid Moves: 
Use correct notation (e.g., e2 e4) and ensure moves are legal.

d. File Issues: 
Save files to the project directory and verify filenames match when loading.

## License

This project is provided as-is for academic purposes. No formal license is specified.
