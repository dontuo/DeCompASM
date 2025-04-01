
# DeComp ASM  

Converts assembly of DeComp to machine code. 

# How to build
 
### Linux 
```bash 
buildrun.sh
```

# How to use it
just give a program path to file with code. The program will generate DeComp.mem file. This file you can copy to DeComp folder.

```bash
./DeComp commands.txt
```

# Features
converts asm to machine code.
In addition I added "command":
STORE index value
command stores the given value into the memory cell at the specified index.
