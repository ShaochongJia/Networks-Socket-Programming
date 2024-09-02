//
// Created by Shaochong Jia on 2/4/23.
//

#include "crc_rx.h"
#include "crc_vs_parity2d.h"


/**
 * decode one piece of data
 * @param input string read from file
 * @param generator string used as divisor
 * @return remainder of the division
 */
string decodeCRC(string input, string generator) {
   // number of bits to append on message
   int r = generator.length() - 1;
   int d = input.length();
   // append r bits of 0 to data
   string codeWord = input + string(r, '0');
   // replace the bits in code word such that if bit in code word is same as generator, replace with 0
   for (int i = 0; i < d; ++i) {
      if (codeWord[i] == '0') {
         continue;
      }
      for (int j = 0; j <= r; ++j) {
         codeWord[i + j] = (codeWord[i + j] == generator[j]) ? '0' : '1';
      }
   }
   string R = codeWord.substr(d, r);
   return R;
}


/**
 * Decode the file and detect error for each line
 * @param fileName file path to read
 */
void decodeFile(string fileName) {
   ifstream file(fileName);
   //add error handling for a more elegant terminal output
   if (! file.is_open()) {
      throw runtime_error("File failed to open: " + fileName);
   }
   string data;
   // read file line by line into data that will be processed
   while (getline(file, data)) {
      string remainder = decodeCRC(data, generator);
      if (allZero(remainder)) {
         cout << "Pass" << endl;
      }
      else {
         cout << "Not Pass" << endl;
      }
   }
}

/**
 * check if string contains only 0
 * @param data
 * @return true iff data contains only 0
 */
bool allZero(string data) {
   for (auto c : data) {
      if (c == '1') {
         return false;
      }
   }
   return true;
}

int main() {
   try {
      decodeFile("pa1_data/dataRx.txt");
   }
   catch (const exception& e) {
      cerr << "Error: " << e.what() << endl;
   }
}
