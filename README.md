To compile the two programs (P1 and P2) provided above, you can follow these steps:

Install the necessary tools:

For Ubuntu or Debian-based distributions, run:
 ->  sudo apt update
 -> sudo apt install build-essential

Open a terminal, navigate to the folder containing the source files, and compile the programs using the following commands:
 -> g++ p1.cpp -o p1 -lrt -pthread
 -> g++ p2.cpp -o p2 -lrt -pthread

After compiling both programs, you should have two executable files: p1 and p2.

To execute the programs, open two separate terminal instances.

In the first terminal, run the P1 program with the input file name and the target word as arguments. For example, if your input file is called input.txt and the target word is "world", run:
 -> ./p1 file.txt world
The P1 program will read the input file, store it in shared memory, and wait for the P2 program to process it.

In the second terminal, run the P2 program with the same arguments as P1:
 -> ./p2 file.txt world
The P2 program will read the content from shared memory, process it using the mapper and reducer functions, and write the result back to shared memory.

Once the P2 program finishes executing, switch back to the first terminal where P1 is running. P1 will now display the lines containing the target word.
Remember to replace input.txt and world with the appropriate file name and target word for your use case.