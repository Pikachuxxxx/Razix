set /p texte=< Engine/content/config/razix_engine.config
set textnew="%texte:~17%"
echo %textnew%

SETX RAZIX_SDK %textnew% /m
SETX RAZIX_SDK_TOOLS %textnew%/Tools /m