README.txt

This project is a  chipKit-based game created for the IS1200 Computer Hardware Engineering course. The game is written in C and runs on a chipKit board. The gameplay involves controlling a ball that encounters various obstacles. There are two types of obstacles:

Lower Obstacles: The ball needs to jump over these.
Upper Obstacles: The ball must jump up to hit these.
If the ball fails to clear a lower obstacle or to hit an upper obstacle, the player loses. The game also supports different difficulty levels.

Prerequisites
MSYS2: Ensure that MSYS2 is installed on your system.
Game Source Package: Download and extract the game’s source package to a local directory.

Installation and Execution

Set Up Environment:

Open the MSYS2 terminal.
Use the cd command to navigate to the directory where you extracted the game’s source files.

Enter the Toolchain Environment:
. /opt/pic32-toolchain/environment

This command sets up the required environment variables and paths for the PIC32 toolchain.

Build the Game:

First, clean any previous builds:
make clean

Then, compile the source code:
make

Install the Game onto the ChipKit Board:
After successful compilation, install the game onto the chipKit device:
make install
Note: Ensure that your chipKit device is properly connected to your computer before running this command.


Windows: In the provided Makefile, the communication port is defined as:
TTYDEV = /dev/ttyS2

This should work on Windows under typical configurations.

Linux / macOS: If you are using a Linux or macOS system, you may need to modify the TTYDEV variable in the Makefile to match the correct serial device for your system (e.g., /dev/ttyUSB0 on Linux).

Enjoy playing!