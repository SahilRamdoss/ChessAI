# Chess AI

This is a chess game where players can play in 1 v/s 1 multiplayer mode or even better, try beating the AI bot. Be careful though, the AI has 1000000 elo! Beating it is more difficult than beating Magnus Carlsen, no cap.

## 🧩Dependencies 

This project is written in C++ version 20.1.8 and uses the following libraries:

- **SDL3** - for rendering graphics
- **SDL3_image** - for loading PNG files
- **libcurl** - for making API requests to Lichess REST API
- **nlohmann-json** - for working with the JSON format returned by Lichess REST API.
                      Include the json file only in your current directory.
- **A C++ Compiler** (e.g. g++, clang++)
- **Cmake** - used to build, compile and run the program. Also responsible for downloading all dependencies.

Make sure SDL3 is properly installed and linked before building. We also make use of CMake to simplify the compiling process.

## 🚀 Running the Program

Run the following commands to build and start **ChessAI**:

> 💡 **Tip:** Make sure you have CMake installed before running these commands.

cmake -B build <br>
cmake --build build <br>
./build/ChessAI <br>


### Compiling command for SDL3
clang++ src/Chess-Controller.cpp src/Chess-View.cpp -o test \
    -std=c++20 \
    -I include \
    -I "C:\SDL3-devel-3.2.24-mingw\SDL3-3.2.24\x86_64-w64-mingw32\include" \
    -I "C:\SDL3_image-devel-3.2.4-mingw\SDL3_image-3.2.4\x86_64-w64-mingw32\include" \
    -L "C:\SDL3-devel-3.2.24-mingw\SDL3-3.2.24\x86_64-w64-mingw32\lib" \
    -L "C:\SDL3_image-devel-3.2.4-mingw\SDL3_image-3.2.4\x86_64-w64-mingw32\lib" \
    -lSDL3_image -lSDL3

### Command to run the program
./build/ChessAI


### Compiling command for libcurl and nohmann-json
clang++ Chess-View.cpp -o test_curl   -I/c/curl-8.16.0_4-win64-mingw/curl-8.16.0_4-win64-mingw/include   -L/c/curl-8.16.0_4-win64-mingw/curl-8.16.0_4-win64-mingw/lib   -lcurl -std=c++20
