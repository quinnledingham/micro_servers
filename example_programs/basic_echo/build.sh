# gcc src/client.c src/buffer.c src/socketM.c  -o build/client
# gcc src/indirectionServer.c src/buffer.c src/socketM.c -o build/indirectionServer
# gcc src/translator.c src/buffer.c src/socketM.c -o build/translator
# gcc src/convertor.c src/buffer.c src/socketM.c -o build/convertor
# gcc src/voting.c src/buffer.c src/socketM.c -o build/voting
# gcc src/driver.c -o build/driver

gcc echo_client.c -o ../../build/echo_client
gcc echo_server.c -o ../../build/echo_server