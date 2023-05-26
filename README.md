# Getting started with GDB

GDB is the debugger linked with GCC and G++. This document / article aims to summarize what I learned recently with GDB.


## First steps : run a program

### A simple C program
Let's consider the following program: 
```C
#include <stdio.h>
#include <stdlib.h>

int fibonacci(int N)
{
        int i = 0;
        int prev = 0;
        int curr = 1;
        int buffer = 0;
        for(i=0; i<N; i++)
        {
                buffer = prev + curr;
                prev = curr;
                curr = buffer;
        }
        return prev;
}

int main(int argc, char **argv)
{
        int i = 0;
        int fibRes =  0;
        for(int i = 0; i<=9; i++)
        {
                fibRes = fibonacci(i);
                printf("Fibonacci %d %d\n", i, fibRes);
        }
        return 0;
}

```

This program is a simple fibonacci program that prints the 10 first elements of the fibonacci sequence. We may compile it as follow:
```
gcc -g test.c -o test
```
The option `-g` enables debugging information in the executable.

### Start in debug
To start the program in debug mode, we run `gdb` with the name of our program in debug mode as follow:
```
gdb test
```
We then get thrown in a new console but our program is not running yet. To run it, we use the command `run` or the simpler `r`.

## Breakpoints
For debugging, breakpoints are very useful since it allows you to stop your program at a given place in order to inspect what's happening.  

### Setup breakpoints
There are multiple ways to setup breakpoints. The main ones are the following:
- `b <line>`
- `b <file>:<line>`
- `b <file>:<function>`

> Actually, the full command is `break` but it is abbreviated `b`.

Let's say I want to stop before the for loop of our program to inspect the initial condition.
In the gdb console, I can use the command `list <filename>`. This will show the lines numbers pf the corresponding file. In our case, we want to stop at line 9. Thus:
```
b test.c:9
```
> I wrote the long version, but we could have used `b 9`.

Now that our breakpoint is set, we can run our program and see it stop at line 9.
```
r
```
After witnessing that the program has stopped at line 9, we can stop the program with the command `q`. 

### List, delete, ignore breakpoints
Now, let's say that we have multiple breakpoints.
```
b 9
b 26
b fibonacci
```
We can list these breakpoints with the command:
```
info break
```
Thus, we have the breakpoint identifier which allows us to:
- delete `delete <ID>`
- disable `disable <ID>`
- enable `enable <ID>`

Another option is to ignore a breakpoint N times. Let's say we want to see how the fibonacci function behave after 4 times.
```
b fibonacci
info break
ignore <ID> 4
r
```
The breakpoint has been ignored 4 times and we can now see how fibonacci's function behave at N=4.

> If the program was already running, `ignore` will step through the breakpoint for the N next times.

### Preconfigure your breakpoints
It's actually possible to set up multiple breakpoints in advance. For this, we can put them in a text file that we will import afterward. 

Ex: breaks.txt
```
b fibonacci
b 14
```
In `gdb` console, we import it with the command `source <file>`. 
> In fact, we can write a set of command in this file to "script GDB". This is detailed [here](#Scripting-GDB).

## Control GDB
To control GDB, there is a set of 4 commands:

Command | Function
---|---
r(un)		| Run the program 
c(ontinue)	| Continue after a breakpoint (will stop to the next breakpoint).
n(ext)		| Go to next source line without diving in functions.
s(tep)		| Go to next source line *diving* in functions.
f(inish)	| Continue until a return is met.

These commands allow you to control your program after a breakpoint.

## Examination
Now we know how to break our program and navigate in it, we want to examine its variables to understand where are the potential problems.

### Variables
It is possible to list the variables in the frame we are in with 
```
info locals
```

Similarly, we can see the arguments passed to the functions with the following:
```
info args
```

To see the value of a given variable, we may use `print` aka `p` with the variable name.

Ex: If we stopped after a breakpoint `b fibonacci`
```
p buffer
```
> If it is a pointer, you may use the same convention than C/C++ language. For instance `p *&buffer` would also work.

### Where am I?
In the case you need to know how you reached the breakpoint, you may use the `backtrace` aka `bt`

```
bt
```

If you need to trace further back, you may use `bt full`.

### Assembly savouring
In case of hunger of assembly code, here we are. This can be useful in case some variable are optimized out although you believe it shouldn't. 

We can use the command `disassemble`. 
If you run this command in a function (e.g. after `b fibonacci`), you will get the function disassembled as follow:
```asm
Dump of assembler code for function fibonacci:
   0x0000555555555149 <+0>:	endbr64 
   0x000055555555514d <+4>:	push   %rbp				; Save the base pointer register
   0x000055555555514e <+5>:	mov    %rsp,%rbp			; Store the stack pointer in RBP
   0x0000555555555151 <+8>:	mov    %edi,-0x14(%rbp)			; Argument N is stored in the stack at -0x14
=> 0x0000555555555154 <+11>:	movl   $0x0,-0x10(%rbp)			; i is at -0x10 set as 0
   0x000055555555515b <+18>:	movl   $0x0,-0xc(%rbp)			; prev at -0xc set as 0
   0x0000555555555162 <+25>:	movl   $0x1,-0x8(%rbp)			; curr at -0x8 set as 1
   0x0000555555555169 <+32>:	movl   $0x0,-0x4(%rbp)			; buffer at -0x4 set as 0
   0x0000555555555170 <+39>:	movl   $0x0,-0x10(%rbp)			; i = 0
   0x0000555555555177 <+46>:	jmp    0x555555555194 <fibonacci+75>	; goto +75
   0x0000555555555179 <+48>:	mov    -0xc(%rbp),%edx			; prev -> reg edx   |
   0x000055555555517c <+51>:	mov    -0x8(%rbp),%eax			; curr -> reg eax   |
   0x000055555555517f <+54>:	add    %edx,%eax			; eax = edx + eax   |
   0x0000555555555181 <+56>:	mov    %eax,-0x4(%rbp)			; reg eax -> buffer -> buffer = prev + curr
   0x0000555555555184 <+59>:	mov    -0x8(%rbp),%eax			; curr -> reg eax   |
   0x0000555555555187 <+62>:	mov    %eax,-0xc(%rbp)			; reg eax -> prev   -> prev = curr
   0x000055555555518a <+65>:	mov    -0x4(%rbp),%eax			; buffer -> reg eax |
   0x000055555555518d <+68>:	mov    %eax,-0x8(%rbp)			; reg eax -> curr   -> curr = buffer 
   0x0000555555555190 <+71>:	addl   $0x1,-0x10(%rbp)			; i = i + 1
   0x0000555555555194 <+75>:	mov    -0x10(%rbp),%eax			; i -> reg eax       |
   0x0000555555555197 <+78>:	cmp    -0x14(%rbp),%eax 		; compare eax with N |
   0x000055555555519a <+81>:	jl     0x555555555179 <fibonacci+48>	; if less, jump      -> if i < N goto +48
   0x000055555555519c <+83>:	mov    -0xc(%rbp),%eax			; prev -> reg eax
   0x000055555555519f <+86>:	pop    %rbp				; retrieve base pointer register
   0x00005555555551a0 <+87>:	ret    					; return 
End of assembler dump.
```
> The comment are added by me. Don't expect them from `gdb`.

You may want to use the `intel` assembly flavor instead of the default `att`:
```
set disassembly-flavor intel
```

which looks like this : 
```asm
Dump of assembler code for function fibonacci:
   0x0000555555555149 <+0>:	endbr64 
   0x000055555555514d <+4>:	push   rbp
   0x000055555555514e <+5>:	mov    rbp,rsp
   0x0000555555555151 <+8>:	mov    DWORD PTR [rbp-0x14],edi
=> 0x0000555555555154 <+11>:	mov    DWORD PTR [rbp-0x10],0x0
   0x000055555555515b <+18>:	mov    DWORD PTR [rbp-0xc],0x0
   0x0000555555555162 <+25>:	mov    DWORD PTR [rbp-0x8],0x1
   0x0000555555555169 <+32>:	mov    DWORD PTR [rbp-0x4],0x0
   0x0000555555555170 <+39>:	mov    DWORD PTR [rbp-0x10],0x0
   0x0000555555555177 <+46>:	jmp    0x555555555194 <fibonacci+75>
   0x0000555555555179 <+48>:	mov    edx,DWORD PTR [rbp-0xc]
   0x000055555555517c <+51>:	mov    eax,DWORD PTR [rbp-0x8]
   0x000055555555517f <+54>:	add    eax,edx
   0x0000555555555181 <+56>:	mov    DWORD PTR [rbp-0x4],eax
   0x0000555555555184 <+59>:	mov    eax,DWORD PTR [rbp-0x8]
   0x0000555555555187 <+62>:	mov    DWORD PTR [rbp-0xc],eax
   0x000055555555518a <+65>:	mov    eax,DWORD PTR [rbp-0x4]
   0x0000555555555190 <+71>:	add    DWORD PTR [rbp-0x10],0x1
   0x0000555555555194 <+75>:	mov    eax,DWORD PTR [rbp-0x10]
   0x0000555555555197 <+78>:	cmp    eax,DWORD PTR [rbp-0x14]
   0x000055555555519a <+81>:	jl     0x555555555179 <fibonacci+48>
   0x000055555555519c <+83>:	mov    eax,DWORD PTR [rbp-0xc]
   0x000055555555519f <+86>:	pop    rbp
   0x00005555555551a0 <+87>:	ret    
End of assembler dump.
```

> Note the symbol `=>` showing where you are.

To read the registers, your can `print` or use `info` as 
```
info registers
p $eax
```

## Manipulate
You can change the value of a variable with the command `set`

ex, after `b fibonacci`:
```
set prev 1
```

You can also return form the function and push a given value with the command `return`.

## Shared library's problem
Sometimes, it happens that the files we what to debug are in fact inside a shared library. And even if the library is compiled with the debug info, it needs to be loaded to access its symbols.

Let's split our example in multiple files.

*test2.c:*
```c
#include <stdio.h>
#include <stdlib.h>

#include "fibonacci.h"

int main(int argc, char **argv)
{
        int i = 0;
        int fibRes =  0;
        for(int i = 0; i<=9; i++)
        {
                fibRes = fibonacci(i);
                printf("Fibonacci %d %d\n", i, fibRes);
        }
        return 0;
}
```

*fibonacci.h:*
```c
#ifndef fibonacci_h__
#define fibonacci_h__

extern int fibonacci(int N);

#endif // fibonacci_h__

```

*fibonacci.c:*
```c
int fibonacci(int N)
{
        int i = 0;
        int prev = 0;
        int curr = 1;
        int buffer = 0;
        for(i=0; i<N; i++)
        {
                buffer = prev + curr;
                prev = curr;
                curr = buffer;
        }
        return prev;
}
```

Compilation process:
```bash
gcc -c -g -Wall -Werror -fpic fibonacci.c
gcc -shared -o libfibonacci.so fibonacci.o
gcc -L. -g -Wall -o test2 test2.c -lfibonacci
export LD_LIBRARY_PATH=`pwd`:$LD_LIBRARY_PATH
./test2
```
This should run our program. Let's debug it!

If we run `gdb test2` and try to apply breakpoints in fibonacci.c, it cannot immediately be done. Thus we should load the library first.

> `gdb` propose to set the breakpoint for future shared library load. But it is harder to navigate in larger project since there is no auto completion. 

A solution to load the library is to set a breakpoint on the main function and then set the breakpoints. For instance, to stop at a breakpoint at the fibonacci function:
```
b main
r
b fibonacci.c:fibonacci
c
```

Now, `gdb` can be controlled as seen before.

Information about the loaded shared library can be obtained with the command `info sharedlibrary`. 

## Scripting GDB
We have already seen that's possible to set breakpoints in a file and that it actually looks like the commands we run. It is indeed the case: we can put any commands wanted in the sourced file. It is particularly interesting in the case where shared library are used because the starting sequence always load the program to stop at the main function.

We could thus expect a file like the following:
```
b main
r
b fibonacci
info break
ignore <fibonacci ID> 4
c
```
Which let the function fibonacci run 4 times before being stopped. 


## More of `gdb`
Here ends the introduction to `gdb`. If you think I missed something important, please let me know (or PR on GitHub).

### Going further / sources
- The `gdb` documentation is available with `man` and `help`
- You may want a [cheat sheet](https://darkdust.net/files/GDB%20Cheat%20Sheet.pdf).
- To learn and play, under agreement, the game [microcoruption](https://microcorruption.com/) make an extensive use of `gdb`.
- Another guide: [Beej's Guide](https://beej.us/guide/bggdb/)
