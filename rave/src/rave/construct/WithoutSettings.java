package rave.construct;

import java.util.HashMap;

import rave.common.INode;

public abstract class WithoutSettings implements INode {
	public HashMap<String, Object> settings() {
		return null;
	}
}
