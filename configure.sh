#!/bin/sh
[ $@ ] || { echo "$0 tarvitsee luotavan tiedoston nimen"; exit 1; }
cp näkymä_conf.c $@
sed -i 's@MAKE_LIITÄ_HOME@'"${HOME}"'@g' $@
fontti=`fc-match -v monospace |grep file: |sed 's@.*"\(.*\)".*@\1@'`
sed -i 's@MAKE_LIITÄ_MONOFONTTI@'"${fontti}"'@g' $@
fontti=`fc-match -v sans-serif |grep file: |sed 's@.*"\(.*\)".*@\1@'`
sed -i 's@MAKE_LIITÄ_SANSFONTTI@'"${fontti}"'@g' $@
sed -i 's@MAKE_LIITÄ_SANS-SERIFFONTTI@'"${fontti}"'@g' $@
fontti=`fc-match -v serif |grep file: |sed 's@.*"\(.*\)".*@\1@'`
sed -i 's@MAKE_LIITÄ_SERIFFONTTI@'"${fontti}"'@g' $@
