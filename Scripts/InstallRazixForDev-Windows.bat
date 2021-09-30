echo "Generating Visual Studio 2019 project files..."
call GenerateVS19.bat
echo "Installing registry for razix project files"
"%~dp0\RazixProjectExtensionRegistry.reg"
echo "Razix Installation is done!"
PAUSE
