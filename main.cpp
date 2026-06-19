#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "database.h"

using namespace std;

int main(){

    Database db(3);

    thread ttlThread(
    &Database::cleanupExpiredKeys,
    &db
    );

    ttlThread.detach();

    cout << "Redis Clone Started\n";
    cout << "Commands:\n";
    cout << "SET key value\n";
    cout << "SET key value EX seconds\n";
    cout << "GET key\n";
    cout << "DEL key\n";
    cout << "EXISTS key\n";
    cout << "SIZE\n";
    cout << "EXIT\n\n";

    while(true){

        cout << "redis> ";

        string line;

        getline(cin, line);

        if(line.empty()){
            continue;
        }

        vector<string> tokens;

        stringstream ss(line);

        string token;

        while(ss >> token){
            tokens.push_back(token);
        }

        if(tokens.empty()){
            continue;
        }

        if(tokens[0] == "EXIT"){

            cout << "Shutting down...\n";

            break;
        }

        else if(tokens[0] == "SET"){

            if(tokens.size() == 3){

                db.set(
                    tokens[1],
                    tokens[2]
                );

                cout << "OK\n";
            }

            else if(
                tokens.size() == 5 &&
                tokens[3] == "EX"
            ){

                int seconds =
                    stoi(tokens[4]);

                db.setWithExpiry(
                    tokens[1],
                    tokens[2],
                    seconds
                );

                cout << "OK\n";
            }

            else{

                cout
                << "Usage:\n"
                << "SET key value\n"
                << "SET key value EX seconds\n";
            }
        }

        else if(tokens[0] == "GET"){

            if(tokens.size() != 2){

                cout << "Usage: GET key\n";

                continue;
            }

            cout
            << db.get(tokens[1])
            << endl;
        }

        else if(tokens[0] == "DEL"){

            if(tokens.size() != 2){

                cout << "Usage: DEL key\n";

                continue;
            }

            db.del(tokens[1]);

            cout << "OK\n";
        }

        else if(tokens[0] == "EXISTS"){

            if(tokens.size() != 2){

                cout
                << "Usage: EXISTS key\n";

                continue;
            }

            cout
            << db.exists(tokens[1])
            << endl;
        }

        else if(tokens[0] == "SIZE"){

            cout
            << db.size()
            << endl;
        }

        else{

            cout
            << "Unknown command\n";
        }
    }

    return 0;
}