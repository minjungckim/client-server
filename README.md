# client-server
# Simple Client-Server program

Server receives 2 user-specified arguments (IP port and repetition of sending a set of data buffers)
It will declare a sockaddr_in structure, open a TCP socket, bind the socket to its local address, and then listen for clients.
Once it establishes a connection with a client, it will read data sent from client and call write to send
back the number of reads as a response. 
It will close the socket once completed.

Client will be given 6 arguments (IP port, repetition of sending a set of data buffers, number of data buffers, 
buffer size in bytes, a server IP name, and the transfer type scenario ranging from 1 - 3**)
It will receive the IP port and IP name from the client, open a socket, connect to the server, write data to it, 
receive the number of reads server has done, and then close the socket.

** Transfer Scenarios
1 = represents multiple writes
2 = writev
3 = single write
