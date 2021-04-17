How to build and run
--------------------

1) https://www.msys2.org/ ==> https://repo.msys2.org/distrib/x86_64/msys2-x86_64-20201109.exe
   to be installed

2) Add 'C:\Qt\6.0.0\mingw81_64\bin' to the %PATH%-variable

3) https://stackoverflow.com/questions/23766721/cant-run-c-in-netbeans-no-shell-found-error
   Adjusting the %PATH%-variable - in my case adding C:\msys64\usr\bin

4) NB C++ plug-in for 8.2:
   NB --> Tools --> Plugins --> Settings --> Enable "NetBeans 8.2 Plugin Portal"
   NB --> Tools --> Plugins --> Available Plugins --> C/C++ --> Install 

5) NB --> Tools --> Options --> C/C++ --> Add...

    Base Directory: C:\Qt\Tools\mingw810_64\bin
    Make Command: C:\Qt\Tools\mingw810_64\bin\make.exe (copy-rename mingw32-make.exe)
    QMake Command: C:\Qt\6.0.0\mingw81_64\bin\qmake.exe
    CMake Command: C:\Qt\Tools\CMake_64\bin\cmake.exe

5) Project Properties --> C++ Compiler --> Include Directories --> C:/Qt/5.15.2/mingw81_64/include

6) Project Properties --> Linker -> Add Library File
                                                ----
C:\Qt\6.0.0\mingw81_64\lib\libQt6Core.a
C:\Qt\6.0.0\mingw81_64\lib\libQt6Gui.a
C:\Qt\6.0.0\mingw81_64\lib\libQt6Sql.a
C:\Qt\6.0.0\mingw81_64\lib\libQt6Widgets.a

The directory of executable in Windows 7,10
-------------------------------------------
	
sdm.demo.qt.sqlite3.thesurus.exe
thesaurus.sqlite......from the root of NB project
url.txt...............contains the only string like https://translate.google.com/#en/ru/
