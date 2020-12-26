
REM *** xlt ***

REM a b  c-e f  g  h  i  j  k-p q r  s  t u [stack] 0 0 [taperam]
REM g ip t   r1 r2 r3 r4 im t   f r5 r6 0 0 -------

REM In each of the clauses, c, d and e are perfectly clean.

c = signum $1        c = $1 mod 2                1[-c+d+1]n++c[m+>-[<<]<[[>+<-]<<]>>c-]m[c+m-]n[-]d[-1+d]
d = signum $2        d = $2 mod 2                2[-d+e+2]n++d[m+>-[<<]<[[>+<-]<<]>>d-]m[d+m-]n[-]e[-2+e]
if(c)                e = 1                       e+
                     while(c)                    c[
                     c = 0                       c-
if(d)                while(d)                    d[
                     e = 0                       e-
                     d = 0                       d-
$1 = $1 < $2         gt $1, $2                   1[m+1-]2[n+2-]m[2+m>[-<]>]<<[[-]1+2-m<<]>>n[2+n-]
else                 wend(d)                     d]
                     while(e)                    e[
                     e = 0                       e-
REM $1 < $2 by def.
                     $1 = 1                      1[-]+
end                  wend(e)                     e]
                     wend(c)                     c]
else
                     while(e)                    e[
if(d)                while(d)                    d[
                     e = 0                       e-
                     d = 0                       d-
REM $1 > $2 by def.
                     $1 = 0                      1[-]
else                 wend(d)                     d]
                     while(e)                    e[
                     e = 0                       e-
$1 = $1 < $2         lt $1, $2                   1[n+1-]2[m+2-]m[2+m>[-<]>]<<[1-m-[2+m-]<<]>>n[-]
end                  wend(e)                     e]
                     wend(e)                     e]
end

*** END ***
