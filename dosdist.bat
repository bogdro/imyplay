cd src

sed 's/[@]VERSION[@]/0.7/' imyp_cfg.hin > imyp_cfg.h
if not "%errorlevel%"=="0" goto end
nasm -f coff -o imypdos.obj imypdos.asm
if not "%errorlevel%"=="0" goto end
gcc -O2 -o imyplay.exe @../DOSFILES.TXT
if not "%errorlevel%"=="0" goto end
upx -9 imyplay.exe

cd ..

mkdir imy-07
mkdir imy-07\doc
copy doc imy-07\doc
mkdir imy-07\imy
copy imy imy-07\imy
mkdir imy-07\po
copy po\imyplay.pot po\pl.gmo imy-07\po
copy src\imyplay.exe imy-07
tr '\n' '\r\n' < AUTHORS  > imy-07\AUTHORS.txt
tr '\n' '\r\n' < ChangeLo > imy-07\Changes.txt
tr '\n' '\r\n' < COPYING  > imy-07\COPYING.txt
tr '\n' '\r\n' < README   > imy-07\README.txt
tr '\n' '\r\n' < THANKS   > imy-07\THANKS.txt

zip -9 -r -T imyplay imy-07
rem del imy-07\po\*.* imy-07\doc\*.* imy-07\imy\*.* imy-07\*.*
rem rmdir imy-07\po
rem rmdir imy-07\doc
rem rmdir imy-07\imy
rem rmdir imy-07

:end

