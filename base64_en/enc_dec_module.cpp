#include<string>
#include<fstream>
#include<iostream>
#include<sstream>
#include<vector>
#include<bitset>
#include<windows.h>
#include"length_exception.h"
using namespace std;
const string dict = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
// The defined BLK_LENGTH should be a multiple of 6, such as 65538
#define BLK_LENGTH 1048578 

string b64c = "";//b64c is the final base64 code of a single block

//int detectcode(string filename) {
//
//	int res = -1;
//	ifstream fin(filename, ios::binary);
//	if (fin.fail()) {
//		return res;
//	}
//	unsigned char s2;
//	fin.read((char*)&s2, sizeof(s2));//读取第一个字节，然后左移8位
//	int p = s2 << 8;
//	fin.read((char*)&s2, sizeof(s2));//读取第二个字节
//	p += s2;
//
//	string code;
//
//	switch (p)//判断文本前两个字节
//	{
//	case 0xfffe:  //65534
//		code = "Unicode";
//		res = 0;
//		break;
//	case 0xfeff://65279
//		code = "Unicode big endian";
//		res = 1;
//		break;
//	case 0xe6a2://59042
//		code = "UTF-8";
//		res = 2;
//		break;
//	default:
//		code = "ANSI";
//		res = 3;
//	}
//	fin.close();
//	cout << code << endl;
//	return res;
//
//}
//
//int CN2Utf8(char input[], char output[]) {
//	int len;
//	//printf("%d\n",strlen(input));
//	wchar_t* out = (wchar_t*)malloc(strlen(input) * sizeof(wchar_t));
//	len = MultiByteToWideChar(CP_ACP, 0, input, -1, out, 0);
//	MultiByteToWideChar(CP_ACP, 0, input, -1, out, len);
//	len = WideCharToMultiByte(CP_UTF8, 0, out, wcslen(out), output, 0, NULL, NULL);
//	WideCharToMultiByte(CP_UTF8, 0, out, wcslen(out), output, len, NULL, NULL);
//	/*for(int i=0;i<len;i++){
//		printf("%d ",output[i]);
//	}
//	printf("%s",output);
//	printf("\n");*/
//	//free(out);
//	return 1;
//}
//int Utf82CN(char input[], char output[]) {
//	int len;
//	//printf("%d\n",strlen(input));
//	wchar_t* out = (wchar_t*)malloc(strlen(input) * sizeof(wchar_t));
//	len = MultiByteToWideChar(CP_UTF8, 0, input, -1, out, 0);
//	MultiByteToWideChar(CP_UTF8, 0, input, -1, out, len);
//	len = WideCharToMultiByte(CP_ACP, 0, out, wcslen(out), output, 0, NULL, NULL);
//	WideCharToMultiByte(CP_ACP, 0, out, wcslen(out), output, len, NULL, NULL);
//	output[len] = '\0';
//	/*for(int i=0;i<len;i++){
//		printf("%d ",output[i]);
//	}*/
//	//printf("%s",output);
//	//printf("\n");
//	//free(out);
//	return 1;
//}

//get a byte
string single_byte(char c) {				
	int vl = c;
	string en;
	char b[9] = "";
	if (vl < 0)vl += 256;
	for (int i = 1; i <= 8; i++) {
		b[8 - i] = vl % 2 + 48;
		vl >>= 1;
	}
	en = b;
	return en;
}

//get a 6-bit
string bit6(int v) {											
	string en;
	char b[7] = "";
	for (int i = 1; i <= 6; i++) {
		b[6 - i] = v % 2 + 48;
		v >>= 1;
	}
	en = b;
	return en;
}

/**
* Encode to base64 by block
*/
string blk_encode(char blk[],int size) {
	string byte = "", block_bitstring = "", block_b64string = "";
	//get bit stream
	for (int i = 0; i < size; i++) {
		byte = single_byte(blk[i]);
		//block_bitstring is the final block bitstream
		block_bitstring += byte;
	}
	size_t lc = block_bitstring.length();
	int padding = 0, bv = 0;
	//pad the length to a multiple of 6
	if (lc % 6 == 2) {
		padding = 2;
		block_bitstring += string("0000");
	}
	else if (lc % 6 == 4) {
		padding = 1;
		block_bitstring += string("00");
	}
	else if (lc % 6 == 0) {
	}
	else {
		throw InvalidLengthException();
	}
	//encrypt
	for (size_t i = 0; i < lc; i += 6) {
		bv = 0;
		for (int j = 0; j <= 5; j++) {
			bv <<= 1;
			bv += block_bitstring[i + j] - 48;
		}
		block_b64string += dict[bv];
	}
	for (int i = 1; i <= padding; i++) {
		block_b64string += '=';
	}
	return block_b64string;
}

/** 
* Encoding function
*/
void encd(ifstream& fin,ofstream &fout) {
	//CN2Utf8(a, a);
	//printf("%d ",strlen(a));
	b64c = "";
	char* temp=new char[BLK_LENGTH]; //buffer size=BLK_LENGTH bytes
	//get bitstream
	cout << "Reading file..." << endl;
	cout << "Encrypting file..." << endl;
	while (!fin.eof()) { //process by block
		//read by block
		fin.read(temp, BLK_LENGTH);
		int bytecount = fin.gcount();
		if (bytecount) {//block size is not 0
			try {
				b64c = blk_encode(temp, bytecount);
				fout << b64c;
				fout.flush(); //flush buffer to file
				//system("pause");
			}
			catch (InvalidLengthException e) {
				delete[] temp;
				throw;
			}
			delete[] temp;
		}
		else {
			break;
		}
	}
}

/**
* Decoding function
*/
string decd(ifstream& fin, int& filelen) {
	cout << "Reading file..." << endl;
	string t_buf;
	stringstream ss;
	filelen = 0;
	ss << fin.rdbuf();
	t_buf = ss.str();//t_buf now stores the file content

	int v = 0, flag = 0;//flag shows the number of '='
	size_t l = t_buf.length();
	size_t lc;
	//cout << l << endl;

	if (l % 4)return "";//not a base64 code
	cout << "Decrypting Base64 file..." << endl;

	string subc = "", bt3 = "", finstring = "";
	//subc is the bitstream
	//finstring is the final content

	for (size_t i = 0; i < l; i += 4) {
		bt3 = "";//bitstream of 3 bytes
		for (size_t j = 0; j < 4; j++) {
			if (dict.find(t_buf[i + j]) != dict.npos) {
				v = dict.find(t_buf[i + j]);
				if (v == 64)flag++;
				else bt3 += bit6(v);
			}
			else {
				return "";
			}
		}
		subc += bt3;
	}

	lc = subc.length() - flag * 2;

	if (lc % 8)return "";
	else {
		for (size_t i = 0; i < lc; i += 8) {
			v = 0;
			for (size_t j = 0; j < 8; j++) {
				v <<= 1;
				v += subc[i + j] - 48;
			}
			if (v > 127)v -= 256;
			filelen++;
			finstring.push_back(char(v));
		}
		//cout<<strlen(dr)<<endl;
		//Utf82CN(dr, dr);
		//fin = dr;
		return finstring;
	}
}