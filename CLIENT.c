#include <stdio.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/* Checking any error occurred during the program like if 
    some player got disconnected or the game got over. 
    Using the perror and printing the server - other player disconnected msg
    that the game is over.
*/
void check_error(const char *str_msg) {
    perror(str_msg);
    printf("Server is shut down or Other player disconnected.\nGame over.\n");
    exit(0);
}

/*
    Writing an message to server, and storing into the int variable n using 
    write, also checking if the value of int n is less than 0 than printing
    error message of that error occured while writing an string mesaage to server.
*/
void write_tictac_server_msg(int cli_tictac_sockfd, char * str_msg) {
    // printing an string message to check
	printf("%s\n",str_msg);
	int n = write(cli_tictac_sockfd, str_msg, strlen(str_msg));
    // Check if n < 0 then print the error message
    if (n < 0)
        check_error("ERROR writing str_msg to Server socket");
}

/*
    Reciving an message from server, using memset to fill the first bytes of given 
    string message sets pointing the value of first 5 bytes. Also, reading the the string 
    into variable int n up to 4 bytes. Checking if the condition of whether the value of n is less than\
    or the value of n is not 4, then printing an error message. 
*/
void recv_tictac_msg(int sockfiledesc, char * str_msg) {
    // Using memset to sets the pointed values first 5 bytes from str_msg
    memset(str_msg, 0, 5);
    // read the msg up to 4 bytes of data file descriptor
    int n = read(sockfiledesc, str_msg, 4);
    // Check the constraints to give the error msg
    if (n < 0 || n != 4)
     check_error("ERROR reading message from server socket.");
    // In the end sending the message of recievd msg
    printf("Please: %s\n", str_msg);
   }

/*
    Reciving an int from server, which gets the socket file descriptor
*/
int recv_tictac_int(int sockfiledesc) {
    int str_msg = 0;
    // reads the integers bytes from socket file descriptor
    int n = read(sockfiledesc, &str_msg, sizeof(int));
    // Checking the constraint and giving an error of reciving an int from server
    if (n < 0 || n != sizeof(int)) 
        check_error("ERROR reading int from server socket");
    
    // else the revied int will be printed
    printf(" Received int: %d\n", str_msg);
    // return the msg
    return str_msg;
}

void write_tictac_server_int(int sockfiledesc, int str_msg) {
    int n = write(sockfiledesc, &str_msg, sizeof(int));
    if (n < 0)
        check_error("ERROR writing int to server socket");
    printf("Write int into the server: %d\n", str_msg);
    }

/*
    Connecting to the server which takes the hostname of the device and server 
    port number to connect with server. 
*/
int connect_tictac_to_server(char * hostname, int portno) {
    struct sockaddr_in serv_tictac_addr;
    struct hostent *server;
    // Create a new socket file descriptor.
    int sockfiledesc = socket(AF_INET, SOCK_STREAM, 0);
    // Check if the socket file descriptor is correct.
    if (sockfiledesc < 0) 
        check_error("ERROR while opening the socket.");
    // collecting the host information from the specified hostname.
    server = gethostbyname(hostname);
    // Checking if the server exists already or not.and exits with 0
    if (server == NULL) {
        fprintf(stderr,"ERROR, There is not such server(host exists) \n");
        exit(0);
    }
    // Initialize the server address structure.
    memset(&serv_tictac_addr, 0, sizeof(serv_tictac_addr));
    serv_tictac_addr.sin_family = AF_INET;
    // Give the server address into the address structure.
    memmove(server->h_addr, &serv_tictac_addr.sin_addr.s_addr, server->h_length);
    // Assgign the port number of the server address.
    serv_tictac_addr.sin_port = htons(portno); 
    // Connect to the server using the socket file descriptor.
    if (connect(sockfiledesc, (struct sockaddr *) &serv_tictac_addr, sizeof(serv_tictac_addr)) < 0) 
        check_error("ERROR Not successful to connect with server");
    // Print a message to indicate that the client has connected to the server.
    printf("Successfully connected to the server.\n");
    // Return the socket file descriptor.
    return sockfiledesc;
}

//     CSV Sum
int sum_csv(const char *str) {
	int sum = 0;
    // creating an flag
	int flag =0;
    // Pointers
	char *p,*s;
	for (;;) {
		if (p == str)                
			break;
		if (flag==0){
            int n = strtol(str, &p, 10); 
            sum += (n-1)*3;
            str = p + strspn(p, ", ");   
            flag=1;
		}		
		if (flag==1){
		int l = strtol(str, &s, 10); 
		sum += (l-1)%3;
		flag=2;
		}
		if(flag==2) break;
	}
	return sum;
}


/*
    Initialize the tic-tac-toe board witht the each box has its own dimension for each
    row and coln. And printing this board while playing the game, so the user knows of valid move.
*/
void draw_board(char tictac_board[][3]) {
    printf(" %c | %c | %c \n", tictac_board[0][0], tictac_board[0][1], tictac_board[0][2]);
    printf("-----------\n");
    printf(" %c | %c | %c \n", tictac_board[1][0], tictac_board[1][1], tictac_board[1][2]);
    printf("-----------\n");
    printf(" %c | %c | %c \n", tictac_board[2][0], tictac_board[2][1], tictac_board[2][2]);
}

/*
    Taking the turn of the client, MOVE, RSGN, DRAW. If the Clients enter the MVOE then the 
    commands get break  to get the command name and the positon on the board, with their X or O
*/
void client_Turn(int sockfiledesc, int id){
	// creating an variable to storiung the temp data
	char res_str[40]="", lenstr[5]="", temp[35]="";
	int  c=0,  pos, lengthofProtArgs=0, step;
    // checking if the the command containd the "|" 
	char delim[] = "|";
    // storing all the values into the buffer 
	char buffer[50];
    
    while (1) { 
        printf("Enter your command Please ->  ");
	    fgets(buffer, 50, stdin);
		char *ptr = strtok(buffer, delim);
        // Check if the command was MOVE, if it was then the 
        // take the Command and extract the position on the board and update the baord. 
		if(!strcmp(ptr,"MOVE")){	
            strcat(res_str, ptr);
            strcat(res_str, "|");
            while(ptr != NULL)	{
                ptr = strtok(NULL, delim);
                if(c==1)
                    pos	=sum_csv(ptr);
                c++;
                if(ptr != NULL) {
                    lengthofProtArgs+=strlen(ptr);
                    strcat(temp,ptr );
                    strcat(temp,"|");
                }
                sprintf(lenstr,"%d",lengthofProtArgs);
                temp[strlen(temp)-1]='|';
            }
            strcat(res_str,lenstr );
            strcat(res_str,"|" );
            temp[strlen(temp)-1]='\0';
            strcat(res_str,temp );
            step = pos;
            printf("\n step is %d \n", step);
            if (step<=9 && step >=0 ){
            write_tictac_server_msg(sockfiledesc,res_str);
            break;
            }
        }
        // Checking if the Command given by the Client is RSGN then the The clients who 
        // gave the command lose and other playuer wins
        else if(!strcmp(ptr,"RSGN")){
            if(id == 0){
                printf("Player id wins. %d \nGame over.\n", id + 1);
                close(sockfiledesc);
                exit(0);
            } else {
                printf("Player id wins. %d \nGame over.\n", id -1 );
                close(sockfiledesc);
                exit(0);
            }
        }
    }
}

// void client_update(int sockfiledesc, char tictac_board[][3]){
//     int player_id = recv_tictac_int(sockfiledesc);
//     int step = recv_tictac_int(sockfiledesc);
//     tictac_board[step/3][step%3] = player_id ? 'X' : 'O';    
// }

// Updating the client with the recived int value from the socket file descrptor
void client_update(int sockfiledesc, char tictac_board[][3]){
    int p_id = recv_tictac_int(sockfiledesc);
    int step = recv_tictac_int(sockfiledesc);
	if(p_id==0)
    tictac_board[step/3][step%3] = p_id ? 'X' : 'O';    
}

// Main
int main(int argc, char *argv[]) {
    // Checking the args given the client was correct the IP/ICP and the corrct port number
      if (argc < 3) {
       fprintf(stderr,"%s Host Name Port Number\n", argv[0]);
       exit(0);
    }

    // Giveing the IP and port numnber to socket file descrptor
    int sockfiledesc = connect_tictac_to_server(argv[1], atoi(argv[2]));
    // geting the id of the client from the recveid socket file descrptor
    int id = recv_tictac_int(sockfiledesc);
    // printing the cleint ID
    printf("My ID is: %d\n", id);
    char str_msg[5];
    char tictac_board[3][3] = { {' ', ' ', ' '}, 
                         {' ', ' ', ' '}, 
                         {' ', ' ', ' '} };
    printf("------------------------Welcome to my Tic-Tac-Toe-----------------\n");
    // Waiting the until the second player joins and give the command of WAIT to client
	recv_tictac_msg(sockfiledesc, str_msg);
    if (!strcmp(str_msg, "WAIT"))
        printf("Waiting for a second player...\n");
    // After the second client joins the game, the Game BGNS
    printf("The Tic-Tac-Toe Game has Started !\n");
    printf("You Got ->  %c's\n", id ? 'X' : 'O');
    // Printing the Tic-Tac-Toe Board
    draw_board(tictac_board);
    // While
    while(1) {
        recv_tictac_msg(sockfiledesc, str_msg);
        // when the str_msg will be BEGN
        if (!strcmp(str_msg, "BEGN")) { 
	        client_Turn(sockfiledesc, id);
        }
        // when the str_msg will be CNT which will print the number of active player in the game.
        else if (!strcmp(str_msg, "CNT")) { 
            int num_players = recv_tictac_int(sockfiledesc);
            printf("Currently we have %d number of active players.\n", num_players); 
        }
        // when the str_msg will be WAIT it will tell the play to wait until the other player moves
        else if (!strcmp(str_msg, "WAIT")) { 
            printf("Wait for the other player to Move\n");
        }
        // when the str_msg will be UPDT it will says UPDT after the valid move or the baord will update
        else if (!strcmp(str_msg, "UPDT")) { 
            // Cleint will update the board : socket file descrptor
            client_update(sockfiledesc, tictac_board);
            draw_board(tictac_board);
        }
        // when the str_msg will be BEGN it will say the move was invalid by the player so re-enter it
        else if (!strcmp(str_msg, "INVD")) { 
            printf("Your Move was not correct, Please enter again...\n"); 
        }
        // when the str_msg will be WINS when the player will win the game it will prints the win msg to the client
        else if (!strcmp(str_msg, "WINS")) { 
            printf("Congrats! You Have Won the Tic-Tac-Toe Game!\n");
            // after wining the loops breaks
            break;
        }
        // when the str_msg will be LOSE when the player will lose the game it will prints  the lose msg to the client
        else if (!strcmp(str_msg, "LOSE")) { 
            printf("Sadly!, You Have Lose the Tic-Tac-Toe Game!\n");
            // after lsoing the loops breaks
            break;
        }
        // when the str_msg will be DRAW when the game is draw it will prints the DRAW msgs to the clients
        else if (!strcmp(str_msg, "DRAW")) { 
            printf("Ops! Its an DRAW.\n");
            // after draw the loops breaks
            break;
        }
        else 
            check_error("My Error Message");
    }
    
    printf("Game is Now Over!! \n");
    close(sockfiledesc);
    return 0;
} //Main
