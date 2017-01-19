#!/bin/bash

chmod +x *
sudo cp 93-gpio.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
