#pragma once
#include <list>
#include <stdio.h>
#include <string.h>
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


void gen_call_info(const char* traceFile, /*out*/ NODE& root);
void add_func_and_file_info(const char* bin, const NODE& thisNode);
void write_dot_file(const char* doxFile, const NODE& root);
void gen_call_map(const NODE& root, map<CALLER, list<CALLEE*> >& m);
void write_dot_file(const char* doxFile, const map<CALLER, list<CALLEE*> >& m);
void free_nodes(NODE& root);
void free_list(map<CALLER, list<CALLEE*> > & m);
