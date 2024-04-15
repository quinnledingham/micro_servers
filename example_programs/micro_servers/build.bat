@echo off

pushd %CD%
IF NOT EXIST ../../build mkdir ../../build
cd ../../build

set CF_DEFAULT= -MD -nologo -Gm- -GR- -EHa- -Od -Oi -FC -Z7 -W3 -EHsc -I./../ -D_CRT_SECURE_NO_WARNINGS 
set LF_DEFAULT= -incremental:no -opt:ref -subsystem:console

cl %CF_DEFAULT% -DOS_WINDOWS ../example_programs/micro_servers/client.cpp             /link %LF_DEFAULT% /out:ms_client.exe 
cl %CF_DEFAULT% -DOS_WINDOWS ../example_programs/micro_servers/convertor.cpp          /link %LF_DEFAULT% /out:ms_convertor.exe 
cl %CF_DEFAULT% -DOS_WINDOWS ../example_programs/micro_servers/indirection_server.cpp /link %LF_DEFAULT% /out:ms_indirection_server.exe 
cl %CF_DEFAULT% -DOS_WINDOWS ../example_programs/micro_servers/translator.cpp         /link %LF_DEFAULT% /out:ms_translator.exe 
cl %CF_DEFAULT% -DOS_WINDOWS ../example_programs/micro_servers/voting.cpp             /link %LF_DEFAULT% /out:ms_voting.exe 

cd ../example_programs/micro_servers
