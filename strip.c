#define a(v,l,r)s=v<0?l:r;v=abs(v);while(v--)putchar(s);v=0;
x,y,s,r,c;main(int argc,char**argv){r=argc==2&&strcmp("--kee"
"p-rle",argv[1]);while((c=getchar())!=-1){if(isspace(c)||!strchr
(9*r+"0123456789+-<>[].,",c));else if(c=='>'||c=='<'){a(y,'-'
,'+');x+=c=='>'?1:-1;}else if(c=='+'||c=='-'){a(x,'<','>');y+=
c=='+'?1:-1;}else{a(y,'-','+');a(x,'<','>');putchar(c);}}}
