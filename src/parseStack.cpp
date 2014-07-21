#include <stdlib.h>
#include <map>
#include <list>
#include <string>
#include "callgraph.h"
#include "steprun.h"
using namespace std;


int main(int argc, char** argv)
{
	printf("usage: parseStack tracefile  binfile \n\t example: ./parseStack ./trace.txt ./testbin\n");
	if(argc < 3)
	{
		printf("input param error! \n");
		return 0;
	}

	const char* traceFile = argv[1];
	const char* bin = argv[2];

	NODE root;
	gen_call_info(traceFile, root);
	add_func_and_file_info(bin, root);
	write_dot_file(string("1.dot").c_str(), root);

	map<CALLER, list<CALLEE*> > m;
	gen_call_map(root, m);
	write_dot_file(string("2.dot").c_str(), m);

	free_nodes(root);
	free_list(m);

	print_step_run(bin, traceFile, "steprun.txt");
	return 0;
}
