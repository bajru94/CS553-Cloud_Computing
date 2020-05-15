import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import java.io.*;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.lib.partition.*;
import org.apache.hadoop.mapreduce.lib.input.KeyValueTextInputFormat;

public class HadoopSort {
	public static class HadoopSortMapper extends Mapper<Object, Text, Text, Text> {

		protected void map(Object key, Text value, Mapper<Object, Text, Text, Text>.Context context)
				throws IOException, InterruptedException {
			String input = key.toString();
			
			//Splitting Key and Value
			String out_k = input.substring(0, 10);
			String out_v = input.substring(10);
			context.write(new Text(out_k), new Text(out_v));
		}

	}

	public static class HadoopSortReducer extends Reducer<Text, Text, Text, Text> {

		protected void reduce(Text key, Iterable<Text> vals, Reducer<Text, Text, Text, Text>.Context context)
				throws IOException, InterruptedException {
			Text out_k = key;
			Text out_v = new Text();
			for (Text val : vals) {
				out_v = val;
			}
			context.write(out_k, out_v);
		}
	}

	public static void main(String[] args) throws IOException, ClassNotFoundException, InterruptedException {
		long s1 = System.currentTimeMillis();
		Configuration conf = new Configuration();
		Job job = Job.getInstance(conf, "Hadoop Sort");
		job.setJarByClass(HadoopSort.class);
		job.setMapperClass(HadoopSortMapper.class);
		job.setReducerClass(HadoopSortReducer.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(Text.class);
    	job.setInputFormatClass(KeyValueTextInputFormat.class);
		job.setNumReduceTasks(4);
		
		// Input File Path
		FileInputFormat.addInputPath(job, new Path(args[0]));
		
		// Output File Path
		FileOutputFormat.setOutputPath(job, new Path(args[1]));
		
		// Folder for total order partition file
		Path partitionPath = new Path(args[1] + "-part.lst");
		job.setPartitionerClass(TotalOrderPartitioner.class);

		// Generate partition file from map-only job's output.
     	TotalOrderPartitioner.setPartitionFile(job.getConfiguration(), partitionPath);
      	InputSampler.writePartitionFile(job, new InputSampler.RandomSampler<Text,Text>(1, 10000));

		if (job.waitForCompletion(true)) {
			long e1 = System.currentTimeMillis();
			double time = ((double) e1 - s1) / 1000.0;
			System.out.println("Total time to sort data on hadoop: " + time + " seconds");
		}
	}

}
