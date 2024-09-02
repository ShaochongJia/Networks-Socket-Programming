//
// Created by Shaochong Jia on 2/4/23.
//

#ifndef PA1_CRC_VS_PARITY2D_H
#define PA1_CRC_VS_PARITY2D_H
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
//#include "crc_tx.h"
//#include "crc_rx.h"
using namespace std;

const int SIZE = 8;

string flip(string data, string err);
void processFile(string fileName);
string addParity2D(const string &input);
void printResult(bool condition, int width = 20);
bool allZero(string data);

string computeCRC(string data, string generator);
string decodeCRC(string input, string generator);

#endif //PA1_CRC_VS_PARITY2D_H
