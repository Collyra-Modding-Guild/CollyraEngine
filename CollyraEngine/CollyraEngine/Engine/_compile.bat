call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
MSBuild "..\..\CollyraGameSystem\CollyraGameSystem.vcxproj" /t:Build /p:Configuration=Debug  /p:Platform=Win32 /p:OutDir=C:\temp\CollyraEngine\ >> _compilation_output.txt
