/*
Full Name: Gautami Langarkande
Student ID: 5798842066
Net ID: langarka 
**/

//SERVER C

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

#define LOCALHOST "127.0.0.1" //host address
#define serverM_UDP_Port 24066 
#define serverC_UDP_Port 21066 
#define MAXDATA 1024 // max number of bytes we can get at once
#define ERROR -1

int serverC_UDP_sockfd; 							
struct sockaddr_in serverM_UDP_addr, serverC_addr;  
char encrypted_username[MAXDATA], encrypted_password[MAXDATA]; 

void authentication_check(std::map<string, string> &userpass_map){ // store username and password in map

    string user;
    string pass;
    string line;

    ifstream mytextfile("cred.txt");
        if(!mytextfile.is_open()){
            cout<<"Error: cred file is not open.";
            return;
        }
    while(getline(mytextfile, line)){
            stringstream ss(line);
            getline(ss, user, ',');
            getline(ss, pass, '\r');
            userpass_map.insert(pair<string, string>(user, pass));
	}
	mytextfile.close();
}


// create UDP socket for serverC
void create_serverC_socket_UDP(){

	serverC_UDP_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (serverC_UDP_sockfd == ERROR){
		perror("ERROR: serverC - UDP socket could not be created. ");
		exit(1);
	}

	// Referred from Beej's Tutorial 2022

	// Initialize UDP connection with serverM
	memset(&serverC_addr, 0, sizeof(serverC_addr)); 
	serverC_addr.sin_family = AF_INET; 
	serverC_addr.sin_addr.s_addr = inet_addr(LOCALHOST); 
	serverC_addr.sin_port = htons(serverC_UDP_Port); 

	// bind socket
	if (::bind(serverC_UDP_sockfd, (struct sockaddr *) &serverC_addr, sizeof(serverC_addr)) == ERROR){  
		perror("ERROR: serverC - could not bind socket. \n");
		exit(1);
	}
}

int main(){

	map<string, string> credentials_map;
	authentication_check(credentials_map);

	create_serverC_socket_UDP();
	cout<<"The ServerC is up and running using UDP on port "<<serverC_UDP_Port<<"."<<endl; 

	while (true){

		// Referred from Beej's Tutorial 2022

		// receive username from main server
		socklen_t serverM_UDP_addr_size = sizeof(serverM_UDP_addr); 	
		if (recvfrom(serverC_UDP_sockfd, encrypted_username, sizeof(encrypted_username), 0, (struct sockaddr *) &serverM_UDP_addr, &serverM_UDP_addr_size) == ERROR){
			perror("ERROR: serverC - could not receive encrypted username from serverM. \n");
			exit(1);
		}

		// receive password from main server
		if (recvfrom(serverC_UDP_sockfd, encrypted_password, sizeof(encrypted_password), 0, (struct sockaddr *) &serverM_UDP_addr, &serverM_UDP_addr_size) == ERROR){ 
			perror("ERROR: serverC - could not receive encrypted password from serverM. \n");
			exit(1);
		}

		printf("The ServerC received an authentication request from the Main Server.\n");
		
		// verify username
		int verify_username = 0;

        for (const auto &userpass_Pair : credentials_map ) {
        	if (strcmp(userpass_Pair.first.c_str(), encrypted_username) == 0){ //check if username exists in map
		 		verify_username = 1; // correct username
				break;
    		}
		}

		// verify password 
		int verify_pass = 0;

        for (const auto & [key, value] : credentials_map){
            if (strcmp(value.c_str(), encrypted_password) == 0){ //check if password exists in map
            verify_pass = 1; // correct password
        	}
        }

		int checkersum = verify_username + verify_pass;
		string authres;

		if(checkersum==2){
			authres = "AUTH_PASS";
		}
		else if(checkersum==1){
			authres = "WRONG_PASS";
		}
		else{
			authres = "WRONG_USERNAME";
		}

		char authentication_res[MAXDATA];
		strcpy(authentication_res, authres.c_str()); //convert to character array
		
		// Referred from Beej's Tutorial 2022
		
		// send authentication response to serverM
		if (sendto(serverC_UDP_sockfd, authentication_res, sizeof(authentication_res), 0, (struct sockaddr *) &serverM_UDP_addr, serverM_UDP_addr_size) == ERROR){ 
			perror("ERROR: serverC -  cannot send authentication response to serverM. \n");
			exit(1);
		}
		
		printf("The ServerC finished sending the response to the Main Server.\n");
	}
	
	close(serverC_UDP_sockfd); //close connection
	return 0;
} 

