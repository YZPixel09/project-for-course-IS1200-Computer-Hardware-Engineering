# A ChipKit Ball Jumping Game Project

This project is a **ChipKit-based game** created for the **IS1200 Computer Hardware Engineering course**. The game is written in C and runs on a ChipKit board. The gameplay involves controlling a ball that encounters various obstacles. 

## Game Features
- **Lower Obstacles**: The ball needs to jump over these.
- **Upper Obstacles**: The ball must jump up to hit these.
- **Game Over**: Failing to clear a lower obstacle or to hit an upper obstacle results in losing the game.
- **Difficulty Levels**: The game supports different difficulty settings.

---

## Prerequisites
1. **MSYS2**: Ensure that MSYS2 is installed on your system.
2. **Game Source Package**: Download and extract the game’s source package to a local directory.

---

## Installation and Execution

### 1. Set Up Environment
- Open the **MSYS2 terminal**.
- Navigate to the directory where the game's source files are located:
  ```bash
  cd /path/to/game/source
 This command configures the required environment variables and paths for the PIC32 toolchain.

 ### 2. Enter the Toolchain Environment
Run the following command to set up the environment:
  ```bash
. /opt/pic32-toolchain/environment

