#![allow(dead_code,
         mutable_transmutes,
         non_camel_case_types,
         non_snake_case,
         non_upper_case_globals,
         unused_mut)]
#![feature(libc)]
extern crate libc;
extern "C" {
    #[no_mangle]
    fn getchar() -> libc::c_int;
    #[no_mangle]
    fn putchar(__c: libc::c_int) -> libc::c_int;
}
/* asm2bf
 *
 * This program has been translated automatically.
 *
 * Copyright (C) Kamila Palaiologos Szewczyk, 2017-2019.
 * License: MIT
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#[no_mangle]
pub unsafe extern "C" fn inchar() -> libc::c_uint {
    let mut c: libc::c_int = getchar();
    if c <= 0i32 { c = 0i32 }
    return c as libc::c_uint;
}
#[no_mangle]
pub unsafe extern "C" fn outbf() {
    let mut current_block: u64;
    let mut r1: libc::c_uint = 0;
    let mut r4: libc::c_uint = 0;
    m[7usize] = 131i32 as libc::c_uint;
    r4 = 0i32 as libc::c_uint;
    while !(r4 >= m[6usize]) {
        loop  {
            r1 = m[m[7usize] as usize];
            m[7usize] = m[7usize].wrapping_add(1);
            if !(r1 != '\u{0}' as i32 as libc::c_uint) { break ; }
        }
        r4 = r4.wrapping_add(1)
    }
    loop  {
        r1 = m[m[7usize] as usize];
        if r1 == '\u{0}' as i32 as libc::c_uint { break ; }
        if !(r1 != '1' as i32 as libc::c_uint) { r1 = m[5usize] }
        if !(r1 != '2' as i32 as libc::c_uint) { r1 = m[4usize] }
        if r1 != '*' as i32 as libc::c_uint {
            if r1 != '^' as i32 as libc::c_uint {
                if r1 < 'a' as i32 as libc::c_uint {
                    current_block = 6476622998065200121;
                } else if r1 > 'z' as i32 as libc::c_uint {
                    current_block = 6476622998065200121;
                } else {
                    r1 = r1.wrapping_sub('a' as i32 as libc::c_uint);
                    current_block = 11459959175219260272;
                }
                match current_block {
                    11459959175219260272 => { }
                    _ => {
                        putchar(r1 as libc::c_int);
                        current_block = 11793792312832361944;
                    }
                }
            } else { r1 = m[10usize]; current_block = 11459959175219260272; }
        } else { r1 = m[9usize]; current_block = 11459959175219260272; }
        match current_block {
            11459959175219260272 => {
                if r1 < m[8usize] {
                    r4 = 0i32 as libc::c_uint;
                    m[15usize] = m[8usize].wrapping_sub(r1);
                    while !(m[15usize] <= r4) {
                        putchar('<' as i32);
                        r4 = r4.wrapping_add(1)
                    }
                } else {
                    r4 = 0i32 as libc::c_uint;
                    m[15usize] = r1.wrapping_sub(m[8usize]);
                    while !(m[15usize] <= r4) {
                        putchar('>' as i32);
                        r4 = r4.wrapping_add(1)
                    }
                }
                m[8usize] = r1
            }
            _ => { }
        }
        m[7usize] = m[7usize].wrapping_add(1)
    };
}
#[no_mangle]
pub static mut m: [libc::c_uint; 2000] = [0; 2000];
#[no_mangle]
pub unsafe extern "C" fn outrep() {
    while 0 != m[3usize] {
        putchar(m[6usize] as libc::c_int);
        m[3usize] = m[3usize].wrapping_sub(1)
    };
}
unsafe fn main_0() -> libc::c_int {
    let mut current_block: u64;
    let mut n: libc::c_uint = 0;
    let mut s: *mut libc::c_char =
        b"addanddecdiveq_ge_gt_in_incjmpjnzjz_lblle_lt_modmovmulne_negnotor_outpoppshrclstosubswpclrretendstkorgdb_txtrawa+b+[\x00b]\x00a[c+d+a-]c[a+c-]d[[-]\x00d]\x002\x002[-]\x002[1+e+2-]e[2+e-]\x001[e+1-]e[e[-]2[e+d+2-]d[2+d-]e[1-e[-]]]\x002-\x001[c+1-]c[2[d+e+2-]e[2+e-]d[e+c-[e[-]k+c-]k[c+k-]e[d-[1-d[-]]+e-]d-]1+c]\x001[d+1-]+2[d-c+2-]c[2+c-]d[1-d[-]]\x001[d+1-]+2[c+k+e+2-]e[2+e-]k[d[l+e+d-]e[d+e-]+l[c-d-e-l[-]]e[k[-]+e-]k-]c[1-c[-]]d[-]\x001[d+1-]2[c+k+e+2-]e[2+e-]k[d[l+e+d-]e[d+e-]+l[c-d-e-l[-]]e[k[-]+e-]k-]d[1+d[-]]c[-]\x002,\x002+\x00a[-]b[-]2[b+c+2-]c[2+c-]\x001[c+d+1-]c[1+c-]d[a[-]b[-]2[b+c+2-]c[2+c-]d[-]]\x00d+1[d[-]c+1-]c[1+c-]d[a[-]b[-]2[b+c+2-]c[2+c-]d[-]]\x00c+a[c-d+a-]d[a+d-]c[-d+b[e-c+b-]c[b+c-]e[d-e[-]]d[a+d-]c]e[-]\x001[d+1-]+2[c+k+e+2-]e[2+e-]k[d[l+e+d-]e[d+e-]+l[c-d-e-l[-]]e[k[-]+e-]k-]d[1-d[-]]c[-]\x001[d+1-]2[c+k+e+2-]e[2+e-]k[d[l+e+d-]e[d+e-]+l[c-d-e-l[-]]e[k[-]+e-]k-]c[1+c[-]]d[-]\x001[k+1-]2[l+e+2-]e[2+e-]k[->>+<-[>]>[[<+>-]>>>+<]<<<<]p[-]m[1+m-]l[-]\x001[-]2[1+e+2-]e[2+e-]\x001[d+1-]d[2[1+e+2-]e[2+e-]d-]\x001[d+1-]2[d-e+2-]e[2+e-]d[1+d[-]]\x002[e-2-]e[2+e-]\x002-[e-2-]e[2+e-]\x001[e+1-]e[1-e[-]]2[e+d+2-]d[2+d-]e[1[-]-e[-]]\x002.\x002[-]q[-]>[>>]<<->[<<<[<<]>+>[>>]>-]<<<[<<]>[2+q-]\x002[e+q+2-]e[2+e-]q>[>>]+<<[<<]>[>[>>]<+<[<<]>-]\x001[-]2[e+o+*>+<2-]e[2+e-]*>[[>>]+[<<]>>-]+[>>]<[<[<<]>+1+*>[>>]<-]<[<<]>[>[>>]<+<[<<]>-]>[>>]<<[-<<]>\x001[e+*>+<1-]e[1+e-]2[e+*+2-]e[2+e-]*>[[>>]+[<<]>>-]+[>>]<[-]<[<<]>[>[>>]<+<[<<]>-]>[>>]<<[-<<]>\x002[1-e+2-]e[2+e-]\x001[e+1-]2[1+2-]e[2+e-]\x002[-]\x00a[-]b[-]q[-]>[>>]<<->[<<<[<<]>+>[>>]>-]<<<[<<]>[b+q-]\x00a[-]b[-]\x00\x00"
            as *const u8 as *const libc::c_char as *mut libc::c_char;
    n = 0i32 as libc::c_uint;
    while n < 1900i32 as libc::c_uint {
        m[n.wrapping_add(20i32 as libc::c_uint) as usize] =
            *s.offset(n as isize) as libc::c_uint;
        n = n.wrapping_add(1)
    }
    m[6usize] = 0i32 as libc::c_uint;
    m[8usize] = 0i32 as libc::c_uint;
    m[9usize] = 18i32 as libc::c_uint;
    m[10usize] = m[9usize].wrapping_add(2i32 as libc::c_uint);
    outbf();
    m[11usize] = 1i32 as libc::c_uint;
    m[12usize] = 0i32 as libc::c_uint;
    m[1usize] = 1i32 as libc::c_uint;
    's_59:
        loop  {
            m[0usize] = inchar();
            loop  {
                if m[0usize] != '\u{0}' as i32 as libc::c_uint {
                    if m[0usize] != '\n' as i32 as libc::c_uint &&
                           m[0usize] != '\u{b}' as i32 as libc::c_uint &&
                           m[0usize] != '\u{c}' as i32 as libc::c_uint &&
                           m[0usize] != '\r' as i32 as libc::c_uint {
                        if m[1usize] == 4i32 as libc::c_uint {
                            if m[0usize] == '\"' as i32 as libc::c_uint {
                                m[1usize] = 1i32 as libc::c_uint;
                                continue 's_59 ;
                            } else {
                                m[2usize] = 34i32 as libc::c_uint;
                                m[3usize] = m[0usize]
                            }
                        } else {
                            if m[0usize] == ' ' as i32 as libc::c_uint ||
                                   m[0usize] == '\t' as i32 as libc::c_uint ||
                                   m[1usize] == 0i32 as libc::c_uint {
                                continue 's_59 ;
                            }
                            if m[1usize] != 3i32 as libc::c_uint {
                                if m[0usize] != ';' as i32 as libc::c_uint {
                                    /* comment */
                                    if m[1usize] != 1i32 as libc::c_uint {
                                        /* get 3 letter instruction */
                                        if m[2usize] != 35i32 as libc::c_uint
                                           {
                                            /* txt command */
                                            if m[0usize] !=
                                                   'r' as i32 as libc::c_uint
                                               {
                                                current_block =
                                                    6281126495347172768;
                                                break ;
                                            } else {
                                                current_block =
                                                    1868291631715963762;
                                                break ;
                                            }
                                        } else {
                                            m[1usize] = 3i32 as libc::c_uint;
                                            continue ;
                                        }
                                    } else {
                                        m[2usize] = inchar();
                                        m[3usize] = inchar();
                                        m[4usize] = 0i32 as libc::c_uint;
                                        current_block = 1847472278776910194;
                                        break ;
                                    }
                                } else if !(m[1usize] == 2i32 as libc::c_uint)
                                 {
                                    m[1usize] = 0i32 as libc::c_uint;
                                    continue 's_59 ;
                                }
                            } else {
                                if m[0usize] != '\"' as i32 as libc::c_uint {
                                    break 's_59 ;
                                }
                                m[1usize] = 4i32 as libc::c_uint;
                                continue 's_59 ;
                            }
                        }
                    } else if !(m[1usize] == 2i32 as libc::c_uint) {
                        m[1usize] = 1i32 as libc::c_uint;
                        continue 's_59 ;
                    }
                } else if !(m[1usize] == 2i32 as libc::c_uint) {
                    if m[11usize] == 0i32 as libc::c_uint ||
                           m[12usize] == 1i32 as libc::c_uint {
                        m[6usize] = 3i32 as libc::c_uint;
                        outbf();
                    }
                    m[6usize] = 2i32 as libc::c_uint;
                    outbf();
                    m[6usize] = 37i32 as libc::c_uint;
                    outbf();
                    m[6usize] = 3i32 as libc::c_uint;
                    outbf();
                    m[6usize] = 1i32 as libc::c_uint;
                    outbf();
                    return 0i32
                }
                if 0 != m[4usize] && m[4usize] == m[5usize] {
                    m[6usize] = 22i32 as libc::c_uint;
                    m[5usize] = 'j' as i32 as libc::c_uint;
                    outbf();
                    m[5usize] = m[4usize];
                    m[4usize] = 'j' as i32 as libc::c_uint
                }
                if m[11usize] == 1i32 as libc::c_uint &&
                       m[2usize] != 12i32 as libc::c_uint {
                    m[6usize] = 2i32 as libc::c_uint;
                    outbf();
                    m[11usize] = 0i32 as libc::c_uint
                }
                match m[2usize] {
                    0 => {
                        if m[4usize] == 0i32 as libc::c_uint {
                            m[6usize] = 4i32 as libc::c_uint;
                            m[4usize] = m[5usize];
                            outbf();
                            m[6usize] = '+' as i32 as libc::c_uint;
                            outrep();
                            current_block = 2942604368452602584;
                        } else { current_block = 5393508166988163885; }
                    }
                    9 => {
                        m[11usize] = 1i32 as libc::c_uint;
                        m[12usize] = 1i32 as libc::c_uint;
                        current_block = 5393508166988163885;
                    }
                    10 => {
                        m[11usize] = 1i32 as libc::c_uint;
                        m[12usize] = 1i32 as libc::c_uint;
                        current_block = 5393508166988163885;
                    }
                    11 => {
                        m[11usize] = 1i32 as libc::c_uint;
                        m[12usize] = 1i32 as libc::c_uint;
                        current_block = 5393508166988163885;
                    }
                    12 => {
                        if m[11usize] == 0i32 as libc::c_uint {
                            m[6usize] = 3i32 as libc::c_uint;
                            outbf();
                        }
                        m[11usize] = 1i32 as libc::c_uint;
                        m[6usize] = 4i32 as libc::c_uint;
                        m[4usize] = 'e' as i32 as libc::c_uint;
                        outbf();
                        m[6usize] = '+' as i32 as libc::c_uint;
                        outrep();
                        m[6usize] = 18i32 as libc::c_uint;
                        outbf();
                        current_block = 2942604368452602584;
                    }
                    16 => {
                        if m[4usize] == 0i32 as libc::c_uint {
                            m[6usize] = 5i32 as libc::c_uint;
                            m[4usize] = m[5usize];
                            outbf();
                            m[6usize] = '+' as i32 as libc::c_uint;
                            outrep();
                            current_block = 2942604368452602584;
                        } else { current_block = 5393508166988163885; }
                    }
                    27 => {
                        if m[4usize] == 0i32 as libc::c_uint {
                            m[6usize] = 4i32 as libc::c_uint;
                            m[4usize] = m[5usize];
                            outbf();
                            m[6usize] = '-' as i32 as libc::c_uint;
                            outrep();
                            current_block = 2942604368452602584;
                        } else { current_block = 5393508166988163885; }
                    }
                    30 => {
                        m[11usize] = 1i32 as libc::c_uint;
                        m[12usize] = 1i32 as libc::c_uint;
                        current_block = 5393508166988163885;
                    }
                    31 => {
                        m[11usize] = 1i32 as libc::c_uint;
                        m[12usize] = 1i32 as libc::c_uint;
                        current_block = 5393508166988163885;
                    }
                    32 => {
                        m[9usize] =
                            m[3usize].wrapping_mul(2i32 as
                                                       libc::c_uint).wrapping_add(18i32
                                                                                      as
                                                                                      libc::c_uint);
                        current_block = 2942604368452602584;
                    }
                    33 => {
                        m[10usize] =
                            m[3usize].wrapping_mul(2i32 as
                                                       libc::c_uint).wrapping_add(m[9usize]).wrapping_add(2i32
                                                                                                              as
                                                                                                              libc::c_uint);
                        current_block = 2942604368452602584;
                    }
                    34 => {
                        m[6usize] = 4i32 as libc::c_uint;
                        m[4usize] = '^' as i32 as libc::c_uint;
                        outbf();
                        m[6usize] = '+' as i32 as libc::c_uint;
                        outrep();
                        m[10usize] =
                            m[10usize].wrapping_add(2i32 as libc::c_uint);
                        if m[1usize] == 4i32 as libc::c_uint {
                            continue 's_59 ;
                        }
                        current_block = 2942604368452602584;
                    }
                    36 => {
                        putchar(m[3usize] as libc::c_int);
                        current_block = 2942604368452602584;
                    }
                    _ => { current_block = 5393508166988163885; }
                }
                match current_block {
                    5393508166988163885 => {
                        if m[4usize] == 0i32 as libc::c_uint {
                            m[6usize] = 4i32 as libc::c_uint;
                            m[4usize] = 'j' as i32 as libc::c_uint;
                            outbf();
                            m[6usize] = '+' as i32 as libc::c_uint;
                            outrep();
                            m[3usize] = m[3usize].wrapping_add(1)
                        }
                        m[6usize] =
                            m[2usize].wrapping_add(6i32 as libc::c_uint);
                        outbf();
                        if !(m[3usize] == 0i32 as libc::c_uint) {
                            m[6usize] = 5i32 as libc::c_uint;
                            outbf();
                        }
                    }
                    _ => { }
                }
                m[1usize] = 1i32 as libc::c_uint;
                if m[12usize] == 1i32 as libc::c_uint {
                    m[6usize] = 3i32 as libc::c_uint;
                    outbf();
                    m[12usize] = 0i32 as libc::c_uint
                }
            }
            match current_block {
                1847472278776910194 => {
                    loop  {
                        m[5usize] =
                            m[4usize].wrapping_add(20i32 as libc::c_uint);
                        m[6usize] = m[m[5usize] as usize];
                        if !(m[0usize] != m[6usize]) {
                            m[5usize] = m[5usize].wrapping_add(1);
                            m[6usize] = m[m[5usize] as usize];
                            if !(m[2usize] != m[6usize]) {
                                m[5usize] = m[5usize].wrapping_add(1);
                                m[6usize] = m[m[5usize] as usize];
                                if m[3usize] == m[6usize] {
                                    m[1usize] = 2i32 as libc::c_uint;
                                    m[2usize] =
                                        m[4usize].wrapping_div(3i32 as
                                                                   libc::c_uint);
                                    m[3usize] = 0i32 as libc::c_uint;
                                    m[4usize] = 0i32 as libc::c_uint;
                                    m[5usize] = 0i32 as libc::c_uint;
                                    break ;
                                }
                            }
                        }
                        m[4usize] = m[4usize].wrapping_add(1);
                        m[4usize] = m[4usize].wrapping_add(1);
                        m[4usize] = m[4usize].wrapping_add(1);
                        if !(m[4usize] == 111i32 as libc::c_uint) {
                            continue ;
                        }
                        /* not found, quit */
                        break 's_59 ;
                    }
                }
                1868291631715963762 => {
                    m[0usize] = inchar();
                    m[0usize] =
                        m[0usize].wrapping_sub('1' as i32 as libc::c_uint);
                    if m[0usize] > 3i32 as libc::c_uint {
                        /* not a number, quit */
                        break ;
                    } else {
                        m[4usize] =
                            m[0usize].wrapping_add('f' as i32 as libc::c_uint)
                    }
                }
                _ => {
                    /* r_ operand */
                    if m[0usize] != ',' as i32 as libc::c_uint {
                        /* , */
                        if m[0usize] != '.' as i32 as libc::c_uint {
                            /* .char */
                            m[0usize] =
                                m[0usize].wrapping_sub('0' as i32 as
                                                           libc::c_uint);
                            if m[0usize] > 9i32 as libc::c_uint {
                                /* not a number, quit */
                                break ;
                            } else {
                                m[3usize] =
                                    m[3usize].wrapping_mul(10i32 as
                                                               libc::c_uint);
                                m[3usize] = m[3usize].wrapping_add(m[0usize])
                            }
                        } else { m[3usize] = inchar() }
                    } else {
                        m[5usize] = m[4usize];
                        m[4usize] = 0i32 as libc::c_uint
                    }
                }
            }
        }
    putchar('#' as i32);
    return 0i32;
}
pub fn main() { unsafe { ::std::process::exit(main_0() as i32) } }
