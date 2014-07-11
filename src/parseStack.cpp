#include <sstream>
#include <fstream>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <list>
#include <string.h>
#include <map>
using namespace std;

class NODE
{
public:
	int addr;
	unsigned int times;
	char func[64];
	char file[128];
	list<NODE*> child;
	NODE* parent;

public:
	NODE(): addr(0), times(0), parent(NULL)
	{
		memset(func, 0x00, sizeof(func));
		memset(file, 0x00, sizeof(file));
	}
private:
	NODE(const NODE&);
	NODE& operator= (const NODE&);
};

// generate call information, and make NODE
void gen_call_info(const char* traceFile, /*out*/ NODE& root)
{
	ifstream fin(traceFile, ios::in);
	assert(fin);
	char line[16];
	if(!fin.getline(line, sizeof(line)))
	{
		fin.clear();
		fin.close();
		assert(0);
		return;
	}

	char type = 0;
	int addr = 0;
	sscanf(line, "%c%x", &type, &addr);
	if(type != 'E')
	{
		fin.clear();
		fin.close();
		assert(0);
		return;
	}

	NODE* thisNode = &root;
	root.addr = addr;
	root.times = 1;
	root.parent = NULL;
	while(fin.getline(line, sizeof(line)))
	{
		sscanf(line, "%c%x", &type, &addr);
		if(type == 'E')
		{
			bool bExist = false;
			list<NODE*>::iterator it = thisNode->child.begin();
			for(; it != thisNode->child.end(); ++it)
			{
				if((*it)->addr == addr) // already exist
				{
					(*it)->times++;
					thisNode = *it;
					bExist = true;
					break;
				}
			}

			if(false == bExist)
			{
				NODE* pNode =new NODE;
				pNode->addr = addr;
				pNode->parent = thisNode;
				pNode->times = 1;
				thisNode->child.push_back(pNode);
				thisNode = pNode;
			}
		}
		else if(type == 'X')
		{
			if(NULL != thisNode->parent)
			{
				thisNode = thisNode->parent;
			}
		}
		else
		{
			printf("type[%c] error, the addr[%x] \n", type, addr);
			assert(0);
		}
	}

	fin.clear();
	fin.close();
}

// write all the functions' informations to the *.dot file
void write_functions(fstream& fout, const NODE& thisNode)
{
	char buf[256];
	if(strlen(thisNode.func) == 0)
	{
		sprintf(buf, "    \"%x\" [shape=ellipse]\n", thisNode.addr);
	}
	else
	{
		sprintf(buf, "    \"%s/%s\" [shape=ellipse]\n", thisNode.func, thisNode.file);
	}

	fout << buf;
	fout.flush();

	list<NODE*>::const_iterator con_it = thisNode.child.begin();
	for(; con_it != thisNode.child.end(); ++con_it)
	{
		write_functions(fout, *(*con_it));
	}
}

// write all the call informations to the *.dot file
void write_call_info(fstream& fout, const NODE& thisNode)
{
	char buf[256];
	list<NODE*>::const_iterator con_it = thisNode.child.begin();
	for(; con_it != thisNode.child.end(); ++con_it)
	{
		if(strlen((*con_it)->func) == 0)
		{
			sprintf(buf, "  \"%x\" -> \"%x\" [label=\"%d \" fontsize=\"10\"]\n", thisNode.addr, (*con_it)->addr, (*con_it)->times);
		}
		else
		{
			sprintf(buf, "  \"%s/%s\" -> \"%s/%s\" [label=\"%d \" fontsize=\"10\"]\n",
					thisNode.func, thisNode.file, (*con_it)->func, (*con_it)->file, (*con_it)->times);
		}
		fout << buf;
		fout.flush();
		write_call_info(fout, *(*con_it));
	}
}

// generate dot file
void write_dot_file(const char* doxFile, const NODE& root)
{
	return;
	fstream fout(doxFile, ios::out);
	assert(fout);
	fout << "digraph g {" << endl
			<< "center=1;" << endl
			<< "orientation = \"portrait\";" << endl
			<< "label = \"justscu\"" << endl;

	write_functions(fout, root);
	write_call_info(fout, root);

	fout << "}" << endl;
	fout.clear();
	fout.close();
}

void add_func_and_file_info(const char* bin, const NODE& thisNode)
{
	char cmd[128];
	sprintf(cmd, "addr2line -e %s -f -s 0x%x", bin, thisNode.addr);
	printf("%s \n", cmd);
	FILE* p = NULL;
	if((p = popen(cmd, "r")) != NULL)
	{
		usleep(20);
		char parse[256] = {0};
		fread(parse, sizeof(parse), 1, p);
		sscanf(parse, "%s\n%s\n", (char*)thisNode.func, (char*)thisNode.file);
		printf("%s/%s\n", thisNode.func, thisNode.file);
	}
	pclose(p);

	list<NODE*>::const_iterator it = thisNode.child.begin();
	for(; it != thisNode.child.end(); ++it)
	{
		add_func_and_file_info(bin, *(*it));
	}
}

// free used memory
void free_nodes(NODE& root)
{
	list<NODE*>::iterator it = root.child.begin();
	for(; it != root.child.end(); ++it)
	{
		free_nodes(*(*it));
		delete *it;
	}
}


///////////////////////////////////////////////
class CALLER
{
public:
	int addr;
	char func[64];
	char file[128];
public:
	CALLER():addr(0)
	{
		memset(func, 0x00, sizeof(func));
		memset(file, 0x00, sizeof(file));
	}
	CALLER(const CALLER& s)
	{
		addr = s.addr;
		strcpy(func, s.func);
		strcpy(file, s.file);
	}
	CALLER(const NODE& node)
	{
		addr = node.addr;
		strcpy(func, node.func);
		strcpy(file, node.file);
	}
	bool operator < (const CALLER&s ) const
	{
		return (addr < s.addr);
	}
private:
	CALLER& operator=(const CALLER&);
};

class CALLEE
{
public:
	int addr;
	unsigned int times;
	char func[64];
	char file[128];
public:
	CALLEE():addr(0), times(0)
	{
		memset(func, 0x00, sizeof(func));
		memset(file, 0x00, sizeof(file));
	}
private:
	CALLEE& operator=(const CALLEE&);
	CALLEE(const NODE& node);
};


void gen_call_map(const NODE& root, map<CALLER, list<CALLEE*> >& m)
{
	CALLER caller(root);
	map<CALLER, list<CALLEE*> >::iterator it_map = m.find(caller);
	if(it_map == m.end())
	{
		list<CALLEE*> nullset;
		std::pair<map<CALLER, list<CALLEE*> >::iterator, bool> ret = m.insert(make_pair(caller, nullset));
		it_map = ret.first;
	}
	assert(it_map != m.end());

	list<NODE*>::const_iterator it_list = root.child.begin();
	for(; it_list != root.child.end(); ++it_list)
	{
		bool bFind = false;
		for(list<CALLEE*>::iterator it_callee = it_map->second.begin(); it_callee != it_map->second.end(); ++it_callee)
		{
			bFind = false;
			if((*it_callee)->addr == (*it_list)->addr)
			{
				(*it_callee)->times += (*it_list)->times;
				bFind = true;
				break;
			}
		}

		if(bFind == false)
		{
			CALLEE* pCallee = new CALLEE;
			pCallee->addr = (*it_list)->addr;
			pCallee->times = (*it_list)->times;
			strcpy(pCallee->file, (*it_list)->file);
			strcpy(pCallee->func, (*it_list)->func);
			it_map->second.push_back(pCallee);
		}

		gen_call_map(*(*it_list), m);
	}
}

// write all the functions' informations to the *.dot file
void write_functions(fstream& fout, const map<CALLER, list<CALLEE*> >& m)
{
	char buf[256];
	map<CALLER, list<CALLEE*> >::const_iterator it = m.begin();
	for(; it != m.end(); ++it)
	{
		if(strlen(it->first.func) == 0)
		{
			sprintf(buf, "    \"%x\" [shape=ellipse]\n", it->first.addr);
		}
		else
		{
			sprintf(buf, "    \"%s/%s\" [shape=ellipse]\n", it->first.func, it->first.file);
		}
		fout << buf;
		fout.flush();
	}
}

// write all the call informations to the *.dot file
void write_call_info(fstream& fout, const map<CALLER, list<CALLEE*> >& m)
{
	char buf[256];
	map<CALLER, list<CALLEE*> >::const_iterator it_map = m.begin();
	for(; it_map != m.end(); ++it_map)
	{
		for(list<CALLEE*>::const_iterator it_set = it_map->second.begin(); it_set != it_map->second.end(); ++it_set)
		{
			if(strlen((*it_set)->func) == 0)
			{
				sprintf(buf, "  \"%x\" -> \"%x\" [label=\"%d \" fontsize=\"10\"]\n", it_map->first.addr, (*it_set)->addr, (*it_set)->times);
			}
			else
			{
				sprintf(buf, "  \"%s/%s\" -> \"%s/%s\" [label=\"%d \" fontsize=\"10\"]\n",
						it_map->first.func, it_map->first.file, (*it_set)->func, (*it_set)->file, (*it_set)->times);
			}
			fout << buf;
			fout.flush();
		}
	}
}

// generate dot file 2
void write_dot_file(const char* doxFile, const map<CALLER, list<CALLEE*> >& m)
{
	fstream fout(doxFile, ios::out);
	assert(fout);
	fout << "digraph g {" << endl
			<< "center=1;" << endl
			<< "orientation = \"portrait\";" << endl
			<< "label = \"justscu\"" << endl;

	write_functions(fout, m);
	write_call_info(fout, m);

	fout << "}" << endl;
	fout.clear();
	fout.close();
}

void free_list(map<CALLER, list<CALLEE*> > & m)
{
	for(map<CALLER, list<CALLEE*> >::iterator it_map = m.begin(); it_map != m.end(); ++it_map)
	{
		for(list<CALLEE*>::iterator it_list = it_map->second.begin(); it_list != it_map->second.end(); ++it_list)
		{
			delete (*it_list);
		}
	}
}

int main(int argc, char** argv)
{
	printf("usage: parseStack tracefile  binfile \n\t example: ./parseStack ./trace.txt ./testbin\n");
	if(argc < 3)
	{
		printf("input param error! \n");
		return 0;
	}

	const char* stackfileName = argv[1];
	const char* bin = argv[2];

	NODE root;
	gen_call_info(stackfileName, root);
	add_func_and_file_info(bin, root);
	write_dot_file(string("1.dot").c_str(), root);

	map<CALLER, list<CALLEE*> > m;
	gen_call_map(root, m);
	write_dot_file(string("2.dot").c_str(), m);

	free_nodes(root);
	free_list(m);
	return 0;
}
