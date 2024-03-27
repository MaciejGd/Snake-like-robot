#!/usr/bin/bash

if [ -e "/dev/rfcomm0" ]; then
    sudo rfcomm release /dev/rfcomm0
    echo "dev/rfcomm0 removed successfully"
else 
    echo "no device has been connected to dev/rfcomm0"
fi 
