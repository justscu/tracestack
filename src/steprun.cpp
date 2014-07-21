#include <sstream>
#include <fstream>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <map>
#include "steprun.h"
using namespace std;

//////////////////// print run step by step ////////////////////////////

void print_step_run(const char* bin, const char* traceFile, const char* outFile)
{
	assert(traceFile && outFile);
	ofstream fout(outFile, ios::out);
	assert(fout);

	ifstream fin(traceFile, ios::in);
	assert(fin);
	char line[16];
	if (!fin.getline(line, sizeof(line)))
	{
		fin.clear();
		fin.close();
		assert(0);
		return;
	}

	char type = 0;
	int addr = 0;
	sscanf(line, "%c%x", &type, &addr);
	if (type != 'E')
	{
		fin.clear();
		fin.close();
		assert(0);
		return;
	}

	char cmd[256];
	STEP_NODE* pNow = new STEP_NODE;

	bool bLastE = true;

	int len = 0;
	do{
		sscanf(line, "%c%x", &type, &addr);
		sprintf(cmd, "addr2line -e %s -f -s 0x%x", bin, addr);
		printf("%s \n", cmd);

		if(type == 'E')
		{
			char func_file[256];
			FILE* p = NULL;
			if((p = popen(cmd, "r")) != NULL)
			{
				usleep(20);
				char parse[256] = {0};
				char file[128];
				fread(parse, sizeof(parse), 1, p);
				sscanf(parse, "%s\n%s\n", (char*)func_file, (char*)file);
				strcat(func_file, "/");
				strcat(func_file, file);
			}
			pclose(p);

			STEP_NODE* pNode = new STEP_NODE;
			assert(pNode);
			pNode->addr = addr;
			strcpy(pNode->func_file, func_file);
			pNode->prev = pNow;
			pNow = pNode;

			if(bLastE == false)
			{
				fout << endl;
				for(int i = 0; i < len; ++i)
					fout << " ";
			}

			fout << " => " << pNode->func_file;
			fout.flush();
			len += 4;
			len += strlen(pNode->func_file);

			bLastE = true;
		}
		else if(type == 'X')
		{
			STEP_NODE* pNode = pNow;
			pNow = pNow->prev;

			len -= strlen(pNode->func_file);
			len -= 4;

			bLastE = false;
			delete pNode;
		}
		else
		{
			assert(0);
		}

	}while(fin.getline(line, sizeof(line)));

	delete pNow;
	pNow = NULL;

	fin.clear();
	fin.close();

	fout.clear();
	fout.close();
}
