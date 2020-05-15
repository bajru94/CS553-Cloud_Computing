Program Information
There are 3 codes in this repository:
1. MySort.java - This can be used to perform external merge sort on larger than emory datasets and also in memory sort for the ones which fit
2. HadoopSort.java - This can be used to perform total order sort in Hadoop.
3. SparkSort.java - This can be used to perform sort in Spark platform.

DATASET GENERATION:
The user can generate the data set using Gensort by using the below command:

./gensort -a no_of_records file_name &

In the above command, user has to give two inputs:
1. No. of Records - This is the record count that the gensort will generate and each record is 100 bytes
2. File Name - Name of the file you want to keep for the data generated

COMPILATION:
There is makefile included in the program which can be used to compile it. You can use the below command to compile it:
make

EXECUTION:
1. MySort.java

Note - Only use OpenJDK 11 to run MySort.java. You can install it by using "sudo apt install openjdk-11-jdk"

Now to run the program, please use the below command:
java MySort.java input_file output_file thread_cnt 

There are three inputs here:
1. Input File Name - File containing the input dataset
2. Output File Name - Output file to store the sorted dataset
3. No. of Threads - Deafault value is 4. If not specifiecd that will be used.

User can also add a flag with javac to limit the memory size available which is Xmx(memory size). Suppose the user wants to limit the memory size to 8gb, then the below command will help in doing so:
java -Xmx8g MySort.java input_file output_file thread_cnt 

OUTPUT FORMAT:
The output will contain the following information:
1. Total time taken
2. Data read
3. Data written

Note:- Please make sure when you are using the program to sort a very large file. In those cases, external sort will be used to sort and temporary files whose total size equal to the input file will be created and thus that much extra space should be available. All the temp files will be deleted once the process finishes.

2. HadoopSort.java
Note - Only use OpenJDK 8 while setting up Hadoop. You can install it by using "sudo apt install openjdk-8-jdk"
Use the configuration files attached in the repository to get similar performance

Compile the code using:
bin/hadoop com.sun.tools.javac.Main HadoopSort.java
jar cf HadoopSort.jar HadoopSort*.class

And it can be run by:
bin/hadoop jar HadoopSort.jar HadoopSort /home/input/ /home/output

/home/input/ is the input folder
/home/output/ is the output folder. Please make sure that output folder is not present before running. Hadoop will make one itself and throw error if already present.

3. SparkSort.java
Use the configuration files attached in the repository to get similar performance

Compile the code using:
javac -cp jars/spark-core_2.12-3.0.0-preview2.jar:jars/scala-library-2.12.10.jar SparkSort.java
jar cvf SparkSort.jar SparkSort.class

And it can be run by:
bin/spark-submit --class SparkSort --master yarn --deploy-mode cluster --driver-memory 4g --executor-memory 2g --executor-cores 4 --num-executors 4 SparkSort.jar /home/input/input.txt /home/output/

The parameters can be set based on users machine and preeference to acheive better performance

/home/input/input.txt is the path to and name of the input file
/home/output/ is the output folder. Please make sure that output folder is not present before running. Spark will make one itself and throw error if already present.

The sorted file can be verified using the valsort which comes with gensort using the below command:
./valsort file_name
Valsort will give the number of records and checksum and tell in case the data is not sorted. 

To use Linux sort, below commands can be used:
LC_ALL=C sort --buffer-size=memory --parallel=no_of_threads input_file -o output_file

Here, there are multiple options that can be passed by the user:
1. Memory - Size of the memory you want sort to use
2. Parallel - No of threads 
3. Input File Name
4. Output File Name
5. LC_ALL=c forces the system to do sorting bytewise.

Example Query:
LC_ALL=C sort --buffer-size=8g --parallel=16 input_64.txt -o out_64.txt &

The above command will sort a file named input_64.txt using 8GB of ram and 16 threads and place the output in out_64.txt






