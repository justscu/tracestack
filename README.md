tracestack
==========

浏览开源代码时，函数间的调用关系通常比较复杂，通过本项目可以将这种调用关系用图的形式显示出来，还可以一步一步的打印函数的调用过程。

gcc提供一项功能，当函数入栈时`cyg_profile_func_enter`被调用，当出栈时`cyg_profile_func_exit`被调用。这样就可以将函数的入栈/出栈信息(函数地址)记录下来。函数地址不易被看懂，借助于`addr2line`可以将函数地址翻译成函数名称。

运行本程序后，最终生成两个文件:`2.dot`和`steprun.txt`。`2.dot`用图的形式显示调用关系；`steprun.txt`可以一步一步的打印函数调用过程。


your project
###
  1. first, you need install xdot: `sudo apt-get install xdot`;
  2. copy `instrument.h` & `instrument.c` to your project;
  3. add `CFALGS="-g -finstrument-functions"` in your Makefile;
  4. run your bin file, then auto generate a file named `trace.txt`;


tracestack project
###
  1. `cmake .`;
  2. `make`, and generate a exe file named `tracestack`;
  3. run: `./tracestack trace.txt yourbinfile`;
  4. then, will generate a file named `2.dot`;
  5. run: `xdot 2.dot`, you will see a calling graph.
  6. `gedit steprun.txt`, you will see function calls step by step.
