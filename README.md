== auto generate functions call graph for your program! ==
 
====================your project======================
1. first, you need install xdot:
      sudo apt-get install xdot
2. copy 'instrument.h' & 'instrument.c' to your project;
3. add CFALGS="-g -finstrument-functions" in your Makefile;
4. run your bin file, then auto generate a file named 'trace.txt';


==================tracestack project================
1. 'cmake .';
2. make, and generate a bin file named 'tracestack';
3. run: ./tracestack trace.txt yourbinfile;
4. then, will generate a file named '2.dot';
5. run: xdot 2.dot, you will see a calling graph.
6. gedit steprun.txt, you will see function calls step by step.
