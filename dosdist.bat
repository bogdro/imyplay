cd src

sed 's/[@]VERSION[@]/0.6/' imyp_cfg.hin > imyp_cfg.h
gcc -o imyplay.exe @../DOSFILES.TXT
upx -9 imyplay.exe
copy imyplay.exe ..
del imyplay.exe

cd ..

mkdir imy-06
cp -r AUTHORS ChangeLo COPYING doc imy README THANKS imyplay.exe imy-06
mkdir imy-06\po
copy po\imyplay.pot po\pl.gmo imy-06\po

zip -9 -r -T imyplay imy-06
rem del imy-06\po\*.* imy-06\doc\*.* imy-06\imy\*.* imy-06\*.*
rem rmdir imy-06\po
rem rmdir imy-06\doc
rem rmdir imy-06\imy
rem rmdir imy-06
