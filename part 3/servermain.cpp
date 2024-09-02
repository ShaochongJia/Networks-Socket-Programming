//
// Created by Shaochong Jia on 4/12/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
//additional necessary libraries
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#define LOCAL_HOST "127.0.0.1"
#define MAIN_PORT_TCP 33574
#define MAIN_PORT_UDP 32574
#define BACKEND_PORT_A 30574
#define BACKEND_PORT_B 31574
#define BACKLOG 10	 // how many pending connections queue will hold
#define MAXDATASIZE 1024
#define FAILURE -1

using namespace std;

int main_tcp_socket, main_udp_socket, child_tcp_socket;
sockaddr_in main_tcp_address, main_udp_address;
sockaddr_in client_tcp_address, backend_udp_address_a, backend_udp_address_b;
socklen_t backend_udp_addr_len_a, backend_udp_addr_len_b;
char recv_buf[MAXDATASIZE];
vector<string> student_and_dept;
bool sent_to_A, sent_to_B;

// self defined functions
void create_bind_socket_TCP();
void create_bind_socket_UDP();
void listen_from_TCP();
void connect_udp(sockaddr_in &target_addr, socklen_t &target_addr_len, int port);
void send_msg_thru_UDP(int server_socket, const string& msg, sockaddr_in backend_address,
                socklen_t backend_address_length, const string& backendName);
string recv_msg_from_UDP(int server_socket, sockaddr_in backend_address,
                         socklen_t backend_address_length, const string& backendName);
void remember_dept_server(string deptList, const char *backendName,
                          unordered_map<string, string> & dept_from_server);
void accept_from_TCP();
vector<string> recv_msg_from_TCP(int new_fd, char *buf, int &clientID, int port);
void send_msg_thru_TCP(int socket, string message);
vector<string> split(string s, char delimiter);

bool handle_send_dept_name(vector<string> student_and_dept, unordered_map<string, string> dept_from_server, int clientID);
string handle_recv_all_info(vector<string> student_and_dept);
void handle_send_all_info(string all_info, int clientID);

int main() {
   create_bind_socket_TCP();
   listen_from_TCP();
   create_bind_socket_UDP();
   connect_udp(backend_udp_address_a, backend_udp_addr_len_a, BACKEND_PORT_A);
   connect_udp(backend_udp_address_b, backend_udp_addr_len_b, BACKEND_PORT_B);

   // boot up
   printf("Main server is up and running.\n");
   printf("---------------------------------------\n");
   string bootup = "";

   // main server asks for department lists by sending a boot up message to backend A and B
   send_msg_thru_UDP(main_udp_socket, bootup, backend_udp_address_a,
                     backend_udp_addr_len_a, "A");
   usleep(50000);
   send_msg_thru_UDP(main_udp_socket, bootup, backend_udp_address_b,
                     backend_udp_addr_len_b, "B");

   //receive the dept lists from backend servers
   unordered_map<string, string> dept_from_server;
   string dept_list = recv_msg_from_UDP(main_udp_socket, backend_udp_address_a,
                                        backend_udp_addr_len_a, "A");
   cout << "Main Server has received the department list from server A using UDP over port "<< BACKEND_PORT_A << endl;
   cout << dept_list << endl;
   remember_dept_server(dept_list, "A", dept_from_server);

   dept_list = recv_msg_from_UDP(main_udp_socket, backend_udp_address_b,
                                 backend_udp_addr_len_b, "B");
   cout << "Main Server has received the department list from server B using UDP over port "<< BACKEND_PORT_B << endl;
   cout << dept_list << endl;
   remember_dept_server(dept_list, "B", dept_from_server);

   int clientID = 0;
   // main loop
   while (true) {
      // accept TCP connection with clients
      accept_from_TCP();

      // Fork a new process to handle the connection
      pid_t pid = fork();
      clientID++;
      if (pid == 0) {
         // Child process
         // Close the listening socket
         close(main_tcp_socket);
         while (true) {
            // Receive message from client
            student_and_dept = recv_msg_from_TCP(child_tcp_socket, recv_buf,
                                                    clientID, MAIN_PORT_TCP);
            // handle the dept name and send query to relative backend server
            if (handle_send_dept_name(student_and_dept, dept_from_server, clientID)) {

               // receive reply from backend server
               string all_info = handle_recv_all_info(student_and_dept);

               cout << all_info << endl;
               // send back to client
               handle_send_all_info(all_info, clientID);
            }
            cout << "------------------New Query------------------" << endl;
            sent_to_A = false;
            sent_to_B = false;
         }
      } else if (pid < 0) {
         perror("fork");
         exit(EXIT_FAILURE);
      } else {
         // Parent process
         // Close the connection socket
         close(child_tcp_socket);
      }
   }

}


void create_bind_socket_TCP() {
   main_tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
   if (main_tcp_socket == FAILURE) {
      cerr << "Error creating main TCP socket\n";
      exit(1);
   }
   // bind main TCP socket
   memset(&main_tcp_address, 0, sizeof(main_tcp_address));
   main_tcp_address.sin_family = AF_INET;
   main_tcp_address.sin_addr.s_addr = inet_addr(LOCAL_HOST);
   main_tcp_address.sin_port = htons(MAIN_PORT_TCP);
   if (bind(main_tcp_socket, (sockaddr*)& main_tcp_address, sizeof(main_tcp_address)) == FAILURE) {
      cerr << "Error binding main TCP socket\n";
      exit(1);
   }
}

void create_bind_socket_UDP() {
   main_udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
   if (main_udp_socket == FAILURE) {
      cerr << "Error creating main TCP socket\n";
      exit(1);
   }
   // bind main UDP socket
   memset(&main_udp_address, 0, sizeof(main_udp_address));
   main_udp_address.sin_family = AF_INET;
   main_udp_address.sin_addr.s_addr = inet_addr(LOCAL_HOST);
   main_udp_address.sin_port = htons(MAIN_PORT_UDP);
   if (bind(main_udp_socket, (sockaddr*)&main_udp_address, sizeof(main_udp_address)) == FAILURE) {
      cerr << "Error binding main UDP socket\n";
      exit(1);
   }
}

void listen_from_TCP() {
   //start listening for connections
   if (listen(main_tcp_socket, BACKLOG) == FAILURE) {
      perror("listen");
      exit(1);
   }
}

void connect_udp(sockaddr_in &target_addr, socklen_t &target_addr_len, int port) {
   target_addr.sin_family = AF_INET;
   target_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST);
   target_addr.sin_port = htons(port);
   target_addr_len = sizeof(target_addr);
}

void send_msg_thru_UDP(int server_socket, const string& msg, sockaddr_in backend_address,
                socklen_t backend_address_length, const string& backendName) {
   const char* send_msg = msg.c_str();
   if (sendto(server_socket, send_msg, strlen(send_msg), 0,
              (struct sockaddr *)&backend_address, backend_address_length) == FAILURE) {
      cerr << "Error: Failed to send to backend server " << backendName << "." << endl;
   }
}

string recv_msg_from_UDP(int server_socket, sockaddr_in backend_address, socklen_t backend_address_length, const string& backendName) {
   char message[MAXDATASIZE];
   int message_length = recvfrom(server_socket, message, sizeof(message), 0,
                                 (struct sockaddr *)&backend_address, &backend_address_length);
   if (message_length == FAILURE) {
      cerr << "Error: Failed to receive message from backend server " << backendName << "." << endl;
      return NULL;
   }
   // process the message received from the backend server
   string recvMsg(message,message_length - 1);
   return recvMsg;
}

void remember_dept_server(string deptList, const char *backendName, unordered_map<string, string> & dept_from_server) {
   stringstream ss(deptList);
   string dept;
   while (getline(ss, dept)) {
      dept_from_server[dept] = backendName;
   }
}

void accept_from_TCP() {
   socklen_t client_tcp_address_len = sizeof(client_tcp_address);
   child_tcp_socket = accept(main_tcp_socket, (struct sockaddr *)& client_tcp_address, & client_tcp_address_len);
   if (child_tcp_socket == FAILURE) {
      cerr << "Failed to accept from TCP \n";
      exit(1);
   }
}

vector<string> recv_msg_from_TCP(int new_fd, char *buf, int &clientID, int port) {
   int num_bytes;
   if ((num_bytes = read(new_fd, buf, MAXDATASIZE)) == -1) {
      perror("recv");
      exit(1);
   }
   buf[num_bytes] = '\0';
   string msg = buf;
   //split the message into Student ID and dept name
   vector<string> student_and_dept = split(msg, '\t');

   cout << "Main server has received request on Student " << student_and_dept[1] << " in "
   << student_and_dept[0] << " from client" << clientID << " using TCP over port "
   << port << endl;
   return student_and_dept;
}

void send_msg_thru_TCP(int socket, string message) {
   if (send(socket, message.c_str(), message.size(), 0) == FAILURE)
      perror("send");
}

vector<string> split(string s, char delimiter) {
   vector<string> tokens;
   string token;
   for (char i : s) {
      if (i != delimiter) {
         token += i;
      } else {
         tokens.push_back(token);
         token = "";
      }
   }
   tokens.push_back(token);
   return tokens;
}

bool handle_send_dept_name(vector<string> student_and_dept, unordered_map<string, string> dept_from_server, int clientID) {
   string dept = student_and_dept[0];
   string student = student_and_dept[1];

   if (! dept_from_server.count(dept)) {
      cout << dept << " does not show up in server A & B." << endl;
      send_msg_thru_TCP(child_tcp_socket, dept + ": not found\n");
      cout << "Main server has sent \"" << dept << ": not found\" to client " << clientID
           << " using TCP over port " << MAIN_PORT_TCP << "." << endl;
      return false;
   }
   else {
      string server = dept_from_server[dept];
      cout << dept << " shows up in server " << server << endl;
      if (server == "A") {
         send_msg_thru_UDP(main_udp_socket, dept + "\t" + student, backend_udp_address_a,
                           backend_udp_addr_len_a, "A");
         sent_to_A = true;
      }
      else {
         send_msg_thru_UDP(main_udp_socket, dept + "\t" + student, backend_udp_address_b,
                           backend_udp_addr_len_b, "B");
         sent_to_B = true;
      }
      cout << "Main server has sent request of Student " << student << " to server " << (sent_to_A ? "A" : "B" )
      << " using UDP over port " << MAIN_PORT_UDP << endl;
      return true;
   }
}

string handle_recv_all_info(vector<string> student_and_dept) {
   string all_info;
   string student = student_and_dept[1];
   if (sent_to_A) {
      all_info = recv_msg_from_UDP(main_udp_socket, backend_udp_address_a,
                                          backend_udp_addr_len_a, "A");
   }
   else {
      all_info = recv_msg_from_UDP(main_udp_socket, backend_udp_address_b,
                                   backend_udp_addr_len_b, "B");
   }
   // all info contains an alphabetic char means student ID not found
   if (isalpha(all_info[0])) {
      cout << "Main server has received \"" << all_info << "\" from server " <<
      (sent_to_A ? "A" : "B" ) << endl;
   }
   else {
      cout << "Main server has received searching result of Student " << student << " from server " <<
           (sent_to_A ? "A" : "B" ) << endl;
   }
   return all_info;
}


void handle_send_all_info(string all_info, int clientID) {
   send_msg_thru_TCP(child_tcp_socket, all_info);
   if (isalpha(all_info[0])) {
      cout << "Main server has sent message to client " << clientID << " using TCP over " << MAIN_PORT_TCP
           << endl;
   }
   else {
      cout << "Main server has sent searching result to client " << clientID << " using TCP over " << MAIN_PORT_TCP
           << endl;
   }
}