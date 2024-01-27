# Socket-Programming
Socket Programming in C++ for CS450 class at USC
Implementation of a simple web registration system for USC. Specifically, a student will use the client to access the central web registration server, which will forward their requests to the department servers in each department. For each department, the department server will store the information of the courses offered in this department. Additionally, a credential server will be used to verify the identity of the student.
  
There are total 5 communication end-points:
1. Client: used by a student to access the registration system.
2. Main server (serverM): coordinate with the backend servers.
3. Credential server (serverC): verify the identity of the student.
4. Department server(s) (serverCS and serverEE)): store the information of courses offered
by this department.
