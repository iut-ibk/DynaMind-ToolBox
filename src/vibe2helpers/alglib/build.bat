@echo off
IF NOT "%~1"=="" GOTO lbl_1_end
type _internal\build-bat-help
EXIT /B 1
:lbl_1_end
IF "%~3"=="" GOTO lbl_2_end
echo Too many parameters specified
echo Did you enclose parameters to be passed to the compiler in double quotes?
EXIT /B 1
:lbl_2_end
CHDIR out
IF EXIST * DEL /F /Q *
CHDIR ..\_tmp
IF EXIST * DEL /F /Q *
CHDIR ..
COPY src\* _tmp > NUL 2> NUL
IF "%~1"=="msvc" GOTO lbl_3_msvc
IF "%~1"=="mingw" GOTO lbl_3_mingw
GOTO lbl_3___error
:lbl_3_msvc
CHDIR _tmp
cl /nologo /EHsc /I. /c %~2 *.cpp  >> ../log.txt 2>&1
IF NOT ERRORLEVEL 1 GOTO lbl_4
echo Error while compiling (see ../log.txt for more info)
CHDIR ..
EXIT /B 1
:lbl_4
lib /out:libalglib.lib *.obj >> ../log.txt 2>> ../log.txt 2>&1
IF NOT ERRORLEVEL 1 GOTO lbl_5
echo Error while running LIB (see ../log.txt for more info)
CHDIR ..
EXIT /B 1
:lbl_5
CHDIR ..
COPY _tmp\libalglib.lib out > NUL 2> NUL
COPY src\*.h out > NUL 2> NUL
CHDIR _tmp
IF EXIST * DEL /F /Q *
CHDIR ..
GOTO lbl_3___end
:lbl_3_mingw
CHDIR _tmp
g++ -I. -c %~2 *.cpp  >> ../log.txt 2>&1
IF NOT ERRORLEVEL 1 GOTO lbl_6
echo Error while compiling (see ../log.txt for more info)
CHDIR ..
EXIT /B 1
:lbl_6
ar -rc libalglib.a *.o >> ../log.txt 2>&1
IF NOT ERRORLEVEL 1 GOTO lbl_7
echo Error while running ar (see ../log.txt for more info)
CHDIR ..
EXIT /B 1
:lbl_7
CHDIR ..
COPY _tmp\libalglib.a out > NUL 2> NUL
COPY src\*.h out > NUL 2> NUL
CHDIR _tmp
IF EXIST * DEL /F /Q *
CHDIR ..
GOTO lbl_3___end
:lbl_3___error
echo unknown compiler
EXIT /B 1
:lbl_3___end
