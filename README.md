# vircon32-shooter

A small Vircon32 top-down shooter prototype called Neon Dusk.

## Current prototype

- Move left and right with the directional keys. The player stays near the
  bottom of the scrolling playfield and always faces upward.
- Fire upward with **Z**. Enemies enter from the top and descend toward you.
- Survive waves of enemies. Each collision costs one of your three hull
  points; clearing a wave increases the next wave's size.
- Earn points by hitting enemies. The game ends when your hull reaches zero.
- Press **Enter** or **X** at the title screen to start. After game over, press
  either again to return to the title screen.

The player uses the transparent sprite in `images\player.png`, a 128 x 128
nearest-neighbor version of `images\playerShipNew.png`. Separate banked sprites
raise one wing and lower the other while keeping the nose pointed up. The title,
HUD, enemies, and projectiles use the Vircon32 BIOS font, so no other art or
audio assets are required.

## Source layout

The source follows the small-module layout used by
[OceanStorm's `src/game` project](https://github.com/vircon32/OceanStorm/tree/main/src/game),
with separate `game`, `states`, and `utils` folders:

- `src/main.c` owns the game loop and switches between states.
- `src/game/` contains the arena, player, enemy, and projectile logic.
- `src/states/` contains the title and game-over screens.
- `src/utils/` contains shared constants and drawing helpers.
- `images/` contains the original ship art, the 128 x 128 player sprite, and
  left- and right-banked sprites, converted to Vircon32 textures during the
  build.

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
bin\neon-dusk.v32
```

Open the Vircon32 emulator and load `bin\neon-dusk.v32` to play. Run
`.\Make.bat` again after changing the source to rebuild the cartridge.