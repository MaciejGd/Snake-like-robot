#!/usr/bin/bash

#example mac address of HC-05 device
#actual mac address of HC-05 module: 58:56:00:01:06:36
MAC_HC="5C:F6:DC:40:C6:DD"

if [ ! -e "/dev/rfcomm0" ]; then
	scan_output=$(hcitool scan)
	if echo "$scan_output" | grep -q "$MAC_HC"; then
		sudo rfcomm bind 0 "$MAC_HC"
		sudo chmod 660 "/dev/rfcomm0"
		echo "HC-05 module connected"
	else
		echo "There is no such device in range"	
		exit
	fi
fi

