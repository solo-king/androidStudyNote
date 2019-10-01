@echo off
cls
dir 

echo [errorlevel]%errorlevel%

if  %errorlevel%==0 (
    echo ok!!!
) else (
    echo error!!!
)
