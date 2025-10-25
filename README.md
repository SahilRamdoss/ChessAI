# Chess AI

This is a chess game where players can play in 1 v/s 1 multiplayer mode or even better, try beating the AI bot. Be careful though, the AI has 1000000 elo! Beating it is more difficult than beating Magnus Carlsen, no cap.

## 🧩Dependencies 

This project is written in C++ version 20.1.8 and uses the following libraries:

- **SDL3** - for rendering graphics
- **SDL3_image extension** - for loading PNG files
- **A C++ Compiler** (e.g. g++, clang++)
- **Cmake Version 3.30 or higher** - used to build, compile and run the program. Also responsible for downloading all dependencies.

Make sure SDL3 is properly installed and linked before building. We also make use of CMake to simplify the compiling process.

## 🚀 Running the Program

Run the following commands to build and start **ChessAI**:

> 💡 **Tip:** Make sure you have CMake version 3.30 or higher installed before running these commands.
> 💡 **Tip:** If your build command keeps failing, run the build command on MSYS2 terminal instead of any other terminal.

cmake -B build <br>
cmake --build build <br>
./build/ChessAI <br>
