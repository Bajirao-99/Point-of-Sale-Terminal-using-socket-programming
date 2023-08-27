#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>
#include <csignal>
#include <set>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "message_format.h"

using namespace std;

int SERVER_FD = -1; // Servre file descriptor
set<int> open_client_sockets; // open client connections 


/**
 * @brief : Client class for handling client
 */
class Client {
    int socketFD; // Socket File descriptor to access server socket
    struct sockaddr_in address; // Client Address structure including port, IPv4 Address;

    public: 

    /**
     * @brief : Construct a new Client object
     * 
     * @param socketFD : client Socket file descriptor 
     * @param address  : address structure of the client
     */
    Client(int socketFD, struct sockaddr_in address) {
        this->socketFD = socketFD;
        this->address = address;
    }
    
    /**
     * @brief : Get the Socket File Descriptor object
     */
    int getSocketFD() {
        return this->socketFD;
    }

    /**
     * @brief : Sending response to client
     * 
     * @param s : message to send
     * @return int : -1 if failed , +ve otherwise 
     */
    int sendResponse(string s) {  
        const char* message = s.c_str(); 
        int result = send(this->socketFD, message, s.length(), 0);
        return result;
    }

    /**
     * @brief : Get the Request object
     * 
     * @return string : recieved message
     */
    string getRequest() {
        char buff[1024];
        int recv_size = recv(this->socketFD, buff, 1024, 0);
        buff[recv_size] = 0;
        if(recv_size == 0) return "";
        string recieved_message(buff);
        return recieved_message;
    }

    /**
     * @brief Destroy the Client object
     */
    ~Client() {
        close(this->socketFD);
        cout << "Connection closed\n";
    }
};


/**
 * @brief : Server class to handle all server functionalities.
 *          Takes port from user and listens for clients.
 */
class Server {
    int socketFD; // Socket File descriptor to access server socket
    int port;     // Port number on which server listens for clients
    struct sockaddr_in * address;   // Server Address structure including port, IPv4 Address;

    public:

    /**
     * @brief : Construct a new Server object
     * 
     * @param port : takes port from user 
     */
    Server(int port) {
        this->port = port;
        
        // Creating new socket of type AF_INET(IPv4) and SOCK_STREAM(TCP socket)
        this->socketFD = socket(AF_INET, SOCK_STREAM, 0);
        
        SERVER_FD = this->socketFD;
        
        // Creating address structure for server
        this->address = new sockaddr_in;
        address->sin_port = htons(port); // setting up the port 
        address->sin_family = AF_INET; // settiung address family as IPv4 
        address->sin_addr.s_addr = INADDR_ANY; // setting IPv4 address of machine

        // Binding server socket to address and port
        int result = bind(this->socketFD,(sockaddr*)this->address, sizeof(struct sockaddr_in));

        if(result == 0)
            printf("socket was bound successfully\n");
    }

    /**
     * @brief : accepts connection on the port
     * 
     * @return : return Client* objects for handling accepted client 
     */
    Client* acceptConnection() {
        struct sockaddr_in  clientAddress ;
        int clientAddressSize = sizeof (struct sockaddr_in);
        
        // Accepts client connections and return socket for accessing client
        int clientSocketFD = accept(this->socketFD, (sockaddr*)&clientAddress, (socklen_t*)&clientAddressSize);
        Client* client = new Client(clientSocketFD, clientAddress);
        
        return client;
    }

    /**
     * @brief : Listens for new client connections
     * 
     * @return (int) : returns int   
     */
    int listenForConnections() {
        // Listen system call
        int listenResult = listen(this->socketFD,10);
        return listenResult;
    }
    
    /**
     * @brief : Destroy the Server object ans shuts doen the server
     */
    ~Server() {
        shutdown(this->socketFD, SHUT_RDWR);
        cout << "Server shutdown\n" << endl;
    }
};


/**
 * @brief : clas to store items object from database
 */
class Item {
    public:

    string upc_code; // upc code of objects
    string item_name; // item name of objects
    double item_price; // item price
    
    /**
     * @brief : Construct a new Item object
     */
    Item() {
        upc_code = "";
        item_name = "";
        item_price = 0;
    }

    /**
     * @brief : Construct a new Item object
     */
    Item(string upc, string in, double ip) {
        upc_code = upc;
        item_name = in;
        item_price = ip;
    }
};


/**
 * @brief : Database class for storing items information
 */
class Database {
    ifstream databaseFile; // database file 
    
    public: 
    
    /**
     * @brief : searches the given upc code
     * @param upc :  takes upc code to search
     * @return Item : returns item found, if not return empty item objects
     */
    Item search_upc_code(string upc) {
    
        databaseFile.open("database.txt");
        string _upc, in, ip;
        // int _upc, ip;
        int flag = 0;
        while(databaseFile) {
            string str, token;
            getline(databaseFile, str);

            stringstream ss(str);
            
            std::getline(ss, token, ',');
            _upc = token;
            // cout << token << endl;
            std::getline(ss, token, ',');
            in = token;
            // cout << token << endl;
            std::getline(ss, token, ',');
            // double pr = stod(token);
            ip = token;

            if(upc == _upc) {
                flag = 1;
                break;
            }
            
        }
        
        databaseFile.close();
        if(flag == 0) {
            Item item;
            return item;
        }
        
        Item item(_upc, in, stod(ip));
        return item;
    }
};

void signal_handler(int sig);
bool is_number(const std::string& s);
void handleClientThread(Client* client, int client_no);

/**
 * @brief : checks if given string is number 
 * @param s : string to check 
 */
bool is_number(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}


/**
 * @brief :  handles client connection, thread is created for each client
 * @param client :  client to handle
 * @param client_no :  client number
 */
void handleClientThread(Client* client, int client_no) {
    cout << "New connection created for client " << client_no << endl;

    double totalCost = 0; // Total cost to return
    
    while(true) {
        // gets the request from client
        string request = client->getRequest();
        
        if(request == "exit" || request == "") break;

        RequestMessageFormat rq; // request object to extract info
        int success = rq.setAttributes(request);

        cout << "\n--------------------- client "<< setw(3) << client_no << " ---------------------\n"; 
        cout << "                                                     |\n";
        // cout << (success? "Success" : "Error") << endl;
        cout << "Decoded string is : " << rq.getRequestFormatString() << endl;
        
        // gets required parameters from request
        string request_type = rq.getAttribute("request_type");
        string upc_code = rq.getAttribute("upc_code");
        string no_of_items = rq.getAttribute("no_of_items");
        
        // Item item = search_upc_code(rq.getAttribute("upc_code"));
        
        int error_type = 0; // 0 - NO Error, 1 - UPC not found, 2 - protocol Error 
        int response_type = 0; // 0 - item found, 1 - total_cost
        
        Database db; // databse object
        Item item; // item that need to find in database

        // checking for errors
        if(success == 0) error_type = 2;
        else if(request_type != "0" && request_type != "1") error_type = 2;
        else if(request_type == "0") {
            item = db.search_upc_code(upc_code);
            if(!is_number(no_of_items) || upc_code.length() != 3) error_type = 2;
            else if(!is_number(no_of_items)) error_type = 2;
            else if(item.item_name == "") error_type = 1;
        }

        // no errors, so define response type
        // else if(request_type == "0") response_type = 0; // Item found
        else if(request_type == "1") response_type = 1; // total cost response
         
        ResponseMessageFormat rsp; // response message object
        
        //send the message
        switch (error_type)
        {
            // no Error
            case 0:
                rsp.addAttribute("response_type", "0"); // OK response
                if(response_type == 0) {
                    cout << "server : Found the item = " << item.upc_code << ", " << item.item_name << ", " << item.item_price << endl << endl;
                    rsp.addAttribute("item_name", item.item_name); // set item name in respone message
                    rsp.addAttribute("item_price", to_string(item.item_price)); // set item_price in respone message
                    totalCost += item.item_price*stoi(no_of_items); // calculates total cost up to this point
                }
                else {
                    cout << "server : Total cost sent" << endl << endl;
                    string total_cost = to_string(totalCost); 
                    rsp.addAttribute("total_cost", total_cost); // sets total cost in respone message
                }
                break;

            // upc not found
            case 1:
                cout << "server : UPC not found" << endl << endl;
                rsp.addAttribute("response_type", "1"); // Error response
                rsp.addAttribute("error_type", "UPC is not found in database!"); // sets in resopnse
                break;

            // Protocol Error
            case 2:
                cout << "server : protocol error" << endl << endl;
                rsp.addAttribute("response_type", "1"); // Error response
                rsp.addAttribute("error_type", "Protocol Error!");
                break;

            default:
                break;
        }

        string response = rsp.getResponseFormatString(); // getting response in string format
        
        // sending response to client
        client->sendResponse(response);

        cout << "Response sent to client : " << response ; 
        cout << "                                                     |\n";
        cout << "------------------------------------------------------\n\n";
    }

    open_client_sockets.erase(client->getSocketFD()); // remove client from open connections
    close(client->getSocketFD()); // closes client connection
    cout << "Client " << client_no << " connection Closed!\n";
}


/**
 * @brief : creates server and accepts connection and creates thread for each client
 */
int main(int argc, char const *argv[]) {

    // signal(SIGABRT, );
    int port = 9001;
    string ip_address;

    // cmd line argument for port
    if(argc == 2) {
        port = stoi(argv[1]);
    }

    // ctrl + c signal handling
    signal(SIGINT,signal_handler);
    
    // Server object
    Server server(port);

    if(server.listenForConnections() < 0) {
        cout << "Listening falied!" << endl;
        exit(0);
    }
    
    cout << "server Listening on port : " << port << endl;
    while(true) {
        // accept new client connection
        Client* client = server.acceptConnection();
        
        // adds client in open client set
        open_client_sockets.insert(client->getSocketFD());

        // creates new thread for accepted client
        thread th(handleClientThread, client, open_client_sockets.size()-1);
        th.detach();
    }
    
    return 0;
}

/**
 * @brief handles ctrl + c signal and close all client connections and server socket 
 */
void signal_handler(int sig) {
	char msg[4098];

    for(int clientSockFD : open_client_sockets) {
        close(clientSockFD);
    }
    
    fputs("\nAll client sockets closed!..\n",stdout);
	
    shutdown(SERVER_FD, SHUT_RDWR);
	fputs("\nServer terminated!..\n",stdout);
	
	exit(0);
}

