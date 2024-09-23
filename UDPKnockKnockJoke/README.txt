This package contains a simple UDP server/client program.

After unzipping the package into a folder (directory), 
you should see these files:

	AddressUtility.c  Practical.h       server-udp.c
	DieWithMessage.c  README.txt        
	Makefile          client-udp.c      

1. Compile the code

Open a terminal window, change working folder (directory) to 
where the code is.

You can compile the package manually by typing:

	gcc -Wall -o udp-server server-udp.c AddressUtility.c DieWithMessage.c
	gcc -Wall -o udp-client client-udp.c AddressUtility.c DieWithMessage.c

But a more standard way is to use the make utility:

	make server
	make client

You should take a peek at the Makefile.

What the two commands do is to create two programs:

	udp-server
	udp-client

2. Run the code

Ideally you run the server on one machine and run the client 
on another machine.  However you can run both the server and
the client on the same machine as well (thanks to multitasking).

2.1 On two different machines

On one machine, open a terminal window and change working 
folder (directory) to where the code is.

Run the server such as:

	./udp-server 6789

This starts the server on local machine at port 6789.

On the other machine, open a terminal window and change working 
folder (directory) to where the code is.

Run the client such as:

	./udp-client <server's ip address> 6789

This starts the client to call the remote server at port 6789.

Follow the prompt and type a message to be sent to the server.

2.2 On the same machine

Open two terminal windows and change working folders
to where the code is.

Run the server in one terminal window such as:

	./udp-server 6789

This starts the server on local machine at port 6789.

Run the client in the other terminal window such as:

	./udp-client localhost 6789

This starts the client to call the server running on local
machine at port 6789. The "localhost" is a special name 
that refers to the local machine.

Follow the prompt and type a message to be sent to the server.
