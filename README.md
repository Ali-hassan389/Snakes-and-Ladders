<div align="center">

<h1>🐍 &nbsp; Snakes & Ladders: Console Edition &nbsp; 🪜</h1>

<p>
  <img src="https://readme-typing-svg.demolab.com?font=Fira+Code&size=16&pause=1000&color=FF8C00&center=true&vCenter=true&width=520&lines=Multiplayer+Snakes+%26+Ladders+in+C%2B%2B;Completely+Terminal-Optimized;ANSI+Escape+Color+Interface;Exact-Roll+Victory+Ceiling" alt="Typing SVG" />
</p>

**A sleek, interactive, and completely terminal-optimized board game built entirely in modern C++.**

![C++](https://img.shields.io/badge/C++-17%2F20-00599C?style=flat-square&logo=cplusplus&logoColor=white)
![Interface](https://img.shields.io/badge/UI-ANSI_Terminal-lightgrey?style=flat-square)
![Paradigm](https://img.shields.io/badge/Paradigm-OOP-blueviolet?style=flat-square)
![Status](https://img.shields.io/badge/Status-Complete-brightgreen?style=flat-square)

</div>

---

## 🎲 Overview

**Snakes & Ladders: Console Edition** is an implementation of the traditional board game developed natively in terminal viewports. It translates a linear 1–100 spatial board directly into a live-updating text matrix layout within your command-line environment. The system features customized multi-player sessions, dynamic inputs, automatic historical leaderboards, and independent, clean object-oriented modules.

---

## ✦ Features

| Feature | Details |
|---|---|
| 👥 **Local Matchmaking** | Supports fully customized local sessions for 2 to 6 unique players simultaneously |
| 🎨 **Vibrant ANSI Interface** | Dynamic map prints using cross-platform color sequences assigned per player token |
| 📊 **Real-Time Board Metrics** | Tracks old positions, landing locations, slide events, and active rolling matrices |
| 💾 **Persistent Score Logs** | Automatically serializes rosters and turn counts straight to disk storage files |
| 🎲 **Pseudorandom Dice** | Smooth randomized generation cycles seeded directly via clock metrics |
| 🪜 **Authentic Hazards** | Includes hardcoded map locations for 6 ladders and 8 unique snakes |

---

## 🗂️ Project Structure

```text
Snakes-And-Ladders/
│
├── data/
│   └── records.txt                    # Saved match histories and past game statistics
│
├── Player.h / Player.cpp              # Entity profiles, color tokens, and tile tracking pointers
├── Board.h / Board.cpp                # Matrix boundary paths and hazard location configurations
├── Dice.h / Dice.cpp                  # Random number generators and rolling sequence layers
├── GameEngine.h / GameEngine.cpp      # Turn queues, boundary checks, and rule validations
├── RecordManager.h / RecordManager.cpp # Delimited file stream parsers and leaderboard tables
├── ConsoleUI.h / ConsoleUI.cpp         # Terminal event screens and grid text drawing arrays
└── main.cpp                           # System initialization and orchestration launcher
```

---

## 💾 File Handling — Match History

The game automatically preserves complete match records locally using a robust parsing structure. Upon match completion, game data is cleanly logged onto the disk layout:

```text
Ali,Usman,Uzwa | Ali | 24 | Console
Hajra,Nafia | Nafia | 18 | Console
```

When checking past metrics through the game menu, the application processes these streams to build a perfectly formatted, aligned history table within your terminal console window:

```text
=========================================================================
                         PAST GAME MATCH HISTORY
=========================================================================
Players                        Winner         Total Turns   Mode
-------------------------------------------------------------------------
Ali,Umer,Hassan                Ali            24            Console
Abdur Rehman, Rehan            Rehan          18            Console
=========================================================================
```

---

## ⚙️ Build & Run

### Prerequisites

A modern standard C++ compiler suite supporting C++17 or higher (GCC, Clang, or MSVC).

### Compile & Execute (Linux / macOS / MinGW)

```bash
# Navigate to project workspace directory
cd path/to/project-root

# Compile all source files simultaneously
g++ -std=c++17 Board.cpp ConsoleUI.cpp Dice.cpp GameEngine.cpp Player.cpp RecordManager.cpp main.cpp -o SnakesAndLadders

# Run the compiled executable binary
./SnakesAndLadders
```

### Controls

| Input | Action |
|---|---|
| `[ENTER]` | Roll the randomized dice / Advance turn sequence stages |
| User Inputs | Choose active player counts and assign custom character name strings |

---

## 👤 Developer Profile

| 🐍 Developer | Roll No | Academic Institution |
|---|---|---|
| Ali Hassan | 25F-0644 | FAST NUCES |

> *"The roll of the dice can change everything."*

⭐ **Star this repo if you enjoyed playing!**
