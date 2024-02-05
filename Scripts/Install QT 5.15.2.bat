echo Installing aqt...
pip install aqtinstall
echo Installing qt 5.15.2 for msvc 2019 64-bit to C:\Qt
aqt install-qt windows desktop 5.15.2 win64_msvc2019_64 -O C:\Qt
echo setting QTDIR env variable and adding to path
setx QTDIR "C:\Qt\5.15.2\msvc2019_64"