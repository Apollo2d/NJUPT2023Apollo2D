player="./sample_player"
host="localhost"
teamname="HELIOS_taker"

config_dir="./formations-taker"

debugopt="--offline_logging --debug --debug_server_connect"

if [ $1 = "off" ]; then
player="./your_team"
teamname="Your_taker"
fi

if [ $2 = "change" ]; then
OPT="-h ${host} -t ${teamname}"
fi

OPT="${OPT} --config_dir ${config_dir}"
OPT="${OPT} ${debugopt}"

$player ${OPT} &>taker.log
