#!/bin/bash

LOG="/tmp/gpio.log"
echo "-------" >> $LOG
date -R >> $LOG
echo $1 >> $LOG
echo "/sys$1" >> $LOG

chmod 775 /sys/class/gpio/export
chmod 775 /sys/class/gpio/unexport
chown root.plugdev -R /sys/class/gpio

chmod 775 -R /sys$1
chown root.plugdev -R /sys$1

