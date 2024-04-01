#!/usr/bin/bash
#actual mac address of HC-05 module: 58:56:00:01:06:36
MAC_HC="58:56:00:01:06:36"
#MAC_HC="5C:F6:DC:40:C6:DD"

#check for MAC addresses connected to ports of rfcomm
MAC_CON=$(rfcomm -a | grep "$MAC_HC")
rfcomm_port=0

if [ -n "$MAC_CON" ]; then
	 echo "HC-05 module already connected to: $MAC_CON"
else 
	scan_output=$(hcitool scan)
	if echo "$scan_output" | grep -q "$MAC_HC"; then
		while [ "$rfcomm_port" -le 29 ] 
		do
			if [ ! -e "/dev/rfcomm$rfcomm_port" ]; then
				if sudo rfcomm bind "$rfcomm_port" "$MAC_HC"; then
					sudo chmod a+rw "/dev/rfcomm$rfcomm_port"
					echo "HC-05 module connected to a port /dev/rfcomm$rfcomm_port"
					exit
				else
					echo "error while trying to connect to /dev/rfcomm$rfcomm_port"
					exit
				fi
			fi
			((rfcomm_port++))
		done
	else
		echo "There is no HC-05 module in range"	
	fi
fi
		
