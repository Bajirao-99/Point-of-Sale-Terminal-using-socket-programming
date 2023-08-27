Group_6:

TOPIC: Application #1: Point-of-Sale Terminal using socket programming

    ---> Using socket programming we are implemented a simple client and server that communicate over the network and
         implement a simple application involving Cash Registers. 

    ---> client.cpp:
                    The client implements a simple cash register that opens a session with the server and then supplies 
                    a sequence of codes (refer request-response messages format) for some products. 
                    
    ---> server.cpp:
                    The server returns the price of each one, if the product is available,
                    and also keeps a running total of purchases for each clients transactions.
                    
    ---> When the client closes the session, the server returns the total cost.
    ---> database.txt: a TXT file as a database to store the UPC code and item description at the server end.
    
    ---> message_format.cpp:
                           ------------------------------------
                           | Request_Type | UPC-Code | Number |
                           ------------------------------------
How to run:

server : g++ -pthread server.cpp message_format.cpp -o server && ./server

client: g++ client.cpp message_format.cpp -o client && ./client


