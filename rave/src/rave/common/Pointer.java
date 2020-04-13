package rave.common;

// TODO: Is there a generic wrapper class?
// so you don't have to write such mumbo-jumbo
public class Pointer {
	private int value = 0;
	
	public void advance(int count) {
		value += count;
	}
	
	public void advance() {
		value++;
	}
	
	public void descend() {
		value--;
	}
	
	public int getValue() {
		return value;
	}
	
	public void setValue(int v) {
		value = v;
	}
}
