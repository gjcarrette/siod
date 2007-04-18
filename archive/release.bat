rem Name: release.bat
rem Purpose:
rem   make siod binary and source release kit for Windows NT.
rem   assumes directory organization
rem   of authors personal project directories.
rem   must be run in SIOD project directory.
rem $Id: release.bat,v 1.2 1997/05/05 12:46:00 gjc Exp $
rem
rem deltree siod_dist
rem deltree siod_src
del /s /q siod_dist
del /s /q siod_src
mkdir siod_dist
mkdir siod_src
copy release\libsiod.dll      siod_dist
copy release\siod.exe         siod_dist
copy release\wcsiod.exe       siod_dist
copy release\wmsiod.exe       siod_dist
copy release\parser_pratt.dll siod_dist
copy release\regex.dll        siod_dist
copy release\ss.dll           siod_dist
copy release\tar.dll          siod_dist
copy release\sql_oracle.dll   siod_dist
copy release\slibw32.dll      siod_dist
copy *.html                   siod_dist
copy *.scm                    siod_dist
copy *.smd                    siod_dist
copy *.txt                    siod_dist
copy csiodall.bat             siod_dist
copy setup.bat                siod_dist
rem make compressed distribution archive, using wonderfull info-zip tool
del c:\Distribute\winsiod.zip
c:\zip\zip -r c:\Distribute\winsiod siod_dist
copy /b c:\zip\unzipsfx.exe+c:\Distribute\winsiod.zip c:\Distribute\winsiod.exe
rem all done with binary kit
rem now make source kit
rem this list of files duplicates what is also in
rem the unix makefile. TODO: scheme list of files
rem and program to create dist kits for all platforms.
copy siod.txt siod_src
copy siod.man siod_src
copy snapshot-dir.txt siod_src
copy snapshot-dir.man siod_src
copy snapshot-compare.txt siod_src
copy snapshot-compare.man siod_src
copy http-get.txt siod_src
copy http-get.man siod_src
copy cp-build.txt siod_src
copy cp-build.man siod_src
copy ftp-cp.txt siod_src
copy ftp-cp.man siod_src
copy csiod.txt siod_src
copy csiod.man siod_src
copy ftp-put.txt siod_src
copy ftp-put.man siod_src
copy ftp-test.txt siod_src
copy ftp-test.man siod_src
copy ftp-get.txt siod_src
copy ftp-get.man siod_src
copy http-stress.txt siod_src
copy http-stress.man siod_src
copy proxy-server.txt siod_src
copy proxy-server.man siod_src
copy fork-test.scm siod_src
copy http-server.scm siod_src
copy http-stress.scm siod_src
copy http.scm siod_src
copy maze-support.scm siod_src
copy pratt.scm siod_src
copy siod.scm siod_src
copy smtp.scm siod_src
copy sql_oracle.scm siod_src
copy sql_rdb.scm siod_src
copy sql_sybase.scm siod_src
copy cgi-echo.scm siod_src
copy find-files.scm siod_src
copy hello.scm siod_src
copy parser_pratt.scm siod_src
copy pop3.scm siod_src
copy selfdoc.scm siod_src
copy sample.c siod_src
copy siod.html siod_src
copy piechart.scm siod_src
copy cgi.scm siod_src
copy ftp.scm siod_src
copy sql_msql.scm siod_src
copy README.txt siod_src
copy siod.c siod_src
copy siod.h siod_src
copy siodm.c siod_src
copy siodp.h siod_src
copy slib.c siod_src
copy sliba.c siod_src
copy slibu.c siod_src
copy sql_oracle.c siod_src
copy sql_rdb.c siod_src
copy sql_sybase.c siod_src
copy sql_sybasec.c siod_src
copy ss.c siod_src
copy ss.h siod_src
copy trace.c siod_src
copy md5.c siod_src
copy md5.h siod_src
copy gd.c siod_src
copy ndbm.c siod_src
copy tar.c siod_src
copy regex.c siod_src
copy acct.c siod_src
copy statfs.c siod_src
copy parser_pratt.c siod_src
copy sql_msql.c siod_src
copy siod_regex.html siod_src
copy cclass.h siod_src
copy regcomp.c siod_src
copy regex2.h siod_src
copy regfree.c siod_src
copy cname.h siod_src
copy regerror.c siod_src
copy utils.h siod_src
copy engine.c siod_src
copy regex.h siod_src
copy regexec.c siod_src
copy makefile siod_src
copy build_vms.com siod_src
copy setup_vms.com siod_src
copy vms_opt_files.txt siod_src
copy make.bat siod_src
copy siod.mak siod_src
copy libsiod.def siod_src
copy parser_pratt.def siod_src
copy tar.def siod_src
copy ss.def siod_src
copy regex.def siod_src
copy release.bat siod_src
copy csiod.smd siod_src
copy snapshot-dir.smd siod_src
copy snapshot-compare.smd siod_src
copy http-get.smd siod_src
copy cp-build.smd siod_src
copy ftp-cp.smd siod_src
copy ftp-put.smd siod_src
copy ftp-test.smd siod_src
copy ftp-get.smd siod_src
copy http-stress.smd siod_src
copy proxy-server.smd siod_src
copy fixcrlf.smd siod_src
copy siod-dist.sh siod_src
copy wcsiod.c siod_src
copy csiodall.bat siod_src
copy sql_oracle.def siod_src
copy *.dsw siod_src
copy *.dsp siod_src
copy slibw32.c siod_src
copy wmsiod.c siod_src
copy setup.bat siod_src
copy setup.scm siod_src
rem make src zip
del c:\Distribute\winsiodsrc.zip
c:\zip\zip -r c:\Distribute\winsiodsrc siod_src




