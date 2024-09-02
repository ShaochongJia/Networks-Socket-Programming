//
// Created by Shaochong Jia on 2/4/23.
//

#include "crc_tx.h"
/**
 * Print the data bits
 * @param type "CRC" or "Codeword"
 * @param number string that contains the data
 */
void printNum(string type, string number) {
   cout << type << " is:" <<endl;
   cout << number << endl;
}

/**
 * Read the file line by line and compute the CRC for each line of data
 * @param fileName input file that contains the original data
 */
void encodeFile(string fileName) {
   ifstream file(fileName);
   //add error handling for a more elegant terminal output
   if (! file.is_open()) {
      throw runtime_error("File failed to open: " + fileName);
   }
   string data;
   // read file line by line into data that will be processed
   while (getline(file, data)) {
      string crc = computeCRC(data, generator);
      string codeWord = data + crc;
      printNum("Codeword", codeWord);
      printNum("CRC", crc);
      cout << endl;
   }
}

/**
 * Compute CRC bits to be added to original data string
 * @param data string that contains bits of 0's and 1's
 * @param generator a fixed string used as generator in the CRC process
 * @return Remainder of the division: data/generator
 */
string computeCRC(string data, string generator) {
   // number of bits to append on message
   int r = generator.length() - 1;
   int d = data.length();
   // append r bits of 0 to data
   string codeWord = data + string(r, '0');
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


int main() {
   try {
      encodeFile("pa1_data/dataTx.txt");
   }
   catch (const exception& e) {
      cerr << "Error: " << e.what() << endl;
   }
}