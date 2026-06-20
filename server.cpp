#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include "database.h"

void handleClient(
    int clientSocket,
    Database* db
)
{
    std::cout
        << "Client connected"
        << std::endl;

    const char* message =
        "Welcome to Redis Clone\n";

    send(
        clientSocket,
        message,
        strlen(message),
        0
    );

    char buffer[1024];

    while(true)
    {
        int bytesReceived =
            recv(
                clientSocket,
                buffer,
                sizeof(buffer) - 1,
                0
            );

        if(bytesReceived <= 0)
        {
            break;
        }

        buffer[bytesReceived] =
            '\0';

        std::string line =
            buffer;

        std::stringstream ss(line);

        std::vector<std::string> tokens;

        std::string token;

        while(ss >> token)
        {
            tokens.push_back(token);
        }

        std::string response;

        if(tokens.empty())
        {
            response =
                "Invalid command\n";
        }

        else if(
            tokens.size() == 5 &&
            tokens[0] == "SET" &&
            tokens[3] == "EX"
        )
        {
            int seconds =
                std::stoi(tokens[4]);

            db->setWithExpiry(
                tokens[1],
                tokens[2],
                seconds
            );

            db->saveToDisk();

            response =
                "OK\n";
        }

        else if(
            tokens.size() == 3 &&
            tokens[0] == "SET"
        )
        {
            db->set(
                tokens[1],
                tokens[2]
            );

            db->saveToDisk();

            response =
                "OK\n";
        }

        else if(
            tokens.size() == 2 &&
            tokens[0] == "GET"
        )
        {
            response =
                db->get(
                    tokens[1]
                );

            response += "\n";
        }

        else if(
            tokens.size() == 2 &&
            tokens[0] == "DEL"
        )
        {
            db->del(
                tokens[1]
            );

            db->saveToDisk();

            response =
                "OK\n";
        }

        else if(
            tokens.size() == 2 &&
            tokens[0] == "EXISTS"
        )
        {
            response =
                std::to_string(
                    db->exists(
                        tokens[1]
                    )
                );

            response += "\n";
        }

        else if(
            tokens.size() == 1 &&
            tokens[0] == "SIZE"
        )
        {
            response =
                std::to_string(
                    db->size()
                );

            response += "\n";
        }

        else
        {
            response =
                "Unknown command\n";
        }

        send(
            clientSocket,
            response.c_str(),
            response.size(),
            0
        );
    }

    close(clientSocket);

    std::cout
        << "Client disconnected"
        << std::endl;
}

int main()
{
    Database db;
    db.loadFromDisk();
    std::thread ttlThread(
        &Database::cleanupExpiredKeys,
        &db
    );
    ttlThread.detach();
    int serverSocket =
        socket(
            AF_INET,
            SOCK_STREAM,
            0
        );

    if(serverSocket < 0)
    {
        std::cout
            << "Socket creation failed"
            << std::endl;

        return 1;
    }

    int opt = 1;

    setsockopt(
        serverSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)
    );

    sockaddr_in serverAddr;

    serverAddr.sin_family =
        AF_INET;

    serverAddr.sin_addr.s_addr =
        INADDR_ANY;

    serverAddr.sin_port =
        htons(6379);

    if(
        bind(
            serverSocket,
            (sockaddr*)&serverAddr,
            sizeof(serverAddr)
        ) < 0
    )
    {
        std::cout
            << "Bind failed"
            << std::endl;

        return 1;
    }

    if(
        listen(
            serverSocket,
            5
        ) < 0
    )
    {
        std::cout
            << "Listen failed"
            << std::endl;

            return 1;
    }

    std::cout
        << "Listening on port 6379"
        << std::endl;
    while(true){
        sockaddr_in clientAddr;

        socklen_t clientSize =
            sizeof(clientAddr);

        int clientSocket =
        accept(
            serverSocket,
            (sockaddr*)&clientAddr,
            &clientSize
        );

    if(clientSocket < 0)
    {
        std::cout
            << "Accept failed"
            << std::endl;

        continue;
    }

    std::thread(
        handleClient,
        clientSocket,
        &db
    ).detach();
    }
    close(serverSocket);

    return 0;
}