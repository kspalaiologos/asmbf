package lba.io;

import lba.op.BalancedOperation;
import lba.op.Operation;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

public class MemoryOptimizer {
    public static List<Integer> extractCoeffs(List<Operation> ops) {
        List<Integer> ret = new ArrayList<>();

        for(Operation o : ops) {
            ret.add(o.getOffset());

            if(o instanceof BalancedOperation)
                ret.addAll(extractCoeffs(((BalancedOperation) o).getInner()));
        }

        return ret;
    }

    public static List<Operation> mergeCoeffs(List<Operation> input, List<Integer> coeffs) {
        List<Operation> ops = new ArrayList<>();

        for(Operation o : input) {
            o.setOffset(coeffs.get(0));
            coeffs.remove(0);
            if(o instanceof BalancedOperation) {
                BalancedOperation op = new BalancedOperation(mergeCoeffs(((BalancedOperation) o).getInner(), coeffs));
                op.setOffset(o.getOffset());
                ops.add(op);
            } else
                ops.add(o);
        }

        return ops;
    }

    private List<List<Integer>> all;

    public List<Integer> optimize(List<Integer> data) {
        List<Integer> distinct = data.stream().distinct().collect(Collectors.toList());
        all = new ArrayList<>();
        printAllRecursive(distinct.size(), distinct.toArray(new Integer[0]), data);
        List<Integer> values = all.stream().mapToInt(
                x -> IntStream.range(1, x.size())
                        .map(i -> Math.abs(x.get(i-1) - x.get(i)))
                        .reduce((a, b) -> a + b)
                        .getAsInt()
        ).boxed().collect(Collectors.toList());
        return all.get(values.indexOf(values.stream().mapToInt(x -> x).min().getAsInt()));
    }

    public void printAllRecursive(int n, Integer[] elements, List<Integer> distinct) {
        if(n == 1) {
            all.add(distinct.stream().map(x -> Arrays.asList(elements).indexOf(x)).collect(Collectors.toList()));
        } else {
            for(int i = 0; i < n-1; i++) {
                printAllRecursive(n - 1, elements, distinct);
                if(n % 2 == 0) {
                    swap(elements, i, n-1);
                } else {
                    swap(elements, 0, n-1);
                }
            }
            printAllRecursive(n - 1, elements, distinct);
        }
    }

    private void swap(Integer[] input, int a, int b) {
        int tmp = input[a];
        input[a] = input[b];
        input[b] = tmp;
    }
}
