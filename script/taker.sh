player="./*player"
teamname="taker"
host="localhost"

config_dir="./formations-taker"

debugopt="--offline_logging --debug --debug_server_connect"

OPT="-h ${host} -t ${teamname}"
OPT="${OPT} --config_dir ${config_dir}"
OPT="${OPT} ${debugopt}"

$player ${OPT} &>taker.log
