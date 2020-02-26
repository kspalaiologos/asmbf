
#include <stdio.h>

#define STYLE_POSTFIX 1
#define STYLE_PREFIX 0

int main(int argc, char * argv[]) {
	int style = STYLE_POSTFIX, c, n = 0, z;
	
	if(argc == 2)
		if(!strcmp("postfix", argv[1]))
			style = STYLE_POSTFIX;
		else if(!strcmp("prefix", argv[1]))
			style = STYLE_PREFIX;
	
	while((c = getchar()) != EOF) {
		if(style == STYLE_PREFIX) {
			z = getchar();
			if(isdigit(z)) {
				n = 0;
				
				while(isdigit(z)) {
					n += z - '0';
					n *= 10;
					
					z = getchar();
				}
				
				n /= 10;
				
				ungetc(z, stdin);
				
				while(n --> 0)
					putchar(c);
			} else {
				putchar(c);
				ungetc(z, stdin);
			}
		} else {
			if(!isdigit(c))
				n = 10;
			else
				n = 0;
			
			while(isdigit(c)) {
				n += c - '0';
				n *= 10;
				
				c = getchar();
			}
			
			n /= 10;
			
			while(n --> 0)
				putchar(c);
		}
	}
}
