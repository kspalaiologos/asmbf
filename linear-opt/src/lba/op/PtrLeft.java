package lba.op;

public class PtrLeft extends GroupableOperation {
    private int coefficient;

    public PtrLeft(int coefficient) {
        this.coefficient = coefficient;
    }

    @Override
    public int getCoefficient() {
        return coefficient;
    }
}
