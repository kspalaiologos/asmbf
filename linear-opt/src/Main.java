import lba.io.MemoryOptimizer;
import lba.io.Pointer;
import lba.io.Tuple;
import lba.op.*;

import java.util.ArrayList;
import java.util.List;
import java.util.function.Function;
import java.util.stream.Collectors;

public class Main {
    public static int hasTimes(String data, char leading) {
        int count = 0;
        for(int i = 0; data.length() > i && data.charAt(i) == leading; i++)
            count++;
        return count;
    }

    private static Tuple<Operation, String> parseOp(String code) {
        ArrayList<Tuple<Function<Integer, Operation>, Character>> lut = new ArrayList<>();

        lut.add(new Tuple<>(Plus::new, '+'));
        lut.add(new Tuple<>(Minus::new, '-'));
        lut.add(new Tuple<>(PtrLeft::new, '<'));
        lut.add(new Tuple<>(PtrRight::new, '>'));

        for(Tuple<Function<Integer, Operation>, Character> e : lut) {
            int op = hasTimes(code, e.y);
            if(op > 0) return new Tuple(e.x.apply(op), code.substring(op));
        }

        if(code.charAt(0) == '.')
            return new Tuple(new ByteOut(), code.substring(1));
        if(code.charAt(0) == ',')
            return new Tuple(new ByteIn(), code.substring(1));
        if(code.charAt(0) == '[') {
            List<Operation> ops = new ArrayList<>();
            code = code.substring(1);

            while(code.charAt(0) != ']') {
                Tuple<Operation, String> res = parseOp(code);
                code = res.y;
                ops.add(res.x);
            }

            ops.add(new LoopEndMarker());

            return new Tuple(new BalancedOperation(ops), code.substring(1));
        }

        throw new RuntimeException("Unexpected instruction: " + code.charAt(0));
    }

    public static List<Operation> parse(String code) {
        List<Operation> ops = new ArrayList<>();

        while(code.length() > 0) {
            Tuple<Operation, String> res = parseOp(code);
            code = res.y;
            ops.add(res.x);
        }

        return ops;
    }

    private static void display(List<Operation> op, Pointer current) {
        for(Operation o : op) {
            if(o.getOffset() != current.getValue()) {
                int difference = o.getOffset() - current.getValue();

                if(difference < 0)
                    for(int i = 0; i < -difference; i++)
                        System.out.print("<");
                else
                    for(int i = 0; i < difference; i++)
                        System.out.print(">");

                current.setValue(o.getOffset());
            }

            if(o instanceof GroupableOperation) {
                for(int i = 0; i < ((GroupableOperation) o).getCoefficient(); i++) {
                    if(o instanceof Plus) System.out.print("+");
                    if(o instanceof Minus) System.out.print("-");
                }
            }

            if(o instanceof ByteIn) System.out.print(",");
            if(o instanceof ByteOut) System.out.print(".");

            if(o instanceof BalancedOperation) {
                System.out.print("["); display(((BalancedOperation) o).getInner(), current); System.out.print("]");
            }
        }
    }

    public static void display(List<Operation> op) {
        display(op, new Pointer(0));
    }

    private static List<Operation> process(List<Operation> ops, Pointer current) {
        return ops.stream().map(op -> {
            if (op instanceof PtrLeft) {
                current.backwards(((PtrLeft) op).getCoefficient());
                return new NoOp();
            } else if(op instanceof PtrRight) {
                current.forwards(((PtrRight) op).getCoefficient());
                return new NoOp();
            } else if(op instanceof BalancedOperation) {
                int curPos = current.getValue();
                BalancedOperation operation = new BalancedOperation(process(((BalancedOperation) op).getInner(), current));
                operation.setOffset(curPos);
                return operation;
            } else {
                op.setOffset(current.getValue());
                return op;
            }
        }).filter(op -> !(op instanceof NoOp)).collect(Collectors.toList());
    }

    public static List<Operation> process(List<Operation> ops) {
        return process(ops, new Pointer(0));
    }

    public static void magic(String arg) {
        List<Operation> op = parse(arg);
        System.out.println(arg);
        op = process(op);
        List<Integer> extracted = MemoryOptimizer.extractCoeffs(op);
        extracted.forEach(x -> System.out.print(x + " ")); System.out.println();
        List<Integer> best = new MemoryOptimizer().optimize(extracted);
        best.forEach(x -> System.out.print(x + " ")); System.out.println();
        List<Operation> op2 = MemoryOptimizer.mergeCoeffs(op, best);
        display(op2);
        System.out.println();
    }
    public static void main(String[] args) {
        magic(args[0]);

        return;
    }
}
