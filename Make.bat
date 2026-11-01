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
png2vircon images\player-bank-left.png -o obj\player-bank-left.vtex
if errorlevel 1 goto failed
png2vircon images\player-bank-right.png -o obj\player-bank-right.vtex
if errorlevel 1 goto failed
png2vircon images\enemy-01.png -o obj\enemy-01.vtex
if errorlevel 1 goto failed
png2vircon images\enemy-01-debri.png -o obj\enemy-debris.vtex
if errorlevel 1 goto failed
png2vircon images\title-screen.png -o obj\title.vtex
if errorlevel 1 goto failed
png2vircon images\jungle-background.png -o obj\jungle-background.vtex
if errorlevel 1 goto failed
png2vircon images\jungle-background-2.png -o obj\jungle-background-2.vtex
if errorlevel 1 goto failed
png2vircon images\jungle-background-3.png -o obj\jungle-background-3.vtex
if errorlevel 1 goto failed
png2vircon images\jungle-background-4.png -o obj\jungle-background-4.vtex
if errorlevel 1 goto failed
png2vircon images\jungle-background-5.png -o obj\jungle-background-5.vtex
if errorlevel 1 goto failed
png2vircon images\health-segment.png -o obj\health-segment.vtex
if errorlevel 1 goto failed
png2vircon images\health-background.png -o obj\health-background.vtex
if errorlevel 1 goto failed
png2vircon images\laser-blast.png -o obj\laser-blast.vtex
if errorlevel 1 goto failed
png2vircon images\game-over-screen.png -o obj\game-over-screen.vtex
if errorlevel 1 goto failed
png2vircon images\explosion-frames.png -o obj\explosion.vtex
if errorlevel 1 goto failed
png2vircon images\coin.png -o obj\coin.vtex
if errorlevel 1 goto failed

echo Assemble the code
assemble obj\main.asm -o obj\main.vbin
if errorlevel 1 goto failed

echo Package the ROM
packrom shooter.xml -o bin\shooty-mc-shoots.v32
if errorlevel 1 goto failed

echo Build successful: bin\shooty-mc-shoots.v32
exit /b 0

:failed
echo Build failed.
exit /b 1
