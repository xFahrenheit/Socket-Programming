/*
Full Name: Gautami Langarkande
Student ID: 5798842066
Net ID: langarka 
**/

// SERVER CS

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
#define serverM_UDP_Port 24066 // 
#define serverCS_UDP_Port 22066 // 

#define MAXDATA 1024 // max number of bytes we can get at once
#define BACKLOG 10 // max number of connections allowed on the incoming queue
#define ERROR -1

int serverCS_UDP_sockfd; 			
struct sockaddr_in serverCS_addr, serverM_UDP_addr; // server addresses
char course_code[MAXDATA], course_category[MAXDATA]; 			
char course_information[MAXDATA]; 
char not_found[MAXDATA];

map<string, map<string,string> > CourseInfo(){ //creates a map which stores all the course info

string course_code, credits, professor, days, course_name;
string line;
map<string, map<string,string> > course_info;


    ifstream mytextfile("cs.txt");
    if(!mytextfile.is_open()){
        cout<< "Error: cs file is not open." << endl;
    }

   while(getline(mytextfile, line)){
        stringstream ss(line);
        getline(ss, course_code , ',');
        getline(ss, credits ,',');
        getline(ss, professor , ',');
        getline(ss, days , ',');
        getline(ss, course_name , '\r');

        course_info[course_code]["Credit"] = credits;
        course_info[course_code]["Professor"] = professor;
        course_info[course_code]["Days"]= days;
        course_info[course_code]["CourseName"] = course_name;
   }
 return course_info; 
}

void create_serverCS_socket_UDP(){ //create UDP socket for serverCS

	// Referred from Beej's Tutorial 2022
	serverCS_UDP_sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
	if (serverCS_UDP_sockfd == ERROR){			
		perror("ERROR: serverCS - could not connect UDP socket.\n");
		exit(1);
	}

	// Initialize UDP connection to main server
	memset(&serverCS_addr, 0, sizeof(serverCS_addr)); // empty struct

	serverCS_addr.sin_family = AF_INET; // IPv4 
	serverCS_addr.sin_addr.s_addr = inet_addr(LOCALHOST); 
	serverCS_addr.sin_port = htons(serverCS_UDP_Port); 

	// bind socket
	if (::bind(serverCS_UDP_sockfd, (struct sockaddr *) &serverCS_addr, sizeof(serverCS_addr)) == ERROR){  
		perror("ERROR: serverCS -  could not bind UDP socket. \n");
		exit(1);
	}
}


int main(){

	map<string, map<string,string> > course_sample = CourseInfo(); //initialize our map and name it course_sample

	create_serverCS_socket_UDP();

	cout<<"The ServerCS is up and running using UDP on port "<<serverCS_UDP_Port<<"."<<endl; 

	while (true){

		// Referred from Beej's Tutorial 2022

		socklen_t serverM_UDP_addr_size = sizeof(serverM_UDP_addr); 

		//receive course code from serverM	
		if (recvfrom(serverCS_UDP_sockfd, course_code, sizeof(course_code), 0, (struct sockaddr *) &serverM_UDP_addr, &serverM_UDP_addr_size) == ERROR){ 
			perror("ERROR: serverCS -  did not recieve course code from serverM\n");
			exit(1);
        }		
		//receive course category from serverM	
        if (recvfrom(serverCS_UDP_sockfd, course_category, sizeof(course_category), 0, (struct sockaddr *) &serverM_UDP_addr, &serverM_UDP_addr_size) == ERROR){ 
			perror("ERROR: serverCS -  did not recieve category from serverM. \n");
			exit(1);
			}	

        string course_code_string = course_code;
        char course_code_char[MAXDATA];
		strncpy(course_code_char, course_code_string.c_str(), MAXDATA); // convert to char array to send to serverM
		cout<<"The ServerCS received a request from the Main Server about "<<course_code_char<<"."<<endl;

        cout<<"The ServerCS received a request from the Main Server about the "<<course_category<<" of "<<course_code<<"."<<endl;

            string send_info;
			string course_category_str = course_category;
            string found_info;
 
            if(course_sample.count(course_code)){ //checks if course code exists in our map: course_sample
            cout<<"The course information has been found: The "<<course_category<<" of "<<course_code<<" is "<<course_sample[course_code][course_category]<<"."<<endl;
            found_info = course_sample[course_code][course_category];
			send_info = "The " + course_category_str + " of " + course_code + " is " + found_info + ".\n" ; 
            
			// Referred from Beej's Tutorial 2022

			course_information[0] = '\0';
			// if exists, send course information to serverM
			strncpy(course_information, send_info.c_str(), strlen(send_info.c_str())); 
			if (sendto(serverCS_UDP_sockfd, course_information, sizeof(course_information), 0, (struct sockaddr *) &serverM_UDP_addr, serverM_UDP_addr_size) == ERROR){ 
				perror("ERROR: serverCS -  cannot send course info. \n");
				exit(1);
			}
		}

		else{ //if course code is not found, tell this to serverM
			string not_found_str;
			string course_code_str = course_code;
			not_found_str = "Didn't find the course: " + course_code_str + ".\n"; 

			// Referred from Beej's Tutorial 2022
			socklen_t serverM_UDP_addr_size = sizeof(serverM_UDP_addr);
			not_found[0] = '\0';
			strncpy(not_found, not_found_str.c_str(), strlen(not_found_str.c_str())); 
			if (sendto(serverCS_UDP_sockfd, not_found, sizeof(course_information), 0, (struct sockaddr *) &serverM_UDP_addr, serverM_UDP_addr_size) == -1){ 
				perror("ERROR: serverCS -  cannot send course info. \n");
					exit(1);
				}
			}
		
		 cout<<"The ServerCS finished sending the response to the main server."<<endl;
    }	
		
	close(serverCS_UDP_sockfd); //close connection
	return 0;
}

