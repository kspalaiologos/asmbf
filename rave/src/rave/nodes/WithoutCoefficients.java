package rave.nodes;

public interface WithoutCoefficients extends INode {
	@Override
	public default void setCoefficient(int c) {
	}

	@Override
	public default int getCoefficient() {
		return 1;
	}
}
