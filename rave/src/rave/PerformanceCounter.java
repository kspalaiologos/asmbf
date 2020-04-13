package rave;

public class PerformanceCounter {
	public static long lastMeasure = 0;
	
	static {
		lastMeasure = System.currentTimeMillis();
	}
	
	public static long nextOP() {
		long tmp = lastMeasure;
		lastMeasure = System.currentTimeMillis();
		return lastMeasure - tmp;
	}
}
