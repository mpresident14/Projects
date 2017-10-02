import java.util.Arrays;

public enum OneCardHandPercentages {

	TWO(new double[] {0.1796947, 0.4444414, 0.2357691, 0.0484802, 0.0334071, 0.0303557, 0.025961, 0.0016807, 1.999E-4, 1.02E-5}),
	THREE(new double[] {0.1764589, 0.4409782, 0.2353283, 0.0483819, 0.0406451, 0.0303007, 0.025961, 0.0016807, 2.549E-4, 1.02E-5}),
	FOUR(new double[] {0.1732231, 0.4375151, 0.2348876, 0.0482836, 0.0478832, 0.0302457, 0.025961, 0.0016807, 3.098E-4, 1.02E-5}),
	FIVE(new double[] {0.1699873, 0.4340519, 0.2344468, 0.0481852, 0.0551213, 0.0301907, 0.025961, 0.0016807, 3.648E-4, 1.02E-5}),
	SIX(new double[] {0.1712816, 0.4346291, 0.2344468, 0.0481852, 0.0532498, 0.0301932, 0.025961, 0.0016807, 3.623E-4, 1.02E-5}),
	SEVEN(new double[] {0.1712816, 0.4346291, 0.2344468, 0.0481852, 0.0532498, 0.0301932, 0.025961, 0.0016807, 3.623E-4, 1.02E-5}),
	EIGHT(new double[] {0.1712816, 0.4346291, 0.2344468, 0.0481852, 0.0532498, 0.0301932, 0.025961, 0.0016807, 3.623E-4, 1.02E-5}),
	NINE(new double[] {0.1712816, 0.4346291, 0.2344468, 0.0481852, 0.0532498, 0.0301932, 0.025961, 0.0016807, 3.623E-4, 1.02E-5}),
	TEN(new double[] {0.1699873, 0.4340519, 0.2344468, 0.0481852, 0.0551213, 0.0301907, 0.025961, 0.0016807, 3.073E-4, 6.77E-5}),
	JACK(new double[] {0.1732231, 0.4375151, 0.2348876, 0.0482836, 0.0478832, 0.0302457, 0.025961, 0.0016807, 2.524E-4, 6.77E-5}),
	QUEEN(new double[] {0.1764589, 0.4409782, 0.2353283, 0.0483819, 0.0406451, 0.0303007, 0.025961, 0.0016807, 1.974E-4, 6.77E-5}),
	KING(new double[] {0.1796947, 0.4444414, 0.2357691, 0.0484802, 0.0334071, 0.0303557, 0.025961, 0.0016807, 1.424E-4, 6.77E-5}),
	ACE(new double[] {0.1796947, 0.4444414, 0.2357691, 0.0484802, 0.0334071, 0.0303557, 0.025961, 0.0016807, 1.424E-4, 6.77E-5});
	
	
	public double[] percentages;
	
	private OneCardHandPercentages(double[] inputPercentages){
		this.percentages = inputPercentages;
	}
	
	public static void main(String[] args){
		System.out.println(Arrays.toString(OneCardHandPercentages.TWO.percentages));
	}

}
