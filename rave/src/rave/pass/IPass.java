package rave.pass;

import java.util.List;

import rave.nodes.INode;

public interface IPass {
	int match(List<INode> input);
	INode build(List<INode> input);
}
