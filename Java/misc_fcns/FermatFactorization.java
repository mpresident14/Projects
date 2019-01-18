import java.util.List;
import java.util.ArrayList;
import java.util.stream.Collectors;

class FermatFactorization {

  public static List<Long> primeFactorization(long n) {
    List<Long> result = new ArrayList<>(); // Store prime factors
    primeFactorization(n, result);
    return result;
  }

  private static void primeFactorization(long n, List<Long> result) {
    if (n < 2) {
      return;
    }
    
    // Get 2 as a prime factor if even
    if (n % 2 == 0) {
      result.add(2l);
      primeFactorization(n / 2, result);
    }

    // Otherwise, factor using Fermat Factorization
    else {
      long[] factors = fermatFactor(n);
      long f0 = factors[0];
      long f1 = factors[1];
      // The number is prime if 1 is the only other factor
      if (f0 == 1 || f1 == 1) {
        result.add(n);
      }
      // Otherwise, continue factoring the factors
      else {
        primeFactorization(factors[0], result);
        primeFactorization(factors[1], result);
      }  
    }
  }

  // Key: Find a & b s.t. a^2 - b^2 = n. Then, n = (a+b)(a-b)
  // Only works for odd numbers and multiples of 4 b/c a+b & a-b are either 
  //  both odd or both even
  private static long[] fermatFactor(long n) {
    long a = (long) Math.ceil(Math.sqrt(n));
    long bSquared = a*a - n;
    double b = Math.sqrt(bSquared);
    
    // Check if b is a  perfect square
    while ((long) b - b != 0) {
      a++;
      bSquared = a*a - n;
      b = Math.sqrt(bSquared);
    }

    long[] result = new long[2];
    result[0] = (long) (a + b);
    result[1] = (long) (a - b);
    return result;
  }

  public static void main(String[] args) {
    if (args.length != 1) {
      System.out.println("Enter a numerical argument.");
      System.exit(0);
    }

    long n = Long.valueOf(args[0]);
    String s = FermatFactorization.primeFactorization(n)
                  .stream()
                  .map(e -> Long.toString(e))
                  .collect(Collectors.joining(",", "[", "]"));
    System.out.println(s);
  }
}