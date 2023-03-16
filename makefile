EXECUTABLE=tftp getudp
WEXECUTABLE=wclient.exe wserver.exe tftp_app.exe

./bin/lib_format.o: ./include/lib_format.c
	gcc ./include/lib_format.c -c -o ./bin/lib_format.o

./bin/lib_client.o: ./include/lib_client.c
	gcc ./include/lib_client.c -c -o ./bin/lib_client.o -lwsock32

wclient: ./bin/lib_client.o ./bin/lib_format.o ./bin/lib_wudp.o
	gcc ./src/wclient.c ./bin/lib_client.o ./bin/lib_format.o ./bin/lib_wudp.o -o wclient.exe -lwsock32

./bin/lib_wudp.o: ./include/lib_wudp.c ./bin/lib_format.o
	gcc ./include/lib_wudp.c -c -o ./bin/lib_wudp.o -lwsock32

./bin/lib_server.o: ./include/lib_server.c
	gcc ./include/lib_server.c -c -o ./bin/lib_server.o -lwsock32

wserver: ./bin/lib_server.o ./bin/lib_format.o ./bin/lib_wudp.o
	gcc ./src/wserver.c ./bin/lib_server.o ./bin/lib_format.o ./bin/lib_wudp.o -o wserver.exe -lwsock32

./bin/lib_app_tftp.o: ./include/lib_app_tftp.c
	gcc ./include/lib_app_tftp.c -c -o ./bin/lib_app_tftp.o -lws2_32
	
tftp_app: ./bin/lib_app_tftp.o 
	gcc ./bin/lib_app_tftp.o ./src/tftp_app.c -o tftp_app.exe -L./include/ -lws2_32

clean:
	del  .\bin\*.o $(W_EXECUTABLE)