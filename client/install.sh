#!/bin/bash


sudo cp autorun.service ../../../../../etc/systemd/system
sudo cp autorun.sh ../../../../../usr/local/bin/autorun.sh
sudo cp  build/auto_client ../../../../../usr/local/bin/auto_client



sudo systemctl daemon-reload
sudo systemctl enable autorun
sudo systemctl start autorun
