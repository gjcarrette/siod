#!/bin/ksh
# name   : siod-dist.sh
# purpose: Grab a binary distribution of siod from
#          one site, into a file that can be untarred
#          at another site. 
#          Then on the unpacked site invoke the csiod
#          command to "recompile" executable files
#          to have the apropriate script name in them.
#          Note that the PATH and LD_LIBRARY_PATH must
#          be properly assigned in the user profiles
#          for the resulting commands to be truly useful.
# created: 2-JULY-1997 George J. Carrette 
# $Id: siod-dist.sh,v 1.2 1997/09/05 20:40:33 gjc Exp $
#
TARNAME="siodb"
AUX_BINFILES="snapshot-dir snapshot-compare http-get cp-build ftp-cp \
             ftp-put ftp-test ftp-get http-stress"
wd=`pwd`
mydir=`dirname $0`
case "$mydir" in
 /*)
 ;;
 *) mydir=$wd/$mydir
 ;;    
esac
#
OPER=${1:?"specify grab or recompile"}
echo "siod-dist $OPER"
if [ "$OPER" = "grab" ]; then
 GRABFROM=${2:-"/usr/local"}
echo grabbing binaries from $GRABFROM
tar cvf - -C $mydir siod-dist.sh \
          -C $GRABFROM lib/libsiod.so \
          -C $GRABFROM lib/siod \
          -C $GRABFROM bin/siod \
          -C $GRABFROM bin/csiod \
          -C $GRABFROM bin/snapshot-dir \
          -C $GRABFROM bin/snapshot-compare \
          -C $GRABFROM bin/http-get \
          -C $GRABFROM bin/cp-build \
          -C $GRABFROM bin/ftp-cp \
          -C $GRABFROM bin/ftp-put \
          -C $GRABFROM bin/ftp-test \
          -C $GRABFROM bin/ftp-get \
          -C $GRABFROM bin/http-stress \
          -C $GRABFROM man/man1/siod.1 \
          -C $GRABFROM man/man1/snapshot-dir.1 \
          -C $GRABFROM man/man1/snapshot-compare.1 \
          -C $GRABFROM man/man1/http-get.1 \
          -C $GRABFROM man/man1/cp-build.1 \
          -C $GRABFROM man/man1/ftp-cp.1 \
          -C $GRABFROM man/man1/csiod.1 \
          -C $GRABFROM man/man1/ftp-put.1 \
          -C $GRABFROM man/man1/ftp-test.1 \
          -C $GRABFROM man/man1/ftp-get.1 \
          -C $GRABFROM man/man1/http-stress.1 \
 | gzip -c > $TARNAME.tgz
echo created $TARNAME.tgz
exit 0
fi
#
if [ "$OPER" = "recompile" ]; then
bindir=$mydir/bin
echo "recompiling commands for $bindir"
PATH=$bindir:$PATH
LD_LIBRARY_PATH=$mydir/lib/siod:$LD_LIBRARY_PATH
export PATH LD_LIBRARY_PATH
chmod u+w $bindir/csiod
siod $bindir/csiod -v01,-m2 :o=$bindir/csiod :i=$bindir/siod \
                            :p=read $bindir/csiod 
for x in $AUX_BINFILES; do
 chmod u+w $bindir/$x
 csiod :o=$bindir/$x $bindir/$x
done
echo done with recompile
exit 0
fi

#
if [ "$OPER" = "unpack" ]; then
 if [ -f $TARNAME.tar ]; then
  rm $TARNAME.tar
 fi
 gunzip -v $TARNAME.tgz
 tar xvf $TARNAME.tar
 ./siod-dist.sh recompile
 echo done with unpacking
 exit 0
fi

#
echo "specify grab or recompile"
exit 1
