$ SELF = F$ENV("PROCEDURE")
$ SELF_DIR = F$PARSE(SELF,,,"DEVICE") + -
             F$PARSE(SELF,,,"DIRECTORY")
$ ARCH_DIR = F$EXTRACT(0,F$LENGTH(SELF_DIR) -1,SELF_DIR) +-
             "." + f$getsyi("ARCH_NAME") + "]"
$ DEFINE/JOB SIOD_LIB 'ARCH_DIR'
$ DEFINE/JOB SIODSHR SIOD_LIB:SIODSHR.EXE
$ DEFINE/JOB HS_REGEXSHR SIOD_LIB:HS_REGEXSHR.EXE
$ SIOD == "$SIOD_LIB:SIOD"
$ csiod == "$SIOD_LIB:csiod"
$ snapshot_dir == "$SIOD_LIB:snapshot-dir"
$ snapshot_compare == "$SIOD_LIB:snapshot-compare"
$ http_get == "$SIOD_LIB:http-get"
$ cp_build == "$SIOD_LIB:cp-build"
$ ftp_cp == "$SIOD_LIB:ftp-cp"
$ ftp_put == "$SIOD_LIB:ftp-put"
$ ftp_test == "$SIOD_LIB:ftp-test"
$ ftp_get == "$SIOD_LIB:ftp-get"
$ http_stress == "$SIOD_LIB:http-stress"
$ proxy_server == "$SIOD_LIB:proxy-server"


