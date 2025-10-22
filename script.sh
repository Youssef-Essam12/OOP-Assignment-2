#!/bin/bash
rm -rf *.cpp *.h
cp ~/Documents/oop-assignment-2/NewProject/Source/*
git add .
git commit -m "$1"
git push
