LIBPATH=/usr/local/lib
if [ x"$LIBPATH" != x ]; then
  if [ x"$LD_LIBRARY_PATH" = x ]; then
    LD_LIBRARY_PATH=$LIBPATH
  else
    LD_LIBRARY_PATH=$LIBPATH:$LD_LIBRARY_PATH
  fi
  export LD_LIBRARY_PATH
fi

host="localhost"
trainer="./*trainer"
time=1
default_keeper="on"
default_taker="on"

usage()
{
  (echo "Usage: $0 [options]"
   echo "Possible options are:"
   echo "  -h, --help   print this"
   echo "  -n           specifies trial times"
   echo "  -k           use your player as keeper"
   echo "  -t           use your player as taker"
  ) 1>&2
}

while [ $# -gt 0 ]
do
  case $1 in

    -h|--help)
      usage
      exit 0
      ;;
    -n)
      if [ $2 -lt 1 ]; then
        usage
        exit 1
      fi
      time=$2
      ;;
    -k)
      default_keeper="off"
      ;;
    -t)
      default_taker="off"
      ;;
    *)
      usage
      exit 1
      ;;
  esac

  shift 1
done

rm *.log *.rcl *.rcg

for i in $(seq 1 $time)
do
rcssserver server::coach=on server::penalty_shoot_outs=on &>/dev/null &
sleep 1
rcssmonitor &>/dev/null &
./taker.sh $default_taker &
sleep 1
./keeper.sh $default_keeper &
$trainer num=$i total=$time >> train.log
kill $(pidof rcssserver) 
kill $(pidof rcssmonitor)
done
awk '{if(NR <=19 || ($1 != "trainer:" && $1 != "OK" )) print $0}' train.log | awk '{if(NR <=19 || (NR-20)%11<=2) print $0}' > train.log
cat train.log
echo Record:
cat result.log
