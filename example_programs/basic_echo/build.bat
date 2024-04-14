@echo off

pushd %CD%
IF NOT EXIST ../../build mkdir ../../build
cd ../../build

set CF_DEFAULT= -MD -nologo -Gm- -GR- -EHa- -Od -Oi -FC -Z7 -W3 -EHsc -I./../
set LF_DEFAULT= -incremental:no -opt:ref -subsystem:console

cl %CF_DEFAULT% -DOS_WINDOWS ../example_programs/basic_echo/echo_client.cpp /link %LF_DEFAULT%
cl %CF_DEFAULT% -DOS_WINDOWS ../example_programs/basic_echo/echo_server.cpp /link %LF_DEFAULT%

cd ../example_programs/basic_echo
