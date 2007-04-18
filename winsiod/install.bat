@ECHO uninstall package
@MSIEXEC /uninstall SIOD.MSI /lv uninstall.log
@ECHO installing package
@MSIEXEC /package SIOD.MSI /lv install.log
@IF "%NOPAUSE%"=="1" GOTO :EOF
@PAUSE
