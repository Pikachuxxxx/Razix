@echo off
mkdir PNG
for %%A IN (*.dot) DO (
    echo "%%~nxA"
    dot -Tpng %%~nxA > PNG/%%~nA.png
)
PAUSE
