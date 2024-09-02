# Term Project: Socket Programming Part 3

## Student and Project info
1. Name: 
2. Student ID: 
3. Platform: Ubuntu 22.04

## Description
This project implements a Student Performance Analysis system, a software application that allows students 
to check their GPA, percentage ranks, and other relevant data. There are 3 components of this system: two backend servers,
a main server, and two clients. Each backend server reads a data file contained in "data.csv" with student grade 
information in each department. Main server connects with backend servers with UDP, sending query and receiving response. 
Main server also connects with clients using TCP, to forward client queries and backend server replies.


## Data Used
The backend server reads input data from csv file, saved in the same directory as the project home directory:
`../dataA.csv`, `../dataB.csv`


## Usage
1. Run **make all** to build all executables.
2. Run **make run-serverA** and **make run-serverB** to start the backend servers who wait for main server's messages.
3. Run **make run-main-server** to start the main server.
4. Run **make run-client** to start a client process and connect to main server.
5. Run **make clean** to remove all executables.


## File Description

### Backend Server
The serverA.cpp and serverB.cpp file build backend servers in the socket programming that read dataA.csv and dataB.csv
respectively, store the department names and student IDs, as well as their grades and statistics data into data structure.
Server A and server B connects to main server using UDP in port 30574 and 31574, respectively. 

Backend server receives the department name and student ID, searches in the database, finds all related information, finally
pack them into a message and then send back to main server.

To start the client and make queries to the main server, run the following command:

    make run-serverA

    make run-serverB

**Extra Credit Notes:**
The backend servers are implemented with the functionality to recommend the closest academic interests to the target 
student, based on the Euclidean Distance of their grades. 

### Main Server
The servermain.cpp file builds a main server in the socket programming that asks each of Backend servers 
which departments they are responsible for. Main server will construct a data structure to book-keep such information.
After clients' bootup, main server receives requested department name and student ID, then forwards them to responsible 
backend servers. Main server then receives the reply information from backend server and then forward it back to clients.
Main server connection uses port 33574 and local host address 127.0.0.1.

To start the main server, run the following command:

    make run-main-server

### Client
The client.cpp file builds a client in the socket programming that requests student GPA and statistics. Client connects to 
main server with TCP and dynamic port on its end.
To start the client, run the following command:

    make run-client

### Makefile

The Makefile supports the following functions:

1. make all: Builds both the server and client executables.
2. make servermain: Builds the server executable.
3. make serverA: Builds the serverA executable.
4. make serverB: Builds the serverB executable.
5. make client: Builds the client executable.
6. make run-main-server: Starts the main server.
7. make run-serverA: Starts the serverA.
8. make run-serverB: Starts the serverB.
9. make run-client: Starts the client.
10. make clean: clean up the executable created.

## Idiosyncrasy
NA

## Notes for Acedemic Integrity
I have developed and written all the source code files with the code framework, with certain pieces of functionality 
inspired by Beej's Guide for Socket Programming. 