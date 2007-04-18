rem This batch procedure bootstraps csiod.exe
rem and compiles commonly used commands.
rem Some commands are commented out because 
rem they don't work on windows yet.
siod -m2 csiod.smd :o=csiod.exe csiod.smd
rem Note: even though we have built csiod.exe already no, we don't
rem invoke it, because it uses wcsiod.exe, which points to the
rem global SIOD_DIR value in the registry.
rem siod -m2 csiod.smd :o=cp-build.exe cp-build.smd
rem siod -m2 csiod.smd :o=fixcrlf.exe fixcrlf.smd
siod -m2 csiod.smd :o=ftp-cp.exe ftp-cp.smd
siod -m2 csiod.smd :o=ftp-get.exe ftp-get.smd
siod -m2 csiod.smd :o=ftp-put.exe ftp-put.smd
siod -m2 csiod.smd :o=ftp-test.exe ftp-test.smd
siod -m2 csiod.smd :o=http-get.exe http-get.smd
siod -m2 csiod.smd :o=http-stress.exe http-stress.smd
siod -m2 csiod.smd :o=proxy-server.exe proxy-server.smd
siod -m2 csiod.smd :o=snapshot-compare.exe snapshot-compare.smd
siod -m2 csiod.smd :o=snapshot-dir.exe snapshot-dir.smd
