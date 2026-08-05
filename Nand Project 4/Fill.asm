// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

(LOOP)

@KBD
D=M
@WHITE
D;JEQ // no keyboard input, jump to white label

@color
M=-1 // else set color to -1 (0xFFFF)

@FILL
0;JMP

(WHITE)
@color
M=0 // set color to 0 (0x0000)

(FILL)
// store next address to fill in @address, starting with SCREEN address
@SCREEN
D=A
@address
M=D

(FILL_LOOP)
// put color in D register
@color
D=M
@address
A=M // put stored address in A register
M=D // put color from D register into M word at A address
@address
M=M+1 // store next address at @address

// compare next address with end of screen at the keyboard address, jump to top of fill loop if not equal
D=M
@KBD
D=A-D
@FILL_LOOP
D;JGT

// jump to fill_loop

@LOOP
0;JMP
