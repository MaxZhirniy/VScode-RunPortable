cd "i:/learns/Trash/cpp_projects/VsStart/" && I:/programs/vscodium/mingw64/bin/windres.exe res.rc -o res.o
cd "i:/learns/Trash/cpp_projects/VsStart/" && I:/programs/vscodium/mingw64/bin/g++.exe -static VSstart.cpp I:\learns\Trash\cpp_projects\VsStart\res.o -o out\RunPortable.exe -static -mwindows
del "res.o" -f
