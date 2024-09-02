# Term Project: Socket Programming Part 2

## Student and Project info
1. Name: 
2. Student ID: 
3. Platform: Ubuntu 22.04

## Description
The project is for socket programming between servers using UDP. There are two backend servers, A and B, which communicate 
with the main server through UDP. Because the storage space and computation resources of the main server are limited, the 
required information is stored in two backend servers. Each backend server stores a data file, dataA.txt for server A and 
dataB.txt for server B, which contains Department names and student IDs. Main server first asks backend servers which 
Department they are responsible for and retrieve a list of Departments from each backend server. Main server will send a 
request only to its responsible backend server which contains the requested Department. The backend server then replies 
to the main server with the requested information.

## Data Used
The project reads input data from text file, named list.txt, saved in the same directory as the project home directory:
`../dataA.txt`, `../dataB.txt`


## Usage
1. Run **make all** to build both the server and client executables.
2. Run **make run-main-server** to start the main server.
3. Run **make run-serverA** and **make run-serverB** to start the client and make queries to the main server.
4. (Separate compilation) Run **make servermain**, **make serverA** and **make serverB* to compile them separately.

## File Description
### Main Server
The servermain.cpp file builds a main server in the socket programming that  asks each of Backend servers 
which departments they are responsible for. Main server will construct a data structure to book-keep such information.
Main server searches in the database and decides which Backend server(s) should handle the requested department name.

To start the main server, run the following command:

    make run-main-server

### Backend Server
The serverA.cpp and serverB.cpp file build backend servers in the socket programming that read dataA.txt and dataB.txt 
respectively, store the department names and student IDs, and count how many distinct students are there in a department.
Backend servers reply with a list of departments to the main server.
Backend server receives the department name, searches in the database, and finds how many distinct student IDs belong to 
the department and what those student IDs are.

To start the client and make queries to the main server, run the following command:

    make run-serverA

    make run-serverB

### Makefile

The Makefile supports the following functions:

1. make all: Builds both the server and client executables.
2. make servermain: Builds the server executable.
3. make serverA: Builds the serverA executable.
4. make serverB: Builds the serverB executable.
5. make run-main-server: Starts the main server.
6. make run-serverA: Starts the serverA.
7. make run-serverB: Starts the serverB.
8. clean: clean up the executable created.

## Idiosyncrasy
NA

## Notes for Acedemic Integrity
I have developed and written all the source code files with the code framework.