@echo off

set CF_DEFAULT= -MD -nologo -Gm- -GR- -EHa- -Od -Oi -FC -Z7 -W3 -EHsc -I./../../
set LF_DEFAULT= -incremental:no -opt:ref -subsystem:console

cl %CF_DEFAULT% -DOS_WINDOWS echo_client.cpp -o ../../build/echo_client /link %LF_DEFAULT%
cl %CF_DEFAULT% -DOS_WINDOWS echo_server.cpp -o ../../build/echo_server /link %LF_DEFAULT%
