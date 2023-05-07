Project-3 Tic-Tac-Toe Game:

This Project implements the socket program in C where the server has been initialize and mulitple client can join the server and play the game. 

->  Our Tic-Tac-Toe Game Implementation <-

1) Obtain the ip address of your machine 
	Substitute XXX.XXX.XXX.XXX with the IP address

2) Open the terminal
	Navigate to the folder p3 which contains the files SERVER.c and CLIENT.c
	Run the command
	$ make all
	Run the command 
	$ ./ttts [Portno]

3) Open a new terminal
	Navigate to the folder p3 which contains the files SERVER.c and CLIENT.c
	Run the command
	$  ./ttt XXX.XXX.XXX.XXX [Portno]

4) Open a new terminal
	Navigate to the folder p3 which contains the files SERVER.c and CLIENT.c
	Run the command
	$  ./ttt XXX.XXX.XXX.XXX [Portno]

Note: 
Three terminals will be open

*** Usage:
-> "Enter the command" active in MOVE|o|1,1|
->  MOVE|o|1,1| terminal will update
-> Switch between the terminal for other client
-> "Enter the command" active MOVE|o|2,2|
-> MOVE|o|2,2| terminal will update
-> Switch between terminals for each move according to the steps mentioned above.
-> If the player wants to Resgin the game "Enter the command" active in RSGN||
-> else continue with the game
-> Program will Stop when the program declares the winner, the loser, or its an draw.

Finally ->  Run the command
$ make clean

