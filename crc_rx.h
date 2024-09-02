//
// Created by Shaochong Jia on 2/4/23.
//

#ifndef PA1_CRC_RX_H
#define PA1_CRC_RX_H
#include <iostream>
#include <string>
#include <fstream>
#include "crc_tx.h"
using namespace std;

//const string generator = "10001000000100001";
string decodeCRC(string input, string generator);
void decodeFile(string fileName);
bool allZero(string data);


#endif //PA1_CRC_RX_H
