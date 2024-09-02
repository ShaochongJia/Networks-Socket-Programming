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
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>

#define LOCAL_HOST "127.0.0.1"  // IP address of the main server
#define MAIN_PORT 32574  // port number used by the main server
#define BACKEND_PORT 30574  // port number used by the backend server
#define FAILURE -1
#define MAXDATASIZE 1024

using namespace std;

int backend_socket;
sockaddr_in backend_address;

unordered_map<string, unordered_set<string>> deptStudentIDs; //deptID -> set of studentID
unordered_map<string, vector<int>> studentGrades; //studentID -> list of all student grades
string deptList;
unordered_map<string, string> studentGPA; //studentID -> studentGPA
unordered_map<string, vector<double>> deptGPAs; //deptID -> list of all deptGPAs
unordered_map<string, string> deptStats; //deptID -> statistics in [mean, var, max, min]
double rankPercentage;


vector<string> split(string s, char delimiter);
void readFile(ifstream &file, unordered_map<string,  unordered_set<string>>& deptStudentIDs,
         unordered_map<string, vector<int>>& studentGrades);
double computeStudentGPA(vector<int> grades);
void server_boot_up();
string deptGPAStatistics(const vector<double> GPAs);
string studentRank(string dept, string studentID);
string round_to_one_decimal(double number);
double euclidean_distance(vector<int> gradesA, vector<int> gradesB);
string recommendFriend(string targetStudent);



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
   if (bind(backend_socket, (struct sockaddr *)&backend_address, sizeof(backend_address)) == FAILURE) {
      cerr << "Error: Failed to bind backend socket." << endl;
      return -1;
   }

   struct sockaddr_in main_address;
   main_address.sin_family = AF_INET;
   main_address.sin_addr.s_addr = inet_addr(LOCAL_HOST);
   main_address.sin_port = htons(MAIN_PORT);

   cout << "Server A is up and running using UDP on port " << BACKEND_PORT << endl;
   server_boot_up();


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
      //recv query from main server
      else {
         vector<string> dept_and_student = split(rcvd.substr(0, message_length), '\t');
         string dept = dept_and_student[0];
         string studentID = dept_and_student[1];
         cout << "Server A has received request for Student " << studentID << " in " <<
         dept << "." << endl;

         string msgToMain;
         if (! deptStudentIDs[dept].count(studentID)) {
            msgToMain = "Student " + studentID + " : not found.\n";
            cout << endl << "Server A has sent \"Student " << studentID << " not found\" to Main Server." << endl;
         }
         else {
            //prepare the message to main server
            msgToMain= studentGPA[studentID] + "\t";
            msgToMain += studentRank(dept, studentID) + "\t" + deptStats[dept];
            cout << "Server A calculated following academic statistics for Student " << studentID <<
                 " in " << dept << endl;
            vector<string> allInfo = split(msgToMain, '\t');
            cout << "Student GPA: " << allInfo[0] << endl;
            cout << "Percentage Rank: " << allInfo[1] << endl;
            cout << "Department GPA Mean: " << allInfo[2] << endl;
            cout << "Department GPA Variance: " << allInfo[3] << endl;
            cout << "Department GPA Max: " << allInfo[4] << endl;
            cout << "Department GPA Min: " << allInfo[5] << endl;
            cout << endl << "Server A has sent results to Main Server." << endl;
            msgToMain += recommendFriend(studentID) + " ";
         }
         //send to main server
         if (sendto(backend_socket, msgToMain.data(), msgToMain.length(), 0,
                    (struct sockaddr *)&main_address, main_address_length) == -1) {
            cerr << "Error: Failed to send result to main server." << endl;
            break;
         }
      }
   }

   close(backend_socket);  // close the backend socket
   return 0;
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



void readFile(ifstream &file, unordered_map<string,  unordered_set<string>>& deptStudentIDs, unordered_map<string, vector<int>>& studentGrades) {
   if (! file.is_open()) {
      cout << "Error opening file. Please check if file is in the project directory." << endl;
      exit(1);
   }
   string line;
   //dump the header line
   getline(file, line);
   while(getline(file, line)) {
      string dept;
      string studentID;
      vector<int> grades;
      string grade;
      stringstream ss(line);
      // push the first string to dept
      getline(ss, dept, ',');
      // 2nd string to student id
      getline(ss, studentID, ',');
      // add rest of the values as grades, into grades vector
      while (getline(ss, grade, ',')) {
         if (grade.empty() || grade == "\r") {
            grades.push_back(-1);
         }
         else {
            grades.push_back(stoi(grade));
         }
      }
      // add student ID to dept student ID list
      deptStudentIDs[dept].insert(studentID);
      // add grades to student info
      studentGrades[studentID] = grades;
   }
   file.close();
}

double computeStudentGPA(vector<int> grades) {
   int size = 0;
   double sum = 0;
   for (int grade : grades) {
      if (grade != -1) {
         sum += grade;
         size++;
      }
   }
   return round(sum / size * 10.0) / 10;
}

void server_boot_up() {
   // read file
   ifstream file("dataA.csv");

   readFile(file, deptStudentIDs, studentGrades);

   // organize the list of depts for main server boot up print

   for (auto pair : deptStudentIDs) {
      deptList += pair.first + "\n";
   }

   // compute necessary statistics

   // 1. compute student GPA and record into dept
   for (auto pair : deptStudentIDs) {
      string dept = pair.first;
      double gpa = 0;
      for (auto id : pair.second) {
         gpa = computeStudentGPA(studentGrades[id]);
         studentGPA[id] = round_to_one_decimal(gpa);
         deptGPAs[dept].push_back(gpa);
      }
      sort(deptGPAs[dept].begin(), deptGPAs[dept].end());
   }
   // 2. dept statistics
   for (const auto& pair : deptGPAs) {
      deptStats[pair.first] = deptGPAStatistics(pair.second);
   }
}

string deptGPAStatistics(const vector<double> GPAs) {
   // mean, var, max, min
   double statistics[4] = {0, 0, -1.0, 101.0};

   // find min, max
   for (double gpa : GPAs) {
      if (gpa > statistics[2]) {
         statistics[2] = gpa;
      }
      if (gpa < statistics[3]) {
         statistics[3] = gpa;
      }
      statistics[0] += gpa;
   }
   // calculate mean
   statistics[0] /= GPAs.size();
   // calculate variance
   for (double gpa : GPAs) {
      statistics[1] += pow(gpa - statistics[0], 2);
   }
   statistics[1] /= GPAs.size();

   string stats_string;
   for (double item : statistics) {
      stats_string += round_to_one_decimal(item) + "\t";
   }
   return stats_string;
}

string studentRank(string dept, string studentID) {
   double gpa = stod(studentGPA[studentID]);
   int higherThan = 0;
   vector<double> deptGPA = deptGPAs[dept];
   int numStudents = deptGPA.size();
   while (gpa > deptGPA[higherThan]) {
      higherThan++;
   }
   string perctenage =  numStudents == 1 ? round_to_one_decimal(0) :
   round_to_one_decimal(higherThan * 100.0 / numStudents);
   return perctenage + "%";
}

string round_to_one_decimal(double number) {
   stringstream ss;
   ss << fixed << setprecision(1) << round(number * 10) / 10.0;
   return ss.str();
}

double euclidean_distance(vector<int> gradesA, vector<int> gradesB) {
   double sum = 0;
   for (int i = 0; i < gradesA.size(); ++i) {
      sum += pow((gradesA[i] - gradesB[i]), 2);
   }
   return sqrt(sum);
}

string recommendFriend(string targetStudent) {
   string friendID;
   double curr_dist = 1000.0;
   vector<int> grades_target = studentGrades[targetStudent];
   //for all departments, find all students whose grades have min euclidean distance with target student
   for (auto& pair : deptStudentIDs) {
      unordered_set<string> students = pair.second;
      for (auto& other : students) {
         vector<int> grades_other = studentGrades[other];
         if (euclidean_distance(grades_target, grades_other) < curr_dist && targetStudent != other) {
            curr_dist = euclidean_distance(grades_target, grades_other);
            friendID = other;
         }
      }
   }
   return friendID;
}