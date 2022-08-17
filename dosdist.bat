cd src

sed 's/[@]VERSION[@]/0.9/' imyp_cfg.hin > imyp_cfg.h
if not "%errorlevel%"=="0" goto end
nasm -O999 -f coff -DIMYPLAY_32BIT=1 -o imypdos.obj imypdos.asm
if not "%errorlevel%"=="0" goto end
gcc -O3 -o imyplay.exe @../DOSFILES.TXT
if not "%errorlevel%"=="0" goto end
upx -9 imyplay.exe

cd ..

set imyver=09

mkdir imy-%imyver%
mkdir imy-%imyver%\doc
copy doc\imelody.txt doc\imyplay.1 doc\imyplay.inf imy-%imyver%\doc
mkdir imy-%imyver%\imy
copy imy imy-%imyver%\imy
mkdir imy-%imyver%\po
copy po\imyplay.pot po\pl.gmo imy-%imyver%\po
copy src\imyplay.exe imy-%imyver%
sed 's/$//' < AUTHORS  > imy-%imyver%\AUTHORS.txt
sed 's/$//' < ChangeLo > imy-%imyver%\Changes.txt
sed 's/$//' < COPYING  > imy-%imyver%\COPYING.txt
sed 's/$//' < README   > imy-%imyver%\README.txt
sed 's/$//' < THANKS   > imy-%imyver%\THANKS.txt

SET OLDTZ=%TZ%
SET TZ=CET-1CES
zip -9 -r -T imyplay imy-%imyver%
SET TZ=%OLDTZ%
SET OLDTZ=
rem del imy-%imyver%\po\*.* imy-%imyver%\doc\*.* imy-%imyver%\imy\*.* imy-%imyver%\*.*
rem rmdir imy-%imyver%\po
rem rmdir imy-%imyver%\doc
rem rmdir imy-%imyver%\imy
rem rmdir imy-%imyver%

set imyver=

:end

