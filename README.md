# DeComp ASM

Converts assembly of DeComp to machine code.

# How to build

### Linux

```bash
buildrun.sh
```

# How to use it

Just give a program path to file with code. The program will generate DeComp.mem file. This file you can copy to DeComp folder.

```bash
./DeCompAsm commands.txt
```

# Features

converts asm to machine code.
In addition I added some commands (only 1 right now):

- **STORE index value**

This command stores the given **value** into the memory cell at the specified **index**.

---

Comments

```
; this is commnent
LOAD 10 ; this is comment too
```

# Future features

- [X] Add support of comments
