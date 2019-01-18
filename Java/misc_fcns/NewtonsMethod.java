class NewtonsMethod {

  // Key: Find the roots of f(x) = x^2 - n
  // x1 = x0 - f(x0) / f'(x0)
  // x1 is intersection of x-axis and tangent line at x0
  public static double squareRoot(double n) {
    double prevResult = n; // initial guess of solution
    double result = prevResult - (prevResult*prevResult - n) / (2 * prevResult);
    
    // Get closer and closer to actual solution until value changes by less than 0.1%
    int i = 0;
    while (prevResult * 0.001 < Math.abs(result - prevResult)) {
      prevResult = result;
      result -= (result*result - n) / (2 * result);
      i++;
    }

    System.out.println("Took " + i + " iterations.");

    return result;
  }

  public static void main(String[] args) {
    if (args.length != 1) {
      System.out.println("Enter a numerical argument.");
      System.exit(0);
    }

    double n = Double.valueOf(args[0]);
    System.out.println(
      "The square root of " + n + " is approximately " + NewtonsMethod.squareRoot(n));
  }
}