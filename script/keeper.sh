#!/bin/sh

LIBPATH=/usr/local/lib
if [ x"$LIBPATH" != x ]; then
  if [ x"$LD_LIBRARY_PATH" = x ]; then
    LD_LIBRARY_PATH=$LIBPATH
  else
    LD_LIBRARY_PATH=$LIBPATH:$LD_LIBRARY_PATH
  fi
  export LD_LIBRARY_PATH
fi

DIR=`dirname $0`

player="${DIR}/*player"
teamname="keeper"
host="localhost"

config_dir="${DIR}/formations-keeper"

debugopt="--offline_logging --debug --debug_server_connect"

OPT="-h ${host} -t ${teamname}"
OPT="${OPT} --config_dir ${config_dir}"
OPT="${OPT} ${debugopt}"

$player ${OPT} -g &>kepper.log
