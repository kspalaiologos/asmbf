package lba.op;

public class Minus extends GroupableOperation {
    private int coefficient;

    public Minus(int coefficient) {
        this.coefficient = coefficient;
    }

    @Override
    public int getCoefficient() {
        return coefficient;
    }
}
