# vircon32-shooter

A small Vircon32 top-down shooter called Shooty McShoots.

## Current prototype

- Move around the playfield with the directional keys. The ship banks when
  moving left or right, always faces upward, and can reach both screen edges.
  Vertical movement has 64 positions.
- Fire upward with **Z**. Enemies enter from the top and fly off the bottom
  unless you shoot them.
- Survive waves of enemies. Each collision costs one of your three hull
  points; clearing a wave increases the next wave's size.
- Earn points by hitting enemies. The game ends when your hull reaches zero.
- Occasionally, destroyed enemy ships drop a spinning gold coin. Collect it
  by flying into it; your coin total appears at the bottom-left of the
  playfield.
- Remaining hull is shown as a vertical, bright-green segmented bar on the
  far right over a black backing, with up to 20 segments.
- Enemy ships burst into a brief five-frame pixel-art explosion when hit,
  based on `unused\newExplosion.png`.
- Enemy ships break into metal-and-hull debris that scatters, then settles and
  scrolls with the jungle ground.
- Nine semi-transparent cloud sprites form a foreground layer that scrolls at
  half the jungle background speed, over the ground and ships.
- Press any button at the title screen to open the hangar, then press any
  button again to start the game. After the game-over screen's two-second
  lockout, press any button to return to the title screen.

The player uses five bank angles from the top and bottom rows of the supplied
sprite sheet, archived as `unused\superPlayer.png` and prepared as
`images\player-down-banking.png` and
`images\player-up-banking.png`. Moving down uses the top thrust row; moving up
uses the bottom row. Straight, light-bank, and hard-bank frames follow the
player's horizontal and diagonal movement.
The title screen is a 640 x 360 center crop of `unused\newerTitle.png`, saved
as `images\title-screen.png` for the console's texture-size limit. The hangar
screen uses `images\hangar-screen.png`, and the game-over screen uses
`images\game-over-screen.png`. The map screen remains in the project for
future use.
Enemies use the first two ships from the enemy sprite sheet:
`images\enemy-01.png` takes one hit, while `images\enemy-02.png` takes five and
moves at half speed. Their stats, movement intervals, and texture sizes are
defined per enemy type. Enemy destruction scatters ten fragments from
`images\enemy-01-debri.png`, with five fragments charred black. The game
scrolls through five aerial
jungle scenes in `images\jungle-background.png` and
`images\jungle-background-2.png` through `-5.png` as consecutive sections,
without runtime crossfades. Their edges are adjusted to meet seamlessly,
including the loop back to the first scene. These 640 x 360 textures are
prepared from `unused\jungle1.png` through `unused\jungle5.png`. Original
artwork and assets not used by the game are archived in the sibling `unused/`
folder.

## Source layout

The source follows the small-module layout used by
[OceanStorm's `src/game` project](https://github.com/vircon32/OceanStorm/tree/main/src/game),
with separate `game`, `states`, and `utils` folders:

- `src/main.c` owns the game loop and switches between states.
- `src/game/` contains the arena, player, enemy, and projectile logic.
- `src/states/` contains the title and game-over screens.
- `src/utils/` contains shared constants and drawing helpers.
- `images/` contains the directional player sprites, active enemy ships and
  debris, title/menu/game-over screens, health-bar graphics, laser, explosion
  animation, coin, clouds, and five jungle backgrounds. All game images are
  converted to Vircon32 textures during the build. Source artwork and other
  unused assets are kept in the sibling `unused/` folder.

Vircon32's C compiler accepts one source file per invocation, so `src/main.c`
includes the module `.c` files to build them together as one program.

## Windows setup

Vircon32's [ComputerSoftware releases](https://github.com/vircon32/ComputerSoftware/releases)
provide the Windows emulator and development tools. The Windows downloads are
ZIP archives; they do not require an installer.

### Install the emulator and development tools

1. Open the [official releases page](https://github.com/vircon32/ComputerSoftware/releases).
2. Download and extract the latest Windows emulator ZIP and Windows Dev Tools
   ZIP into a folder you can keep. The versions on this machine are Emulator
   **v26.02.18** and Dev Tools **v26.04.24**.
3. This machine's packages are extracted under
   `%LOCALAPPDATA%\Programs\Vircon32`. The executables are in:
   - `%LOCALAPPDATA%\Programs\Vircon32\DevTools\Vircon32\DevTools`
   - `%LOCALAPPDATA%\Programs\Vircon32\Emulator\Vircon32\Emulator`
4. Add the Dev Tools folder to your **user PATH** so Windows can find
   `compile`, `assemble`, and `packrom`. Add the Emulator folder too if you
   want to launch the emulator from a terminal. On this machine both folders
   are already on the user PATH.
5. Restart VS Code (or open a new terminal outside VS Code) after changing
   PATH. Existing terminals do not automatically pick up PATH changes.

To confirm the tools are available in a new terminal, run:

```powershell
compile --version
assemble --version
packrom --version
```

Each command should print its Vircon32 tool version.

## Build and run

From the project folder, run:

```powershell
.\Make.bat
```

The script compiles `src\main.c` to assembly, assembles it into a Vircon32
binary, then packages it using `shooter.xml`. Intermediate files go in `obj\`,
and the playable cartridge is written to:

```text
bin\shooty-mc-shoots.v32
```

Open the Vircon32 emulator and load `bin\shooty-mc-shoots.v32` to play. Run
`.\Make.bat` again after changing the source to rebuild the cartridge.