cd src

sed 's/[@]VERSION[@]/0.3/' imyp_cfg.hin > imyp_cfg.h
gcc -o imyplay.exe imyp_all.c imyplay.c imyp_mid.c imyp_sig.c imypwrap.c midifile.c -lalleg
upx -9 imyplay.exe
copy imyplay.exe ..
del imyplay.exe

cd ..

mkdir imy-03
cp -r AUTHORS ChangeLo COPYING doc imy README THANKS imyplay.exe imy-03
mkdir imy-03\po
copy po\imyplay.pot po\pl.gmo imy-03\po

zip -9 -r -T imyplay imy-03
rem del imy-03\po\*.* imy-03\doc\*.* imy-03\imy\*.* imy-03\*.*
rem rmdir imy-03\po
rem rmdir imy-03\doc
rem rmdir imy-03\imy
rem rmdir imy-03
