#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>           
#include <string.h>  
#include <cmath>
#include "Table.hpp"
#include <sstream>
using namespace std;


class MD5 {
private:
	// 初始向量IV
	bit32 IV[4] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476};
	// 128-bit的MD缓存区，表示为4个32-bit的寄存器
	bit32 CV[4] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476};

	// 填充操作的padding_len个8-bit数据分组
	int padding_len; // padding_len = 64n
	vector<bit8> paddingMessage;

	// 分块操作的N个32-bit分组
	// 也是L个512-bit区块分组, L = N * 16
	int N;
	int L;
	vector<vector<bit32>> Y;

    
	// 将输入message字符串padding拓展，并按8-bit写成数组分组
	void padding(string message) {
		// message的字符数
		int len = message.length();
		// 判断要填充的字符数 
		int add = len%64;
		add = (add<56)?(56-add):(56+64-add);
		// 填充后总的字符数
		padding_len = len+add+8;
		// K-bit原文数据
		for(int i = 0; i < len; i++) {
			paddingMessage.push_back(message[i]);
		}
		// P-bit填充数据
		paddingMessage.push_back(0x80);
		for(int i = len+1; i < len+add; i++) {
			paddingMessage.push_back(0x00);
		}
		// 64-bit的长度数据
		// 采用小端模式
		// 也即K=128*3时，该64-bit长度数据为：80 01 00 00 00 00 00 00
		unsigned int bit_len = len*8;
		char mask = 0xff;
		for(int i = 0; i < 8; i++) {
			paddingMessage.push_back((bit8)((bit_len) & mask));
			bit_len = bit_len >> 8;
		}
		// 显示padding后的8-bit数组结果
		// cout << "padding: ---" << padding_len << endl;
		// for(int i = 0; i < paddingMessage.size(); i++) {
		// 	printf("0x%02x ", paddingMessage[i]);
		// }
		// cout << endl;
    }

	// 将拓展后的8-bit分组进行分块操作
	// 每个分块512-bit
	void blocking() {
		// padding数据分组为N个32-bit字
		N = padding_len/4;
		// padding数据分组为L个512-bit分组
		L = N/16;
		for(int i = 0; i < L; i++) {
			vector<bit32> M;
			for(int j = 0; j < 16; j++) {
				// 采用小端存储
				// 0x01, 0x23, 0x45, 0x67 --->>>  0x67452301
				bit32 word = 0;
				for(int k = 0; k < 4; k++) {
					word |= (bit32)(paddingMessage[i*64+j*4+k]) << (k*8);
				}
				M.push_back(word);
			}
			Y.push_back(M);
		}
		// 显示分块操作后的结果
		// cout << "blocking: ---" << L << endl;
		// for(int i = 0; i < Y.size(); i++) {
		// 	for(int j = 0; j < Y[i].size(); j++) {
		// 		printf("0x%08x ", Y[i][j]);
		// 	}
		// 	cout << endl;
		// }
		// cout << endl;
	}

	// MD5压缩函数HMD5
	// 对每个区块分组经过4轮循环(共64次迭代)的压缩算法
	void HMD5() {
		for(int i = 0; i < Y.size(); i++) {
			bit32 A = CV[0];
			bit32 B = CV[1];
			bit32 C = CV[2];
			bit32 D = CV[3];

			// 4轮循环
			for(int j = 0; j < 4; j++) {
				// 每轮循环16次迭代
				for(int u = 0; u < 16; u++) {
					bit32 a = A;
					bit32 b = B;
					bit32 c = C;
					bit32 d = D;
					bit32 g;// 迭代使用的轮函数结果
					bit32 x;// 迭代选取的消息分组中的字的X
					bit32 t = T[j*16+u+1];// 加法常数
					bit8 s = S[j*16+u+1];// 左移位数
					switch(j) {
						case 0:
							g = F(b, c, d);
							x = Y[i][u];
							break;
						case 1:
							g = G(b, c, d);
							x = Y[i][(1+5*u)%16];
							break;
						case 2:
							g = H(b, c, d);
							x = Y[i][(5+3*u)%16];
							break;
						case 3:
							g = I(b, c, d);
							x = Y[i][(7*u)%16];
							break;
					} 
					// 对A迭代
					a = b + leftShitf((a + g + x + t), s);
					// 缓冲区循环轮换
					//(B, C, D, A) <- (A, B, C, D)
					A = d;
					B = a;
					C = b;
					D = c;
					// 输出测试
					// cout << "After handling " << j*16+u << "------" << endl;
					// printf("g: 0x%08x------x: 0x%08x------t: 0x%08x------s: 0x%02x\n", g, x, t, s);
					// printf("0x%08x 0x%08x 0x%08x 0x%08x\n", A, B, C, D);
					// cout << endl;
				}
			}
			CV[0] += A;
			CV[1] += B;
			CV[2] += C;
			CV[3] += D;
			
		}
	}

	// HMD5压缩函数中用到的轮函数F、G、H、I
	bit32 F(bit32 b, bit32 c, bit32 d) {
		return ((b) & (c)) | ((~b) & (d));
	}
	bit32 G(bit32 b, bit32 c, bit32 d) {
		return ((b) & (d)) | ((c) & (~d));
	}
	bit32 H(bit32 b, bit32 c, bit32 d) {
		return (b) ^ (c) ^ (d);
	}
	bit32 I(bit32 b, bit32 c, bit32 d) {
		return (c) ^ ((b) | (~d));
	}

	// 循环左移函数,将32-bit input循环左移s位
	bit32 leftShitf(bit32 input, bit8 s) {
		return (input << s) | (input >> (32 - s));
	}

	// 拼接4*32bit共128bit的CV块转化成字符串返回
	// 输出结果用16进制表示,用sstream实现字符串流转成16进制显示字符串
	// 注意小端模式
	string getDigest() {
		string result = "";
		bit32 mask[4] = {0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000};
		for(int i = 0; i < 4; i++) {
			stringstream ioss;
			ioss << hex << CV[i];
			string temp;
			ioss >> temp;
			for(int j = 0; j < 8-temp.size(); j++)
				temp[j] = '0';
			for(int j = 0; j < 4; j++) {
				string s = temp.substr(6-j*2, 2);
				result += s;
			}
		}
		return result;
	}

public:
	// 主函数：使用MD5算法得到编码后的HASH值字符串的接口函数
    string getMD5Digest(string message) {
		// message长度为K-bits
    	int K = message.length() * 8;
		padding(message);
		blocking();
		HMD5();
		return getDigest();
    }
};
