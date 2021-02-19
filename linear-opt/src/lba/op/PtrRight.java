package lba.op;

public class PtrRight extends GroupableOperation {
    private int coefficient;

    public PtrRight(int coefficient) {
        this.coefficient = coefficient;
    }

    @Override
    public int getCoefficient() {
        return coefficient;
    }
}
