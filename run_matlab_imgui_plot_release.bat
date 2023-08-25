@ECHO ON

set BASEDIR=%~dp0
PUSHD %BASEDIR%

RMDIR /Q /S build

conan install . -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True --output-folder=build --build=missing --profile=default --settings=build_type=Release
cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=./build/generators/conan_toolchain.cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW
cmake --build . --config Release
robocopy ../Test Release TestMatlabImGuiPlot.m /z
cd Realease
matlab -nosplash -noFigureWindows -r "try; cd('Release'); TestMatlabImGuiPlot(); catch; end;"