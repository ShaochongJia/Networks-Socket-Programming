//
// Created by Shaochong Jia on 2/4/23.
//

#include "crc_vs_parity2d.h"

bool checkParity2D(string message);

/**
 * Flip the original data according to random error bits
 * @param data string that contains original data
 * @param err string that contains random error bits
 * @return the flipped data as string
 */
string flip(string data, string err) {
   int size = min(data.length(), err.length());
   string err_data = data;
   for (int i = 0; i < size; ++i) {
      // what a flipped bit should be for 0 and 1
      char flipped = (data[i] == '0') ? '1' : '0';
      // if error bits is 0, do not flip; if it's 1, replace the bit with flipped bit
      err_data[i] = (err[i] == '0') ? err_data[i] : flipped;
   }
   return err_data;
}

/**
 * Read data from file line by line, and perform 2-d parity check and CRC check for each line
 * @param fileName file that contains data
 */
void processFile(string fileName) {
   const string generator = "10001000000100001";
   //read the file line by line
   ifstream file(fileName);
   string line, data, err;
   while(getline(file, line)){
      // separate line into data and error bits
      istringstream sstream(line);
      sstream >> data >> err;
      // do parity2D
      string message = addParity2D(data);
      // separate the message into col and row parity bits, and transmitted data
      string col;
      string row;
      istringstream bitStream(message);
      bitStream >> col >> row >> message;
      cout << "2D Parity: Col: " << col << "; Row: " << row << ";";
      // flip data per error bits
      string flippedMsg = flip(message, err);
      // check the flipped message and see if 2d parity passed
      // output with alignment
      printResult(checkParity2D(flippedMsg), 20);

      // do CRC
      string CRC = computeCRC(data, generator);
      cout << "CRC: " << CRC << ";";
      string originalWord = data + CRC;
      string flippedWord = flip(originalWord, err);
      string flippedCRC = decodeCRC(flippedWord, generator);
      // if data contains no error bits, the CRC in the receiver side should be all 0
      printResult(allZero(flippedCRC), 39);

      cout << "=========================================" << endl;
   }
}

/**
 * check the parity 2-d matrix at receiver side
 * @param message the transmitted message that contains the parity bits
 * @return true iff the 2-d parity of the transmitted message is satisfied
 */
bool checkParity2D(string message) {
   // decompose message to 9x9 2d matrix
   char matrix[SIZE+1][SIZE+1];
   int index = 0;
   for (int i = 0; i < SIZE + 1; ++i) {
      for (int j = 0; j < SIZE + 1; ++j) {
         matrix[i][j] = message[index++];
      }
   }
   //check if message contains any flipped bits using 2d
   for (int i = 0; i < SIZE; ++i) {
      bool rowEven = true;
      bool colEven  = true;
      for (int j = 0; j < SIZE; ++j) {
         // traverse horizontally
         if (matrix[i][j] == '1') {
            // if it's 1, change the previous state
            rowEven ^= true;
         }
         // traverse vertically
         if (matrix[j][i] == '1') {
            // if it's 1, change the previous state
            colEven ^= true;
         }
      }
      // if data bits is even and parity bit is 1, or data bits is odd and parity bit is 0
      // parity check fails
      if ((rowEven && matrix[i][SIZE] == '1') || (colEven && matrix[SIZE][i] == '1')
      ||(!rowEven && matrix[i][SIZE] == '0') || (!colEven && matrix[SIZE][i] == '0')) {
         return false;
      }
   }
   return true;
}

/**
 * Add parity bits into original data
 * @param input string containing original data
 * @return the strings containing info in the following format:
 * column parity + " " + row parity + " " + entire message
 */
string addParity2D(const string &input) {
   // arrays used to store row and column parity bits
   int rowPar[SIZE + 1] = {0};
   int colPar[SIZE] = {0};
   int index = 0;
   char result[SIZE+1][SIZE+1];
   // add each bit of the input into the 2-d matrix, meanwhile update the parity
   for (int i = 0; i < SIZE; ++i) {
      for (int j = 0; j < SIZE; ++j) {
         char bit = input[index++];
         result[i][j] = bit;
         // update the row and column bits
         // if bit is one, change the row and column parity
         rowPar[i] = (bit == '1'? rowPar[i] ^ 1 : rowPar[i]);
         colPar[j] = (bit == '1'? colPar[j] ^ 1 : colPar[j]);
      }
   }
   // write the final bit of row parity, by counting column parity
   for (int i = 0; i < SIZE; ++i) {
      rowPar[SIZE] = (colPar[i] == 1? rowPar[SIZE] ^ 1 : rowPar[SIZE]);
   }
   //write row and col parity bits to matrix
   for (int i = 0; i < SIZE + 1; ++i) {
      result[i][SIZE] = rowPar[i] + '0';
   }
   for (int i = 0; i < SIZE; ++i) {
      result[SIZE][i] = colPar[i] + '0';
   }
   // write rowPar and colPar to string
   string row;
   string col;
   for (int i = 0; i < SIZE; ++i) {
      row.push_back(rowPar[i] + '0');
      col.push_back(colPar[i]+ '0');
   }
   row.push_back(rowPar[SIZE]+ '0');

   //write entire message into string
   string message;
   for (int i = 0; i < SIZE + 1; ++i) {
      for (int j = 0; j < SIZE + 1; ++j) {
         message.push_back(result[i][j]);
      }
   }
   return col + " " + row + " " + message;
}


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

/**
 * print the output
 * @param condition boolean value that affects the output
 * @param width the width of alignment of output
 */
void printResult(bool condition, int width) {
   if (condition) {
      cout << setw(width) << "Result: Pass" << endl;
   }
   else {
      cout << setw(width) << "Result: Not Pass" << endl;
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
   processFile("pa1_data/dataVs.txt");
}
