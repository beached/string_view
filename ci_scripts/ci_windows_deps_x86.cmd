echo "Making build directory"
md build
cd build

REM ##############################
REM Setting VCVars
@call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x86 

REM ##############################
REM Running cmake
cmake -DCMAKE_BUILD_TYPE=%1 -G "Visual Studio 16 2019" -A Win32 -DCMAKE_CXX_COMPILER=cl.exe -DCMAKE_C_COMPILER=cl.exe -DDAW_NUM_RUNS=1 -DDAW_ENABLE_TESTING=ON -DDAW_WERROR=OFF -DCMAKE_CXX_STANDARD=%2 ..

