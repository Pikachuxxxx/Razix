@echo off
mkdir SVG
for %%A IN (*.dot) DO (
    echo "%%~nxA"
    dot -Tsvg %%~nxA > SVG/%%~nA.svg
)
PAUSE
