package rave.common;

import java.util.ArrayList;
import java.util.List;

import rave.construct.primitive.BracketLeft;
import rave.construct.primitive.BracketRight;
import rave.construct.primitive.Comma;
import rave.construct.primitive.Dot;
import rave.construct.primitive.Minus;
import rave.construct.primitive.Plus;
import rave.construct.primitive.PtrLeft;
import rave.construct.primitive.PtrRight;
import rave.nodes.INode;

public class PatternParser {
	
	public class MatchResult {
		public MatchResult(List<INode> variableNodes, boolean hasAny, int parsedTerms) {
			super();
			this.variableNodes = variableNodes;
			this.hasAny = hasAny;
			this.parsedTerms = parsedTerms;
		}
		
		public List<INode> variableNodes;
		public boolean hasAny;
		public int parsedTerms;
	}
	
	private String pattern;
	private boolean pedantic = false;
	
	public PatternParser(String pattern) {
		this.pattern = pattern;
	}
	
	public MatchResult tryMatch(List<INode> queue) {
		List<INode> variableNodes = new ArrayList<INode>();
		int ptr = 0, pc = 1, pptr = 0;
		
		while(ptr < pattern.length()) {
			if(pptr >= queue.size()) {
				return new MatchResult(null, false, 0);
			}
			
			if(Character.isDigit(pattern.charAt(ptr))) {
				pc = pattern.charAt(ptr) - '0';
				ptr++;
				continue;
			}
			
			if(pattern.charAt(ptr) == '+' && queue.get(pptr) instanceof Plus && (pedantic ? queue.get(pptr).getCoefficient() == pc : true)) { ptr++; pptr++; }
			else if(pattern.charAt(ptr) == '-' && queue.get(pptr) instanceof Minus && (pedantic ? queue.get(pptr).getCoefficient() == pc : true)) { ptr++; pptr++; }
			else if(pattern.charAt(ptr) == '<' && queue.get(pptr) instanceof PtrLeft && (pedantic ? queue.get(pptr).getCoefficient() == pc : true)) { ptr++; pptr++; }
			else if(pattern.charAt(ptr) == '>' && queue.get(pptr) instanceof PtrRight && (pedantic ? queue.get(pptr).getCoefficient() == pc : true)) { ptr++; pptr++; }
			else if(pattern.charAt(ptr) == '.' && queue.get(pptr) instanceof Dot && (pedantic ? queue.get(pptr).getCoefficient() == pc : true)) { ptr++; pptr++; }
			else if(pattern.charAt(ptr) == ',' && queue.get(pptr) instanceof Comma && (pedantic ? queue.get(pptr).getCoefficient() == pc : true)) { ptr++; pptr++; }
			else if(pattern.charAt(ptr) == '[' && queue.get(pptr) instanceof BracketLeft && (pedantic ? queue.get(pptr).getCoefficient() == pc : true)) { ptr++; pptr++; }
			else if(pattern.charAt(ptr) == ']' && queue.get(pptr) instanceof BracketRight && (pedantic ? queue.get(pptr).getCoefficient() == pc : true)) { ptr++; pptr++; }
			else if(pattern.charAt(ptr) == 'V' && (queue.get(pptr) instanceof Plus || queue.get(pptr) instanceof Minus)) {
				variableNodes.add(queue.get(pptr)); { ptr++; pptr++; }
			} else if(pattern.charAt(ptr) == 'P' && (queue.get(pptr) instanceof PtrLeft || queue.get(pptr) instanceof PtrRight)) {
				variableNodes.add(queue.get(pptr)); { ptr++; pptr++; }
			} else
				return new MatchResult(null, false, 0);
		}
		
		return new MatchResult(variableNodes, true, pptr);
	}

	public boolean isPedantic() {
		return pedantic;
	}

	public void setPedantic(boolean pedantic) {
		this.pedantic = pedantic;
	}
	
}
