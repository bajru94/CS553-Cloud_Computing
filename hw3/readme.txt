Program Information
There is one C program in the project folder. It can be utilised to perform benchmarking of the hard disk based on various parameters over a 10 GB file such as:
1. Record Size
2. No of Threads
3. Operation type

Folder Structure:
/team-8/hw3/
├── Data
│   ├── 1
│   ├── 2
│   ├── 4
│   ├── 8
│   ├── 12
│   ├── 24
│   └── 48
├── disk.c
├── makefile
└── readme.txt


DATASET GENERATION:
The user can either use a pre-existing dataset or generate a new one from the snippet below:

for i in {1..24}
do
	dd if=/dev/urandom of=$i bs=100 count=10
done
In the above shell script:
1. The dd command runs the 24 times(one for each thread in a 24 thread experimentation), user will need to change it based on for how many thread the dataset is being generated
2. bs=100 & count=10 decide the total size of each file. Basically 100*1(bs*count) gives the size of each file in bytes. It will be required to be changes based on the user requirement. For ex - if the user requires a single 10GB file, then he can keep bs=4M and count=2560

Note: - The files for a particular thread experimentation are supposed to go into that number folder in the Data folder. For ex - The files for 1 thread experiment should go in the Folder /Data/1 and for 24 thread experiment, /Data/24 etc.
Also, all the folders should pe populated with their respective datasets before executing the program.

COMPILATION:
There is makefile included in the program which can be used to compile it. Please go the project folder and use the below command to compile it
make

EXECUTION:
Now to run the program, please use the below command:
./disk

OUTPUT FORMAT:
RS: Threads=1, Block_Size(in Bytes)=65536, Time=0.119678

Here RS means Read Sequential, similarly we have:
RR: Random Read
WS: Write Sequential
WR: Write Random

Time is given in seconds







