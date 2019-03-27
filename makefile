CC = g++
SERVER_FOLDER = ./server
CLIENT_FOLDER = ./client

JSON_LINK_FILE = -l jsoncpp
CFLAGS = -Wall -ansi 
CONFIG_FILE = config.h
# Wall: Show warning messages
# ansi: Enhance portability

#all:
#	g++ -o $(SERVER_FOLDER)/server $(SERVER_FOLDER)/server.cpp -l jsoncpp -o $(CLIENT_FOLDER)/client $(CLIENT_FOLDER)/client.cpp -l jsoncpp

all: $(SERVER_FOLDER)/server $(CLIENT_FOLDER)/client

$(SERVER_FOLDER)/server: $(SERVER_FOLDER)/server.cpp $(CONFIG_FILE)
	$(CC) -o $(SERVER_FOLDER)/server $(SERVER_FOLDER)/server.cpp $(JSON_LINK_FILE)
$(CLIENT_FOLDER)/client: $(CLIENT_FOLDER)/client.cpp $(CONFIG_FILE)
	$(CC) -o $(CLIENT_FOLDER)/client $(CLIENT_FOLDER)/client.cpp $(JSON_LINK_FILE)

clean:
	rm $(SERVER_FOLDER)/server $(CLIENT_FOLDER)/client
