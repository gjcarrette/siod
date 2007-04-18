@SETLOCAL
@SET WIXBIN=%PROGRAMFILES%\Windows Installer XML\bin
@SET NETBIN=%WINDIR%\Microsoft.NET\Framework\v2.0.50727
@SET VS8BIN=%PROGRAMFILES%\Microsoft Visual Studio 8\Common7\IDE
@PATH %PATH%;%WIXBIN%;%NETBIN%;%VS8BIN%
@SET T=Release
@ECHO Compiling SIOD program
@REM In an environment with VCExpress only the
@REM The MSBUILD is giving me this error
@REM LINK : fatal error LNK1104: cannot open file 'user32.lib'
@REM even though Visual Studio Express is able to build
@REM Also, DEVENV.EXE might exist, but not be able to compile C
@REM programs, getting this error:
@REM  Make sure the application for the project type (.vcproj) is installed.
@REM Therefore, check for Visual C++ Express edition, and use it,
@REM if it exists, but default to DEVENV.
@REM It could be that MSBUILD could work in all situations,
@REM given proper adjustments. But at least this works now.
@IF EXIST "%VS8BIN%\VCEXPRESS.EXE" GOTO :VCEXPRESS
@DEVENV SIOD.SLN /Build %T%
@ECHO Done
@GOTO :CSIOD
:VCEXPRESS
@VCExpress SIOD.SLN /Build %T%
@ECHO Done
:CSIOD
@ECHO Done
@ECHO Compiling CSIOD and utilities.
@%T%\siod -v01 "(print (list 'siod-lib (siod-lib)))"
@SET CSIOD=%T%\siod -m2,-v01 csiod.smd 
@%CSIOD% :o=%T%\csiod.exe csiod.smd
@%CSIOD% :o=%T%\ftp-cp.exe ftp.scm ftp-cp.smd
@%CSIOD% :o=%T%\ftp-get.exe ftp.scm ftp-get.smd
@%CSIOD% :o=%T%\ftp-put.exe ftp.scm ftp-put.smd
@%CSIOD% :o=%T%\ftp-test.exe ftp.scm ftp-test.smd
@%CSIOD% :o=%T%\http-get.exe http-stress.scm http-get.smd
@%CSIOD% :o=%T%\http-stress.exe http-stress.scm http-stress.smd
@%CSIOD% :o=%T%\proxy-server.exe proxy-server.smd
@%CSIOD% :o=%T%\snapshot-compare.exe snapshot-compare.smd
@%CSIOD% :o=%T%\snapshot-dir.exe find-files.scm snapshot-dir.smd
@ECHO Done
@ECHO Compiling SIOD installer
@IF EXIST SIOD.WIXOBJ DEL SIOD.WIXOBJ
@CANDLE SIOD.WXS -nologo
@ECHO Linking SIOD installer.
@IF EXIST SIOD.MSI DEL SIOD.MSI
@REM Note: WixUI_en-us.wxl might be in your wix-2.0.4820.0-binaries
@REM       and not in the %WIXBIN% as installed.
@LIGHT SIOD.WIXOBJ -nologo "%WIXBIN%\WixUI.wixlib" -loc "%WIXBIN%\WixUI_en-us.wxl" -out SIOD.MSI
@Echo Build Done
@IF "%NOPAUSE%"=="1" GOTO :EOF
@PAUSE
