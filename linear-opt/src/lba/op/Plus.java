package lba.op;

public class Plus extends GroupableOperation {
    private int coefficient;

    public Plus(int coefficient) {
        this.coefficient = coefficient;
    }

    @Override
    public int getCoefficient() {
        return coefficient;
    }
}
