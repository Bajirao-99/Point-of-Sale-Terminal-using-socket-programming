#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "message_format.h"

using namespace std;

/**
 * @brief : Client class for handling send and recv messages
 */
class Client {
    int socketFD; // client socket connection
    struct sockaddr_in * address; //address of client

    public:

    /**
     * @brief : Construct a new Client object
     */
    Client() {
        this->socketFD = socket(AF_INET, SOCK_STREAM, 0);
        this->address = new sockaddr_in;
    }

    /**
     * @brief : connect to given ip address and port
     * @param ip : ip address to connect
     * @param port : port to connect
     * @return int : -1 for fail, +ve otherwise
     */
    int connectTo(string ip, int port) {
        address->sin_family = AF_INET; // IPv4
        address->sin_port = htons(port); // port
        
        const char* ipAddr = ip.c_str(); 
        inet_pton(AF_INET, ipAddr, &address->sin_addr.s_addr); // setting address from string ipAddr

        // connects with server 
        int result = connect(this->socketFD, (sockaddr*)this->address, sizeof(struct sockaddr_in));

        if(result == 0) {
            cout << "Connection was succcessful!" << endl;
        }
        
        return result;
    }    

    /**
     * @brief : sends request to server
     * 
     * @param s  : messasge to send
     * @return int : -1 for fail, +ve otherwise
     */
    int sendRequest(string s) {  
        const char* message = s.c_str(); 
        int result = send(this->socketFD, message, s.length(), 0);
        return result;
    }

    /**
     * @brief : Get the Response object in string format
     * @return : string 
     */
    string getResponse() {
        char buff[1024];
        int recv_size = recv(this->socketFD, buff, 1024, 0);
        if(recv_size < 0) return "exit";
        buff[recv_size] = 0;
        string recieved_message(buff);
        return recieved_message;
    }

    /**
     * @brief : Destroy the Client object
     */
    ~Client() {
        close(this->socketFD);
        cout << "Connection Closed\n";
    }

};


/**
 * @brief main functions that gets input from cmd line and send to server and recieve response
 */
int main(int argc, char const *argv[]) {

    string ip_address = "127.0.0.1";
    int port = 9001;
    
    // cmd line args
    if(argc == 3) {
        ip_address.assign(argv[1]);
        port = stoi(argv[2]);
    }

    Client client;
    
    // connecting to server
    if(client.connectTo(ip_address, port) < 0) {
        cout << "Error connecting to server\n";
        exit(0);
    }
    string str;
    int sent = -1;
    while(true){
        // getline(cin, str);
        RequestMessageFormat rq; // request format
        cout << "\n------------------------------------------------------\n"; 
        cout << "                                                     |\n";
        cout << "Enter the request type (0/1) : ";
        cin >> str;
        rq.addAttribute("request_type", str); // setting the request type
        // cout << endl;
        if(str == "1") {  
            rq.addAttribute("request_type", "1"); 

            // sending close request to server
            sent = client.sendRequest(rq.getRequestFormatString());
            if(sent < 0) {
                cout << "Server is disconnected, Connection Closed!" << endl;
            }
            break;
        }
        // gets the upc code
        cout << "Enter the upc_code : ";
        cin >> str;
        rq.addAttribute("upc_code", str);
        
        // gets no of items to purchase
        cout << "Enter the no of items to purchase : ";
        cin >> str;
        rq.addAttribute("no_of_items", str);
        cout << endl;
        
        // sending item request 
        sent = client.sendRequest(rq.getRequestFormatString());
        if(sent < 0) {
            cout << "Server is disconnected, cannot send the message!" << endl;
            break;
        }

        cout << "request send : " << rq.getRequestFormatString() << "\n";
        
        // recv message from server
        string response = client.getResponse();
        if(response == "exit" || response == "") {
            sent = -1; 
            cout << "Server is disconnected, cannot send the message!" << endl;
            break;
        }
        cout << "response from server : \n" << response << endl;
        cout << "                                                     |\n";
        cout << "------------------------------------------------------\n\n";
    };

    // checking for close request response for total cost
    if(sent >= 0) {
        string response = client.getResponse();
        cout << "\nresponse from server : " << response;
        cout << "                                                     |\n";
        cout << "------------------------------------------------------\n\n";
    }
    return 0;
}

