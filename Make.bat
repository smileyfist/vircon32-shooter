@echo off
cd /d "%~dp0"
if not exist obj mkdir obj
if not exist bin mkdir bin

echo Compile the C code
compile src\main.c -o obj\main.asm
if errorlevel 1 goto failed

echo Convert the player sprite
png2vircon images\player.png -o obj\player.vtex
if errorlevel 1 goto failed

echo Assemble the code
assemble obj\main.asm -o obj\main.vbin
if errorlevel 1 goto failed

echo Package the ROM
packrom shooter.xml -o bin\neon-dusk.v32
if errorlevel 1 goto failed

echo Build successful: bin\neon-dusk.v32
exit /b 0

:failed
echo Build failed.
exit /b 1
