# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# List of executables
EXES = crc_tx crc_rx crc_vs_parity2d

# List of object files for each executable
crc_tx_OBJS = crc_tx.o
crc_rx_OBJS = crc_rx.o
crc_vs_parity2d_OBJS = crc_vs_parity2d.o

# Default target
all: $(EXES)

# Compile .cpp files to .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

# Link object files to form executables
crc_tx: $(crc_tx_OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@
crc_rx: $(crc_rx_OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@
crc_vs_parity2d: $(crc_vs_parity2d_OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

# Clean up object files and executables
clean:
	rm -f $(crc_tx_OBJS) $(crc_rx_OBJS) $(crc_vs_parity2d_OBJS) $(EXES)

