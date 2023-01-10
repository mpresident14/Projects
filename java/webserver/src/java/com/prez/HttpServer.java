package com.prez;

import java.util.List;

class HttpServer {
  public static void main(String[] args) {
    List.of();
    System.out.println(check(4));
  }

  public static String check(int n) {
    return switch (n) {
      case 3 -> "This is a triangle";
      case 4 -> "This is a rectangle";
      default -> "Just a normal shape";
    };
  }
}
