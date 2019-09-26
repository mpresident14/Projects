package com.example.miles.holdem_cpp;

import android.os.AsyncTask;
import android.util.Log;
import java.util.Arrays;

public class WinPercentagesBackgroundTask extends AsyncTask<Void, Void, double[][]> {

    // Load "native-lib library
    static{
        System.loadLibrary("native-lib");
    }

    public interface WinPercentagesListener {
        void onWinPercentagesLoaded(double[] winPercentages, double[] tiePercentages);
    }

    private int[] info;
    private WinPercentagesListener listener;

    WinPercentagesBackgroundTask(int[] info) {
        super();
        this.info = info;
    }

    @Override
    protected double[][] doInBackground(Void... voids) {
        return getWinPercentages(info);
    }

    @Override
    protected void onPostExecute(double[][] doubles) {
        Log.v("MyTest", Arrays.toString(doubles[0]));
        Log.v("MyTest", Arrays.toString(doubles[1]));
        listener.onWinPercentagesLoaded(doubles[0], doubles[1]);
    }

    WinPercentagesBackgroundTask setListener(WinPercentagesListener listener) {
        this.listener = listener;
        return this;
    }

    // Declare C++ fcns
    public native double[][] getWinPercentages(int[] info);
}
