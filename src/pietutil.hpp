#ifndef PIETUTIL_HPP_
#define PIETUTIL_HPP_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <cmath>
#include <cstring>

class PietUtil
{
public:
	static bool parseText(const char* text, std::vector<std::string>& cmds)
	{
		cmds.clear();
		int b = -1;
		int e = -1;
		const char delim[] = " \n";
		bool isComment = false;

		for (int i = 0; i < strlen(text) + 1; i++)
		{
			const char* tptr = text + i;
			const char* isDelim = strchr(delim, *tptr);
			if (b == -1)
			{
				if (isDelim == nullptr)
				{
					b = i;
					e = i;
				}
			}
			else
			{
				if (isDelim != nullptr)
				{
					e = i;
					int L = e - b;
					char* tmp = new char[L + 1];
					strncpy(tmp, text + b, L);
					tmp[L] = '\0';

					//	isComment?
					if (tmp[0] == '#')
					{
						isComment = true;
					}
					if (!isComment)
					{
						if (PietUtil::isLabel(tmp) || PietUtil::isCommand(tmp))
						{
							cmds.push_back(tmp);
						}
					}

					//	Comment region ended?
					if (isDelim >= delim + 1)
					{
						isComment = false;
					}
					//	initialize
					b = -1;
					e = -1;
					delete[] tmp;
				}
			}
		}
		return true;
	}
	static bool isLabel(std::string word)
	{
		if (word[0] != ':') { return false; }
		if (isdigit(word[1])) { return false; }
		if (!std::all_of(word.cbegin() + 1, word.cend(), [](char c) {return (isalnum(c) || (c == '_')); })) { return false; }

		return true;
	}
	static bool isIfCommand(std::string word, std::string* outT = nullptr, std::string* outF = nullptr)
	{
		if (word.size() <= 4) { return false; }	//	longer than "if::"
		if (word.substr(0, 3) != "if:") { return false; }
		std::vector<int> labels;
		for (size_t i = 0; i < word.size(); i++)
		{
			if (word[i] == ':') { labels.push_back(i); }
		}
		if (labels.size() != 2) { return false; }
		std::string arg1 = word.substr(labels[0], labels[1] - labels[0]);
		std::string arg2 = word.substr(labels[1]);
		if (!isLabel(arg1)) { return false; }
		if (!isLabel(arg2)) { return false; }
		if (arg1 == "" && arg2 == "") { return false; }
		if (outT != nullptr) { *outT = arg1; }
		if (outF != nullptr) { *outF = arg2; }
		return true;
	}
	static bool isGotoCommand(std::string word, std::string* outT = nullptr)
	{
		if (word.size() <= 5) { return false; }	//	longer than "goto:"
		if (word.substr(0, 5) != "goto:") { return false; }
		std::vector<int> labels;
		for (size_t i = 0; i < word.size(); i++)
		{
			if (word[i] == ':') { labels.push_back(i); }
		}
		if (labels.size() != 1) { return false; }
		std::string _arg = word.substr(labels[0]);
		if (!isLabel(_arg)) { return false; }
		if (_arg == ":") { return false; }	//	ommiting label is not permitted
		if (outT != nullptr) { *outT = _arg; }
		return true;
	}
	static bool isEndCommand(std::string word)
	{
		return (word == "end");
	}
	static bool isPushCommand(std::string word, int* _arg = nullptr)
	{
		if (word.size() < 5) { return false; }	//	no shorter than for example "push1"
		if (word.substr(0, 4) != "push") { return false; }
		std::string arg = word.substr(4);
		if (arg[0] == '0') { return false; }
		if (!std::all_of(arg.cbegin(), arg.cend(), isdigit)) { return false; }
		if (_arg != nullptr) { *_arg = std::atoi(arg.c_str()); }
		return true;
	}
	static bool isCommand(std::string word)
	{
		return (false
			|| word == "pop"
			|| word == "add"
			|| word == "sub"
			|| word == "mul"
			|| word == "div"
			|| word == "mod"
			|| word == "not"
			|| word == "gt"
			|| word == "dup"
			|| word == "roll"
			|| word == "inc"
			|| word == "inn"
			|| word == "outc"
			|| word == "outn"
			|| isEndCommand(word)
			|| isPushCommand(word)
			|| isIfCommand(word)
			|| isGotoCommand(word)
			);
	}
public:
	static const char* getPietColor_ascii(int h, int b)
	{
		static char colors[][3]
			= {
				{'R','r','0'},
				{'Y','y','1'},
				{'G','g','2'},
				{'C','c','3'},
				{'B','b','4'},
				{'M','m','5'},
		};
		static char black = { 'K' };
		static char white = { 'W' };

		if (h < 0 && b < 0) { return &black; }
		else if (h < 0 || b < 0) { return &white; }
		return &(colors[h % 6][b % 3]);
	}
	static const unsigned char* getPietColor(int h, int b)
	{
		static unsigned char colors[][3][3]
			= {
				{{0xFF,0xC0,0xC0},{0xFF,0x00,0x00},{0xC0,0x00,0x00}},
				{{0xFF,0xFF,0xC0},{0xFF,0xFF,0x00},{0xC0,0xC0,0x00}},
				{{0xC0,0xFF,0xC0},{0x00,0xFF,0x00},{0x00,0xC0,0x00}},
				{{0xC0,0xFF,0xFF},{0x00,0xFF,0xFF},{0x00,0xC0,0xC0}},
				{{0xC0,0xC0,0xFF},{0x00,0x00,0xFF},{0x00,0x00,0xC0}},
				{{0xFF,0xC0,0xFF},{0xFF,0x00,0xFF},{0xC0,0x00,0xC0}},
		};
		static unsigned char black[3] = { 0x00,0x00,0x00 };
		static unsigned char white[3] = { 0xFF,0xFF,0xFF };

		if (h < 0 && b < 0) { return black; }
		else if (h < 0 || b < 0) { return white; }
		return colors[h % 6][b % 3];
	}
	static bool getNextPietColor(std::string cmd, int& h, int&b, bool inverse = false)
	{
		static std::string commands[][3]
			= {
				{"nop","push","pop"},
				{"add","sub","mul"},
				{"div","mod","not"},
				{"gt","ptr","swt"},
				{"dup","roll","inn"},
				{"inc","outn","outc"},
		};
		int dh = 0;
		int db = 0;
		for (int H = 0; H < 6; H++)
		{
			for (int B = 0; B < 3; B++)
			{
				if (cmd == commands[H][B])
				{
					dh = H;
					db = B;
					break;
				}
			}
		}
		if (dh == 0 && db == 0) { return false; }
		if (inverse)
		{
			h = (h + 6 - dh) % 6;
			b = (b + 3 - db) % 3;
		}
		else
		{
			h = (h + dh) % 6;
			b = (b + db) % 3;
		}

		return true;
	}
	template <typename T>
	static void setColor(T* dst, const T* col, int C)
	{
		for (int c = 0; c < C; c++)
		{
			dst[c] = col[c];
		}
	}
public:
	static bool getPushCommandSize(int val, int& w, int& h)
	{
		if (val <= 0) { return false; }
		h = std::sqrt(val);
		w = val / h;
		if (h*w < val) { h += 1; }
		return true;
	}
	static bool export_ppm(const char* fname, const unsigned char* data, int W, int H)
	{
		std::stringstream ss;
		ss << "P6" << std::endl;
		ss << W << " " << H << std::endl << 255 << std::endl;
		std::string s = ss.str();
		std::ofstream ofs(fname, std::ios::out | std::ios::binary);
		if (!ofs.fail())
		{
			ofs.write(s.c_str(), s.length());
			ofs.write(reinterpret_cast<const char*>(data), 3 * W*H);
		}
		return true;
	}
};
#endif