cd src

sed 's/[@]VERSION[@]/0.2/' imyp_cfg.hin > imyp_cfg.h
gcc -o imyplay.exe *.c -lalleg
upx -9 imyplay.exe
copy imyplay.exe ..
del imyplay.exe

cd ..

mkdir imy-02
cp -r AUTHORS ChangeLo COPYING doc imy README THANKS imyplay.exe imy-02
mkdir imy-02\po
copy po\imyplay.pot po\pl.gmo imy-02\po

zip -9 -r -T imyplay imy-02
rem del imy-02\po\*.* imy-02\doc\*.* imy-02\imy\*.* imy-02\*.*
rem rmdir imy-02\po
rem rmdir imy-02\doc
rem rmdir imy-02\imy
rem rmdir imy-02
