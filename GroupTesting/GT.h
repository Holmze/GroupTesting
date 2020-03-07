#ifndef _CMSKETCH_H_VERSION_2
#define _CMSKETCH_H_VERSION_2

#include <sstream>
#include <cstring>
#include <algorithm>
#include <cstdint>
#include "../common/BOBHash32.h"
#include <iostream>
#include <string>
#include <bitset>
using namespace std;

struct bucket{//Ͱ
	int node[105] = {0};
};



template<int key_len, int d>//ģ��
class GT
{
private:
	int memory_in_bytes = 0;
	typedef pair <uint8_t[key_len], int> KV;

	int w = 0;//һ��w��Ͱ
    //int d;//ģ�����������ϣd��
    int logn = 33;//Ͱ�Ĵ�СΪlogn
    //int size = 0;

	bucket* counters[d] = {NULL};
	BOBHash32* hash[d] = {NULL};

public:
	string name;

	GT(){}
	GT(int memory_in_bytes)
	{	initial(memory_in_bytes);	}//����initial�������г�ʼ��
	~GT(){clear();}

	void initial(int memory_in_bytes)
	{
		this->memory_in_bytes = memory_in_bytes;
		w = memory_in_bytes / 4 / d / logn;

		for(int i = 0; i < d; ++i){//������λ����Ϊ0
			counters[i] = new bucket[w];
			for(int j = 0;j<logn;j++){
				for(int k=0;k<logn;k++){
					counters[i][j].node[k]=0;
				}
			}
			hash[i] = new BOBHash32(i + 750);
		}


		stringstream name_buffer;//������
        name_buffer << "GT@" << memory_in_bytes << "@" << d;
        name = name_buffer.str();//stringsteam.str()�ɽ�name_bufferתΪstring
	}
	void clear()
	{
		for(int i = 0; i < d; ++i)
			delete[] counters[i];

		for (int i = 0; i < d; ++i)
			delete hash[i];
	}

	void print_basic_info() //���������Ϣ
    {
        printf("GroupTesting\n");
        printf("\tCounters: %d\n", w);
        printf("\tMemory: %.6lfMB\n", w * 4.0 / 1024 / 1024);
    }



    void insert(uint8_t * key, int a,int f = 1 ) //����
    {
		int key_int[13]={0};
		string str_key = string((const char*)key,key_len);
		uint32_t srcIP;
		memcpy(&srcIP, str_key.c_str(), 4);
        for (int i = 0; i < d; i++) {
			int index = (hash[i]->run((const char *)key, key_len)) % w;
			for(int j=0;j<32;j++){
				//cout << key_bit<<" ";
				for(int k=0;k<key_len;k++){
					counters[i][index].node[j*key_len+k+1] += (srcIP%2);
					srcIP/=2;
				}
			}
			//cout<<endl;
			counters[i][index].node[0] += f;
        }
    }

	int query(uint8_t * key) //��ѯ,��cm�����ͬ
    {
        int ret = 1 << 30;
        for (int i = 0; i < d; i++) {
        	int index = (hash[i]->run((const char *)key, key_len)) % w;
            int tmp = counters[i][index]->node[0];
            ret = min(ret, tmp);
        }
        return ret;
    }
	
	void get_heavy_hitters(uint32_t threshold, std::vector<pair<uint32_t, uint32_t> >& ret,int k=2){//find majority item
		ret.clear();
		for(int i = 0;i<d;i++){
			for(int j = 0;j<w;j++){
				if(counters[i][j].node[0]>=threshold){//node[0]���ڸ�����ֵʱ��get heavy hitter
					long key_long = 0;
					int t=1;
					int cntk = counters[i][j].node[0]/k;
					for(int k=0;k<32;k++){
						if(counters[i][j].node[k+1]>=cntk){
							key_long += t;
						}
						t=t*2;
					}
					if(key_long==0) break;
					uint32_t key_32 = (uint32_t)key_long;
					ret.emplace_back(make_pair(key_32,counters[i][j].node[0]));
				}
			}
		}
	}
};


#endif
