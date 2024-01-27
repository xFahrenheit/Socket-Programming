============================ EE450 Socket Programming Project, Fall 2022 ============================
Full Name: Gautami Langarkande
Student ID#: 5798842066
Net ID: langarka

Assignment Description:
In this assignment, I have created USC authentication system which authenticates the user and executes queries according to what the user requests.
I have not completed the extra credit portion of this project. All the data exchanged between client and servers is through char arrays only.
Given below are further details on what each file does:
1. serverM.cpp:
This file consists code of the main server. The main server performs various functions such as sending requests and responses from client to the rest of the servers. It also encrypts the username and password credentials it received from the client
2. serverC.cpp:
This file checks whether the received encrypted credentials from serverM match with the credentials stored in the cred.txt file. The resulting response is sent back to the main server.
3. serverEE and ServerCS:
These files send and receive requests from the main server to provide information about their corresponding courses. They will check if the requested information exists in the respective text files and accordingly send the results of the findings to serverM
4. client server:
the client server is the interface for the user. User gets authenticated at the client server and also sends course information requests to this server. This information is then directed to the main server.


Idiosyncrasy: 1. This code only works for IPv4.
2. If the wrong category is entered, the client prints garbage values on the next correct query along with the correct answer for the inputed query. This is not a major issue, the code works completely fine overall. 

Reused Code:
I have not copied any code except for the initialization, binding, connecting, sending and receiving  codes from Beej's tutorial. Referred code sections have also been commented in the code.
All other codes are solely written by me.