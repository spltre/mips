#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <map>
#include <vector>
//#define debug
using namespace std;
typedef long long LL;
const int maxn = (1 << 20) * 8;
const int text_const = 100,data_const = (1 << 21);
char space[maxn];
int pos_text = text_const,pos_data = data_const;
int T,F;
class parcel {
public:
	map <string,int> encode;
	map <string,int> label_text;
	map <string,int> label_data;
	void _clear() {
		label_text.clear();
		label_data.clear();
		encode.clear();
	}
	void _encode_init() {
		string name1[85] = {
			"$0","$1","$2","$3","$4","$5","$6","$7","$8","$9","$10","$11","$12","$13","$14","$15",
			"$16","$17","$18","$19","$20","$21","$22","$23","$24","$25","$26","$27","$28","$29","$30","$31","lo","hi",// 33
			"add","addu","addiu","sub","subu","mul","mulu","div","divu","xor","xoru","neg","negu","rem","remu",// 48
			"li","la","lb","lh","lw","sb","sh","sw",// 56
			"seq","sne","sge","sgt","sle","slt",// 62
			"b","beq","bne","bge","bgt","ble","blt","beqz","bnez","bgez","bgtz","blez","bltz",// 75
			"j","jr","jal","jalr",// 79
			"move","mflo","mfhi",// 82
			"nop","syscall"// 84
		},
		name2[32] = {
			"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",
			"$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra"
		};
		int i;
		for (i = 0; i < 32; ++i) {
			encode[name1[i]] = i;
			encode[name2[i]] = i;
		}
		encode["$s8"] = 30;
		for (i = 32; i < 85 ; ++i)
			encode[name1[i]] = i;
	}
	void _read_code(char name[],int &num) {
		string token[50000];
		ifstream is;
		is.open(name);
		enum {text,data} mark;
		int i,j,k,l,len,n;
		char flag;
		bool neg,is_string;
		string str,line;
		while (getline(is,line)) {
			j = 0;
			is_string = 0;
			while (line[j] != '\0') {
				neg = 0;
				if (is_string == 0) {
					for (; line[j] != '\0' && (line[j] == ' ' || line[j] == '	'); ++j);
					if (line[j] != '\0') ++num;
					for (; line[j] != '\0' && (line[j] != ' ' && line[j] != '	'); ++j) {
						if (line[j] == '\\') {
							++j;
							if (line[j] == 'n') token[num] += '\n';
							else if (line[j] == '\'') token[num] += '\'';
							else if (line[j] == '\"') token[num] += '\"';
							else if (line[j] == '\\') token[num] += '\\';
						}
						else if (line[j] == '#') {--num;neg = 1;break;}
						else if (line[j] == '\'') continue;
						else if (line[j] == '\"') {is_string ^= 1;++j;break;}
						else token[num] += line[j];
					}
				}
				else {
					for (; line[j] != '\0'; ++j) {
						if (line[j] == '\\') {
							++j;
							if (line[j] == 'n') token[num] += '\n';
							else if (line[j] == '\'') token[num] += '\'';
							else if (line[j] == '\"') token[num] += '\"';
							else if (line[j] == '\\') token[num] += '\\';
						}
						else if (line[j] == '\"') {is_string ^= 1;++j;break;}
						else token[num] += line[j];
					}
				}
				if (neg) break;
			}
		}
		is.close();
		ofstream os;
		os.open("test.out");
		vector <string> label;
		vector <int> pos;
		for (i = 1; i <= num; ++i) {
			len = token[i].length();
			if (token[i][0] == '.') {
				str = "";
				str = token[i].substr(1,len - 1);
				if (str == "text") mark = text;
				else if (str == "data") mark = data;
				else {
					if (str == "align" || str == "space") {
						++i; len = token[i].length();
						for(n = 0,j = 0; j < len; ++j)
							n = n * 10 + token[i][j] - '0';
						if (str == "align") {
							n = (1 << n);
							pos_data = ((pos_data - 1) / n + 1) * n;
						}
						else pos_data += n;
					}
					else if (str == "ascii" || str == "asciiz") {
						++i; len = token[i].length();
						for(j = 0; j < len; ++j) space[pos_data++] = token[i][j];
						if (str == "asciiz") space[pos_data++] = '\0';
					}
					else if (str == "byte") {
						do {
							++i; len = token[i].length();
							space[pos_data++] = token[i][0];
						} while (token[i][len - 1] == ',');
					}
					else if (str == "half" || str == "word") {
						do {
							++i; len = token[i].length();
							neg = 0;
							j = 0;
							if (token[i][j] == '-') {neg = 1;++j;}
							for(n = 0; j < len; ++j)
								n = n * 10 + token[i][j] - '0';
							if (neg) n = -n;
							if (str == "half") len = 2;
							else len = 4;
							memcpy(space + pos_data,&n,len);
							pos_data += len;
						} while (token[i][len - 1] == ',');
					}
				}
			}
			else if (token[i][len - 1] == ':') {
				if (mark == text) label_text[token[i].substr(0,len - 1)] = pos_text;
				if (mark == data) label_data[token[i].substr(0,len - 1)] = pos_data;
			}
			else {
				if (mark == data) continue;
				string op,a[3];
				op = token[i];
				for (j = 0; j < 3; ++j) {
					++i;
					if (i > num) break;
					if (encode.find(token[i]) != encode.end() && 33 < encode.at(token[i]) && encode.at(token[i]) < 85) {--i;break;}
					if (token[i][0] == '.' || token[i][token[i].length() - 1] == ':') {--i;break;}
					a[j] = token[i];
				}
				char t;
				t = char(encode.at(op));
				memcpy(space + pos_text,&t,1); ++pos_text;
				for (k = 0; k < 3; ++k) {
					flag = 2;
					n = 0;
					if (j > k) {
						flag = 0;
						len = a[k].length();
						if (a[k][len - 1] == ',') a[k] = a[k].substr(0,len - 1);
						if (a[k][len - 1] == ')') {
							flag = 1;
							l = 0;
							neg = 0;
							if (a[k][0] == '-') {neg = 1; ++l;}
							for (n = 0; '0' <= a[k][l] && a[k][l] <= '9'; ++l)
								n = n * 10 + a[k][l] - '0';
							if (neg) n = -n;
							a[j++] = a[k].substr(l + 1,len - l - 2);
						}
						else if (a[k][0] == '-' || ('0' <= a[k][0] && a[k][0] <= '9')) {
							flag = 1;
							l = 0;
							neg = 0;
							if (a[k][0] == '-') {neg = 1; ++l;}
							for (n = 0; '0' <= a[k][l] && a[k][l] <= '9'; ++l)
								n = n * 10 + a[k][l] - '0';
							if (neg) n = -n;
						}
						else if (encode.find(a[k]) != encode.end()) {
							n = encode.at(a[k]);
						}
						else {
							label.push_back(a[k]);
							pos.push_back(pos_text);
						}
					}
					memcpy(space + pos_text,&n,4);
					memcpy(space + pos_text + 4,&flag,1);
					pos_text += 5;
				}
			}
		}
		int siz = label.size();
		for (i = 0; i < siz; ++i) {
			if (label_text.find(label[i]) != label_text.end()) n = label_text.at(label[i]);
			else if (label_data.find(label[i]) != label_data.end()) n = label_data.at(label[i]);
			memcpy(space + pos[i],&n,4);
		}
		os.close();
	}
} P;
class simulater {
private:
	int pos,pos_pre,pos_label;
	
	int regis[34],regist[34];
	
	char inst[16];
	int op[6],a[6][3];
	char flag[6][3];
	int data_a[6][3];
	string str;
	LL ans1[6],ans2[6];
	
	bool mark[6],res;
	bool control_hazard[6];
	int predict_table[10000][4][2],condition[10000];
	
	void _IF(const bool &mark) {
		if (!mark) return;
		memcpy(inst,space + pos,16);
		pos += 16;
	}
	void _ID_and_DP(int &op,int a[],char flag[],int data_a[],const bool &mark) {
		if (!mark) return;
		int i;
		op = int(inst[0]);
		for (i = 0; i < 3; ++i) {
			a[i] = *(reinterpret_cast<const int*> (inst + i * 5 + 1));
			flag[i] = inst[i * 5 + 5];
			if (flag[i] == 0) {
				if (0 <= a[i] && a[i] < 34) data_a[i] = regist[a[i]];
				else if (a[i] >= data_const) data_a[i] = a[i];
				else if (a[i] >= text_const) data_a[i] = a[i];
			}
			else if (flag[i] == 1) {
				data_a[i] = a[i];
			}
		}
		if (op == 81) data_a[0] = regist[32];
		if (op == 82) data_a[0] = regist[33];
		if (op == 84) {
			data_a[0] = regist[2];
			data_a[1] = regist[4];
			data_a[2] = regist[5];
		}
	}
	void _EX(LL &ans1,LL &ans2,
		const int &op,const int a[],const char flag[],const int data_a[],const bool &mark) {
		if (!mark) return;
		unsigned long long n;
		switch (op) {
			case 34:case 35:case 36:
				regist[a[0]] = ans1 = data_a[1] + data_a[2];
				break;
			case 37:case 38:
				regist[a[0]] = ans1 = data_a[1] - data_a[2];
				break;
			case 39:case 40:
				if (flag[2] != 2) {
					regist[a[0]] = ans1 = data_a[1] * data_a[2];
				}
				else {
					n = (unsigned long long) data_a[0] * data_a[1];
					regist[32] = ans1 = n & ((1LL << 32) - 1);
					regist[33] = ans2 = n >> 32;
				}
				break;
			case 41:case 42:
				if (flag[2] != 2) {
					regist[a[0]] = ans1 = data_a[1] / data_a[2];
				}
				else {
					regist[32] = ans1 = data_a[0] / data_a[1];
					regist[33] = ans2 = data_a[0] % data_a[1];
				}
				break;
			case 43:case 44:
				regist[a[0]] = ans1 = data_a[1] ^ data_a[2];
				break;
			case 45:case 46:
				regist[a[0]] = ans1 = -data_a[1];
				break;
			case 47:case 48:
				regist[a[0]] = ans1 = data_a[1] % data_a[2];
				break;
			case 49:
				regist[a[0]] = ans1 = data_a[1];
				break;
			case 50:
				if (flag[2] == 2) regist[a[0]] = ans1 = data_a[1];
				else regist[a[0]] = ans1 = data_a[1] + data_a[2];
				break;
			case 51:case 52:case 53:
			case 54:case 55:case 56:
				if (flag[2] == 2) ans1 = data_a[1];
				else ans1 = data_a[1] + data_a[2];
				break;
			case 57:
				regist[a[0]] = ans1 = (data_a[1] == data_a[2]);
				break;
			case 58:
				regist[a[0]] = ans1 = (data_a[1] != data_a[2]);
				break;
			case 59:
				regist[a[0]] = ans1 = (data_a[1] >= data_a[2]);
				break;
			case 60:
				regist[a[0]] = ans1 = (data_a[1] > data_a[2]);
				break;
			case 61:
				regist[a[0]] = ans1 = (data_a[1] <= data_a[2]);
				break;
			case 62:
				regist[a[0]] = ans1 = (data_a[1] < data_a[2]);
				break;
			case 63:
			case 76:
				ans2 = data_a[0];
				break;
			case 64:
				ans1 = (data_a[0] == data_a[1]);
				ans2 = data_a[2];
				break;
			case 65:
				ans1 = (data_a[0] != data_a[1]);
				ans2 = data_a[2];
				break;
			case 66:
				ans1 = (data_a[0] >= data_a[1]);
				ans2 = data_a[2];
				break;
			case 67:
				ans1 = (data_a[0] > data_a[1]);
				ans2 = data_a[2];
				break;
			case 68:
				ans1 = (data_a[0] <= data_a[1]);
				ans2 = data_a[2];
				break;
			case 69:
				ans1 = (data_a[0] < data_a[1]);
				ans2 = data_a[2];
				break;
			case 70:
				ans1 = (data_a[0] == 0);
				ans2 = data_a[1];
				break;
			case 71:
				ans1 = (data_a[0] != 0);
				ans2 = data_a[1];
				break;
			case 72:
				ans1 = (data_a[0] >= 0);
				ans2 = data_a[1];
				break;
			case 73:
				ans1 = (data_a[0] > 0);
				ans2 = data_a[1];
				break;
			case 74:
				ans1 = (data_a[0] <= 0);
				ans2 = data_a[1];
				break;
			case 75:
				ans1 = (data_a[0] < 0);
				ans2 = data_a[1];
				break;
			case 78:
				ans2 = data_a[0];
				break;
			case 80:
				regist[a[0]] = data_a[1];
				break;
			case 81:case 82:
				regist[a[0]] = data_a[0];
				break;
			case 84:
				switch (data_a[0]) {
					case 1:
					case 4:
						ans1 = data_a[1];
						break;
					case 5:
						cin >> ans1;
						regist[2] = ans1;
						break;
					case 8:
						cin >> str;
						str.substr(0,data_a[2] - 1);
						break;
					case 17:
						ans1 = data_a[1];
						break;
				}
		}
	}
	void _MA(LL &ans1,LL &ans2,
		const int &op,const int a[],const int data_a[],const bool &mark) {
		if (!mark) return;
		int i,len;
		switch (op) {
			case 51:
				ans1 = *(reinterpret_cast<char*> (space + ans1));
				break;
			case 52:
				ans1 = *(reinterpret_cast<short*> (space + ans1));
				break;
			case 53:
				ans1 = *(reinterpret_cast<int*> (space + ans1));
				break;
			case 54:
				memcpy(space + ans1,&data_a[0],1);
				break;
			case 55:
				memcpy(space + ans1,&data_a[0],2);
				break;
			case 56:
				memcpy(space + ans1,&data_a[0],4);
				break;
			case 84:
				switch (data_a[0]) {
					case 4:
						i = data_a[1];
						while (space[i] != '\0') {
							cout << space[i];
							++i;
						}
						break;
					case 8:
						len = str.length();
						for (i = 0; i < len; ++i)
							space[i + data_a[1]] = str[i];
						space[i + data_a[1]] = '\0';
						break;
					case 9:
						ans1 = pos_data;
						pos_data += data_a[1];
						break;
				}
				break;
		}
	}
	void _WB(const int &ans1,const int &ans2,
		const int &op,const int a[],const char flag[],const int data_a[],const bool &mark) {
		if (!mark) return;
		switch (op) {
			case 34:case 35:case 36:
			case 37:case 38:
			case 43:case 44:
			case 45:case 46:
				
			case 49:case 50:case 51:case 52:case 53:
				
			case 57:case 58:case 59:case 60:case 61:case 62:
				regis[a[0]] = ans1;
				break;
			case 39:case 40:
			case 41:case 42:
			case 47:case 48:
				if (flag[2] != 2) regis[a[0]] = ans1;
				else regis[32] = ans1,regis[33] = ans2;
				break;
			case 64:case 65:case 66:case 67:case 68:case 69:
			case 70:case 71:case 72:case 73:case 74:case 75:
				break;
			case 63:case 76:
				break;
			case 77:
				break;
			case 78:
				regis[31] = pos;
				break;
			case 79:
				regis[31] = pos;
				break;
			case 80:
				regis[a[0]] = data_a[1];
				break;
			case 81:case 82:
				regis[a[0]] = data_a[0];
				break;
			case 84:
				switch (data_a[0]) {
					case 1:
						cout << regis[4];
						break;
					case 5:case 9:
						regis[2] = ans1;
						break;
					case 10:
//						cout << pos_text << ' ' << 1.0 * T / (T + F) << endl;
						exit(0);
					case 17:
//						cout << pos_text << ' ' << 1.0 * T / (T + F) << endl;
						exit(regis[4]);
				}
		}
	}
public:
	void _init() {
		int i;
		for (i = 1; i <= 5; ++i) 
			mark[i] = 0;
		for (i = 0; i < 34; ++i)
			regis[i] = regist[i] = 0;
		regis[29] = regist[29] = maxn - 1;
		mark[1] = 1;
		memset(control_hazard,0,sizeof(control_hazard));
		memset(data_a,0,sizeof(data_a));
		memset(predict_table,0,sizeof(predict_table));
		memset(condition,0,sizeof(condition));
	}
	void _work(const map <string,int> label_text) {
		int i;
		pos = label_text.at("main");
		do {
			_WB(ans1[5],ans2[5],op[5],a[5],flag[5],data_a[5],mark[5]);
			if (mark[5]) {
				if (control_hazard[5] && (64 <= op[5] && op[5] <= 75)) {
					control_hazard[5] = 0;
					mark[1] = 1;
				}
				if (pos >= pos_text) {
					mark[1] = 0;
				}
			}
			
			_MA(ans1[4],ans2[4],op[4],a[4],data_a[4],mark[4]);
			if (mark[4]) {
				ans1[5] = ans1[4];
				ans2[5] = ans2[4];
				op[5] = op[4];
				for (i = 0; i < 3; ++i) {
					a[5][i] = a[4][i];
					flag[5][i] = flag[4][i];
					data_a[5][i] = data_a[4][i];
				}
				control_hazard[5] = control_hazard[4];
			}
			mark[5] = mark[4];
			
			_EX(ans1[3],ans2[3],op[3],a[3],flag[3],data_a[3],mark[3]);
			if (mark[3]) {
				if (control_hazard[3]) {
					int cond = condition[pos_label >> 4];
					++predict_table[pos_label >> 4][cond][ans1[3]];
					cond = (cond << 1) | ans1[3];
					cond &= 3;
					condition[pos_label >> 4] = cond;
					if (ans1[3] != res) {
						++F;
						pos = pos_pre;
						mark[1] = mark[2] = 0;
					}
					else { 
						++T;
						control_hazard[3] = 0;
					}
				}
				ans1[4] = ans1[3];
				ans2[4] = ans2[3];
				op[4] = op[3];
				for (i = 0; i < 3; ++i) {
					a[4][i] = a[3][i];
					flag[4][i] = flag[3][i];
					data_a[4][i] = data_a[3][i];
				}
				switch (op[3]) {
					case 51:
						regist[a[3][0]] = *(reinterpret_cast<char*> (space + ans1[3]));
						break;
					case 52:
						regist[a[3][0]] = *(reinterpret_cast<short*> (space + ans1[3]));
						break;
					case 53:
						regist[a[3][0]] = *(reinterpret_cast<int*> (space + ans1[3]));
						break;
					case 84:
						if (data_a[3][0] == 9)
						regist[2] = pos_data;
						break;
				}
				control_hazard[4] = control_hazard[3];
			}
			mark[4] = mark[3];
			
			_ID_and_DP(op[2],a[2],flag[2],data_a[2],mark[2]);
			if (mark[2]) {
				control_hazard[2] = 0;
				if (63 <= op[2] && op[2] <= 79) {
					if (op[2] == 63 || (76 <= op[2] && op[2] <= 79)) {
						if (op[2] == 78 || op[2] == 79) regist[31] = pos;
						pos = data_a[2][0];
						if (!control_hazard[3] && mark[1] == 0 && pos < pos_text) mark[1] = 1;
					}
					else {
						control_hazard[2] = 1;
						pos_label = pos;
						int cond = condition[pos_label >> 4];
						res = (predict_table[pos_label >> 4][cond][0] <= predict_table[pos_label >> 4][cond][1]);
						if (res) {
							pos_pre = pos;
							if (64 <= op[2] && op[2] <= 69) pos = data_a[2][2];
							else pos = data_a[2][1];
						}
						else {
							if (64 <= op[2] && op[2] <= 69) pos_pre = data_a[2][2];
							else pos_pre = data_a[2][1];
						}
					}
				}
				op[3] = op[2];
				for (i = 0; i < 3; ++i) {
					a[3][i] = a[2][i];
					flag[3][i] = flag[2][i];
					data_a[3][i] = data_a[2][i];
				}
				control_hazard[3] = control_hazard[2];
			}
			mark[3] = mark[2];
			
			_IF(mark[1]);
			mark[2] = mark[1];
		} while (mark[1] || mark[2] || mark[3] || mark[4] || mark[5]);
	}
} S;
int main(int argc,char **argv) {
	int num;
	char name[233];
	P._clear();
	P._encode_init();
	num = 0;
	strcpy(name,argv[1]);
	P._read_code(name,num);
	S._init();
	S._work(P.label_text);
//	cout << pos_text << ' ' << 1.0 * T / (T + F) << endl;
	return 0;
}
