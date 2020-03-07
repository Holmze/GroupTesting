#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "../GroupTesting/GT.h"
using namespace std;

#define START_FILE_NO 1
#define END_FILE_NO 10


struct FIVE_TUPLE{	char key[13];	};//五元组
typedef vector<FIVE_TUPLE> TRACE;//TRACE是放置五元组的动态数组
TRACE traces[END_FILE_NO - START_FILE_NO + 1];//TRACE类型的数组，一共10个数据集

void ReadInTraces(const char *trace_prefix)//读入数据集
{
	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)//datafileCnt:读入第几个数据集
	{
		char datafileName[100];
		sprintf(datafileName, "%s%d.dat", trace_prefix, datafileCnt - 1);//输出文件名
		FILE *fin = fopen(datafileName, "rb");

		FIVE_TUPLE tmp_five_tuple;//临时五元组变量
		traces[datafileCnt - 1].clear();
		while(fread(&tmp_five_tuple, 1, 13, fin) == 13)
		{
			traces[datafileCnt - 1].push_back(tmp_five_tuple);//将tmp_five_tuple插入traces数组
		}
		fclose(fin);

		printf("Successfully read in %s, %ld packets\n", datafileName, traces[datafileCnt - 1].size());
	}
	printf("\n");
}

int main()
{
	ReadInTraces("../../../data/");//读入数据集


	#define SK_D 3
	GT<4, SK_D> * gt = NULL;//key_len = 4；d=3


	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		//unordered_map<string, int> Real_Freq;//关联容器,计算真实频率，根据key引用，而不是根据索引引用
		gt = new GT<4, SK_D>(600 * 1024 * 105); //初始化一个GT

		int packet_cnt = (int)traces[datafileCnt - 1].size();//包大小
		cout << packet_cnt <<endl;
		for(int i = 0; i < packet_cnt; ++i) //将traces内的数据插入到CMSketch
		{
			
			gt->insert((uint8_t*)(traces[datafileCnt - 1][i].key),i);

			//string str((const char*)(traces[datafileCnt - 1][i].key), 4);
			//Real_Freq[str]++;//计算无误差频率
		}
#define HEAVY_HITTER_THRESHOLD(total_packet) (total_packet * 1 / 1000)
		vector< pair<uint32_t,uint32_t>> heavy_hitters;
		gt->get_heavy_hitters(HEAVY_HITTER_THRESHOLD(packet_cnt), heavy_hitters);
		printf("heavy hitters: <srcIP, count>, threshold=%d\n", HEAVY_HITTER_THRESHOLD(packet_cnt));
		for(int i = 0, j = 0; i < heavy_hitters.size(); ++i)
		{
			printf("<%.8x, %d>", heavy_hitters[i].first, (int)heavy_hitters[i].second);
			if(++j % 5 == 0)
				printf("\n");
			else printf("\t");
		}
		printf("\n");

		delete gt;
	}
}	
