rem
rem IMYplay - A program for playing iMelody ringtones (IMY files).
rem 	-- Build script for the DOS system.
rem
rem  Copyright (C) 2021-2025 Bogdan Drozdowski, bogdro (at) users.sourceforge.net
rem  License: GNU General Public License, v3+
rem
rem  This program is free software; you can redistribute it and/or
rem  modify it under the terms of the GNU General Public License
rem  as published by the Free Software Foundation; either version 3
rem  of the License, or (at your option) any later version.
rem
rem  This program is distributed in the hope that it will be useful,
rem  but WITHOUT ANY WARRANTY; without even the implied warranty of
rem  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem  GNU General Public License for more details.
rem
rem  You should have received a copy of the GNU General Public License
rem  along with this program. If not, see <http://www.gnu.org/licenses/>.
rem

rem cd src

set imymajor=2
set imyminor=0
rem set imysubminor=

rem sed 's/[@]VERSION[@]/%imymajor%.%imyminor%.%imysubminor%/' src\imyp_cfg.hin > src\imyp_cfg.h
sed 's/[@]VERSION[@]/%imymajor%.%imyminor%/' src\imyp_cfg.hin > src\imyp_cfg.h
if not "%errorlevel%"=="0" goto end
nasm -O999 -f coff -DIMYPLAY_32BIT=1 -o src\imypdos.obj src\imypdos.asm
if not "%errorlevel%"=="0" goto end
gcc -O3 -o imyplay.exe @DOSFILES.TXT src\imyparse.c
if not "%errorlevel%"=="0" goto end
upx -9 imyplay.exe

rem cd ..

rem set imyver=%imymajor%-%imyminor%-%imysubminor%
set imyver=%imymajor%-%imyminor%

del imy%imyver%\po\*.* imy%imyver%\doc\*.* imy%imyver%\imy\*.* imy%imyver%\*.*
rmdir imy%imyver%\po
rmdir imy%imyver%\doc
rmdir imy%imyver%\imy
rmdir imy%imyver%

mkdir imy%imyver%
mkdir imy%imyver%\doc
copy doc\imelody.txt doc\imyplay.1 doc\imyplay.inf imy%imyver%\doc
mkdir imy%imyver%\imy
copy imy imy%imyver%\imy
mkdir imy%imyver%\po
copy po\imyplay.pot po\pl.gmo imy%imyver%\po
copy imyplay.exe imy%imyver%
sed 's/$//' < AUTHORS  > imy%imyver%\AUTHORS.txt
sed 's/$//' < ChangeLo > imy%imyver%\Changes.txt
sed 's/$//' < COPYING  > imy%imyver%\COPYING.txt
sed 's/$//' < README   > imy%imyver%\README.txt
sed 's/$//' < THANKS   > imy%imyver%\THANKS.txt

SET OLDTZ=%TZ%
SET TZ=CET-1CES
zip -9 -r -T imyplay imy%imyver%
SET TZ=%OLDTZ%
SET OLDTZ=
rem del imy-%imyver%\po\*.* imy-%imyver%\doc\*.* imy-%imyver%\imy\*.* imy-%imyver%\*.*
rem rmdir imy-%imyver%\po
rem rmdir imy-%imyver%\doc
rem rmdir imy-%imyver%\imy
rem rmdir imy-%imyver%

set imyver=

:end
