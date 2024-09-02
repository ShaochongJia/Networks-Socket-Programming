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

#define MAIN_IP "127.0.0.1"  // IP address of the main server
#define MAIN_PORT 32574  // port number used by the main server
#define BACKEND_PORT 30574  // port number used by the backend server

using namespace std;

unordered_map<string, unordered_set<int>> readFile(ifstream &file);

int main() {
   int backend_socket = socket(AF_INET, SOCK_DGRAM, 0);  // create a UDP socket
   if (backend_socket == -1) {
      cerr << "Error: Failed to create backend socket." << endl;
      return -1;
   }

   struct sockaddr_in backend_address;
   backend_address.sin_family = AF_INET;
   backend_address.sin_addr.s_addr = INADDR_ANY;
   backend_address.sin_port = htons(BACKEND_PORT);

   // bind the socket to the backend address and port
   if (bind(backend_socket, (struct sockaddr *)&backend_address, sizeof(backend_address)) == -1) {
      cerr << "Error: Failed to bind backend socket." << endl;
      return -1;
   }

   struct sockaddr_in main_address;
   main_address.sin_family = AF_INET;
   main_address.sin_addr.s_addr = inet_addr(MAIN_IP);
   main_address.sin_port = htons(MAIN_PORT);

   cout << "Server A is up and running using UDP on port " << BACKEND_PORT << endl;
   // read file
   ifstream file("dataA.txt");
   unordered_map<string, unordered_set<int>> deptToID = readFile(file);

   //organize the list of depts
   string deptList;
   for (auto pair : deptToID) {
      deptList += pair.first + "\n";
   }

   char message[1024];
   while (true) {
      socklen_t main_address_length = sizeof(main_address);
      // receive request from the main server
      int message_length = recvfrom(backend_socket, message, sizeof(message), 0,
                                    (struct sockaddr *)&main_address, &main_address_length);
      if (message_length == -1) {
         cerr << "Error: Failed to receive message from main server." << endl;
         break;
      }
      // process the request received from the main server
      string rcvd(message);

      if (message_length == 0) {
         // send the processed result back to the main server
         if (sendto(backend_socket, deptList.data(), deptList.length(), 0,
                    (struct sockaddr *)&main_address, main_address_length) == -1) {
            cerr << "Error: Failed to send result to main server." << endl;
            break;
         }
         cout << "Server A has sent a department list to Main Server." << endl;
      }
      //recv dept name from main server
      else if (isalpha(message[0])){
         string name = rcvd.substr(0, message_length);
         cout << "Server A has received request for " << name << "." << endl;
         unordered_set<int> IDs = deptToID[name];
         int numStudents = IDs.size();
         string msgToMain = to_string(numStudents);
         cout << "Server A has found " << numStudents << " distinct students from " << name << ": ";
         for (auto id : IDs) {
            cout << id << "\t";
            msgToMain += to_string(id) + "\t";
         }

         //send to main server
         if (sendto(backend_socket, msgToMain.data(), msgToMain.length(), 0,
                    (struct sockaddr *)&main_address, main_address_length) == -1) {
            cerr << "Error: Failed to send result to main server." << endl;
            break;
         }
         cout << endl << "Server A has sent results to Main Server." << endl;
      }
   }
   close(backend_socket);  // close the backend socket
   return 0;
}

unordered_map<string, unordered_set<int>> readFile(ifstream &file) {
   if (! file.is_open()) {
      cout << "Error opening file. Please check if file is in the project directory." << endl;
      exit(1);
   }
   unordered_map<string, unordered_set<int>> deptToIDs;
   string line;
   string dept;
   while(getline(file, line)) {
      // a line that contains alphabetic char is the dept ID
      if (isalpha(line[0])) {
         stringstream(line) >> dept;
      }
         // read student IDs separated with ,
      else {
         stringstream ss(line);
         string ID;
         //separate line into dept names
         while (getline(ss, ID, ',')) {
            //remove '\r' from dept names
            size_t pos = dept.find('\r');
            if (pos != string::npos) {
               dept.erase(pos, 1);
            }
            deptToIDs[dept].emplace(stoi(ID));
         }
      }
   }
   file.close();
   return deptToIDs;
}
