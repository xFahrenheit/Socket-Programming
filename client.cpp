/*
Full Name: Gautami Langarkande
Student ID: 5798842066
Net ID: langarka 
**/

// CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <map>

using namespace std;

#define LOCALHOST "127.0.0.1" // host address
#define serverM_TCP_Port 25066 // serverM TCP port
#define MAXDATA 1024 // max number of bytes we can get at once
#define BACKLOG 10 // max number of connections allowed on the incoming queue
#define ERROR -1

int client_TCP_sockfd; 							
struct sockaddr_in serverM_addr, client_addr; 			

string username, password; 							     
char input_username[MAXDATA], input_password[MAXDATA]; 		
char authentication_response[MAXDATA]; 								

string course_code, course_category; 					
char code[MAXDATA], category[MAXDATA]; 			
char course_res[MAXDATA]; 							
char client_IP_addr[MAXDATA]; 									
unsigned int clientPort; 								


// create TCP client socket
void create_client_socket_TCP(){

	// Referred from Beej's Tutorial 2022

	client_TCP_sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (client_TCP_sockfd == ERROR){						 
		perror("ERROR: client -  could not create TCP socket. \n");
		exit(1);
	}

	// Initialize TCP connection with serverM
	memset(&serverM_addr, 0, sizeof(serverM_addr)); 
	serverM_addr.sin_family = AF_INET; 
	serverM_addr.sin_addr.s_addr = inet_addr(LOCALHOST); 
	serverM_addr.sin_port = htons(serverM_TCP_Port); 

	// Request TCP connection with serverM
	connect(client_TCP_sockfd, (struct sockaddr *) &serverM_addr, sizeof(serverM_addr));
	

}

// Get client TCP port
void create_client_TCP_port(){
	
	// Referred from Beej's Tutorial 2022

	memset(&client_addr, 0, sizeof(client_addr)); 	
	socklen_t client_addr_len = sizeof(client_addr);
	int client_socket = getsockname(client_TCP_sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
	if(client_socket == ERROR) {						 
		perror("ERROR: Client server cannot create TCP port\n");
		exit(1);
	}
	inet_ntop(AF_INET, &client_addr.sin_addr, client_IP_addr, sizeof(client_IP_addr));
	clientPort = ntohs(client_addr.sin_port); // TCP port obtained
}


// Unencrypted username input
void get_username(){

	cout<<"Please enter the username: ";
    cin>>username;
}

// Unencrypted password input
void get_password(){

	cout<<"Please enter the password: ";
    cin>>password;

}

// send unencrypter user inputs to serverM
void send_userinput_to_serverM(string unencrypted_username, string unencrypted_password){
	
	// Referred from Beej's Tutorial 2022
	// send username
	strncpy(input_username, unencrypted_username.c_str(), 128);
	send(client_TCP_sockfd, input_username, sizeof(input_username), 0);  
	// 	perror("ERROR: client - could not send username to serverM. \n");
	// 	exit(1);
	// }
	//send password
	strncpy(input_password, unencrypted_password.c_str(), 128);
	send(client_TCP_sockfd, input_password, sizeof(input_password), 0);  
	// 	perror("ERROR: client - could not send password to serverM. \n");
	// 	exit(1);
	// }

	cout<<unencrypted_username<<" sent an authentication request to the main server."<<endl;  
}

// Receive authentication result from serverM
string receive_authres_from_serverM(){

	// Referred from Beej's Tutorial 2022
	if(recv(client_TCP_sockfd, authentication_response, sizeof(authentication_response), 0) == ERROR){		  
		perror("ERROR: client - could not receive authentication resposnse from serverM. \n");
		exit(1);
	}
	
	string res = authentication_response; 
	return res;               
}


int check_authentication(){ //check if client got authorised

	int attempt_counter = 3; 	// attempt_counter of login attempts
	while (true){
		if(attempt_counter == 0){ // close connection if failed
			break;		
		}

		get_username();
		get_password();
		
		send_userinput_to_serverM(username, password); // send user inputs to serverM
		
		string auth_res = receive_authres_from_serverM();	// receive authentication response from serverM

		if (auth_res == "AUTH_PASS"){ 	
			cout<<username<<" received the result of authentication using TCP over port "<<clientPort<<". Authentication is successful."<<endl;
			break;
		}
		else if (auth_res == "WRONG_PASS"){	
			cout<<username<<" received the result of authentication using TCP over port "<<clientPort<<". Authentication failed: Password does not match"<<endl;
			attempt_counter--;
			cout<<"Attempts remaining: "<<attempt_counter<<endl;
		}
		else if (auth_res == "WRONG_USERNAME"){	
			cout<<username<<" received the result of authentication using TCP over port "<<clientPort<<". Authentication failed: Username Does not exist"<<endl;
			attempt_counter--;
			cout<<"Attempts remaining: "<<attempt_counter<<endl;
		}
	}
	return attempt_counter; 
}


// get course code input from client
void request_coursecode_from_client(){

	cout<<"Please enter the course code to query: ";
	getline(cin, course_code);
}


// get course category input from client
void request_category_from_client(){

	cout<<"Please enter the category (Credit / Professor / Days / CourseName): ";
	getline(cin, course_category);
}


// send this info from serverM
void request_courseinfo_from_serverM(string input_code, string input_category){

	// Referred from Beej's Tutorial 2022
	//send user input course code to serverM
	strncpy(code, input_code.c_str(), MAXDATA);
	if (send(client_TCP_sockfd, code, sizeof(code), 0) == ERROR){  
		perror("ERROR: client -  could not send course code to serverM. \n");
		exit(1);
	}

	//send user input course category to serverM
	strncpy(category, input_category.c_str(), MAXDATA);						
	if (send(client_TCP_sockfd, category, sizeof(category), 0) == ERROR){ 
			perror("ERROR: client -  could not send category to serverM. \n");
			exit(1);
		}
		cout<<username<<" sent a request to the main server."<<endl;

}

// Receive course query results from serverM
void receive_courseinfo_from_serverM(){

	// Referred from Beej's Tutorial 2022
	if(recv(client_TCP_sockfd, course_res, sizeof(course_res), 0) == ERROR){  
		perror("ERROR: client -  could not receive course info from serverM. \n");
		exit(1);
	}

	cout<<"The client received the response from the Main server using TCP over port "<<clientPort<<"."<<endl;
}

int main(){

	// connect client and serverM
	create_client_socket_TCP();
	create_client_TCP_port();
	printf("The client is up and running.\n"); 

	int attempts = check_authentication();
	if (attempts == 0){	// all attempts failed
		printf("Authentication Failed for 3 attempts. Client will shut down.\n"); // close client connection
		close(client_TCP_sockfd);
		return 0;
	}

	cin.ignore();

	while (true){

		course_code = ""; 
		course_category = "";  
		
		// Course Code
		request_coursecode_from_client();
		request_category_from_client();
		// send to main server
		request_courseinfo_from_serverM(course_code, course_category);
		// receive query results
		receive_courseinfo_from_serverM();
		
		cout<<course_res<<endl;

		printf("-----Start a new request-----\n"); 
	}

	close(client_TCP_sockfd); // close connection
	return 0;
}

