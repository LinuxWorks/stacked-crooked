#!/bin/bash
while read line ; do 
    echo "$line" | perl -pe 's,(.*?)\[.*?\](.*),\1 \2,g; s,_, ,g ; s,(.*?)\(.*?\)(.*),\1 \2,g ; s,\s+(.*)\s+,\1,g' 
done
