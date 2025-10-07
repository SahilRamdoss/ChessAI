# Chess AI

This is a chess game where players can play against each other or better, they can play against the AI. The AI will have different levels: Adpative, Beginner, Intermediate, Advanced and Impossible, where a player can test his chess skills against this adaptive opponent. Be careful though, the opponent can trash talk you real bad when you are losing!

## 🧩Dependencies 

This project is written in C++ version 20.1.8 and uses the following libraries:

- **SDL3** - for rendering graphics
- **A C++ Compiler** (e.g. g++, clang++)

Make sure SFML is properly installed and linked before building.

clang++ Chess-View.cpp -o test -I "C:\SDL3-devel-3.2.24-mingw\SDL3-3.2.24\x86_64-w64-mingw32\include" -L "C:\SDL3-devel-3.2.24-mingw\SDL3-3.2.24\x86_64-w64-mingw32\include" -lSDL3
