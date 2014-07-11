  auto generate functions call graph for your program!

====================your project======================
1. first, you need install xdot: 
		sudo apt-get install xdot
2. copy 'instrument.h' & 'instrument.c' to your project;
3. add CFALGS="-g -finstrument-functions" when you make;
4. run your bin file, then auto generate a file named 'trace.txt';


==================parseStack project================
1. 'cmake .';
2. make, and generate a bin file named 'parseStack';
3. run: ./parseStack trace.txt  yourbinfile;
4. then, will generate a file named '2.dot';
5. run: xdot 2.dot

