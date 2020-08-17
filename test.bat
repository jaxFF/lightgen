@echo off

IF NOT EXIST build/lightgen.exe call build.bat
pushd build
lightgen.exe ../data/test
popd