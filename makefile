WEXECUTABLE=tftp_app.exe

./bin/lib_app_tftp.o: ./include/lib_app_tftp.c
	gcc ./include/lib_app_tftp.c -c -o ./bin/lib_app_tftp.o -lws2_32
	
tftp_app: ./bin/lib_app_tftp.o 
	gcc ./bin/lib_app_tftp.o ./src/tftp_app.c -o tftp_app.exe -L./include/ -lws2_32

clean:
	del  .\bin\*.o $(W_EXECUTABLE)