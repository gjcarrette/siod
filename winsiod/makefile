# Makefile for SIOD under Unix.
# George J. Carrette, gjc@alum.mit.edu
#
# 25-MAR-2014: this file was hacked to work in the winsiod
# folder you get from the source downloaded from http://siod.codeplex.com/
# svn co https://siod.svn.codeplex.com/svn siod-src
# It has been tested with "make linux" under Ubuntu 13.1
# There are a large amount of warnings.

#
# Note: The recursive call to make attempts to set LD_LIBRARY_PATH
#       to include the current directory, which is required for the execution
#       of the SIOD compiler. If you are debugging in the current directory
#       you must also set environment LD_LIBRARY_PATH=.
#       Otherwise SIOD won't work at all, or whats worse, the previously
#       installed libsiod will override the local copy you are trying to debug.
#      
#       For debug builds, use make XXX CDEBUG=-g
#
#       You might also need "make SHELL=/bin/sh target" if your default shell
#       is not compatible with sh or ksh.
#
#       Static executable 'sample' is built for comparison purposes
#       on some platforms.
#
# 
MANSEC=1
MANDIR=$(IROOT)/usr/local/man/man$(MANSEC)
BINDIR=$(IROOT)/usr/local/bin
INCDIR=$(IROOT)/usr/local/include
LIBDIR=$(IROOT)/usr/local/lib
LIBSIODDIR=$(LIBDIR)/siod
CP_F=cp -f
# -Wmissing-prototypes
GCCW=-Wall -Wstrict-prototypes
#
SAMPLE_OBJS = sample.o slib.o sliba.o trace.o
SIOD_OBJS_COMMON = slib.o sliba.o trace.o slibu.o md5.o
SQL_SYBASE_OBJS = sql_sybase.o sql_sybasec.o
HS_REGEX_OBJS=regcomp.o regerror.o regexec.o regfree.o
#
#
default:
	@echo "*****************************************************"
	@echo "* Please specify target from operating system list: *"
	@echo "*  osf1 hpux solaris linux sgi sco unknown          *"
	@echo "*  gccflags hpuxgcc                                 *"
	@echo "*****************************************************"
	@echo
	@echo  " dist    ... siod.tgz"
	@echo  " install ... copies to $(BINDIR) etc, see makefile for doc"
	@echo  " clean   ... delete objects and binaries."

.SUFFIXES: .o .so .man .txt .sl .smd

# the build_driver is the target of the recursive call to make.

CMDFILES = csiod snapshot-dir snapshot-compare http-get cp-build \
           ftp-cp ftp-put ftp-test ftp-get http-stress proxy-server

build_driver: $(PROGS) $(EXTRA_PROGS) $(CMDFILES)
	@echo "Build done."

LDLP=LD_LIBRARY_PATH=.:$$LD_LIBRARY_PATH
SLD=-DSIOD_LIB_DEFAULT=\\\"$(LIBSIODDIR)\\\"


# Digital Equipment Corporation, OSF/1, DIGITAL UNIX.
# uname = OSF1

osf1:
	$(MAKE) $(LDLP) \
	PROGS="siod sample\
              ndbm.so tar.so parser_pratt.so ss.so regex.so\
              acct.so statfs.so" \
	CFLAGS="$(CDEBUG) -readonly_strings -O2 $(SLD)" \
	LD_EXE_FLAGS="-call_shared" \
	LD_EXE_LIBS="-lm -lc" \
	LD_LIB_FLAGS="-shared" \
	LD_LIB_LIBS="-lm -lc" \
	SO="so" \
	SYBINCS="-I$(SYBASE)/include" \
	SYBLIBS="$(SYBASE)/lib/libct.a $(SYBASE)/lib/libcs.a \
                 $(SYBASE)/lib/libcomn.a $(SYBASE)/lib/libtcl.a \
                 $(SYBASE)/lib/libinsck.a $(SYBASE)/lib/libsdna.a \
                 -ldnet_stub $(SYBASE)/lib/libintl.a -lm -lc" \
        build_driver

# Hewlett-Packard Company, HP-UX
# From: Craig Denson <denson@sdd.hp.com>, hacked up by gjc
# uname = HP-UX

hpux:
	$(MAKE) $(LDLP) \
	PROGS="siod ndbm.sl tar.sl parser_pratt.sl ss.sl \
	       regex.sl" \
	CFLAGS="-Ae -O +z -Dhpux -Dunix" \
	LD_EXE_FLAGS="" \
	LD_EXE_LIBS="-lm -ldld" \
	LD_LIB_FLAGS="-b" \
	LD_LIB_LIBS="-lm -ldld -lndbm" \
	SO="sl" \
	SYBINCS="-I$(SYBASE)/include" \
	SYBLIBS="$(SYBASE)/lib/libct.a $(SYBASE)/lib/libtcl.a \
	         $(SYBASE)/lib/libinsck.a \
	         $(SYBASE)/lib/libcs.a $(SYBASE)/lib/libcomn.a \
	         $(SYBASE)/lib/libintl.a \
	         -Wl,-a,archive -lcl $(SYSLIBS) -lBSD -lm" \
        build_driver

## hpux but with GCC by the author.

hpuxgcc:
	$(MAKE) $(LDLP) \
	PROGS="siod ndbm.sl tar.sl parser_pratt.sl ss.sl \
	       regex.sl regex.sl" \
        HS_REGEX_OBJS_NEEDED="$(HS_REGEX_OBJS)" \
        CC=gcc \
        LD=ld \
	CFLAGS="-O -fPIC $(SLD)" \
	LD_EXE_FLAGS="" \
	LD_EXE_LIBS="-lm -ldld" \
	LD_LIB_FLAGS="-b" \
	LD_LIB_LIBS="-lm -ldld -lndbm" \
	SO="sl" \
	SYBINCS="-I$(SYBASE)/include" \
	SYBLIBS="$(SYBASE)/lib/libct.a $(SYBASE)/lib/libtcl.a \
	         $(SYBASE)/lib/libinsck.a \
	         $(SYBASE)/lib/libcs.a $(SYBASE)/lib/libcomn.a \
	         $(SYBASE)/lib/libintl.a \
	         -Wl,-a,archive -lcl $(SYSLIBS) -lBSD -lm" \
        build_driver


### Sun Microsystems ###
# Solaris 2.4, Sybase R10.0.2
# From: Philip G Wilson <pgw9@columbia.edu>
# hacked by gjc for dlopen.
# uname = SunOS, uname -r = 5.3
# Most recently tested in 5.5 by gjc.
# Note: I recommend dealing with dynamic library management and name
#       conflict issues on solaris by making soft links
#       from /usr/lib/libSYBxxx.so -> /sybase/lib/libxxx.so
#       for each shared library in /sybase/lib.

solaris:
	$(MAKE) $(LDLP) \
	PROGS="siod ndbm.so tar.so parser_pratt.so ss.so regex.so" \
	CC=gcc \
	LD=ld \
	CFLAGS="$(GCCW) $(CDEBUG) -DSUN5 -O2 $(SLD)" \
	LD_EXE_FLAGS="-R $(LIBDIR) -R $(LIBSIODDIR)" \
	LD_EXE_LIBS="-lm -lsocket -lnsl -lposix4 -dl" \
	LD_LIB_FLAGS="-G" \
	LD_LIB_LIBS="$(LD_EXE_LIBS)" \
	SO="so" \
        build_driver

sybsolaris:
	$(MAKE) $(LDLP) \
	PROGS="siod ndbm.so tar.so parser_pratt.so ss.so regex.so\
               sql_sybase.so gd.so sample" \
	CC=gcc \
	LD=gcc \
	CFLAGS="$(GCCW) $(CDEBUG) -DSUN5 -O2 $(SLD)" \
	LD_EXE_FLAGS="-Xlinker -R -Xlinker $(LIBDIR) -Xlinker -R -Xlinker $(LIBSIODDIR)" \
	LD_EXE_LIBS="-lm -lsocket -lnsl -lposix4 -dl" \
	LD_LIB_FLAGS="-G" \
	LD_LIB_LIBS="$(LD_EXE_LIBS)" \
	SO="so" \
	SYBINCS="-I$(SYBASE)/include" \
	SYBLIBS="-lSYBct -lSYBcs -lSYBcomn -lSYBtcl -lSYBtli \
                 -lSYBintl -lsocket -lnsl -lm" \
        build_driver

## Sun Microsystems ###
# SUN-OS 4.1.1
# From: Leo Harten lph@paradigm.com
# uname = ???, uname -r = ???

sunos:
	$(MAKE) $(LDLP) \
	PROGS="siod tar.so parser_pratt.so ss.so" \
	CC=gcc \
	LD=gcc \
	CFLAGS="$(GCCW) $(CDEBUG) -O2" \
	LD_EXE_FLAGS="" \
	LD_EXE_LIBS="-lm -lsocket -lnsl -lposix4 -dl" \
	LD_LIB_FLAGS="-G" \
	LD_LIB_LIBS="$(LD_EXE_LIBS)" \
	SO="so" \
	SYBINCS="-I$(SYBASE)/include" \
	SYBLIBS="-L$(SYBASE)/lib -lct -lcs -lcomn -ltcl -ltli \
                 -lintl -lsocket -lnsl -lm" \
        build_driver

# Tested by GJC on an intel pentium running Linux 2.0.32
# under Redhat-5.0. Additional modules such as ndbm.c
# may work without modification in other environments.
# uname = Linux

linux:
	$(MAKE) $(LDLP) \
	PROGS="siod tar.so parser_pratt.so ss.so \
	       regex.so acct.so" \
	CC="gcc" \
	LD="gcc" \
	CFLAGS="$(GCCW) $(CDEBUG) -fPIC -O2 -D__USE_MISC -D__USE_GNU -D__USE_SVID -D__USE_XOPEN_EXTENDED -D__USE_XOPEN $(SLD)" \
	LD_EXE_FLAGS="-rdynamic -Xlinker -rpath -Xlinker $(LIBDIR) -Xlinker -rpath -Xlinker $(LIBSIODDIR)" \
	LD_EXE_LIBS="-ldl" \
	LD_LIB_FLAGS="-shared" \
	LD_LIB_LIBS="-lm -lc -ldl -lcrypt" \
	SO="so" \
        build_driver

# adapt by Philippe Laliberte for MkLinux on ppc
# I used -fsigned-char becaused their are unexplicited char
# and we're using unsigned as a default.
# uname -a = Linux <machine name> 2.0.28-osfmach3 #1 <date> ppc
# Date: Tue, 22 Apr 1997 08:48:03 -0400
# From: Philippe Laliberte <arsphl@oeil.mlink.net>

linux-ppc:
	@echo ""
	@echo ""
	@echo "You need a shared lib version of Linux to make this work"
	@echo ""
	@echo ""
	@echo ""
	@echo ""
	@echo "This an experimental compilation for MkLinux"
	@echo "I didn't test it on other PPC systems"
	@echo "Philippe Laliberte"
	@echo ""
	@echo ""
	@echo ""
	@echo ""
	@echo ""
	@echo ""
	@echo ""

	$(MAKE) $(LDLP) \
	PROGS="siod tar.so parser_pratt.so ss.so \
	       regex.so acct.so md5.o" \
	CC="gcc" \
	LD="gcc" \
	CFLAGS="$(GCCW) $(CDEBUG) -fPIC -fsigned-char -O2 -D__USE_MISC" \
	LD_EXE_FLAGS="" \
	LD_EXE_LIBS="-ldl -lcrypt" \
	LD_LIB_FLAGS="-shared" \
	LD_LIB_LIBS="-lm -lc -ldl -lcrypt" \
	SO="so" \
        build_driver


# Silicon Graphics, MIPS.
# uname = IRIX

sgi:
	$(MAKE) $(LDLP) \
	PROGS="siod \
              ndbm.so tar.so parser_pratt.so ss.so regex.so"\
        HS_REGEX_OBJS_NEEDED="$(HS_REGEX_OBJS)" \
	CFLAGS="$(CDEBUG) -fullwarn -O2 -I. $(SLD)" \
	LD_EXE_FLAGS="-call_shared -rpath $(LIBDIR):$(LIBSIODDIR)" \
	LD_EXE_LIBS="-lm -lc" \
	LD_LIB_FLAGS="-shared" \
	LD_LIB_LIBS="-lm -lc" \
	SO="so" \
        build_driver

sco:
	-ln -s ssiod siod
	$(MAKE) \
	PROGS="sample ssiod" \
	CFLAGS="$(CDEBUG)" \
	LD_EXE_LIBS="-lm -ldl" \
	build_driver

unknown:
	-ln -s ssiod siod
	$(MAKE) \
	PROGS="sample ssiod" \
	CFLAGS="$(CDEBUG) -U__osf__ -Usun -USUN5 -Ulinux" \
	LD_EXE_LIBS="-lm" \
	build_driver


### Finally, the actual compilation and linking commands.

libsiod.$(SO): $(SIOD_OBJS_COMMON)
	@echo LD_LIBRARY_PATH = $$LD_LIBRARY_PATH
	$(LD) -o libsiod.$(SO) $(LD_LIB_FLAGS) $(SIOD_OBJS_COMMON) \
                               $(LD_LIB_LIBS) 

ssiod: siod.o $(SIOD_OBJS_COMMON)
	$(CC) -o ssiod $(LD_EXE_FLAGS) siod.o \
                       $(SIOD_OBJS_COMMON) $(LD_EXE_LIBS)

siod: siod.o libsiod.$(SO) 
	$(CC) -o siod $(LD_EXE_FLAGS) siod.o libsiod.$(SO) $(LD_EXE_LIBS)

sample: $(SAMPLE_OBJS)
	$(CC) -o sample $(LD_EXE_FLAGS) $(SAMPLE_OBJS) $(LD_EXE_LIBS)

.o.$(SO):
	$(LD) -o $@ $(LD_LIB_FLAGS) $< libsiod.$(SO) $(LD_LIB_LIBS)


tar.$(SO): tar.o libsiod.$(SO)

ss.$(SO): ss.o libsiod.$(SO)

acct.$(SO): acct.o libsiod.$(SO)

sql_sybase.$(SO): $(SQL_SYBASE_OBJS)
	$(LD) -o sql_sybase.$(SO) $(LD_LIB_FLAGS) $(SQL_SYBASE_OBJS) \
                 libsiod.$(SO) $(LD_LIB_LIBS) $(SYBLIBS)

MSQL_ROOT=/usr/local/Minerva

sql_msql.$(SO): sql_msql.o libsiod.$(SO)
	$(LD) -o sql_msql.$(SO) $(LD_LIB_FLAGS) sql_msql.o libsiod.$(SO) \
                          $(MSQL_ROOT)/lib/libmsql.a $(LD_LIB_LIBS)


gd.$(SO): gd.o libsiod.$(SO)
	$(LD) -o gd.$(SO) $(LD_LIB_FLAGS) gd.o libsiod.$(SO) -lgd \
                          $(LD_LIB_LIBS)

ndbm.$(SO): ndbm.o libsiod.$(SO)
	$(LD) -o ndbm.$(SO) $(LD_LIB_FLAGS) ndbm.o libsiod.$(SO) \
                            $(LD_LIB_LIBS)

regex.$(SO): regex.o libsiod.$(SO) $(HS_REGEX_OBJS_NEEDED)
	$(LD) -o regex.$(SO) $(LD_LIB_FLAGS) regex.o $(HS_REGEX_OBJS_NEEDED) \
                 libsiod.$(SO) $(LD_LIB_LIBS)


siod.o: siod.c siod.h

sample.o: sample.c siod.h

slib.o:	slib.c siod.h siodp.h

sliba.o:	sliba.c siod.h siodp.h

trace.o:	trace.c siod.h siodp.h

slibu.o:	slibu.c siod.h siodp.h md5.h

ss.o:	ss.c siod.h ss.h

md5.o:	md5.c md5.h

sql_sybase.o:	sql_sybase.c siod.h
	$(CC) $(CFLAGS) $(SYBINCS) -c sql_sybase.c

sql_sybasec.o:	sql_sybasec.c siod.h
	$(CC) $(CFLAGS) $(SYBINCS) -c sql_sybasec.c

# Note: http://siva.cshl.org/gd/gd.html
gd.o: gd.c
	$(CC) $(CFLAGS) -c gd.c -I$(INCDIR)

sql_msql.o: sql_msql.c
	$(CC) $(CFLAGS) -c sql_msql.c -I$(MSQL_ROOT)/include

ndbm.o: ndbm.c siod.h

statfs.o: statfs.c siod.h

tar.o: tar.c siod.h

regex.o: regex.c siod.h

acct.o: acct.c siod.h

parser_pratt.o: parser_pratt.c siod.h

MANPAGES = siod snapshot-dir snapshot-compare http-get \
           cp-build ftp-cp csiod ftp-put ftp-test ftp-get \
           http-stress proxy-server

LIBFILES = fork-test.scm http-server.scm http-stress.scm http.scm \
           maze-support.scm pratt.scm siod.scm smtp.scm sql_oracle.scm \
           sql_rdb.scm sql_sybase.scm cgi-echo.scm find-files.scm \
           hello.scm parser_pratt.scm pop3.scm selfdoc.scm \
	   sample.c siod.html piechart.scm cgi.scm ftp.scm \
           sql_msql.scm

SOLIBFILES=gd ndbm tar ss regex acct sql_sybase sql_oracle parser_pratt \
           statfs

PUBINCS = siod.h

COMMON_SRCS=README.txt siod.c siod.h \
            siodm.c siodp.h slib.c sliba.c slibu.c sql_oracle.c \
            sql_rdb.c sql_sybase.c sql_sybasec.c ss.c \
            ss.h trace.c md5.c md5.h \
            gd.c ndbm.c tar.c regex.c acct.c statfs.c \
            parser_pratt.c sql_msql.c

REGEX_SRCS=siod_regex.html cclass.h regcomp.c regex2.h regfree.c \
           cname.h regerror.c utils.h engine.c regex.h regexec.c

UNIX_MK=makefile

VMS_MK=build_vms.com setup_vms.com vms_opt_files.txt

NT_MK=libsiod.def parser_pratt.def \
      tar.def ss.def regex.def release.bat

CMDSRCS = $(CMDFILES:=.smd)

SRCFILES= $(COMMON_SRCS) $(UNIX_MK) $(VMS_MK) $(NT_MK) fixcrlf.smd \
          siod-dist.sh $(REGEX_SRCS)

DISTFILES= $(CMDSRCS) $(LIBFILES) $(SRCFILES) $(MANPAGES:=.man) \
           $(MANPAGES:=.txt)

INTO_BINDIR=$(CMDFILES) siod
INTO_LIBDIR=libsiod.so libsiod.sl

install: $(DISTFILES)
	@echo "Note: This does not do a build. Only installs what already"
	@echo "      sits in the directory."
	-mkdir -p $(MANDIR)
	-mkdir -p $(BINDIR)
	-mkdir -p $(LIBDIR)
	-mkdir -p $(INCDIR)
	-mkdir -p $(LIBSIODDIR)
	-for X in $(INTO_BINDIR)  ; do \
	 $(CP_F) $$X $(BINDIR) ;\
	done
	-for X in $(LIBFILES) ; do \
	 $(CP_F) $$X $(LIBSIODDIR) ;\
	done
	-for X in $(SOLIBFILES) ; do \
	  for E in so sl ; do \
	   $(CP_F) $$X.$$E $(LIBSIODDIR) ;\
	  done ;\
	done
	-for X in $(INTO_LIBDIR) ; do \
	 $(CP_F) $$X $(LIBDIR) ;\
	done
	-for X in $(MANPAGES) ; do \
	 $(CP_F) $$X.man $(MANDIR)/$$X.$(MANSEC) ;\
	done
	-for X in $(PUBINCS) ; do \
	 $(CP_F) $$X $(INCDIR) ;\
	done
	@echo "Install done."

clean:
	-rm *.o *.so *.sl *~ $(MANPAGES:.man=.txt) so_locations \
            siod sample siod.tar siod.tgz siod.zip selfdoc.txt TAGS \
	$(CMDFILES)

# make manpage txt files for distribution to people who do not have 
# nroff.

.man.txt:
	nroff -man $< | col -bx > $@

siod.tar: $(DISTFILES)
	tar cvf siod.tar $(DISTFILES)

siod.tgz: $(DISTFILES)
	tar cvf - $(DISTFILES) | gzip -c -v > siod.tgz

dist: siod.tgz siod.zip
	@echo "distribution kit created."

siod.zip: $(DISTFILES)
	zip siod.zip $(DISTFILES)

fixcrlf_problems: $(DISTFILES)
	./siod -v01,-m2 fixcrlf.smd :action=write $(DISTFILES)

# another case of makefile trouble. I need the binding of $(SO)
# here.

selfdoc:
	./selfdoc.scm *.so > selfdoc.txt

# 

WIN95BIN=./release/libsiod.dll ./release/libsiod.lib ./release/siod.exe \
         ./release/parser_pratt.dll 

win95bin.zip: $(WIN95BIN)
	zip -rj win95bin.zip $(WIN95BIN)

PUBFILES=siod.html siod_regex.html README.txt siod.tgz siod.zip

ftp.std.com: dist $(PUBFILES)
	for X in $(PUBFILES); do \
	  ftp-put $$X world.std.com /ftp/pub/$$LOGNAME/$$X ; \
	done
	touch ftp.std.com

ftp.delphiforums.com: $(PUBFILES)
	for X in $(PUBFILES); do \
	  ftp-put $$X ftp.delphiforums.com web/$$X :rename=false ; \
	done
	touch ftp.delphiforums.com

whatsup:
	@rlog -R -L RCS/*

csiod:	csiod.smd
	./siod -v01,-m2 csiod.smd csiod.smd \
               :o=csiod :i=$(BINDIR)/siod :p=read


snapshot-dir: snapshot-dir.smd

snapshot-compare: snapshot-compare.smd

http-get: http-get.smd

http-stress: http-stress.smd

cp-build: cp-build.smd

ftp-cp: ftp-cp.smd

ftp-put: ftp-put.smd

ftp-test: ftp-test.smd

ftp-get: ftp-get.smd

gccflags:
	@echo "*********************************"
	@echo "*** built-in gcc defines are: ***"
	gcc -E -dM -x c /dev/null
	@echo "*********************************"

# Note: You can use the following default rule in your
#       own makefiles, but it doesn't work here until siod has
#       been installed to its BINDIR
#      
#.smd:
#	csiod $< :o=$@
#
.smd:
	./siod -v01,-m2 csiod.smd :o=$@ :i=$(BINDIR)/siod $<


# Instead of copying additional files to the winsiod folder
# these rules pull these extra files in.
EXTRA_SRC_FOLDER=../archive
EXTRA_SRC_FILES=acct.c cp-build.smd fork-test.scm build_vms.com \
                setup_vms.com vms_opt_files.txt release.bat \
                siod-dist.sh $(MANPAGES:=.man)

$(EXTRA_SRC_FILES):
	cp -v $(EXTRA_SRC_FOLDER)/$@ $@



