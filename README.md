# Chess AI

A desktop chess application written in **modern C++** featuring a fully implemented chess engine, multiplayer gameplay, and an AI opponent powered by the **Minimax algorithm with Alpha-Beta pruning**.

This project was built from scratch to explore advanced concepts in **object-oriented programming**, **data structures and algorithms**, **game engine architecture**, and **artificial intelligence search techniques**.

---

## Features

- ♟️ Complete chess game implementation
- 🤖 AI opponent using Minimax with Alpha-Beta pruning
- 👥 Local multiplayer mode
- 🧠 Custom board evaluation heuristics
- ✅ Full chess move validation
- 🔄 Move generation for all pieces
- 👑 Check, checkmate, and stalemate detection
- 📜 Move history tracking
- 🎨 Graphical user interface built with SDL3
---

## Key Technical Achievements

### Chess Engine Development

Implemented the complete game logic from scratch, including:

- Legal move generation
- Piece movement rules
- Check detection
- Checkmate detection
- Stalemate detection
- Turn management
- Board state updates

---

### Artificial Intelligence

The AI opponent uses:

- **Minimax Search**
- **Alpha-Beta Pruning**
- **Custom Heuristic Evaluation Functions**

The search depth was optimized to **depth 5**, providing a balance between:

- Search performance
- Computational cost
- Playing strength

The evaluation function considers multiple positional factors, including:

- Material advantage
- Piece activity
- Board control
- King safety
- Positional advantages

---

### Software Engineering

This project required extensive use of:

- Object-Oriented Programming
- Polymorphism
- Inheritance
- Encapsulation
- Data structures
- Algorithm optimization
- Recursive search algorithms

---

## Technologies Used

| Technology | Purpose |
|------------|---------|
| C++20 | Core application development |
| SDL3 | Rendering and event handling |
| SDL3_image | Loading graphical assets |
| CMake | Build system |
| Minimax | AI decision-making |
| Alpha-Beta Pruning | Search optimization |

---

## Requirements

Before building the project, ensure you have:

- C++20 compatible compiler
  - GCC
  - Clang
  - MSVC
- CMake 3.30+
- SDL3
- SDL3_image

---

## Installation

Clone the repository:

```bash
git clone https://github.com/<your-username>/ChessAI.git
cd ChessAI
```

Configure the project:

```bash
cmake -B build
```

Build the project:

```bash
cmake --build build
```

Run the executable:

```bash
./build/ChessAI
```
---

## Learning Outcomes

This project strengthened practical experience in:

- Artificial intelligence algorithms
- Search optimization techniques
- Modern C++ programming
- Object-oriented software design
- Data structures and algorithms
- GUI programming with SDL3
