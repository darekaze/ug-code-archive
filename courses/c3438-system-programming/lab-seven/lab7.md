# COMP3438 Lab 7

JAHJA Darwin, 16094501d

---

## 1. Identify tokens

Keywords: `var, while, do, if, then, else, begin, end`

Identifiers: `i, a, b, c, d, var1`

Constant:  1, 5, 6, 99, 2.0E12, 3E-12, 0.21

Operators: `= < == + / ^ *`

Special symbols: `, {} () ;`

## 2. Regex

For {a, b}, Regex for:

**(a)** `^b(a|b)*aba$` : Find strings that begin with b and end with aba.

**(b)** `^a*ba*ba*$` : Find strings that contain exactly two b's. 

**(c)** `^a*bba*$` : Find strings that contain exactly two consecutive b's. 

## 3. C program

See q3_tb.c
