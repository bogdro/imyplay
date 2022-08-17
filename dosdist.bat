cd src

sed 's/[@]VERSION[@]/0.4/' imyp_cfg.hin > imyp_cfg.h
gcc -o imyplay.exe imyp_all.c imyplay.c imyp_mid.c imyp_sig.c imyp_exe.c imypwrap.c midifile.c -lalleg
upx -9 imyplay.exe
copy imyplay.exe ..
del imyplay.exe

cd ..

mkdir imy-04
cp -r AUTHORS ChangeLo COPYING doc imy README THANKS imyplay.exe imy-04
mkdir imy-04\po
copy po\imyplay.pot po\pl.gmo imy-04\po

zip -9 -r -T imyplay imy-04
rem del imy-04\po\*.* imy-04\doc\*.* imy-04\imy\*.* imy-04\*.*
rem rmdir imy-04\po
rem rmdir imy-04\doc
rem rmdir imy-04\imy
rem rmdir imy-04
