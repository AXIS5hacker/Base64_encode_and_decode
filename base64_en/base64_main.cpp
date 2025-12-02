#include<iostream>
#include<cmath>
#include<cstring>
#include<cstdlib>
#include<cstdio>
#include<string>
#include<windows.h>
#include<fstream>
#include<locale>
#include"length_exception.h"

using namespace std;
char m[600000];
//string bcode = "", b64c = "";
extern const string dict = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
//extern int detectcode(string filename);
extern void encd(ifstream& fin,ofstream &fout);
extern string decd(ifstream& fin, int& filelen);


ifstream fin;
ofstream fout;
int main() {
	int i = 0, flag;
	string s, base64;
	printf("=====BASE64 Encoder/Decoder by AXIS5=====\nSelect Mode(encrypt=1,decrypt=2):");
	cin >> flag;
	cin.ignore();
	if (flag == 1) {
		printf("Please input the filename that you wanted to encrypt\n");
		string filename;
		getline(cin, filename);
		//detect the file code
		//int code = detectcode(filename);
		fin.open(filename.c_str(), ios::binary | ios::in);
		filename += ".base64";
		fout.open(filename.c_str());
		if (fin.fail()) {
			cout << "未找到文件" << endl;
			system("pause");
			return 0;
		}
		try {
			encd(fin,fout);//encode
			cout << "Encrypted file saved to " << filename << endl;
			fin.close();
			fout.close();
		}
		catch(InvalidLengthException e){
			fin.close();
			fout.close();
			cout << e.what() << endl;
		}
		/*while (!cin.eof()) {
			cin.get(m[i]);
			i++;
		}
		if (strlen(m))m[strlen(m) - 1] = 0;
		s = encd(m);*/
	}
	else {
		printf("Please input the Base64 file that you wanted to decrypt\n");
		string filename;
		getline(cin, filename);
		fin.open(filename.c_str());
		if (fin.fail()) {
			cout << "未找到文件" << endl;
			system("pause");
			return 0;
		}
		int decode_length;
		s = decd(fin, decode_length);
		filename += ".b64decode";
		fout.open(filename.c_str(), ios::binary);
		fout.write(s.data(), decode_length);
		cout << "Decrypted file saved to " << filename << endl;
		fout.close();
		/*while (!cin.eof()) {
			cin.get(m[i]);
			if (m[i] != '\n')i++;																	//cannot read '\n' when pressed CTRL+Z
		}
		if (strlen(m))m[strlen(m) - 1] = 0;
		base64 = m;
		//cout<<base64<<endl;
		s = decd(base64);*/
	}

	system("pause");
	return 0;
}