player="./sample_player"
host="localhost"
teamname="HELIOS_keeper"

config_dir="./formations-keeper"

debugopt="--offline_logging --debug --debug_server_connect"

if [ $1 = "off" ]; then
player="./your_player"
teamname="Your_keeper"
fi

if [ $2 = "change" ]; then
OPT="-h ${host} -t ${teamname}"
fi

OPT="${OPT} --config_dir ${config_dir}"
OPT="${OPT} ${debugopt}"

$player ${OPT} -g &>kepper.log
