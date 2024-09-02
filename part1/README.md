# Student and Project info
1. Name:  
2. Student ID: 
3. Platform: Ubuntu 22.04


# Introduction

This project contains three files, each of which serves a different purpose in a communication system that 
involves transmitting and receiving data. The aim of this project is to implement and compare two error detection 
techniques, namely, CRC (Cyclic Redundancy Check) and 2D parity check (Parity2D).

# File Descriptions

1. crc_tx.h and crc_tx.cpp
   > The crc_tx.h header file and crc_tx.cpp source file are used to generate the transmitter side CRC bits and codeword 
given an input string of 0's and 1's from a dataTx.txt file. The generated codeword and corresponding CRC bits are 
then printed out.

2. crc_rx.h and crc_rx.cpp
   > The crc_rx.h header file and crc_rx.cpp source file are used to read the received data at the receiver side. 
The CRC_Rx function will then print out the decision whether the received data should be accepted (Pass) or not (Not Pass) 
based on the error detection using CRC.

3. crc_vs_parity2d.h and crc_vs_parity2d.cpp
   > The crc_vs_parity2d.h header file and crc_vs_parity2d.cpp source file are used to compare the performance of the two 
error detection techniques, CRC and Parity2D, for some example data from dataVs.txt. The results of the comparison are 
then printed out.

4. MakeFile
   > Make file supports following functions:

   > 1. **make all**: Compile all your files, create executable files, and run all three executables
   > 2. **./crc_tx**: Run CRC_Tx
   > 3. **./crc_rx**: Run CRC_Rx
   > 4. **./crc_vs_parity2d**: Run CRC_vs_Parity2D


# Libraries Included
The project has included the following libraries necessary for functionalities:

   > 1. iostream: for console output
   > 2. string: for string processing and accessing 
   > 3. fstream: for input file reading
   > 4. sstream: to break lines read from file into separate data and error strings
   > 5. iomanip: for output formatting

# Data Used
The project has 3 input data from text file saved in the same directory as the project home directory:
>../pa1_data

The 3 data files are:
1. pa1_data/dataTx.txt: transmitter-side CRC test data
2. pa1_data/dataRx.txt: receiver-side CRC test data
3. pa1_data/dataVs.txt: contains data and random error bits that are used to test the functionalities of CRC and 2-d parity 
error checking. 

# Usage
Before compiling and running, please make sure the data files are in the same directory as project home directory, as following:
>../pa1_data

To compile the project, run the following command in the terminal:

    make all

To run the project, use the following command:

    ./crc_tx
    ./crc_rx
    ./crc_vs_parity2d


# Conclusion

In this project, we have implemented and compared two error detection techniques, CRC and Parity2D, in a communication system. 
The generated code and the results of the comparison can be used as a reference for further studies and applications in 
error detection and correction.

# Idiosyncrasy
To my best knowledge, the project codes works with all the data provided and works across different terminal platforms.


# Notes for Academic Integrity
In this project, I have written crc_tx, crc_rx and crc_vs_parity2d, containing _.h_ and _.cpp_ files by myself. However, with 
limited knowledge on makefile, to allow **make all** function, I had referred to chatGPT examples for **only** the structure of 
compiling and linking, and apply it to this project. 
# Networks-Socket-Programming
