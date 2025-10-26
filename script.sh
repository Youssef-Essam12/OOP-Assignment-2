#!/bin/bash
set -e
set -u
rm -rf Youssef/*.cpp Youssef/*.h
cp ~/Documents/oop-assignment-2/NewProject/Source/* Youssef
git add .
git commit -m "$1"
git push
