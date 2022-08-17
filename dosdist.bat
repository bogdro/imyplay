cd src

sed 's/[@]VERSION[@]/0.5/' imyp_cfg.hin > imyp_cfg.h
gcc -o imyplay.exe imyp_all.c imyplay.c imyparse.c imyp_mid.c imyp_sig.c imyp_exe.c imypwrap.c midifile.c -lalleg
upx -9 imyplay.exe
copy imyplay.exe ..
del imyplay.exe

cd ..

mkdir imy-05
cp -r AUTHORS ChangeLo COPYING doc imy README THANKS imyplay.exe imy-05
mkdir imy-05\po
copy po\imyplay.pot po\pl.gmo imy-05\po

zip -9 -r -T imyplay imy-05
rem del imy-05\po\*.* imy-05\doc\*.* imy-05\imy\*.* imy-05\*.*
rem rmdir imy-05\po
rem rmdir imy-05\doc
rem rmdir imy-05\imy
rem rmdir imy-05
