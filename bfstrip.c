#define a(v,l,r,z)({s=v<0?l:r;v=abs(v);while(v--)putchar(s);v=0;z;})
x,y,s,r,c;main(int n,char**p){r=n==2&&strcmp("--keep-rle",p+1);while
(~(c=getchar()))!isspace(c)&&strchr(9*r+"0123456789+-<>[].,",c)?c-60
&~2?c-43&~2?a(y,45,43,a(x,60,62,putchar(c))):a(x,60,62,y+=44-c):a(y,
45,43,x+=c-61):0;}
