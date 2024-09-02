#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string>
#include <vector>

#define MAIN_SERVER_IP "127.0.0.1"
#define PORT 33574
#define MAXDATASIZE 1024

using namespace std;

// retrieve the input department names
string getClientInput();

vector<string> split(string s, char delimiter);

int main() {
   int sockfd, numbytes;
   struct sockaddr_in server_address, client_address;
   socklen_t client_address_len = sizeof(client_address);
   char buf[MAXDATASIZE];

   // Creating socket file descriptor
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0) {
      std::cout << "Socket creation failed" << std::endl;
      return -1;
   }

   // Setting up server address
   memset(&server_address, 0, sizeof(server_address));
   server_address.sin_family = AF_INET;
   server_address.sin_addr.s_addr = inet_addr(MAIN_SERVER_IP);
   server_address.sin_port = htons(PORT);

   // Connecting to main server
   if (connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
      std::cout << "Connection failed" << std::endl;
      return -1;
   }

   // Getting client address and port number
   getsockname(sockfd, (struct sockaddr *)&client_address, &client_address_len);
   char client_ip[INET_ADDRSTRLEN];
   inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
   int client_port = ntohs(client_address.sin_port);

   cout << "Client is up and running." << endl;
   while (1) {

      cout << "Enter Department Name: " ;
      string deptName = getClientInput();
      cout << "Enter student ID: " ;
      string studentID = getClientInput();
      string query = deptName + "\t" + studentID;
      if (send(sockfd, query.c_str(), query.length(), 0) == -1)
         perror("send");

      cout << "Client has sent Department " << deptName <<" and Student " << studentID
           << " to Main Server using TCP over port " << client_port << endl;

      if ((numbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
         perror("recv");
         exit(1);
      }
      buf[numbytes] = '\0';
      string message = string(buf);
      if (message.find("not") != string::npos) {
         cout << message << endl;
      }
      else {
         cout << "Client has received results from Main Server: \n" << endl;
         vector<string> allInfo = split(message, '\t');
         cout << "Student GPA: " << allInfo[0] << endl;
         cout << "Percentage Rank: " << allInfo[1] << endl;
         cout << "Department GPA Mean: " << allInfo[2] << endl;
         cout << "Department GPA Variance: " << allInfo[3] << endl;
         cout << "Department GPA Max: " << allInfo[4] << endl;
         cout << "Department GPA Min: " << allInfo[5] << endl;
         cout << "Friend Recommendation:  " << allInfo[6] << endl;
      }


      cout << "------Start a new query------" << endl;

   }
   // Closing socket
   close(sockfd);

   return 0;
}

string getClientInput() {
   string input;
   cin >> input;
   return input;
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