player="./*player"
teamname="keeper"
host="localhost"

config_dir="./formations-keeper"

debugopt="--offline_logging --debug --debug_server_connect"

OPT="-h ${host} -t ${teamname}"
OPT="${OPT} --config_dir ${config_dir}"
OPT="${OPT} ${debugopt}"

$player ${OPT} -g &>kepper.log
