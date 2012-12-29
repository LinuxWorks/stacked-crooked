sudo apt-get -y install python-software-properties subversion vim
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get upgrade
sudo apt-get -y install g++-4.7 ccache 
mkdir -p $HOME/bin
echo 'ccache g++-4.7 $*' >  $HOME/bin/g++ && chmod 755 $HOME/bin/g++
echo 'ccache gcc-4.7 $*' >  $HOME/bin/gcc && chmod 755 $HOME/bin/gcc
cd $HOME/bin && ln -fs `pwd`/g++ c++ && ln -fs `pwd`/gcc cc && cd -
set -x
sudo apt-get -y install vim
cd Vim && ./Install.sh && cd -
cd Utilities/CodeFormatter && ./Install.sh && cd -
cd Utilities/svnscripts && ./Install.sh && cd -
sudo apt-get -y install synaptic aptitude
sudo apt-get install libboost1.50-dev libpoco-dev
