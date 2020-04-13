package rave.nodes;

public abstract class CoefficientalNode implements INode {

	private int coefficient;
	
	@Override
	public void setCoefficient(int c) {
		this.coefficient = c;
	}

	@Override
	public int getCoefficient() {
		return coefficient;
	}

}
