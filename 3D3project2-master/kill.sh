#!/usr/bin/env bash

#./kill.sh <router_name>

# this will open a terminal and run the data injection add will close the router in the argument
gnome-terminal --geometry=40x20 --title="CLOSE" --command="./data_injection $1 B !close!" &