#!/bin/bash


mkdir -p ~/bin

sudo apt-get update
sudo apt-get update --fix-missing
sudo apt-get upgrade
sudo apt-get -y install subversion vim
svn co https://github.com/StackedCrooked/stacked-crooked.git/trunk ~/stacked-crooked
cp ~/stacked-crooked/Profile/_profile ~/.profile
cp ~/stacked-crooked/Vim/_vimrc_minimal ~/.vimrc

