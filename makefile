CC=g++
CFLAGS=-I.

#server
sns: NetworkManager.o server/ServerSocket.o server/Session.o fs/FileSystem.o
	$(CC) -o sns NetworkManager.o server/ServerSocket.o server/Session.o fs/FileSystem.o $(CFLAGS)
	
#client
snc: ClientNetwokManager.o client/client.o fs/FileSystem.o
	$(CC) -o snc ClientNetwokManager.o client/client.o fs/FileSystem.o $(CFLAGS)
