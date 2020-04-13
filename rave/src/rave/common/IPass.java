package rave.common;

import java.util.List;

public interface IPass {
	int match(List<INode> input);
	INode build(List<INode> input);
}
