#!/bin/sh

ps -ef | grep waspp | grep testing | awk '{print $2}' | xargs sudo kill -15

echo "waiting.."
sleep 3

ps -ef | grep waspp | grep testing