# ABElectronics_IOPi
C Library to interface with ABElectronics IO Pi Plus...

This library makes a simple and easy interface to [ABElectronics IO Pi Plus](https://www.abelectronics.co.uk/p/54/IO-Pi-Plus).

In order to compile and test, please do this:
 1. download the repo
 2. make (if you want to install on your system, after make, just type make install)
 3. gcc IOPItest.c -o IOPItest -L. -lABEIOPI
 4. LD_LIBRARY_PATH=/home/pi/ABElectronics/IOPI/ (or any other folder)
 5. export LD_LIBRARY_PATH
 6. ./IOPItest
