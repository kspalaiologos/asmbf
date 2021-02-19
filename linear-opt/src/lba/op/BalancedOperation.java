package lba.op;

import java.util.List;

public class BalancedOperation extends Operation {
    private List<Operation> inner;

    public BalancedOperation(List<Operation> inner) {
        this.inner = inner;
    }

    public List<Operation> getInner() {
        return inner;
    }
}
