import java.io.*;
import java.util.*;

public class MySort {
	
	static String input_file = "";
	static String output_file = "";
	static int thread_cnt = 4;
	static long file_size =-1;

	//This method is used to calculate the maximum memory JVM can use at a time
	public static long max_aval_mem() {
		System.gc();
		Runtime r = Runtime.getRuntime();
		return r.maxMemory() - (r.totalMemory() - r.freeMemory());
	}

	public static void main(String[] args) throws IOException, InterruptedException {

		if(args.length == 3) {
			input_file = args[0];
			output_file = args[1];
			thread_cnt = Integer.parseInt(args[2]);
			
		}else if(args.length == 2){
			input_file = args[0];
			output_file = args[1];
			System.out.println("No input for the number of threads.");
			System.out.println("Default no of threads equal to 4 will be used");
		}else {
			try {
				throw new Exception("Incorrect no. of arguments. Please enter Input & Output file name & no. of threads if required respectively");
			} catch (Exception e) {
				
  			System.out.println("Error: " + e);
			}
		}
		
		long startTime = System.nanoTime();
		//Opening the Input file
		File file = new File(input_file);

		long free_memory = max_aval_mem();
		free_memory = free_memory / 2;		//Dividing by 2 since merge sort requires additional O(n) space to sort the data

		long mem_per_thread = free_memory / thread_cnt; // Memory allocated to each thread
		mem_per_thread = mem_per_thread / 100;
		mem_per_thread *= 100;

		file_size = file.length();		//Size of file in  bytes

		long file_per_thread = file_size / thread_cnt; // Portion of file which will be allocated to each thread
		file_per_thread = file_per_thread / 100;
		file_per_thread = file_per_thread * 100;


		if (file_size<free_memory) {
			
			System.out.println("Complete data can be sorted in the memory and hence in-memory merge sort will be used");

			BufferedReader br = new BufferedReader(new FileReader(file));
			ArrayList<String> arr = new ArrayList<String> ();
			String line;

			while ((line = br.readLine()) != null) {
				arr.add(line);
			}

			mergesort(arr, 0, arr.size() - 1);
			FileWriter final_out = new FileWriter(output_file);

			for (String str: arr) {
				final_out.write(str + "\r\n");
			}

			final_out.flush();
			final_out.close();
			br.close();
		} else {

			System.out.println("Complete data cannot be sorted in the memory and hence external merge sort will be used");
			
			File temp_folder = new File("temp_files/");
			temp_folder.mkdir();

			BufferedReader br = new BufferedReader(new FileReader(file));

			Thread th[] = new Thread[thread_cnt];

			long start_pos = 0;			//To keep track of starting position in file for each thread
			for (int i = 0; i<thread_cnt; i++) {
				if (i == thread_cnt - 1) {
					file_per_thread = file_size;
				}
				th[i] = new Thread(new create_chunks(start_pos, file_per_thread, mem_per_thread, file));
				th[i].start();
				start_pos += file_per_thread;
			}

			for (int i = 0; i<thread_cnt; i++) {
				th[i].join();
			}
			
			merge_sorted_files();			//Merge the smaller sorted chunks into the final sorted file
			temp_folder.delete();
		}
		long elapsedTime = System.nanoTime() - startTime;
		System.out.println("Done, total time taken for sorting: " + elapsedTime / 1000000000 + " sec");

		if(file_size<free_memory){
			System.out.println("Total Data Read: "+file_size/(1024*1024*1024)+" GB");
			System.out.println("Total Data Write: "+file_size/(1024*1024*1024)+" GB");
		} else{
			System.out.println("Total Data Read: "+2*file_size/(1024*1024*1024)+" GB");
			System.out.println("Total Data Write: "+2*file_size/(1024*1024*1024)+" GB");
		}
	}
	
	public static void merge_sorted_files() throws IOException{

		File folder = new File("temp_files/");

		File[] temp_files = folder.listFiles();
		int n = temp_files.length;

		BufferedReader[] br = new BufferedReader[n];

		TreeMap<String, Integer> mappedTree = new TreeMap<String, Integer> ();
		BufferedWriter final_out = new BufferedWriter(new FileWriter(output_file));

		for (int i = 0; i<n; i++) {
			br[i] = new BufferedReader(new FileReader(temp_files[i]));
			mappedTree.put(br[i].readLine(),i);
		}

		String line;
		long index = 0;
		int min = 0;

		while (index<file_size) {
			line = mappedTree.firstKey();
			min = mappedTree.get(line);

			final_out.write(line + "\r\n");
			mappedTree.remove(line);
			index += 100;

			if ((line = br[min].readLine()) != null) {
				mappedTree.put(line, min);
			} else {
				br[min].close();			
				temp_files[min].delete(); //Deleting temp files which have already been completely used to save on disk space
			}

		}
		final_out.flush();
		final_out.close();
			
	}
	
	public static void mergesort(ArrayList<String> arr, int l, int r) {
		if (l<r) {
			int m = l + (r - l) / 2;
			mergesort(arr, l, m);
			mergesort(arr, m + 1, r);
			merge(arr, l, m, r);
		}
	}

	public static void merge(ArrayList<String> arr, int l, int m, int r) {
		int n1 = m - l + 1;
		int n2 = r - m;

		String[] left = new String[n1];
		String[] right = new String[n2];

		for (int i = 0; i<n1; i++) {
			left[i] = arr.get(l + i);
		}
		for (int i = 0; i<n2; i++) {
			right[i] = arr.get(m + 1 + i);
		}

		int i = 0, j = 0, k = l;
		while (i<n1 && j<n2) {
			if (right[j].compareTo(left[i]) > 0) {
				arr.set(k, left[i]);
				++i;
			} else {
				arr.set(k, right[j]);
				++j;
			}
			++k;
		}

		while (i<n1) {
			arr.set(k, left[i]);
			++i;
			++k;
		}
		while (j<n2) {
			arr.set(k, right[j]);
			++j;
			++k;
		}
	}
}
//This class creates the smaller chunks of the original file which are each induvidually sorted
class create_chunks implements Runnable {

	File file;
	long start_pos;
	long tot_size;
	long temp_file_size;

	public create_chunks(long start_pos, long tot_size, long temp_file_size, File file) {
		this.start_pos = start_pos;			//Starting position for each thread in the original file
		this.tot_size = tot_size;			//Total Data that will be processed by each thread
		this.temp_file_size = temp_file_size; 		//Size of data thread can sort in-memory in a go.Max size of each divided chunk
		this.file = file;
	}

	@Override
	public void run() {

		try {

			BufferedReader br = new BufferedReader(new FileReader(file));
			br.skip(start_pos);
			String line;
			boolean flag = true;
			long curr_tot_read = 0;
			int i = 1;
			long thread_no = Thread.currentThread().getId();

			while (flag && curr_tot_read<tot_size) {
				ArrayList<String> arr = new ArrayList<String> ();
				long curr_data = 0;
				while (curr_data<temp_file_size && curr_tot_read<tot_size) {
					line = br.readLine();
					if (line == null) {
						flag = false;
						break;
					}
					arr.add(line);
					curr_data += 100;
					curr_tot_read += 100;
				}
				MySort.mergesort(arr, 0, arr.size() - 1);
				thread_write(thread_no, i, arr);
				++i;
			}
			br.close();
		} catch (Exception e) {
			e.printStackTrace();
		}

	}

	public static void thread_write(long t, int i, ArrayList<String> arr) throws IOException {

		BufferedWriter myWriter = new BufferedWriter(new FileWriter("temp_files/" + String.valueOf(t) + String.valueOf(i) + ".txt"));

		for (String str: arr) {
			myWriter.write(str +  "\r\n");
		}
		myWriter.close();
	}


}
