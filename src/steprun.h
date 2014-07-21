#pragma once


class STEP_NODE
{
public:
	int addr;
	char func_file[128];
	STEP_NODE* prev;

public:
	STEP_NODE():addr(0), prev(0)
	{
		memset(func_file, 0x00, sizeof(func_file));
	}
private:
	STEP_NODE(const STEP_NODE&);
	STEP_NODE& operator=(const STEP_NODE&);
};

void print_step_run(const char* bin, const char* traceFile, const char* outFile);
