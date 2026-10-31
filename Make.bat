@echo off
if not exist obj mkdir obj
if not exist bin mkdir bin

echo Compile the C code
compile hello-world.c -o obj\hello-world.asm
if errorlevel 1 goto failed

echo Assemble the code
assemble obj\hello-world.asm -o obj\hello-world.vbin
if errorlevel 1 goto failed

echo Package the ROM
packrom hello-world.xml -o bin\hello-world.v32
if errorlevel 1 goto failed

echo Build successful: bin\hello-world.v32
exit /b 0

:failed
echo Build failed.
exit /b 1
