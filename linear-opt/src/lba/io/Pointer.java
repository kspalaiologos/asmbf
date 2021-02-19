package lba.io;

public class Pointer {
    private int address;

    public Pointer(int address) {
        this.address = address;
    }

    public Pointer forwards(int delta) {
        this.address += delta;
        return this;
    }

    public Pointer backwards(int delta) {
        return forwards(-delta);
    }

    public Pointer forwards() {
        return forwards(1);
    }

    public Pointer backwards() {
        return backwards(1);
    }

    public int getValue() {
        return address;
    }

    public Pointer setValue(int value) {
        this.address = value;
        return this;
    }
}
