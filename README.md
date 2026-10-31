# vircon32-shooter

A small Vircon32 game project. The current cartridge is a minimal Hello World
starter that displays “Hello, world!” on a black screen.

## Project goals

The longer-term idea is a compact, action-focused shooter:

- Destructible cubes, splashes, and flying debris
- Blue player and red enemies, using simple shapes
- Three levels and three bosses
- A minimal Play/Quit menu, with gameplay prioritized over menus
- Original 8-bit synthwave music and sound effects; no paid assets
- No world map, shop, or lives system

## Windows setup

Vircon32's [ComputerSoftware releases](https://github.com/vircon32/ComputerSoftware/releases)
provide the Windows emulator and development tools. The Windows downloads are
ZIP archives; they do not require an installer.

### Install the emulator and development tools

1. Open the [official releases page](https://github.com/vircon32/ComputerSoftware/releases).
2. Download and extract the latest Windows emulator ZIP and Windows Dev Tools
   ZIP into a folder you can keep. The versions used to build this starter are
   Emulator **v26.2.18** and Dev Tools **v26.04.24**.
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

The script compiles `hello-world.c` to assembly, assembles it into a Vircon32
binary, then packages it using `hello-world.xml`. Intermediate files go in
`obj\`, and the playable cartridge is written to:

```text
bin\hello-world.v32
```

Open the Vircon32 emulator and load `bin\hello-world.v32` to run the game. Run
`.\Make.bat` again after changing the source to rebuild the cartridge.



- see https://github.com/vircon32/OceanStorm/tree/main/src/game