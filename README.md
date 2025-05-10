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
In addition, I added some features:

**dvar name = number**
---
Creates variable **name** with decimal value **number**

All variables are visible everywhere. if you make variable on line 10, statement on line 1 knows about that variable

**Lab name:**
---
like goto in c. Creates label **name** that point to adress of next instruction relative to this Lab
All labels are visible everywhere. if you make label on line 10, statement on line 1 knows about that label


Comments:
---

```
; this is commnent
LOAD 10 ; this is comment too
```

# Some examples
```
dvar NUMBER1 = 10 ; number1
dvar NUMBER2 = 23 ; number2
LOAD NUMBER2      ; loads number2 in memory
SUB NUMBER1       ; sub number2 from number1
```
Example with label
---
```
dvar NUMBER1 = 1
dvar ONE = 1

LOAD NUMBER1
SUB ONE
JNC skip        ; If C=0, then go to skip

LOAD NUMBER1
ADD ONE
STORE NUMBER1
Lab skip:           
    LOAD NUMBER1
```
# Future features

- [X] Add support of comments (kinda)
- [X] Add support of variables
- [X] Add support of lables
