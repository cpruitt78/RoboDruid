#!/bin/sh
#script ran on init to start a wifi hotspot
sleep 10
rfkill unblock all
sleep 1
touch /dev/shm/cmd
ifconfig wlan0 up
sleep 2
ifconfig wlan0 192.168.1.1
#wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant/client.conf
sleep 5
systemctl start hostapd
sleep 1
touch /dev/shm/cmd
/usr/bin/uv4l -k --sched-rr --config-file=/etc/uv4l/uv4l-raspicam.conf --driver raspicam --driver-config-file=/etc/uv4l/uv4l-raspicam.conf
/usr/bin/transmit_cmd.sh&
sleep infinity
