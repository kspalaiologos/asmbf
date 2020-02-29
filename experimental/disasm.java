/* A disassembler proof-of-concept. It has to be improved before it can be used
   efficiently. Help is more than welcome. Copyright (C) by Palaiologos, 2019. */

package bfdisasm;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.function.Predicate;
import java.util.stream.Collectors;
import java.util.stream.Stream;

public class Start {
	
	@SuppressWarnings("serial")
	private class CharacterList<T> extends ArrayList<T> {
		public void chomp(int amount) {
			if(amount > this.size())
				throw new IndexOutOfBoundsException();
			
			this.removeRange(0, amount);
		}
	}
	
	private class Disassembly {
		String mnemonic;
		int consumption;
		
		public Disassembly(String a, int b) {
			mnemonic = a;
			consumption = b;
		}
	}
	
	private interface Pattern {
		Disassembly match(Start instance);
	}
	
	private static class Helper {
		public static boolean hasRegex(Start instance, String regex, int offset) {
			String value = instance.queue
							.stream()
							.map(e -> e.toString())
							.reduce((acc, e) -> acc  + e).get();
			java.util.regex.Matcher m = java.util.regex.Pattern.compile(regex).matcher(value);
			return m.find() && value.indexOf(m.group(1)) == offset;
		}
		
		public static int regexLength(Start instance, String regex, int offset) {
			String value = instance.queue
							.stream()
							.map(e -> e.toString())
							.reduce((acc, e) -> acc  + e).get();
			java.util.regex.Matcher m = java.util.regex.Pattern.compile(regex).matcher(value);
			return m.find() ? m.group(1).length() : -1;
		}
		
		public static String regexText(Start instance, String regex, int offset) {
			String value = instance.queue
							.stream()
							.map(e -> e.toString())
							.reduce((acc, e) -> acc  + e).get();
			java.util.regex.Matcher m = java.util.regex.Pattern.compile(regex).matcher(value);
			return m.find() ? m.group(1) : "";
		}
		
		public static boolean hasRegex(Start instance, String regex, int offset, int group) {
			String value = instance.queue
							.stream()
							.map(e -> e.toString())
							.reduce((acc, e) -> acc  + e).get();
			java.util.regex.Matcher m = java.util.regex.Pattern.compile(regex).matcher(value);
			return m.find() && value.indexOf(m.group(group)) == offset;
		}
		
		public static int regexLength(Start instance, String regex, int offset, int group) {
			String value = instance.queue
							.stream()
							.map(e -> e.toString())
							.reduce((acc, e) -> acc  + e).get();
			java.util.regex.Matcher m = java.util.regex.Pattern.compile(regex).matcher(value);
			return m.find() ? m.group(group).length() : -1;
		}
		
		public static boolean hasSequence(Start instance, String seq, int offset) {
			String value = instance.queue
							.stream()
							.map(e -> e.toString())
							.reduce((acc, e) -> acc  + e).get();
			return value.indexOf(seq) == offset;
		}

		public static String stringifyPos(Start instance) {
			switch(instance.position) {
			case 0: return "g"; case 1: return "ip"; case 2: return "t0"; case 3: return "t1"; case 4: return "t2";
			case 5: return "r1"; case 6: return "r2"; case 7: return "r3"; case 8: return "r4"; case 9: return "im";
			case 10: return "t3"; case 11: return "t4"; case 12: return "t5"; case 13: return "t6"; case 14: return "t7";
			case 15: return "0";case 16: return "a";case 17: return "b";case 18: return "c";case 19: return "d";
			case 20: return "e";case 21: return "f";case 22: return "g";case 23: return "h";case 24: return "i";
			case 25: return "j";case 26: return "k";case 27: return "l";case 28: return "m";case 29: return "n";
			case 30: return "o";case 31: return "p";case 32: return "q";case 33: return "r";case 34: return "s";
			case 35: return "t";case 36: return "u";case 37: return "v";case 38: return "w";case 39: return "x";
			case 40: return "z";default: return "??";
			}
		}
	}
	
	@SuppressWarnings("serial")
	private class BreakException extends RuntimeException {}
	
	protected CharacterList<Character> queue = new CharacterList<Character>();
	protected List<Pattern> patterns = new CharacterList<Pattern>();
	protected int position = 0;
	
	private String readAll(java.io.InputStream is) {
	    java.util.Scanner s = new java.util.Scanner(is).useDelimiter("\\A");
	    return s.hasNext() ? s.next() : "";
	}
	
	public Start() throws IOException {
		// entry pattern
		patterns.add(new Pattern() {
			@Override
			public Disassembly match(Start instance) {
				if(Start.Helper.hasSequence(instance, "+>+[", 0)) {
					instance.position++;
					return new Disassembly("; -> frs", 4);
				}
				
				return null;
			}
		});
		
		// and pattern
		patterns.add(new Pattern() {
			@Override
			public Disassembly match(Start instance) {
				// 1[e+1-]e[e[-]2[e+d+2-]d[2+d-]e[1-e[-]]]\0
				String disasm = "; -> and ";
				String par1, par2;
				
				// Happy Halloween!
				// <[<+>-]<[[-]>>[<<+<+>>>-]<<<[>>>+<<<-]>[>-<[-]]]
				if(Start.Helper.hasRegex(instance,
						"(^(?:<+|>+|)\\[(?:>+|<+)\\+(?:>+|<+)\\-\\](?:>+|<+)\\[\\[\\-\\](?:>+|<+)\\[(?:>+|<+)\\+(?:>+|<+)\\+(?:>+|<+)\\-\\](?:>+|<+)\\[(?:>+|<+)\\+(?:>+|<+)\\-\\](?:>+|<+)\\[(?:>+|<+)\\-(?:>+|<+)\\[\\-\\]\\]\\])", 0)) {
					// Got a possible match.
					
					int p1 = Start.Helper.regexLength(instance, "^(<+|>+|)", 0);
					int p2 = Start.Helper.regexLength(instance, "^(?:<+|>+|)\\[(?:>+|<+)\\+(?:>+|<+)\\-\\](?:>+|<+)\\[\\[\\-\\](>+|<+)", 0);
					
					p2 *=  Start.Helper.regexText(
									instance,
									"^(?:<+|>+|)\\[(?:>+|<+)\\+(?:>+|<+)\\-\\](?:>+|<+)\\[\\[\\-\\](>+|<+)",
									0).charAt(0) == '<' ? -1 : 1;
					p1 *= instance.queue.get(0)  == '<' ? -1 : 1;
					
					instance.position += p1;
					par1 = Start.Helper.stringifyPos(instance);
					
					instance.position += 4+p2;
					par2 = Start.Helper.stringifyPos(instance);
					
					instance.position = 4;
					
					return new Disassembly(
							disasm + par1 + ", " + par2, Start.Helper.regexLength(
									instance,
									"(^(?:<+|>+|)\\[(?:>+|<+)\\+(?:>+|<+)\\-\\](?:>+|<+)\\[\\[\\-\\](?:>+|<+)\\[(?:>+|<+)\\+(?:>+|<+)\\+(?:>+|<+)\\-\\](?:>+|<+)\\[(?:>+|<+)\\+(?:>+|<+)\\-\\](?:>+|<+)\\[(?:>+|<+)\\-(?:>+|<+)\\[\\-\\]\\]\\])",
									0
							)
					);
				}
				
				return null;
			}
		});
		
		// immediate clear pattern
		patterns.add(new Pattern() {
			@Override
			public Disassembly match(Start instance) {
				if(Start.Helper.hasRegex(instance, "(^(?:<+|>+|)\\[\\-\\])", 0)) {
					instance.position +=
							instance.queue.get(0) == '<' ?
									-Start.Helper.regexLength(instance, "(^(?:<+|>+|)\\[\\-\\])", 0)+3
								   : Start.Helper.regexLength(instance, "(^(?:<+|>+|)\\[\\-\\])", 0)-3;
					return new Disassembly(
							"; -> imc " + Start.Helper.stringifyPos(instance),
							Start.Helper.regexLength(instance, "(^(?:<+|>+|)\\[\\-\\])", 0)
					);
				}
				
				return null;
			}
		});
		
		// immediate modify pattern
		patterns.add(new Pattern() {
			@Override
			public Disassembly match(Start instance) {
				if(Start.Helper.hasRegex(instance, "(^(?:<+|>+|)[\\+\\-\\,\\.])", 0)) {
					int backup = instance.position;
					backup +=
							instance.queue.get(0) == '<' ?
									-Start.Helper.regexLength(instance, "^(<+|>+|)", 0)
								   : Start.Helper.regexLength(instance, "^(<+|>+|)", 0);
					if(backup >= 5 && backup <= 8) {
						switch(Start.Helper.regexText(instance, "^(?:<+|>+|)([\\+\\-\\,\\.])", 0).charAt(0)) {
						case '+':
							return new Disassembly(
										"; -> inc "
								      + Start.Helper.stringifyPos(instance), 
								Start.Helper.regexLength(instance, "(^(?:<+|>+|)[\\+\\-\\,\\.])", 0)
							);
						case '-':
							return new Disassembly(
										"; -> dec "
								      + Start.Helper.stringifyPos(instance), 
								Start.Helper.regexLength(instance, "(^(?:<+|>+|)[\\+\\-\\,\\.])", 0)
							);
						case ',':
							return new Disassembly(
										"; -> in_ "
								      + Start.Helper.stringifyPos(instance), 
								Start.Helper.regexLength(instance, "(^(?:<+|>+|)[\\+\\-\\,\\.])", 0)
							);
						case '.':
							return new Disassembly(
										"; -> out "
								      + Start.Helper.stringifyPos(instance), 
								Start.Helper.regexLength(instance, "(^(?:<+|>+|)[\\+\\-\\,\\.])", 0)
							);
						}
					}
				}
				
				return null;
			}
		});
		
		// immediate modify pattern
		patterns.add(new Pattern() {
			@Override
			public Disassembly match(Start instance) {
				if(Start.Helper.hasRegex(instance, "(^(?:<+|>+|)(?:\\++|\\-+))", 0)) {
					instance.position +=
							instance.queue.get(0) == '<' ?
									-Start.Helper.regexLength(instance, "^(<+|>+|)", 0)
								   : Start.Helper.regexLength(instance, "^(<+|>+|)", 0);
					return new Disassembly(
								"; -> imm "
						      + Start.Helper.stringifyPos(instance)
						      + " "
						      + (
						    		  instance.queue.get(Start.Helper.regexLength(instance, "^(<+|>+|)", 0)) == '-'
						    		  ? "-"
						    		  : "+" 
						    	)
						      + "= "
						      + Start.Helper.regexLength(instance, "(^(?:<+|>+|)(\\++|\\-+))", 0), 
						Start.Helper.regexLength(instance, "(^(?:<+|>+|)(?:\\++|\\-+))", 0)
					);
				}
				
				return null;
			}
		});
		
		// last pattern
		patterns.add(new Pattern() {
			@Override
			public Disassembly match(Start instance) {
				String match = "";
				int move = 1 - instance.position;
				if(move > 0) {
					match = Stream.generate(() -> ">").limit(move).collect(Collectors.joining(""));
				} else if(move < 0) {
					match = Stream.generate(() -> "<").limit(-move).collect(Collectors.joining(""));
				}
				match += "]";
				
				if(Start.Helper.hasSequence(instance, match, 0)) {
					instance.position = 1;
					return new Disassembly(
								"; -> lst ",
								match.length()
					);
				}
				
				return null;
			}
		});
		
		// post pattern
		patterns.add(new Pattern() {
			@Override
			public Disassembly match(Start instance) {
				String match = "";
				int move = 3 - instance.position;
				if(move > 0) {
					match = Stream.generate(() -> ">").limit(move).collect(Collectors.joining(""));
				} else if(move < 0) {
					match = Stream.generate(() -> "<").limit(-move).collect(Collectors.joining(""));
				}
				match += "]";
				
				if(Start.Helper.hasSequence(instance, match, 0)) {
					instance.position = 3;
					return new Disassembly(
								"; -> pst ",
								match.length()
					);
				}
				
				return null;
			}
		});
		
		// prefix pattern
		patterns.add(new Pattern() {
			@Override
			public Disassembly match(Start instance) {
				// a[c+d+a-]c[a+c-]d[[-]
				
				String match = "";
				int move = -instance.position;
				if(move > 0) {
					match = Stream.generate(() -> ">").limit(move).collect(Collectors.joining(""));
				} else if(move < 0) {
					match = Stream.generate(() -> "<").limit(-move).collect(Collectors.joining(""));
				}
				match += "[>>+>+<<<-]>>[<<+>>-]>[[-]";
				
				if(Start.Helper.hasSequence(instance, match, 0)) {
					instance.position = 3;
					return new Disassembly(
								"; -> pre ",
								match.length()
					);
				}
				
				return null;
			}
		});
		
		// add pattern
		patterns.add(new Pattern() {
			@Override
			public Disassembly match(Start instance) {
				// 2[1+e+2-]e[2+e-]
				String disasm = "; -> add ";
				String par1, par2;
				
				if(Start.Helper.hasRegex(instance,
						"(^(?:<+|>+|)\\[(?:<+|>+)\\+(?:<+|>+)\\+(?:<+|>+)\\-\\](?:<+|>+)\\[(?:<+|>+)\\+(?:<+|>+)\\-\\])", 0)) {
					// Got a possible match.
					
					int p2 = Start.Helper.regexLength(instance, "^(<+|>+|)\\[(?:<+|>+)", 0);
					int p1 = Start.Helper.regexLength(instance, "^(?:<+|>+|)\\[(<+|>+)", 0);
					
					p1 *= instance.queue.get(p2+1)     == '<' ? -1 : 1;
					p2 *= instance.queue.get(0)        == '<' ? -1 : 1;
					
					instance.position += p2;
					par2 = Start.Helper.stringifyPos(instance);
					
					instance.position += p1;
					par1 = Start.Helper.stringifyPos(instance);
					
					instance.position = 4;
					
					return new Disassembly(
							disasm + par1 + ", " + par2, Start.Helper.regexLength(
									instance,
									"(^(?:<+|>+|)\\[(?:<+|>+)\\+(?:<+|>+)\\+(?:<+|>+)\\-\\](?:<+|>+)\\[(?:<+|>+)\\+(?:<+|>+)\\-\\])",
									0
							)
					);
				}
				
				return null;
			}
		});
		
		patterns.add(new Pattern() {
			@Override
			public Disassembly match(Start instance) {
				return new Disassembly("raw ." + instance.queue.get(0), 1);
			}
		});
		
		queue.addAll(readAll(System.in).chars().mapToObj(c -> {return (char) c;}).collect(Collectors.toList()));
		queue.removeIf(new Predicate<Character>() {
			@Override
			public boolean test(Character c) {
				return c != '+' && c != '-' && c != '<' && c != '>' && c != '.' && c != ',' && c != '[' && c != ']' && c != '*';
			}
		});
		
		while(!queue.isEmpty())
			match();
	}
	
	public void match() {
		// An antipattern (escaping forEach)
		// but it works. Just a proof of concept, c'mon.
		
		try {
			patterns.forEach((Pattern arg0) -> {
				Disassembly result = arg0.match(this);
				
				if(result != null && result.consumption > 0) {
					System.out.println(result.mnemonic);
					queue.chomp(result.consumption);
					
					throw new BreakException();
				}
			});
		} catch(BreakException r) {}
	}
	
	public static void main(String[] args) {
		try {
			new Start();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}
