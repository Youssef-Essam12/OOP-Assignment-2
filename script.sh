#!/bin/bash
set -e
set -u
rm -rf finalApp/*.cpp finalApp/*.h
cp -r ~/Documents/oop-assignment-2/NewProject/Source/* finalApp
git add .
git commit -m "$1"
git push
