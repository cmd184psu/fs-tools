#!/bin/sh

mkdir ../images
mkdir ../docs
mkdir ../zip

mv -i *.txt ../docs/
mv -i *.doc* ../docs/
mv -i *.xls* ../docs/
mv -i *.ppt* ../docs/
mv -i *.pdf ../docs/

mv -i *.bmp ../images/
mv -i *.pcx ../images/
mv -i *.gif ../images/
mv -i *.jpg ../images/
mv -i *.png ../images/
mv -i *.tif ../images/

mv -i *.zip ../zip/
mv -i *.tgz ../zip/
mv -i *.tar.gz ../zip/
mv -i *.bz2 ../zip/
mv -i *.gz ../zip/


