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


struct FIVE_TUPLE{	char key[13];	};//��Ԫ��
typedef vector<FIVE_TUPLE> TRACE;//TRACE�Ƿ�����Ԫ��Ķ�̬����
TRACE traces[END_FILE_NO - START_FILE_NO + 1];//TRACE���͵����飬һ��10�����ݼ�

void ReadInTraces(const char *trace_prefix)//�������ݼ�
{
	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)//datafileCnt:����ڼ������ݼ�
	{
		char datafileName[100];
		sprintf(datafileName, "%s%d.dat", trace_prefix, datafileCnt - 1);//����ļ���
		FILE *fin = fopen(datafileName, "rb");

		FIVE_TUPLE tmp_five_tuple;//��ʱ��Ԫ�����
		traces[datafileCnt - 1].clear();
		while(fread(&tmp_five_tuple, 1, 13, fin) == 13)
		{
			traces[datafileCnt - 1].push_back(tmp_five_tuple);//��tmp_five_tuple����traces����
		}
		fclose(fin);

		printf("Successfully read in %s, %ld packets\n", datafileName, traces[datafileCnt - 1].size());
	}
	printf("\n");
}

int main()
{
	ReadInTraces("../../../data/");//�������ݼ�


	#define SK_D 3
	GT<4, SK_D> * gt = NULL;//key_len = 4��d=3


	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		//unordered_map<string, int> Real_Freq;//��������,������ʵƵ�ʣ�����key���ã������Ǹ�����������
		gt = new GT<4, SK_D>(600 * 1024 * 105); //��ʼ��һ��GT

		int packet_cnt = (int)traces[datafileCnt - 1].size();//����С
		cout << packet_cnt <<endl;
		for(int i = 0; i < packet_cnt; ++i) //��traces�ڵ����ݲ��뵽CMSketch
		{
			
			gt->insert((uint8_t*)(traces[datafileCnt - 1][i].key),i);

			//string str((const char*)(traces[datafileCnt - 1][i].key), 4);
			//Real_Freq[str]++;//���������Ƶ��
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
