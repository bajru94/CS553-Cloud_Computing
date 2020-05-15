import org.apache.spark.SparkConf;
import org.apache.spark.api.java.JavaPairRDD;
import org.apache.spark.api.java.JavaRDD;
import org.apache.spark.api.java.JavaSparkContext;
import scala.Tuple2;
import java.util.Arrays;
public class SparkSort {
	
  public static void main(String[] args) {
	  
    SparkConf sparkConf = new SparkConf().setAppName("Sort");
    JavaSparkContext sparkContext = new JavaSparkContext(sparkConf);
	
	// Input File
    String in = args[0];
 
	// Output Folder Path
	String out = args[1];
	
    JavaRDD<String> textFile = sparkContext.textFile(in);
	
    JavaPairRDD<String, String> sort = textFile
      .mapToPair(line -> new Tuple2<>(line.substring(0, 10), line.substring(10) + '\r'))
      .sortByKey();
    sort.map(x -> x._1 + x._2).saveAsTextFile(out);
  
  }
}
