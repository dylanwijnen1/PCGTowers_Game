@ECHO OFF

ECHO [DragonEngine] Relinking Engine Files.
CALL "../../tools/premake5.exe" vs2019 --file="../../premake5.lua" > nul

CALL "../../tools/premake5.exe" vs2019 --file="premake5.lua"

PAUSE