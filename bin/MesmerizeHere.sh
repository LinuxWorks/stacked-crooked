#!/bin/bash
[ "$#" == "0" ] && {
    MESMERIZE_AUTOLOAD="$(pwd)" open -n /Users/francis/programming/projects/build-Mesmerize-Qt_5_9_1_5_9_1-Release/Mesmerize.app
    exit
}

for i in "$@" ; do
    (cd "$i" && echo "pwd=$(pwd)" && MESMERIZE_AUTOLOAD="$(pwd)" open -n /Applications/Mesmerize.app/Contents/MacOS/Mesmerize)
done

#A
