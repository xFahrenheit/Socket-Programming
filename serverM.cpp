/*
Full Name: Gautami Langarkande
Student ID: 5798842066
Net ID: langarka 
**/

//SERVER M

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

#define localhost "127.0.0.1" //LOCALHOST
#define serverC_UDP_Port 21066 
#define serverCS_UDP_Port 22066 
#define serverEE_UDP_Port 23066 
#define serverM_UDP_Port 24066
#define serverM_TCP_Port 25066 

#define MAXDATA 1024 // max number of bytes we can get at once
#define BACKLOG 10 // max number of connections allowed on the incoming queue
#define ERROR -1


int sockfd_client_TCP, child_client_sockfd, serverM_sockfd_UDP; 													
struct sockaddr_in serverM_addr, serverM_UDP_addr, client_addr, serverC_addr, serverEE_addr, serverCS_addr; 

char input_username[MAXDATA], input_password[MAXDATA]; 																   
char encrypted_username[MAXDATA], encrypted_password[MAXDATA]; 																									
char authentication_response[MAXDATA]; 																						
char code[MAXDATA], category[MAXDATA]; 																	
char final_info[MAXDATA]; 																					
char fin_res[MAXDATA]; 	



// Create TCP socket to connect with client
void create_serverM_socket_TCP(){

	// Referred from Beej's Tutorial 2022

	sockfd_client_TCP = socket(AF_INET, SOCK_STREAM, 0); // IPv4 TCP socket
	if (sockfd_client_TCP == ERROR){			
		perror("[ERROR] ServerM: failed to create socket for client. \n");
		exit(1);
	}

	memset(&serverM_addr, 0, sizeof(serverM_addr)); // empty struct
	serverM_addr.sin_family = AF_INET; // IPv4 
	serverM_addr.sin_addr.s_addr = inet_addr(localhost); // host address
	serverM_addr.sin_port = htons(serverM_TCP_Port); // main server TCP port for client connection
		
	// bind socket
	if (::bind(sockfd_client_TCP, (struct sockaddr *) &serverM_addr, sizeof(serverM_addr)) == ERROR){  //CHECK
        perror("[ERROR] serverM server: failed to bind client socket");
		exit(1);
	}
}

// listen for incoming requests from client through TCP connection
void listen_client(){

	// Referred from Beej's Tutorial 2022
	if (listen(sockfd_client_TCP, BACKLOG) == ERROR){	
        perror("[ERROR] serverM server: fail to listen for client socket");
		exit(1);
	}
}

// Create UDP socket for backend servers
void create_serverM_socket_UDP(){

	// Referred from Beej's Tutorial 2022

	serverM_sockfd_UDP = socket(AF_INET, SOCK_DGRAM, 0); // IPv4 UDP socket 
	if (serverM_sockfd_UDP == ERROR){						// CHECK
        perror("[ERROR] serverM : failed to create UDP socket");
		exit(1);
	}

	memset(&serverM_UDP_addr, 0, sizeof(serverM_UDP_addr)); //empty struct
	serverM_UDP_addr.sin_family = AF_INET; // IPv4 
	serverM_UDP_addr.sin_addr.s_addr = inet_addr(localhost); // host address
	serverM_UDP_addr.sin_port = htons(serverM_UDP_Port); // main server UDP port

	// bind socket
	if (::bind(serverM_sockfd_UDP, (struct sockaddr *) &serverM_UDP_addr, sizeof(serverM_UDP_addr)) == -1){ //CHECK
        perror("[ERROR] serverM server: fail to bind UDP socket");
		exit(1);
	}
}

// initialise connection with ServerC
void init_connect_serverC(){ 

	memset(&serverC_addr, 0, sizeof(serverC_addr)); //empty struct
	serverC_addr.sin_family = AF_INET; // IPv4 
	serverC_addr.sin_addr.s_addr = inet_addr(localhost); // host address
	serverC_addr.sin_port = htons(serverC_UDP_Port); // Credential server UDP port

}

// initialise connection with ServerEE
void init_connect_serverEE(){

	memset(&serverEE_addr, 0, sizeof(serverEE_addr)); //empty struct
	serverEE_addr.sin_family = AF_INET; // IPv4 
	serverEE_addr.sin_addr.s_addr = inet_addr(localhost); // host address
	serverEE_addr.sin_port = htons(serverEE_UDP_Port); // EE server UDP port
}

// initialise connection with ServerCS
void init_connect_serverCS(){
	memset(&serverCS_addr, 0, sizeof(serverCS_addr)); //empty struct
	serverCS_addr.sin_family = AF_INET; 
	serverCS_addr.sin_addr.s_addr = inet_addr(localhost); // host address
	serverCS_addr.sin_port = htons(serverCS_UDP_Port); 
}

// Request coursecode and category info from ServerEE 
void request_EEinfo_from_serverEE(){

	init_connect_serverEE();
	// Referred from Beej's Tutorial 2022
	if (sendto(serverM_sockfd_UDP, code, sizeof(code), 0, (struct sockaddr *) &serverEE_addr, sizeof(serverEE_addr)) == ERROR){ 
		perror("ERROR: serverM - CANNOT SEND COURSE CODE TO EE SERVER\n");
		exit(1);
	}

	if (sendto(serverM_sockfd_UDP, category, sizeof(category), 0, (struct sockaddr *) &serverEE_addr, sizeof(serverEE_addr)) == ERROR){ 
	perror("ERROR: serverM - CANNOT SEND COURSE QUERY TO EE SERVER\n");
	exit(1);
	}
}

// Request coursecode and category info from ServerCS
void request_CSinfo_from_serverCS(){

	init_connect_serverCS();
	// Referred from Beej's Tutorial 2022
	if (sendto(serverM_sockfd_UDP, code, sizeof(code), 0, (struct sockaddr *) &serverCS_addr, sizeof(serverCS_addr)) == -1){ // CHECK
		perror("ERROR: serverM - CANNOT SEND COURSE CODE TO CS SERVER\n");
		exit(1);
	}
	if (sendto(serverM_sockfd_UDP, category, sizeof(category), 0, (struct sockaddr *) &serverCS_addr, sizeof(serverCS_addr)) == -1){ // CHECK
	perror("ERROR: serverM - CANNOT SEND COURSE QUERY TO CS SERVER\n");
	exit(1);
	}

}

//Encrypter function to offset each character from user authentication by 4
void encrypter(char (&a)[1024]) {

    for(int i=0;i<strlen(a);i++){
    int c = int(a[i]);
    if(!( (c>=48 && c<=57) || (c>=65 && c<=90) || (c>=97 && c<=122) )){ //if character is a special character, ignore and continue
        continue;
    }
    else if (c>=48 && c<=57){ //if character is between 0 and 9, offset it
        c=c+4;
        if(c>57){
        	c = (c - 57) + 47;
            a[i] = c;
        }
        else{
            a[i] = c;
        }
    }  
    else if (c>=65 && c<=90){ //else if character is between A and Z, offset it
        c=c+4;
        if(c>90){
            c = (c - 90) + 64;
            a[i] = c;
        }
        else{
            a[i] = c;
        }
        }
                
        else if (c>=97 && c<=122){ //else if character is between a and z, offset it
        c=c+4;
        if(c>122){
            c = (c - 122) + 96;
            a[i] = c;
        }
        else{ //else just return the character value
            a[i] = c;
        }
        }
    }
}

int main(){

	create_serverM_socket_TCP();
	create_serverM_socket_UDP();

	printf("The main server is up and running.\n"); 

	while (true){
		listen_client();

		// Referred from Beej's Tutorial 2022
	
		socklen_t client_addr_size = sizeof(client_addr); 						
		child_client_sockfd = accept(sockfd_client_TCP, (struct sockaddr *) &client_addr, &client_addr_size); 
		if (child_client_sockfd == ERROR){									
			perror("ERROR: serverM - did not accept connection from client. \n");
			exit(1);
		}

		int done = 0;
		while (done==0){

            // Referred from Beej's Tutorial 2022
			// receive username from client
			if (recv(child_client_sockfd, input_username, sizeof(input_username), 0) == ERROR){ 
				perror("ERROR: serverM - did not receive username from client. \n");
				exit(1);
			}
			// receive password from client
			if (recv(child_client_sockfd, input_password, sizeof(input_password), 0) == ERROR){ 
				perror("ERROR: serverM - did not receive password from client. \n");
				exit(1);
			}
			cout<<"The main server received the authentication for "<<input_username<<" using TCP over port "<<serverM_TCP_Port<<"."<<endl;
			
			strncpy(encrypted_username, input_username, strlen(input_username)+1);
			strncpy(encrypted_password, input_password, strlen(input_password)+1);

			// run encrypter function on receieved username and password
			encrypter(encrypted_username);
			encrypter(encrypted_password);

			// initialise connection with serverC 
			init_connect_serverC();
			// send encrypted username to serverC
			if (sendto(serverM_sockfd_UDP, encrypted_username, sizeof(encrypted_username), 0, (struct sockaddr *) &serverC_addr, sizeof(serverC_addr)) == -1){ // CHECK
				perror("ERROR: serverM - could not send encrypted username to serverC. \n");
				exit(1);
			}
			// send encrypted password to serverC
			if (sendto(serverM_sockfd_UDP, encrypted_password, sizeof(encrypted_password), 0, (struct sockaddr *) &serverC_addr, sizeof(serverC_addr)) == -1){ // CHECK
				perror("ERROR: serverM - could not send encrypted password to serverC. \n");
				exit(1);
			}

			printf("The main server sent an authentication request to serverC.\n");

			// Receive authentication response from serverC
			socklen_t serverC_addr_size = sizeof(serverC_addr); 
			if (recvfrom(serverM_sockfd_UDP, authentication_response, sizeof(authentication_response), 0, (struct sockaddr *) &serverC_addr, &serverC_addr_size) == -1){ // CHECK
				perror("[ERROR] serverM: fail to receive auth response from serverC\n");
				exit(1);
			}

			cout<<"The main server received the result of the authentication request from ServerC using UDP over port "<<serverM_UDP_Port<<"."<<endl;

			// forward this authentication response to client
			if (send(child_client_sockfd, authentication_response, sizeof(authentication_response), 0) == -1){ 			// CHECK
				perror("ERROR: serverM - could not send authentication response to serverC.\n");
				exit(1);
			}

			printf("The main server sent the authentication result to the client.\n");

			string res = authentication_response;
			if (res=="AUTH_PASS"){					
				done = 1; 	//if user is authorised, exit while loop		
			}
		}	
		while (true){
			// Referred from Beej's Tutorial 2022

			// receive course code from client
			if (recv(child_client_sockfd, code, sizeof(code), 0) == ERROR){ // CHECK
				perror("ERROR: serverM - could not receive course code from client.\n");
				exit(1);
			}

			// receive course category from client
			if (recv(child_client_sockfd, category, sizeof(category), 0) == -1){ // CHECK
				perror("ERROR: serverM - could not receive course category from client. \n");
				exit(1);
			}
			cout<<"The main server received from "<<input_username<<" to query course "<<code<<
				" about "<<category<<" using TCP over port "<<serverM_TCP_Port<<"."<<endl;
			
			string course_code_string = code;

			//convert to string and use substring
			if (course_code_string.substr(0, 2) == "EE"){ // if E is the first character of the course code, request info from serverEE
				request_EEinfo_from_serverEE();
				printf("The main server sent a request to serverEE.\n");
			}
			
			else { // otherwise, request info from serverCS
				request_CSinfo_from_serverCS();
				printf("The main server sent a request to serverCS.\n");
			}
				
			if (course_code_string.substr(0, 2) == "EE"){  // EE server
				socklen_t serverEE_addr_size = sizeof(serverEE_addr); 
				final_info[0] = '\0';

				// Referred from Beej's Tutorial 2022
				if (recvfrom(serverM_sockfd_UDP, final_info, sizeof(final_info), 0, (struct sockaddr *) &serverEE_addr, &serverEE_addr_size) == ERROR){ // CHECK
					perror("ERROR: serverM - could not receive course info from serverEE. \n");
					exit(1);
				}	
				cout<<"The main server received the response from the serverEE using UDP over port "<<serverM_UDP_Port<<"."<<endl;	
			}
	
			else{ 	

				// Referred from Beej's Tutorial 2022									  
				socklen_t serverCS_addr_size = sizeof(serverCS_addr); 
				final_info[0] = '\0';
				if (recvfrom(serverM_sockfd_UDP, final_info, sizeof(final_info), 0, (struct sockaddr *) &serverCS_addr, &serverCS_addr_size) == ERROR){ // CHECK
					perror("ERROR: serverM - could not receive course info from serverCS. \n");
					exit(1);
				}
				cout<<"The main server received the response from the serverCS using UDP over port "<<serverM_UDP_Port<<"."<<endl;
			}
			// Send the final result to the client
			if (send(child_client_sockfd, final_info, sizeof(final_info), 0) == ERROR){ // CHECK
				perror("ERROR: serverM - could not send course info to client. \n");
				exit(1);
			}
			cout<<"The main server sent the query information to the client."<<endl;
		}
		close(child_client_sockfd); //close child socket
	}
	
	// close rest of the sockets
	close(serverM_sockfd_UDP);
	close(sockfd_client_TCP);

	return 0;
}

