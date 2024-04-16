# Autorunning Executable using Systemd & Identifying MAC Address


## Finding MAC Addresses

Linux: etc/sys/net: Contains network device information. You can find details about network interfaces (like eth0, wlan0, etc.) in this directory.

However, this is no longer the case due to Consistent Network Device Naming scheme. Now, the interface name starts with a two-character prefix based on the type of interface:

- en for Ethernet
- wl for wireless LAN (WLAN)
- ww for wireless wide area network (WWAN)


Furthermore, lo: The loopback interface, which is a virtual interface used for communication within the local machine. It has the IP address 127.0.0.1.





https://man7.org/linux/man-pages/man5/sysfs.5.html - sysfs

https://www.binarytides.com/c-program-to-get-mac-address-from-interface-name-on-linux/ - get mac address from socket connection


## Systemd

To autorun background daemons, use systemd

How to write .service files:

https://www.freedesktop.org/wiki/Software/systemd/NetworkTarget/

https://linuxhandbook.com/create-systemd-services/

https://www.howtogeek.com/687970/how-to-run-a-linux-program-at-startup-with-systemd/


We want to use network-online.target for client, and network.target for server. This will ensure that all configured network devices are up and have an IP address assigned before the service is started. network-online.target will time out after 90s. Enabling this might considerably delay your boot even if the timeout is not reached.

https://systemd.io/


Steps to add service files

sudo systemctl daemon-reload
sudo systemctl enable autorun


If you want to launch it now

sudo systemctl start autorun

verifying: sudo systemctl status autorun.service
disabling: sudo systemctl disable autorun.service
status: systemctl status autorun.service


timer:
systemctl enable autorun.timer
systemctl start autorun.timer
## Sets for infiltration

1. binary will copy binary into /usr/local/bin
2. Put .service file in system directory via `sudo cp Documents/botnet_c/code/autorun/autorun.service ../../etc/systemd/system`

2. Run .service file. 







