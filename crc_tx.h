//
// Created by Shaochong Jia on 2/4/23.
//

#ifndef PA1_CRC_TX_H
#define PA1_CRC_TX_H
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

const string generator = "10001000000100001";
void encodeFile(string fileName);
string computeCRC(string data, string generator);



#endif //PA1_CRC_TX_H
