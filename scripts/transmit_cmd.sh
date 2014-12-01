#!/bin/sh
#script to fetch commands from nginx and send them
while :
do
	CURR_CMD=`cat /dev/shm/cmd`
	if [ "$CURR_CMD" = "NULL" ]
	then
		:
	else
		#echo $CURR_CMD
		if [ "$CURR_CMD" = "take_pic" ]; then
			/opt/vc/bin/raspistill -o /var/www/druid/files/`date +%s`.jpg
		elif [ "$CURR_CMD" = "start_vid" ]; then
			/opt/vc/bin/raspivid -t 9999999 -o /var/www/druid/files/`date +%s`.mp4&
		elif [ "$CURR_CMD" = "stop_vid" ]; then
			killall -9 raspivid
		else
			/usr/bin/robo $CURR_CMD
		fi
		echo NULL > /dev/shm/cmd
	fi
	sleep 0.5
done
