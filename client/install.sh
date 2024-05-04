#!/bin/bash


sudo cp autorun.service ../../../../../etc/systemd/system
sudo cp  auto_client ../../../../../usr/local/bin/auto_client

sudo systemctl daemon-reload
sudo systemctl enable autorun
sudo systemctl start autorun
