CC=g++
CFLAGS=-I.

#server
sns: NetworkManager.cpp server/ServerSocket.cpp server/Session.cpp fs/FileSystem.cpp
	$(CC) -o sns NetworkManager.cpp server/ServerSocket.cpp server/Session.cpp fs/FileSystem.cpp $(CFLAGS)
	
#client
snc: ClientNetwokManager.cpp client/client.cpp fs/FileSystem.cpp
	$(CC) -o snc ClientNetwokManager.cpp client/client.cpp fs/FileSystem.cpp -I.

