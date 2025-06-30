# 🟡 Console Pac-Man Game

A console-based implementation of **Pac-Man** written in C++. The game is played entirely in the terminal and features four ghosts with unique behaviors, mimicking the original game mechanics.

---

## 🎮 Gameplay Overview

- You play as **Pac-Man (P)** navigating a closed map.
- Four ghosts (Red, Pink, Blue, Green) are released at different score thresholds.
- The objective is to **collect all 4 apples (`@`)** while avoiding the ghosts.
- The map is generated based on the dimensions **you input at the start** (width × height).

---

## 🧱 How to Play

1. Run the program.
2. Enter two numbers (e.g. `20 10`) for the map's width and height.
3. Use **arrow keys or WASD** to move Pac-Man around the grid.
4. Eat:
   - `.` (points) — 1 point
   - `@` (apple) — 20 points
5. Ghosts appear based on your score:
   - **Red Ghost (Blinky)** – active from the start, follows you directly.
   - **Pink Ghost (Pinky)** – appears at 20 points, uses a predictive path.
   - **Blue Ghost (Inky)** – appears at 40 points, uses Pac-Man + Blinky vector.
   - **Green Ghost (Clyde)** – appears at 60 points, chases or retreats based on distance.
6. **Win condition**: Collect all 4 apples.
7. **Lose condition**: A ghost catches Pac-Man.

---

## 👻 Ghost Behavior (Just like the original!)

| Ghost  | Color   | Symbol | Release Score | Behavior                              |
|--------|---------|--------|----------------|----------------------------------------|
| Blinky | Red     | `B`    | 0              | Direct chase (targets Pac-Man)         |
| Pinky  | Pink    | `Y`    | 20             | Predicts Pac-Man's next position       |
| Inky   | Cyan    | `I`    | 40             | Vectors between Blinky and Pac-Man     |
| Clyde  | Green   | `C`    | 60             | Alternates between chase and scatter   |

---

## 🧾 Symbols

| Symbol | Meaning         |
|--------|-----------------|
| `P`    | Pac-Man         |
| `#`    | Wall            |
| `.`    | Small point     |
| `@`    | Apple (big dot) |
| ` `    | Empty space     |

---

## ✅ Requirements

- C++17 or later
- A terminal that supports ANSI escape sequences (for color)
- Windows or Linux (console compatible)

---

## 🚀 Build & Run

### Compile (using g++):
```bash
g++ main.cpp -o pacman -std=c++17
