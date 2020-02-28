import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.Random;
import java.util.Set;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

/* Note: commented out code was used in testing, but not part of final algorithm */

public class ImageGraph {
	private static final int INFINITY = Integer.MAX_VALUE;
	private static final String SMALL = "small";
	private static final Random rand = new Random();
	
	public static class Image {
		String name;
		// User seeds
		private int[][] fgPixels; // [[x0, y0], [x1, y1]...[xn, yn]]
		private int[][] bgPixels; // [[x0, y0], [x1, y1]...[xm, ym]]
		
		public Image(String name, int[][] fgPixels, int[][] bgPixels) {
			this.name = name;
			this.fgPixels = fgPixels;
			this.bgPixels = bgPixels;
		}
	}
	
	/* Represents a directed edge */
	private class Edge {
		int from;
		int to;
		int weight;
		
		private Edge(int from, int to, int weight) {
			this.from = from;
			this.to = to;
			this.weight = weight;
		}
		
		public String toString() {
			return "(" + from + ", " + to + ", " + weight + ")";
		}
	}
	
	/* Represents a vertex and all the edges pointing away from it */
	private class Vertex {
		int num;
		List<Edge> edges;
		
		private Vertex(int num, List<Edge> edges) {
			this.num = num;
			this.edges = edges;
		}
		
		public String toString() {
			return num + ": " + edges.toString();
		}
	}
	
	/* *********************
	 * Variables to adjust *
	 ***********************/
	private static final String PATH = 
			"C:/Users/Miles/Desktop/graph_theory/project1/Images/";
	private static final int MAX_PIXELS = 5000;
	private static final double COLOR_LENIENCY = 5;
	private static final double MAX_DISTANCE_ADJ = 3;
	
	private static final String PARAMS = 
			"CL_" + COLOR_LENIENCY + "_MDA_" + MAX_DISTANCE_ADJ + "_test";

//	private static double DISTANCE_LENIENCY = 2;
	
	
	private List<Vertex> vertices;
	private Mat matrix;
	private String imageName;
	private int sourceNum; // Foreground
	private int sinkNum;   // Background
	private int[][] fgPixels; // [[x0, y0], [x1, y1]...[xn, yn]]
	private int[][] bgPixels; // [[x0, y0], [x1, y1]...[xm, ym]]
	
	
	public ImageGraph(String imageNameWithExt, int[][]fgPixels, int[][] bgPixels) {
		imageName = imageNameWithExt.substring(0, imageNameWithExt.indexOf('.'));
		matrix = Imgcodecs.imread(PATH + SMALL + "/" + imageName + "_" + SMALL + ".bmp");
		vertices = new ArrayList<Vertex> (matrix.height() * matrix.width());
		this.fgPixels = fgPixels;
		this.bgPixels = bgPixels;
	}
	
	
	/* ***************************
	 * Initialization From Image *
	 *****************************/
	public void resizeImage(String imageNameWithExt) {
		Mat mat = Imgcodecs.imread(PATH + "original/" + imageNameWithExt);
		matrix = new Mat();
		
		float widthHeightRatio = mat.width() * 1f / mat.height();
		int newHeight = (int) Math.sqrt(1 / widthHeightRatio * MAX_PIXELS);
		int newWidth = (int) (widthHeightRatio * newHeight);
		
		Size scaleSize = new Size(newWidth,newHeight);
		Imgproc.resize(mat, matrix, scaleSize);
		Imgcodecs.imwrite(PATH + SMALL + "/" + imageName + "_" + SMALL + ".bmp", matrix);
	}
	
	public void initialize() {
//		initPixelVertices(); // Testing
		initPixelVertices_distanceAdjacency();
		initSourceAndSink();
	}
	
	/* Used for connecting only pixels directly next to each other */
//	private void initPixelVertices() {
//		
//		int height = matrix.height();
//		int width = matrix.width();
//		for (int i = 0; i < height; i++) {
//        	for (int j = 0; j < width; j++) {
//        		// Insert vertex
//        		int pixelNum = coordToPixelNum(i, j);
//        		Vertex vertex = new Vertex(pixelNum, new LinkedList<Edge>());
//        		
//        		// Check to the left
//        		if (j != 0) {
//        			int pixelLeft = pixelNum - 1;
//        			int weight = interpixelWeight_exp(i, j, i, j-1);
//        			vertex.edges.add(new Edge(pixelNum, pixelLeft, weight));
//        			vertices.get(pixelLeft).edges.add(new Edge(pixelLeft, pixelNum, weight));
//        		}
//        		
//        		// Check above
//        		if (i != 0) {
//        			int pixelAbove = pixelNum - matrix.width();
//        			int weight = interpixelWeight_exp(i, j, i-1, j);
//        			vertex.edges.add(new Edge(pixelNum, pixelAbove, weight));
//        			vertices.get(pixelAbove).edges.add(new Edge(pixelAbove, pixelNum, weight));
//        		}
//        		
//        		vertices.add(vertex);
//        	}
//		}
//	}
	
	/* Initialize pixels within MAX_DISTANCE_ADJ to be adjacent */
	private void initPixelVertices_distanceAdjacency() {
		int height = matrix.height();
		int width = matrix.width();
		for (int i = 0; i < height * width; i++) {
         	vertices.add(new Vertex(i, new LinkedList<Edge>()));
		}
		
		for (int row1 = 0; row1 < height; row1++) {
			for (int col1 = 0; col1 < width; col1++) {
				for (int row2 = row1; row2 < height; row2++) {
					for (int col2 = col1; col2 < width; col2++) {
						// Same pixel, nothing to do
						if (row1 == row2 && col1 == col2) {
							continue;
						}
						
						if (euclideanDistance(row1, col1, row2, col2) <= MAX_DISTANCE_ADJ) {
							int pixelNum1 = coordToPixelNum(row1, col1);
							int pixelNum2 = coordToPixelNum(row2, col2);
							int weight = interpixelWeight_exp(row1, col1, row2, col2);
							vertices.get(pixelNum1).edges.add(new Edge(pixelNum1, pixelNum2, weight));
							vertices.get(pixelNum2).edges.add(new Edge(pixelNum2, pixelNum1, weight));
						}
					}
				}
			}
		}
	}
	
	private double euclideanDistance(int row1, int col1, int row2, int col2) {
		return Math.sqrt(Math.pow(row1-row2, 2) + Math.pow(col1-col2, 2));
	}
	
	// Source vertex points to all vertices
	// Sink vertex is pointed to by all vertices
	private void initSourceAndSink() {
		int size = vertices.size();
		sourceNum = size;
		sinkNum = size + 1;
		List<Edge> sourceEdges = new ArrayList<>(size);
		List<Edge> sinkEdges = new ArrayList<>(size);
		
		int height = matrix.height();
		int width = matrix.width();
		for (int i = 0; i < height; i++) {
        	for (int j = 0; j < width; j++) {
        		int pixelNum = coordToPixelNum(i, j);
        		// Source to vertex
				sourceEdges.add(
						new Edge(sourceNum, pixelNum, terminalPixelWeight(i, j, /* isSource */ true)));
				// Vertex to sink
				vertices.get(pixelNum).edges.add(
						new Edge(pixelNum, sinkNum, terminalPixelWeight(i, j, /* isSource */ false)));
				// Vertex to source (residual w/ inital capacity 0)
				vertices.get(pixelNum).edges.add(new Edge(pixelNum, sourceNum, 0));
				// Sink to vertex (residual w/ inital capacity 0)
				sinkEdges.add(new Edge(sinkNum, pixelNum, 0));
				
        	}
		}
		
		vertices.add(new Vertex(sourceNum, sourceEdges));
		vertices.add(new Vertex(sinkNum, sinkEdges));
	}
	
	// User "clicks" on some number pixels that correspond to each of background and foreground
	// Get the closest match and compute the weight
	private int terminalPixelWeight(int row, int col, boolean isFg) {
		int[][] seedPixels = isFg ? fgPixels : bgPixels;
		int length = seedPixels.length;
		int maxWeight = 0;
		for (int i = 0; i < length; i++) {
			maxWeight = 
					Math.max(maxWeight, 
							interpixelWeight_exp(row, col, seedPixels[i][1], seedPixels[i][0]));
		}
		return maxWeight + 10000;
	}
	
	// Uses linear color "distance" between pixels
//	private int interpixelWeight(int row1, int col1, int row2, int col2) {		
//		return 500 - (int) colorDifference(row1, col1, row2, col2);
//	}
	
	// Uses color "distance" between pixels on an exponential scale
	private int interpixelWeight_exp(int row1, int col1, int row2, int col2) {		
		double colorDifference = colorDifference(row1, col1, row2, col2);
		// Magic # 1 million used to get a nice distribution of integers instead of floating pt
		return (int) (1000000 * Math.exp(-colorDifference / COLOR_LENIENCY));
	}
	
	/* This was used in testing and found to be largely unhelpful */
//	private int interpixelWeight_exp_distance(int row1, int col1, int row2, int col2, double distance) {		
//		double colorDifference = colorDifference(row1, col1, row2, col2);
//		// Magic # 1 million and 100,000 used to get a nice distribution of integers instead of floating pt
//		return (int) (1000000 * Math.exp(-colorDifference / COLOR_LENIENCY) + 
//				100000 * Math.exp(-distance / MAX_DISTANCE_ADJ / DISTANCE_LENIENCY));
//	}
//	
	
	/* Calculate Euclidean color distance between two pixels */
	private double colorDifference(int row1, int col1, int row2, int col2) {
		double[] rbg1 = matrix.get(row1, col1);
		double[] rbg2 = matrix.get(row2, col2);
		// Error check my user seeds
		if (rbg2 == null) {
			System.err.println("height = " + matrix.height() + " width = " + matrix.width());
			System.err.println("row2 = " + row2 + ", col2 = " + col2);
			System.exit(1);
		}
		
		return 
				Math.sqrt(
						Math.pow(rbg1[0]-rbg2[0], 2) + 
						Math.pow(rbg1[1]-rbg2[1], 2) + 
						Math.pow(rbg1[2]-rbg2[2], 2));
	}
	
	private int coordToPixelNum(int x, int y) {
		return matrix.width() * x + y;
	}

	
	/* *********************
	 * Ford-Fulkerson Alg  *
	 ***********************/	 
	 public void fordFulk() {
		 Edge[] result = null;
		 while ((result = sendFlowAlongShortestAugPath()) != null) {
			 sendFlow(result);
		 }
	 }
	
	private Edge[] sendFlowAlongShortestAugPath() {
		int numVertices = vertices.size();
		Edge[] result = new Edge[numVertices];	
		Queue<Integer> q = new LinkedList<>();
		
		q.add(sourceNum);
		result[sourceNum] = new Edge(-1, sourceNum, 0); // Just to get the process started
		
		while (!q.isEmpty()) {
			List<Edge> neighbors = vertices.get(q.poll()).edges;
			for (Edge e : neighbors) {
				if (e.weight > 0) {
					if (e.to == sinkNum) {
						result[sinkNum] = e;
						return result;
					}
					
					// Since this is a BFS, if encounter a vertex again, we know it 
					// took path as long as or longer than the first time we found it
					if (result[e.to] == null) {
						q.offer(e.to);
						result[e.to] = e;
					}						
				}
			}		
		}

		return null;
	}
	
	/* Traverse result array from sink to source to find min cap and then send flow */
	private void sendFlow(Edge[] result) {
		
		// Find bottleneck capacity
		int minCapacity = INFINITY;
		int nextVertex = sinkNum;
		while (nextVertex != sourceNum) {
			minCapacity = Math.min(minCapacity, result[nextVertex].weight);
			nextVertex = result[nextVertex].from;
		}
		
		// Send Flow
		nextVertex = sinkNum;
		while (nextVertex != sourceNum) {
			Edge e = result[nextVertex];
			e.weight -= minCapacity;
			Edge residualE = getResidualEdge(e);
			residualE.weight += minCapacity;
			nextVertex = result[nextVertex].from;
		}
	}
	
	private Edge getResidualEdge(Edge e) {
		List<Edge> edges = vertices.get(e.to).edges;
		// If source or sink, ArrayList can index
		if (edges instanceof ArrayList) {
			return edges.get(e.from);
		}
		// Otherwise, need to search through linked list
		else {
			for (Edge other : vertices.get(e.to).edges) {
				if (other.to == e.from) {
					return other;
				}
			}
		}
		// Shouldn't reach this ever
		return null;
	}
	
	/* ********************************
	 * Separate Vertices Into S and T *
	 **********************************/
	public void writeSegmentedImage() {
		Mat mat = Mat.zeros(matrix.height(), matrix.width(), CvType.CV_32SC3);
        int[] whitePixel = {255, 255, 255};
        int height = mat.height();
        int width = mat.width();
        
        for (int i = 0; i < height; i++) {
        	for (int j = 0; j < width; j++) {
        		int pixelNum = coordToPixelNum(i, j);
        		if (existsAugPath(pixelNum)) {
        			mat.put(i, j, matrix.get(i, j));
        		}
        		else {
        			mat.put(i, j, whitePixel); 
        		}
        	}
        }
        
        File newDir = new File(PATH + PARAMS);
        if (!newDir.exists()) {
        	newDir.mkdirs();
        }
        
        Imgcodecs.imwrite(PATH + PARAMS + "/" + imageName + "_" + PARAMS + ".bmp", mat);
	}
	
	
	/* BFS to find augmenting path in the residual graph from s to destVertex */
	private boolean existsAugPath(int destVertex) {
		Queue<Integer> q = new LinkedList<>();
		Set<Integer> visited = new HashSet<>();
		q.offer(sourceNum);
		visited.add(sourceNum);
		
		while (!q.isEmpty()) {
			int vertex = q.poll();
			List<Edge> neighbors = vertices.get(vertex).edges;
			for (Edge e : neighbors) {
				if (e.weight > 0) {
					if (e.to == destVertex) {
						return true;
					}
					if (!visited.contains(e.to)) {
						visited.add(e.to);
						q.offer(e.to);
					}
				}
			}
		}
		return false;
	}
		
	/* *********************
	 * Making a Test Image *
	 ***********************/	
	public static void makeTestImage(int rows, int cols) {
		Mat mat = Mat.zeros(rows, cols, CvType.CV_32SC3);
		int[] lightPixel = {200, 200, 200};
        int[] darkPixel = {30, 30, 30};
        
        for (int i = 0; i < mat.size().height; i++) {
        	for (int j = 0; j < mat.size().width; j++) {
        		if (i > j) {
        			// Slight randomization
        			lightPixel[0] = randLightPixel(); 
        			lightPixel[1] = randLightPixel(); 
        			lightPixel[2] = randLightPixel(); 
        			mat.put(i, j, lightPixel); 
        		}
        		else {
        			// Slight randomization
        			darkPixel[0] = randDarkPixel(); 
        			darkPixel[1] = randDarkPixel(); 
        			darkPixel[2] = randDarkPixel(); 
        			mat.put(i, j, darkPixel); 
        		}
        	}
        }
        Imgcodecs.imwrite(PATH + "test_" + cols + "x" + rows + ".bmp", mat);
	}
	
	private static int randLightPixel(){
		return 200 + rand.nextInt(40) - 20; 
	}
	private static int randDarkPixel(){
		return 30 + rand.nextInt(40) - 20; 
	}
	
	public String toString() {
		StringBuilder sb = new StringBuilder();
		for (Vertex v : vertices) {
			sb.append(v.toString());
			sb.append("\n");
		}
		return sb.toString();
	}
	
	public static void main(String[] args){
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        
        Image[] initialImages = new Image[] {
        		new Image("fox.jpg", 
        				new int[][] {{25,20}, {22,28}}, 
        				new int[][] {{41,51}, {55,25}} ),
        		new Image("person.jpg", 
        				new int[][] {{45,12}, {37,57}, {53,64}}, 
        				new int[][] {{11,10}, {13,40}, {61,46}} ),
        		new Image("mountain.jpg", 
        				new int[][] {{15,32}, {36,48}}, 
        				new int[][] {{15,9}, {31,17}} ),
        		new Image("flower.jpg", 
        				new int[][] {{30,17}}, 
        				new int[][] {{62,43}, {8,18}} ),
        		new Image("beach.jpg", 
        				new int[][] {{23,17}, {70,47}, {46,7}}, 
        				new int[][] {{68,11}, {77,34}} ),
        		new Image("lion.jpg", 
        				new int[][] {{54,34}}, 
        				new int[][] {{17,11}, {40,34}} ),
        		new Image("monument.jpg", 
        				new int[][] {{17,42}, {32,60}}, 
        				new int[][] {{29,11}} ),
        		new Image("seagull.jpg", 
        				new int[][] {{54,51}, {65,24}, {51,23}}, 
        				new int[][] {{22,45}} ),
        };
        
        Image[] newImages = new Image[] {
        		new Image("orca.jpg", 
        				new int[][] {{59,14}, {30,19}}, 
        				new int[][] {{4,34}} ),
        		new Image("guy.jpg", 
        				new int[][] {{39,12}, {21,53}, {27,32}}, 
        				new int[][] {{11,31}} ),
        		new Image("cow.jpg", 
        				new int[][] {{57,36}, {80,29}}, 
        				new int[][] {{25,16}, {17,46}} ),
        		new Image("plantsbeach.jpg", 
        				new int[][] {{4,34}, {25,26}}, 
        				new int[][] {{43,5}, {8,54}} ),
        		new Image("yosemite.jpg", 
        				new int[][] {{27,25}, {28,33}, {22,54}}, 
        				new int[][] {{41,8}, {66,45}} ),
        		new Image("me.jpg", 
        				new int[][] {{44,25}, {37,15}}, 
        				new int[][] {{8,46}, {69,26}, {16,15}} ),
        		new Image("tiger.jpg", 
        				new int[][] {{33,22}, {4,48}}, 
        				new int[][] {{61,19}, {68,55}} ),
        		new Image("sd.jpg", 
        				new int[][] {{55,50}, {58,48}, {44,47}}, 
        				new int[][] {{15,42}} ),
        };
        
        for (Image i : initialImages) {
        	System.out.println("********************************\nStarting " + i.name);
	        ImageGraph g = new ImageGraph(i.name, i.fgPixels, i.bgPixels);
	        
	        /* Comment in to resize */
//	        g.resizeImage(i.name);
	        
	        /* Run the algorithm */
	        g.initialize();

	        long startTime = System.nanoTime();   
	        g.fordFulk();
	        long endTime = System.nanoTime();
	        System.out.println("F-F alg took " + (endTime - startTime)*1.0  / 1000000000 + " seconds.");
	        
	        startTime = System.nanoTime();       
	        g.writeSegmentedImage();
	        endTime = System.nanoTime();
	        System.out.println("Write took " + (endTime - startTime)*1.0  / 1000000000 + " seconds.");
		}
	}	
}
