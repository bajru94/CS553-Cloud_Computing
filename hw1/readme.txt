There are two bash codes in the assignment:

1.generate-dataset.sh - This is used to generate a file with the name given as input by the user along with the number of records that needs to be generated. You should use the below command to use it:

bash generate-dataset.sh <File-Name> <No-of-records>
For ex - bash generate-dataset.sh team8 1000

2.sort-data.sh - This is used to sort the file generated previously on the first column. The sorted file is outputted as <Input_File_Name>_sorted.txt You should use the below command to use it:

bash sort-data.sh <File-Name>
For ex -bash sort-data.sh team8

Please note that you should not provide the extension of the file. The output for both of them is created in the same folder

There is one Python code present as well "Plot.py" which is used to plot a graph for the comparison of generation and sort time. The input needs to be manually changed in the code.
