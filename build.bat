@echo off

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::/MT  - Causes the application to use the multithreaded, static version of the run-time library.Defines _MT
::/MD  - Causes the application to use the multithread-specific and DLL-specific version of the run-time library
::       Defines _MT and _DLL. Applications compiled with this options are statically linked to MSVCRT.lib
::/LD  - Passes the /DLL option to the linker. Links the dll startup code. Implies /MT unless you explicitly specify /MD
::/LDd - Creates a debug DLL. Defines _MT and _DEBUG
::/WX  - Treats all compiler warnings as errors. (TODO(al): use this option)
::/Z7  - Removes vc120.pdb file, and shrinks down the size of the main .pdb file
::/Oi  - Replaces some function calls with intrinsic or otherwise special forms of the function that help your app run faster
::/GR  - Adds code to check object types at runtime. When /GR is on, the compiler defines the _CPPRTTI preprocessor macro.
::       By default /GR is on. /GR- disables run-time type information
::/EHa - /EHa- disable C++ exception handling
::/Fm  - Tells the linker to produce a mapfile containing a list of segments in the order in which they appear in the corresponding 
::       .exe file or DLL
:: -opt:ref - Reduces the size of the .map file
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

::Always perform the full link, no incremental link
::-W4 - temporarily has been removed
set compile_options=-nologo -Od -Z7 -FC -TP -Oi -EHa- -F4294967296 /wd4533 /wd4042 /INCREMENTAL:NO /MT  /wd4996 /wd4201 /wd4100 /wd4505 -GR-
::ole32.lib - Com stuff (don't know whether I have to link with it)
set libs_to_link=user32.lib kernel32.lib opengl32.lib gdi32.lib
set include_dirs=/I ..\platform_abstraction_layer\code\ext\

if not exist ..\..\build ( mkdir ..\..\build )
pushd ..\..\build
:: Build application dll. Note that the order of /link and /DLL flags is important
cl %compile_options% %include_dirs% -Fmapp_code.map ..\platform_abstraction_layer\code\platform.cpp /LD /link /out:app_code.dll opengl32.lib -opt:ref

::should be rewritten in the future!
cl %compile_options% %include_dirs% -Fm ..\platform_abstraction_layer\code\w32_main.cpp /link %libs_to_link% /out:W32Application.exe -opt:ref

popd