#!/usr/bin/env bash

#./add.sh <router_name> <connections>

# this will add a new router with maximum 6 connections
gnome-terminal --geometry=40x20 --title="$1" --command="./my-router $1 $2 $3 $4 $5 $6 $3 $4 $5 $6 $7 $8 $9" &