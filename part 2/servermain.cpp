#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <string>

#define BACKEND_IP "127.0.0.1"  // IP address of the backend server
#define SERVER_PORT 32574  // port number used by the main server
#define BACKEND_PORT 30574  // port number used by the backend server
#define BACKEND_PORT_B 31574
using namespace std;

void sendMsg(int server_socket, string msg, sockaddr_in backend_address, socklen_t backend_address_length, string backendName);
string recvMsg(int server_socket, sockaddr_in backend_address, socklen_t backend_address_length, string backendName, int PORT);
void rememberDeptAndServer(string deptList, const char *backendName, unordered_map<string, string> & deptFromServer);
int numStudents(const string& studentList);



int main() {
   int server_socket = socket(AF_INET, SOCK_DGRAM, 0);  // create a UDP socket
   if (server_socket == -1) {
      cerr << "Error: Failed to create server socket." << endl;
      return -1;
   }

   struct sockaddr_in server_address;
   server_address.sin_family = AF_INET;
   server_address.sin_addr.s_addr = INADDR_ANY;
   server_address.sin_port = htons(SERVER_PORT);

   // bind the socket to the server address and port
   if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
      cerr << "Error: Failed to bind server socket." << endl;
      return -1;
   }

   struct sockaddr_in backend_address;
   backend_address.sin_family = AF_INET;
   backend_address.sin_addr.s_addr = inet_addr(BACKEND_IP);
   backend_address.sin_port = htons(BACKEND_PORT);
   socklen_t backend_address_length = sizeof(backend_address);
   
   struct sockaddr_in backend_address_b;
   backend_address_b.sin_family = AF_INET;
   backend_address_b.sin_addr.s_addr = inet_addr(BACKEND_IP);
   backend_address_b.sin_port = htons(BACKEND_PORT_B);
   socklen_t backend_address_length_b = sizeof(backend_address_b);

   // boot up and request list to A and B
   cout << "Main server is up and running" << endl;
   string bootup= "";

   sendMsg(server_socket, bootup, backend_address, backend_address_length, "A");
   usleep(50000);
   sendMsg(server_socket, bootup, backend_address_b, backend_address_length_b, "B");

   // receive message from the backend server A
   string deptList = recvMsg(server_socket, backend_address, backend_address_length, "A", BACKEND_PORT);
   cout << "Main Server has received the department list from server A using UDP over port "<< BACKEND_PORT << endl;
   cout << deptList << endl;
   unordered_map<string, string> deptFromServer;
   rememberDeptAndServer(deptList.substr(0, deptList.size()), "A", deptFromServer);

   // receive message from the backend server B
   deptList = recvMsg(server_socket, backend_address_b, backend_address_length_b, "B", BACKEND_PORT_B);
   cout << "Main Server has received the department list from server B using UDP over port "<< BACKEND_PORT_B << endl;
   cout << deptList << endl;
   rememberDeptAndServer(deptList.substr(0, deptList.size()), "B", deptFromServer);

   while (true) {
      bool sentToA = false, sentToB = false;
      // ask user to input query
      string name;
      cout << "Enter Department Name: ";
      cin >> name;
      // not there
      if (! deptFromServer.count(name)) {
         cout << name << " does not show up in server A & B." << endl;
         continue;
      }
      // found dept name
      else {
         string server = deptFromServer[name];
         cout << name << " shows up in server " << server << endl;

         if (server == "A") {
            sendMsg(server_socket, name, backend_address, backend_address_length, "A");
            sentToA = true;
         }
         else {
            sendMsg(server_socket, name, backend_address_b, backend_address_length_b, "B");
            sentToB = true;
         }
         cout << "The Main Server has sent request for " << name << " to server "<< server << " using UDP over port "
         << (server == "A" ? BACKEND_PORT : BACKEND_PORT_B) << "." << endl;
      }

      // recv message from target server
      if (sentToA) {
         string studentInfo = recvMsg(server_socket, backend_address, backend_address_length, "A", BACKEND_PORT);
         studentInfo = studentInfo.substr(1, studentInfo.length() - 1);
         cout << "There are " << numStudents(studentInfo) << " distinct students in " << name << ". \nTheir IDs are: "
         << studentInfo << endl;
      }
      else if (sentToB) {
         string studentInfo = recvMsg(server_socket, backend_address_b, backend_address_length_b, "B", BACKEND_PORT_B);
         studentInfo = studentInfo.substr(1, studentInfo.length() - 1);
         cout << studentInfo << endl;
         cout << "There are " << numStudents(studentInfo) << " distinct students in " << name << ". \nThere IDs are: "
              << studentInfo << endl;
      }
      cout << "-----Start a new query-----" << endl;
   }

   close(server_socket);  // close the server socket
   return 0;
}


void sendMsg(int server_socket, string msg, sockaddr_in backend_address, socklen_t backend_address_length, string backendName) {
   if (sendto(server_socket, msg.c_str(), msg.length(), 0,
              (struct sockaddr *)&backend_address, backend_address_length) == -1) {
      cerr << "Error: Failed to send reply to backend server " << backendName << "." << endl;
   }
}

string recvMsg(int server_socket, sockaddr_in backend_address, socklen_t backend_address_length, string backendName, int PORT) {
   char message[1024];
   int message_length = recvfrom(server_socket, message, sizeof(message), 0,
                                 (struct sockaddr *)&backend_address, &backend_address_length);
   if (message_length == -1) {
      cerr << "Error: Failed to receive message from backend server " << backendName << "." << endl;
      return NULL;
   }
   // process the message received from the backend server
   string recvMsg(message,message_length - 1);
   // cout << recvMsg << endl;
   return recvMsg;
}

void rememberDeptAndServer(string deptList, const char *backendName, unordered_map<string, string> & deptFromServer) {
   stringstream ss(deptList);
   string dept;
   while (getline(ss, dept)) {
      deptFromServer[dept] = backendName;
   }
}

int numStudents(const string& studentList) {
   int count = 1;
   for (auto c : studentList) {
      if (c == '\t') {
         // if the character is a tab, increment the count of substrings
         count++;
      }
   }
   return count;
}