#!/bin/bash

# This script is used to check for memory leaks in the program

while true; do
    leaks Webserv | grep 'leaks for' -A 1
    sleep 1
done