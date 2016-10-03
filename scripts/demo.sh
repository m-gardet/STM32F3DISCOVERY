#!/bin/bash

# ---------------gpio configuration---------------
# gpio configuration
# INIT NAME MODE PULL
#         PC13, PA00, PB15, ...
#   MODE (3 bytes)
#         INP   (Input Floating)
#         OPP   (Output Push Pull Mode)
#         OOD   (Output Open Drain Mode)
#   PULL (2 bytes)
#         NO   (No Pull-up or Pull-down)
#         UP   (Pull-up)
#         DO   (Pull-down)
# -----------------gpio read/write----------------
# GET NAME
# SET NAME VALUE (10 bytes)

tty=
value=

#speed=115200
speed=230400

CFG_PA08="INIT PA05 INP NO"
CFG_PA07="INIT PA07 OPP NO"
CFG_PA06="INIT PA06 OPP NO"


LD3="PE09"  #[Red Led]
LD4="PE08"  #[Blue Led]
LD5="PE10"  #[Orange Led]
LD6="PE15"  #[Green Led]
LD7="PE11"  #[Green Led]
LD8="PE14"  #[Orange Led]
LD9="PE12"  #[Blue Led]
LD10="PE13" #[Red Led]


INPUT_READ="GET PA05"

led_init()
{
 uart_send "INIT $1 OPP NO"
}


led_on()
{
    uart_send "SET $1 1"
    uart_send "GET $1"
}

led_off()
{
    uart_send "SET $1 0"
    uart_send "GET $1"
}


uart_open()
{
local uart=$1
stty -F ${uart} ispeed $speed ospeed $speed cs8 raw -echo
if [ $? -ne 0 ]; then
    return 1
fi

#stty --all -F ${uart}

## open file descriptor for uart
exec 101< ${uart}
exec 102> ${uart}
}

uart_close()
{

## close file descriptor for uart
exec 101<&-
exec 102>&-

}


uart_send()
{
    value=
    local verbose=true
    #echo "${1}"
    if [ "${2}" == false ] ; then
     verbose=false
    fi
    printf "%s\r" "${1}" >& 102
    sleep 0.02
    read -n 200 -t 2 -u 101 line
    if ! echo "$line" | grep -q "*OK*"  ; then
        if $verbose ;then
        echo "error : $line"
        fi
        return 1
    fi
    if echo "$line" | grep -q "value"  ; then
        value=$( echo "$line" | awk -F ":" '{ $1 = "" ; print $0 }')
        if $verbose ;then
          echo "$value"
        fi
    fi

     return 0
}


detect()
{
  local uart="unknow"
  local uid=
  local name=

  for t in $(find /dev/ -name "ttyACM*"); do
    echo "try : $t"
    uart_open $t
    uart_send "GET UID" "false"
    if [ $? -ne 0 ]; then
        continue
    fi
    uart=$t
    uid=$value

    uart_send "GETNAME" "false"
    name==$value
    break
  done

  if [ "$uart" != "unknow" ] ; then
   tty=$uart
   echo "found device name $name : UID $uid on $tty "
   return 0
  fi

  echo "device not found"
  exit 1

}


detect


echo "tty $tty"

uart_open $tty

#config
#led_init "$LD3"
#led_init "$LD4"
#led_init "$LD5"
#led_init "$LD6"
#led_init "$LD7"
#led_init "$LD8"
#led_init "$LD9"
#led_init "$LD10"

#uart_send "${CFG_PA08}"
#uart_send "${CFG_PA07}"
#uart_send "${CFG_PA06}"


#demo
while true; do

led_on "$LD3"
sleep 0.02
led_off "$LD3"
led_on "$LD4"
sleep 0.02
led_off "$LD4"
led_on "$LD5"
sleep 0.02
led_off "$LD5"
led_on "$LD6"
sleep 0.02
led_off "$LD6"
led_on "$LD7"
sleep 0.02
led_off "$LD7"
led_on "$LD8"
sleep 0.02
led_off "$LD8"
done

uart_close

