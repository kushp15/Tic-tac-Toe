#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

int client_counts = 0;
pthread_mutex_t ptm_count;

/* Checking any error occurred during the program like if 
    some player got disconnected or the game got over. 
    Using the perror to show the string message error as well as
    pthread_exits with null. 
*/
void check_error(const char *str_msg) {
    perror(str_msg);
    pthread_exit(NULL);
}

// CSV Sum
int sum_csv(const char *str) {
	int sum = 0;
	int flag =0;
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
    Recving or reading an message from server socket file descriptor. Using the if
    statement checks for the error in reading or reciving msg from the server socket
    calling the check_error method. else there will be no error than printing the recvied message.  
*/

void recv_tictac_server_msg(int sockfiledesc, char * str_msg) {
    // memeset for the str_msg for the first 15 bytes
    memset(str_msg, 0, 15);
    // read the data from the socket file descriptor
    int n = read(sockfiledesc, str_msg, 14);
    // Check the constraints and giving the error msg.
    if (n < 0 || n != 14)
        check_error("ERROR reading message from server socket.");
    // else printing the recived msg
    printf("Please: %s\n", str_msg);
   }

/*
    This methods takes the additonal leng input into the function
    Having an same reading an string message with the leng from the socket file descriptor
    and checking the error message with the if statement and call the check error method. 
    If there is no error then prints the recived messsage. 
*/
void recv_tictac_server_msg_length(int sockfiledesc, char * str_msg, int leng) {
    // memeset for the str_msg taking the given leng+1 bytes 
 	memset(str_msg, 0, leng+1);
	// reading the data from the socket file descriptor
    int n = read(sockfiledesc, str_msg, leng);
    // Check the constraint and giving the error msg.
    if (n < 0 || n != leng)
        check_error("ERROR reading message from server socket.");
    // else printing the recived msg
    printf("Please: %s\n", str_msg);
   }

// sum of the tic tac toe game csv
int sum_tictac_csv(const char *str) {
    int sum = 0;
    for (;;) {
        char *p;
        int n = strtol(str, &p, 10); 
        if (p == str)                
            break;
        sum += n;                    
        str = p + strspn(p, ", ");   
    }
    return sum;
}
/*
    Writing to client int from the client socket fd, and checking the if the 
    the in variable n is less than 0 than calling the check error method of writng 
    error in int. 
*/
void write_tictac_client_int(int cli_tictac_sockfd, int str_msg){
    // Use the write
    int n = write(cli_tictac_sockfd, &str_msg, sizeof(int));
    // Checking the constraint whethere n is less than 0 and give error
    if (n < 0)
        check_error("ERROR in wrting int to socket (client) ");
}
/*
    Writing int to clients in that game  the msg to that client socket fd
    so taking the index of 0 and 1 of the client scoket file descriptor
*/
void write_tictac_clients_int(int * cli_tictac_sockfd, int str_msg){
    write_tictac_client_int(cli_tictac_sockfd[0], str_msg);
    write_tictac_client_int(cli_tictac_sockfd[1], str_msg);
}
/*
    Writing to client msg from the client socket fd, and checking the if the 
    the in variable n is less than 0 than calling the check error method of writng 
    error in msg. 
*/
void write_tictac_client_msg(int cli_tictac_sockfd, char * str_msg) {
    // Use the write
    int n = write(cli_tictac_sockfd, str_msg, strlen(str_msg));
    // Checking the constraint whethere n is less than 0 and give error
    if (n < 0)
        check_error("ERROR in writing str_msg to socket (client) ");
}
/*
    Writing msg to clients in that game  the msg to that client socket fd
    so taking the index of 0 and 1 of the client scoket file descriptor
*/
void write_tictac_clients_msg(int * cli_tictac_sockfd, char * str_msg)
{
    write_tictac_client_msg(cli_tictac_sockfd[0], str_msg);
    write_tictac_client_msg(cli_tictac_sockfd[1], str_msg);
}

/*
    This method sets up a listener socket using TCP/IP protocol on the local machine at 
    the provided / given port number by the client. It then binds the listener socket to the server address 
    and returns the file descriptor for the listener socket.
*/
int setup_tictac_listener(int portno) {
    // Declare a file descriptor for the listener socket
    int sockfiledesc;
    // Declare a struct to hold the server address (ip and port number)
    struct sockaddr_in serv_tictac_addr;

    // Create a new listener socket using TCP/IP protocol
    sockfiledesc = socket(AF_INET, SOCK_STREAM, 0);
    // Check for errors when opening the listener socket
    if (sockfiledesc < 0) 
        check_error("ERROR opening listener socket.");
    
    // Set all bytes of the server address struct to 0
    memset(&serv_tictac_addr, 0, sizeof(serv_tictac_addr));
    // Set the address family to IP
    serv_tictac_addr.sin_family = AF_INET;	
    // Set the IP address to the local machine
    serv_tictac_addr.sin_addr.s_addr = INADDR_ANY;	
    // Set the port number to the provided port number, converting to network byte order
    serv_tictac_addr.sin_port = htons(portno);		

    // Bind the listener socket to the server address, checking for errors
    if (bind(sockfiledesc, (struct sockaddr *) &serv_tictac_addr, sizeof(serv_tictac_addr)) < 0)
        check_error("ERROR Listner has not successfuly setup.");

    printf("Tic-Tac-Toe Game Listner has been successfully setup:\n");    
    // retunr the socket file descriptor
    return sockfiledesc;
}

int recv_tictac_int(int cli_tictac_sockfd) {
    int str_msg = 0;
    int n = read(cli_tictac_sockfd, &str_msg, sizeof(int));
    if (n < 0 || n != sizeof(int))  return -1;
    printf(" Received the int data: %d\n", str_msg);
    return str_msg;
}

/*
    Initiating the clients, checking connectiona and accepting the connections of the clients, if they are successful 
    than printed with the succefull msg or else they get declined that connection is not succefull. 
*/
void get_tictac_clients(int lis_tictac_sockfd, int * cli_tictac_sockfd) {
    // Creating an clilen it will use to store the leng of client socket addr
    socklen_t clilen;
    // creating an struct that will hold or take client socket addr
    struct sockaddr_in  cli_tictac_addr;
    // printing that the server is waiting for the clients to join
    printf(" Server is waiting for clients...\n");
    int num_tictac_conn = 0;
    // for the 2 clients using the while loops
    while(num_tictac_conn < 2) {
        // Max of 253 clients allowed subtracting the currently have, and listing all the connections get from the clients
	    listen(lis_tictac_sockfd, 253 - client_counts);
        // initialze the clinet addr to 0 and setup the clilen to client addr
        memset(&cli_tictac_addr, 0, sizeof(cli_tictac_addr));
        clilen = sizeof(cli_tictac_addr);
        // Accepting an incoming connections
        cli_tictac_sockfd[num_tictac_conn] = accept(lis_tictac_sockfd, (struct sockaddr *) &cli_tictac_addr, &clilen);
        // if the scoket file decrptor is less than 0 then there was error in accepting coonnecting.
        if (cli_tictac_sockfd[num_tictac_conn] < 0)
            check_error("ERROR !! Accepting Connections.");
        // esle printing the accepted connections. 
        printf(" Connections is Acceptied  %d\n", num_tictac_conn);  
        write(cli_tictac_sockfd[num_tictac_conn], &num_tictac_conn, sizeof(int));
        // replace above function with write message BEGN
        printf(" Sent  %d  to client.\n", num_tictac_conn); 
        pthread_mutex_lock(&ptm_count);
        // incremane the client counts to print the clients in the game
        client_counts++;
        printf("There are now this number of players in game are %d.\n", client_counts);
        pthread_mutex_unlock(&ptm_count);
        // If the server will sends the wait msg to clients until the they get the second cleints for the game.
        if (num_tictac_conn == 0) {
            write_tictac_client_msg(cli_tictac_sockfd[0],"WAIT");
            printf("  client is waiting.\n");      
        }
        num_tictac_conn++;
    }
}

int get_tictac_player_move(int cli_tictac_sockfd) {
    //declare a character array of 50 size to storePlease
	char str_msg[50];
    //declare variables c and pos of type integer
	int c=0, pos;
    //declare a character array delim to store delimiter
	char delim[] = "|";
    //print message "Getting player step..." to console
    printf(" Setting up the PLayers ......\n");
    //write message "BEGN" to client through the socket
    write_tictac_client_msg(cli_tictac_sockfd, "BEGN");
	recv_tictac_server_msg(cli_tictac_sockfd,  str_msg);
	char *ptr = strtok(str_msg, delim);
        //compare ptr with "MOVE"
		if(!strcmp(ptr,"MOVE")){	
            while(ptr != NULL)	{
                ptr = strtok(NULL, delim);
                if(c==2) {
                    pos	=sum_csv(ptr);
                }			
                    c++;
            }
		}
    int step = pos;
	printf("Got this step : %d\n", step);	
	return step;
}

/*
    Checking the Move was valid or not. If the move was not valid then asking the client to enter the 
    thier command again, printing th error message of the move was invalid.
*/
int check_tictac_move(char tictac_board[][3], int step, int player_tictac_id) {
    // If the player has the valid step then print the valid step and returns with 1
    if ((step == 9) || (tictac_board[step/3][step%3] == ' ')) { 
        printf("Player with %d's had valid step .\n", player_tictac_id);
        return 1;
   }
   // If the player has the unvalid step then print the valid step and returns with 0
   else {       
       printf("Player with %d's had unvalid step .\n", player_tictac_id);
       return 0;
   }
}

/*
    Updating the Tic-Tac-Toe game baord witht the player move. If the player moves then the both clients 
    gets the updates the board with the correct positions of the X or O
*/
void update_tictac_board(char tictac_board[][3], int step, int player_tictac_id) {
    tictac_board[step/3][step%3] = player_tictac_id ? 'X' : 'O';
    printf("Updated Board.\n");
}

/*
    Initialize the tic-tac-toe board witht the each box has its own dimension for each
    row and coln. And printing this board while playing the game, so the user knows of valid move.
*/
void draw_tictac_board(char tictac_board[][3]) {
    printf(" %c | %c | %c \n", tictac_board[0][0], tictac_board[0][1], tictac_board[0][2]);
    printf("-----------\n");
    printf(" %c | %c | %c \n", tictac_board[1][0], tictac_board[1][1], tictac_board[1][2]);
    printf("-----------\n");
    printf(" %c | %c | %c \n", tictac_board[2][0], tictac_board[2][1], tictac_board[2][2]);
}

// After creating all the boards and updates, creating an method which will send the correct version of the 
// board to the clients. 
void send_tictac_update(int * cli_tictac_sockfd, int step, int player_tictac_id) {
    // It will print the sending the correct veriosn
    printf("Uploading the update...\n");
    // Giving the UPDT msg to the vlient of the updated board
    write_tictac_clients_msg(cli_tictac_sockfd, "UPDT");
    write_tictac_clients_int(cli_tictac_sockfd, player_tictac_id);
    write_tictac_clients_int(cli_tictac_sockfd, step);
    printf("Confirmation of the update has been sent.\n");
}

// Using this method to get the clients count while playing the board. 
void send_tictac_player_count(int cli_tictac_sockfd) {
    write_tictac_client_msg(cli_tictac_sockfd, "CNT");
    write_tictac_client_int(cli_tictac_sockfd, client_counts);
    printf("Clients Counts.\n");
}


// After the every move, checking the baord has the wining strick of O or X
int check_tictac_board(char tictac_board[][3], int last_tictac_move){
    int row = last_tictac_move/3;
    int col = last_tictac_move%3;
    // If the client wins by the row then it will print the Win by row and return 1
    if ( tictac_board[row][0] == tictac_board[row][1] && tictac_board[row][1] == tictac_board[row][2] ) { 
        printf("Won with row %d.\n", row);
        return 1;
    }
    // If the client wins by the coln then it will print the Win by coln and return 1 
    else if ( tictac_board[0][col] == tictac_board[1][col] && tictac_board[1][col] == tictac_board[2][col] ) { 
        printf("Won with column %d.\n", col);
        return 1;
    }
    // If the client wins by the backslash diagonal then it will print the Win by backslash diagonal and return 1 
    else if (!(last_tictac_move % 2)) { if ( (last_tictac_move == 0 || last_tictac_move == 4 || last_tictac_move == 8) && (tictac_board[1][1] == tictac_board[0][0] && tictac_board[1][1] == tictac_board[2][2]) ) {  
            printf("Won with backslash diagonal.\n");
            return 1;
        }
        
        // If the client wins by the frontslash diagonal then it will print the Win by frontslash diagonal and return 1 
        if ( (last_tictac_move == 2 || last_tictac_move == 4 || last_tictac_move == 6) && (tictac_board[1][1] == tictac_board[0][2] && tictac_board[1][1] == tictac_board[2][0]) ) { 
            printf("Won with frontslash diagonal.\n");
            return 1;
        }
    }
    return 0;
}

/* 
    Starting the game and checking the has been moving smoothly otherwise shows the error msgs, until get the 
    valid move. If the games gets to winnning or losing the checks after every move by teh vlients. If the game is been won
    then the server will says that player id is won, lose, or the game has non reuslt it will print the game is been draw
*/
void *run_tictac_game(void *thread_tictac_data) {
    int *cli_tictac_sockfd = (int*)thread_tictac_data; 
    char tictac_board[3][3] = { 

                              {' ', ' ', ' '},  
                              {' ', ' ', ' '}, 
                              {' ', ' ', ' '}           };

    printf("Game has started!\n");
    draw_tictac_board(tictac_board);
    int prev_tictac_player_turn = 1;
    int player_tictac_turn = 0;
    int game_tictac_over = 0;
    int turn_tictac_count = 0;
    // Running the game until the game get over.
    while(!game_tictac_over) {
        // if the player is tkaing the chanvce then other player is waiting
        if (prev_tictac_player_turn != player_tictac_turn)
            write_tictac_client_msg(cli_tictac_sockfd[(player_tictac_turn + 1) % 2], "WAIT");

        int valid = 0;
        int step = 0;
        // If the players commands the invalid move then it gets the chances until it commands the correct
        while(!valid) { 
            // Also telling the client that the move is not valid by showing that the box is being already filled out by the 
            // other client.             
		step = get_tictac_player_move(cli_tictac_sockfd[player_tictac_turn]);
		printf("Step: -> %d \n", step);
            if (step == -1) break; 
            printf("Player %d played this Position ->  %d\n", player_tictac_turn, step);
            // Check until get the valid move
            valid = check_tictac_move(tictac_board, step, player_tictac_turn);
            if (!valid) { 
                printf("Your Move was not correct, Please enter again...\n");
                write_tictac_client_msg(cli_tictac_sockfd[player_tictac_turn], "INVD");
            }
        }
        // if the players gets the disconnected then print the player has been disconnected and break the loop
	    if (step == -1) { 
            printf("Player has been Disconnected.\n");
            break;
        }
        // Also, checking the ining and loosing staus of the game. 
        else {
            // updating the board with the clients move
			update_tictac_board(tictac_board, step, player_tictac_turn);
            // sending the board update both the clients
            send_tictac_update( cli_tictac_sockfd, step, player_tictac_turn );
            // draw the baord to the baord clints, to shows the wining or loosing status
            draw_tictac_board(tictac_board);
            //  if the game get over with the WINS, LOSE or DRAW
            game_tictac_over = check_tictac_board(tictac_board, step);
            // If the games get the over with the player wins or loose and prints the player with id wins or lose
            if (game_tictac_over == 1) {
                write_tictac_client_msg(cli_tictac_sockfd[player_tictac_turn], "WINS");
                write_tictac_client_msg(cli_tictac_sockfd[(player_tictac_turn + 1) % 2], "LOSE");
                printf("Player %d won.\n", player_tictac_turn);
            }
            // if the game has been draw ->  non result of the game then the write that the game has been drawed
            else if (turn_tictac_count == 8) {               
                 printf("Draw.\n");
                write_tictac_clients_msg(cli_tictac_sockfd, "DRAW");
                game_tictac_over = 1;
            }
            prev_tictac_player_turn = player_tictac_turn;
            player_tictac_turn = (player_tictac_turn + 1) % 2;
            turn_tictac_count++;
        }
    }
    // printing the message of that the game has been officaly over
    printf("Thank you playing the game! Game is Over.\n");
    // closing both socket file descrptor 
	close(cli_tictac_sockfd[0]);
    close(cli_tictac_sockfd[1]);
    pthread_mutex_lock(&ptm_count);
    client_counts--;
    printf("Number of players is now %d.", client_counts);
    client_counts--;
    printf("Number of players is now %d.", client_counts);
    pthread_mutex_unlock(&ptm_count);
    // free client sokcet file descriptor and exit p theread woith null
    free(cli_tictac_sockfd);
    pthread_exit(NULL);
}

// Main 
int main(int argc, char *argv[]) {   
    // Checking when initialize the server that the prot is provided or not
    // If the port number is not provided then it prints the not prots
    if (argc < 2) {
        fprintf(stderr,"ERROR, Please provide Port Number\n");
        exit(1);
    }
    int lis_tictac_sockfd = setup_tictac_listener(atoi(argv[1])); 
    pthread_mutex_init(&ptm_count, NULL);
    // while
    while (1) {
        // Maximumn clients can have is 252 but it also inclued the current clients
        if (client_counts <= 252) {   
            int *cli_tictac_sockfd = (int*)malloc(2*sizeof(int)); 
            memset(cli_tictac_sockfd, 0, 2*sizeof(int));
            get_tictac_clients(lis_tictac_sockfd, cli_tictac_sockfd);
            pthread_t thread;
			int result = pthread_create(&thread, NULL, run_tictac_game, (void *)cli_tictac_sockfd);
            if (result){
                printf("thread failed at: %d\n", result);
                exit(-1);
            }
        }
    }
    close(lis_tictac_sockfd);
    pthread_mutex_destroy(&ptm_count);
    pthread_exit(NULL);
} // End of main
