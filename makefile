CC=g++
CFLAGS=-I.

#server
sns: networkManager.cpp server/ServerSocket.cpp server/Session.cpp fs/FileSystem.cpp
	$(CC) -o sns networkManager.cpp server/ServerSocket.cpp server/Session.cpp fs/FileSystem.cpp $(CFLAGS)
	
#client
snc: ClientNetwokManager.cpp client/client.cpp fs/FileSystem.cpp
	$(CC) -o snc ClientNetwokManager.cpp client/client.cpp fs/FileSystem.cpp -I.

