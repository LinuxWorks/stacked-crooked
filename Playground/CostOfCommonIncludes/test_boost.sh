ls *.cpp | grep boost | while read line ; do export line ; echo "$(cat $line | cut -d'<' -f2 | cut -d'>' -f1) $(/usr/bin/g++ -std=c++11 -c -E $line | wc -l) $({ time /usr/bin/g++ -std=c++11 -c "$line" -O2 ; } 2>&1 | grep -v '^$' | sed -e 's,real,,g')" ;  done 2>&1 | grep -v 'user\|sys'  