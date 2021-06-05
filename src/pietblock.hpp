#ifndef PIETBLOCK_HPP_
#define PIETBLOCK_HPP_

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <cstring>

#include "pietutil.hpp"

class PietBlock
{
private:
	std::string name;
	int pos;
	int dir;
	int w;
	int h;
	int ws;
	int hs;
	int bufIn;
	int bufOut;

	char hash;

	std::string outT;
	std::string outF;

	bool isEnded;
	bool flag;
	std::vector<std::string> cmds;

	class Err_NotKeyword {};
	class Err_UnknownCommand {};
	class Err_InvalidTermination {};

	static char getHash()
	{
		static int i = 0;
		const static char arrHash[] = "023456789ABCDEFGHJKLMNOPQRSTUWXYZabcdefghjkmnopqrstuwxyz@$%&#";
		const int L = strlen(arrHash);
		char c = arrHash[i];
		i = (i + 1) % L;
		return c;
	}
	void init(std::string name, int pos, int dir)
	{
		this->name = name;
		this->pos = pos;
		this->dir = dir;
		this->w = 0;
		this->h = 0;
		this->ws = 5;
		this->hs = 3;
		this->bufIn = 0;
		this->bufOut = 0;
		hash = getHash();
		outT = "";
		outF = "";
		isEnded = false;
		flag = false;
		cmds.clear();
	}
	void copyTo(PietBlock& dst) const
	{
		dst.name = name;
		dst.pos = pos;
		dst.dir = dir;
		dst.w = w;
		dst.h = h;
		dst.ws = ws;
		dst.hs = hs;
		dst.bufIn = bufIn;
		dst.bufOut = bufOut;
		dst.hash = hash;
		dst.outT = outT;
		dst.outF = outF;
		dst.isEnded = isEnded;
		dst.flag = flag;
		dst.cmds.clear();
		for (auto itr = cmds.begin(); itr != cmds.end(); itr++)
		{
			dst.cmds.push_back(*itr);
		}
	}
	PietBlock(std::string name, int pos, int dir)
	{
		init(name, pos, dir);
	}
public:
	PietBlock()
	{
		init("", 0, 3);
	}
	PietBlock(std::string name)
	{
		init(name, 0, 3);
	}
	PietBlock(const PietBlock& obj)
	{
		obj.copyTo(*this);
	}
	~PietBlock() {}

	bool clearCommands()
	{
		cmds.clear();
		return true;
	}
	bool appendCommand(std::string cmd, std::string& nextLabel, int& nextLabelIndex)
	{
		bool isAdded = false;
		int cw = 0;
		int ch = 0;
		int cws = 0;
		int chs = 0;

		if (PietUtil::isLabel(cmd))
		{
			if (!isEnded)
			{
				setOutT(cmd);
				cws = 3;
			}
			nextLabel = cmd;
			isEnded = true;
			isAdded = true;
		}
		else if (PietUtil::isCommand(cmd))
		{
			if (!isEnded)
			{
				int arg = 0;
				std::string outT;
				std::string outF;
				if (PietUtil::isPushCommand(cmd, &arg))
				{
					PietUtil::getPushCommandSize(arg, cw, ch);
				}
				else if (PietUtil::isGotoCommand(cmd, &outT))
				{
					cws = 3;
					chs = 3;
					if (outT == ":")
					{
						nextLabel = ":" + std::to_string(nextLabelIndex);
						nextLabelIndex++;
						outT = nextLabel;
					}
					setOutT(outT);
					isEnded = true;
				}
				else if (PietUtil::isIfCommand(cmd, &outT, &outF))
				{
					cws = 11;
					chs = 3;
					if (outT == ":" || outF == ":")
					{
						nextLabel = ":" + std::to_string(nextLabelIndex);
						nextLabelIndex++;
						if (outT == ":") { outT = nextLabel; }
						if (outF == ":") { outF = nextLabel; }
					}
					setOutT(outT);
					setOutF(outF);
					isEnded = true;
				}
				else if (PietUtil::isEndCommand(cmd))
				{
					cws = 5;
					chs = 4;
					isEnded = true;
				}
				else
				{
					cw = 1;
					ch = 1;
				}
				cmds.push_back(cmd);
				isAdded = true;
			}
		}
		else
		{
			throw Err_UnknownCommand();
		}

		w += cw;
		h = std::max(h, ch);
		ws += cws;
		hs = std::max(hs, chs);
		
		return isAdded;
	}
	bool appendErrorCommand(std::string cmd)
	{
		cmds.push_back(cmd);
		return true;
	}
	std::string getName() const
	{
		return name;
	}
	std::string getOutT() const
	{
		return outT;
	}
	std::string getOutF() const
	{
		return outF;
	}
	bool setOutT(std::string outT)
	{
		this->outT = outT;
		return true;
	}
	bool setOutF(std::string outF)
	{
		this->outF = outF;
		return true;
	}
	void resetBuffers()
	{
		bufIn = 0;
		bufOut = 0;
		return;
	}
	bool setFlag() { return (flag = true); }
	bool resetFlag() { return (flag = false); }
	bool getFlag() const { return flag; }
	int getPos() const { return pos; }
	int setPos(int pos) { return (this->pos = pos); }
	int setDir(int dir) { return (this->dir = dir); }
	int getDir() const { return this->dir; }
	int getW() const
	{
		int ret = 0;
		if (outF == "")
		{
			if (outT == "")
			{
				//	end
				ret = (bufIn + w + 10);
			}
			else
			{
				//	goto or normal commands
				ret = (bufIn + w + 8);
			}
		}
		else
		{
			//	if
			ret = (bufIn + bufOut + w + 16);
		}
		return ret;
	}
	int getH() const 
	{
		return std::max(h, hs);
	}
	void getXY(int& X, int& Y, const int pathSize[2], const int heights[4]) const
	{
		std::cerr << "HEIGHTS={" << heights[0] << ", " << heights[1] << ", " << heights[2] << ", " << heights[3] << std::endl;
		std::cerr << "pathSize={" << pathSize[0] << ", " << pathSize[1] <<  std::endl;
		if (dir == 0)
		{
			//	E
			X = heights[2] + pathSize[0] + heights[0] - getH();
			Y = heights[3] + pos;
		}
		else if (dir == 1)
		{
			//	S
			X = heights[2] + pos;
			Y = heights[3] + pathSize[1] + heights[1] - getH();
		}
		else if (dir == 2)
		{
			//	W
			X = 0;
			Y = heights[3] + pos;
		}
		else if (dir == 3)
		{
			//	N
			X = heights[2] + pos;
			Y = 0;
		}
		return;
	}
	bool calcBuffer(int nPos, int nOutT, int nOutF)
	{
		bool ret = false;
		if (outF == "")
		{
			if (outT == "")
			{
				//	end
				pos = nPos - 2;
				ret = (pos>=0);
			}
			else
			{
				//	goto or normal commands
				pos = nPos - 2;
				bufIn = ((nOutT < 0) ? 0 : (nOutT - nPos - w - 5));
				ret = (pos >= 0 && bufIn >= 0);
			}
		}
		else
		{
			//	if
			pos = nPos - 2;
			bufIn = ((nOutF < 0) ? 0 : (nOutF - nPos - w - 7));
			bufOut = ((nOutF < 0 || nOutT<0) ? 0 : (nOutT - nOutF - 5)); ;
			ret = (pos >= 0 && bufIn >= 0 && bufOut >= 0);
		}
		return ret;
	}

	int getInPos() const
	{
		return pos + 2;
	}
	int getOutTPos() const
	{
		int ret = 0;
		if (outF == "")
		{
			if (outT == "")
			{
				//	end
				ret = -1;
			}
			else
			{
				//	goto or normal commands
				ret = (pos + bufIn + w + 7);
			}
		}
		else
		{
			//	if
			ret = (pos + bufIn + w + bufOut + 14);
		}
		return ret;
	}

	int getOutFPos() const
	{
		int ret = 0;
		if (outF == "")
		{
			//	end
			//	goto or normal commands
			ret = -1;
		}
		else
		{
			//	if
			ret = (pos + bufIn + w + 9);
		}
		return ret;
	}

	std::string str(std::string pad = "") const
	{
		std::stringstream ss;
		ss << pad << "*BLOCK " << name << "( hash : " << hash << " )[" << static_cast<const void*>(this) << "] : ";
		ss << pad << "  [" << getW() << "," << getH() << "] @ (pos,dir)=(" << pos << ", " << dir << ") user[" << w << "," << h << "]/sys[" << ws << "," << hs << "]/buf[" << bufIn << "," << bufOut << "]";
		if (outT != "") { ss << " outT=" << outT; }
		if (outF != "") { ss << " outF=" << outF; }
		for (size_t i = 0; i < cmds.size(); i++)
		{
			ss << std::endl << pad << " " << std::setw(5) << std::setfill(' ') << i << " | " << cmds[i];
		}

		return ss.str();
	}

	static const char* getPietBlockHash(int, int, const void* ptr)
	{
		static char c = '\0';
		if (ptr == nullptr)
		{
			c = '%';
		}
		else
		{
			const PietBlock* pb = reinterpret_cast<const PietBlock*>(ptr);
			c = pb->hash;
		}
		return &c;
	}

	void convertLocal2World(int& X, int& Y, int x, int y, int W, int H, int x0, int y0) const
	{
		switch (dir)
		{
		case 0: { X = W - y - 1; Y = y0 + pos + x; break; }
		case 1: { Y = H - y - 1; X = x0 + pos + x; break; }
		case 2: { X = y; Y = y0 + pos + x; break; }
		case 3: { Y = y; X = x0 + pos + x; break; }
		}
		return;
	}

	template <typename T>
	void draw(T* data, int W, int H, int C, int _x0, int _y0, T blank, const T* (*getPietColor)(int h, int b, const void*)) const
	{
		size_t ori = 0;
		int dx = 0;
		int dy = 0;

		switch (dir)
		{
		case 0: { ori = ((_y0 + pos)*W + (W - 1))*C; dx = W * C; dy = -C; break; }
		case 1: { ori = ((H - 1)*W + (_x0 + pos))*C; dx = C; dy = -W * C; break; }
		case 2: { ori = (_y0 + pos)*W*C; dx = W * C; dy = C; break; }
		case 3: { ori = (_x0 + pos)*C; dx = C; dy = W * C; break; }
		}

		T* const ptr = data + ori;
		const T* col = nullptr;

		int h = 0; int b = 0;
		//	header
		col = getPietColor(-1, -1, this);
		PietUtil::setColor(ptr + dy, col, C);
		PietUtil::setColor(ptr + dy + 4 * dx, col, C);
		if (dir == 0 || dir == 3)
		{
			col = getPietColor(h, b, this);
			PietUtil::setColor(ptr + dy * 2 + dx * 2, col, C);

			PietUtil::getNextPietColor("push",h,b);
			col = getPietColor(h, b, this);
			PietUtil::setColor(ptr + dy * 1 + dx * 2, col, C);
			PietUtil::setColor(ptr + dy * 1 + dx * 1, col, C);

			PietUtil::getNextPietColor("ptr", h, b);
			col = getPietColor(h, b, this);
			PietUtil::setColor(ptr + dx * 1, col, C);
			PietUtil::setColor(ptr + dx * 2, col, C);
			PietUtil::setColor(ptr + dx * 3, col, C);
			PietUtil::setColor(ptr + dy * 1 + dx * 3, col, C);
		}
		else
		{
			col = getPietColor(h, b, this);

			PietUtil::setColor(ptr + dy * 2, col, C);
			PietUtil::setColor(ptr + dy * 2 + dx * 1, col, C);
			PietUtil::setColor(ptr + dy * 2 + dx * 2, col, C);
			PietUtil::getNextPietColor("push", h, b);
			col = getPietColor(h, b, this);

			PietUtil::setColor(ptr + dy * 1 + dx * 2, col, C);
			PietUtil::setColor(ptr + dy * 1 + dx * 1, col, C);
			PietUtil::getNextPietColor("ptr", h, b);
			col = getPietColor(h, b, this);

			//PietUtil::setColor(ptr, col, C);	//	I think it's not required...
			PietUtil::setColor(ptr + dx * 1, col, C);
			PietUtil::setColor(ptr + dx * 2, col, C);
			PietUtil::setColor(ptr + dx * 3, col, C);
			PietUtil::setColor(ptr + dy * 1 + dx * 3, col, C);
		}
		//	user
		h = 0; b = 0;
		col = getPietColor(h, b, this);
		int x = 5 + bufIn;
		bool isEnded = false;
		int arg=0;
		for (size_t i = 0; i < cmds.size(); i++)
		{
			std::string cmd = cmds[i];
			if (PietUtil::isEndCommand(cmd))
			{
				PietUtil::setColor(ptr + dx * x, col, C);
				if (dir == 1 || dir == 2)
				{
					PietUtil::setColor(ptr + dx * x + dy * 1, col, C);
					PietUtil::setColor(ptr + dx * x + dy * 2, col, C);
					PietUtil::getNextPietColor("push", h, b);
					col = getPietColor(h, b, this);

					PietUtil::setColor(ptr + dx * (x + 1), col, C);
					PietUtil::getNextPietColor("ptr", h, b);
					col = getPietColor(h, b, this);

					PietUtil::setColor(ptr + dx * (x + 2), col, C);
				}

				PietUtil::setColor(ptr + dx * (x + 2) + dy * 2, col, C);
				PietUtil::setColor(ptr + dx * (x + 3) + dy * 2, col, C);
				PietUtil::setColor(ptr + dx * (x + 3) + dy * 1, col, C);

				col = getPietColor(-1, -1, this);
				PietUtil::setColor(ptr + dx * (x + 1) + dy * 2, col, C);
				PietUtil::setColor(ptr + dx * (x + 2) + dy * 3, col, C);
				PietUtil::setColor(ptr + dx * (x + 3) + dy * 3, col, C);
				PietUtil::setColor(ptr + dx * (x + 4) + dy * 2, col, C);
				PietUtil::setColor(ptr + dx * (x + 4) + dy * 1, col, C);
				PietUtil::setColor(ptr + dx * (x + 3), col, C);
				isEnded = true;
				break;
			}
			else if (PietUtil::isGotoCommand(cmd))
			{
				break;
			}
			else if (PietUtil::isIfCommand(cmd))
			{
				PietUtil::setColor(ptr + dx * x, col, C);
				PietUtil::getNextPietColor("not", h, b);
				col = getPietColor(h, b, this);

				PietUtil::setColor(ptr + dx * (x + 1), col, C);
				PietUtil::getNextPietColor("swt", h, b);
				col = getPietColor(h, b, this);


				PietUtil::setColor(ptr + dx * (x + 2), col, C);
				PietUtil::setColor(ptr + dx * (x + 2) + dy * 1, col, C);
				PietUtil::setColor(ptr + dx * (x + 2) + dy * 2, col, C);

				if (dir == 1 || dir == 2)
				{
					PietUtil::getNextPietColor("push", h, b);
					col = getPietColor(h, b, this);

					PietUtil::setColor(ptr + dx * (x + 3), col, C);
					PietUtil::setColor(ptr + dx * (x + 3) + dy * 2, col, C);
					PietUtil::getNextPietColor("ptr", h, b);
					col = getPietColor(h, b, this);
				}
				PietUtil::setColor(ptr + dx * (x + 4) + dy * 2, col, C);

				PietUtil::setColor(ptr + dx * (x + 9 + bufOut), col, C);
				if (dir == 1 || dir == 2)
				{
					PietUtil::getNextPietColor("ptr", h, b, true);
					col = getPietColor(h, b, this);

					PietUtil::setColor(ptr + dx * (x + 8 + bufOut), col, C);

				}

				if (dir == 0 || dir == 3)
				{
					col = getPietColor(-1, -1, this);
					PietUtil::setColor(ptr + dx * (x + 10 + bufOut), col, C);
					PietUtil::setColor(ptr + dx * (x + 5) + dy * 2, col, C);
				}
				isEnded = true;
				break;
			}
			else if (PietUtil::isPushCommand(cmd, &arg))
			{
				int hh = 0; int ww = 0;
				PietUtil::getPushCommandSize(arg, ww, hh);
				int cnt = 0;
				for (int yy = 0; yy < hh; yy++)
				{
					for (int xx = 0; xx < ww; xx++)
					{
						PietUtil::setColor(ptr + dx * (x + xx) + dy * yy, col, C);
						cnt++;
						if (arg <= cnt) { break; }
					}
					if (arg <= cnt) { break; }
				}
				x += ww;
				PietUtil::getNextPietColor("push", h, b);
				col = getPietColor(h, b, this);
			}
			else if (PietUtil::isCommand(cmd))
			{
				PietUtil::setColor(ptr + dx * x, col, C);
				x += 1;
				PietUtil::getNextPietColor(cmd, h, b);
				col = getPietColor(h, b, this);
			}
		}
		if (!isEnded)
		{
			PietUtil::setColor(ptr + dx * x, col, C);
			if (dir == 1 || dir == 2)
			{
				PietUtil::setColor(ptr + dx * x + dy * 1, col, C);
				PietUtil::setColor(ptr + dx * x + dy * 2, col, C);
			}
			PietUtil::getNextPietColor("push", h, b);
			col = getPietColor(h, b, this);

			PietUtil::setColor(ptr + dx * (x + 1), col, C);
			PietUtil::getNextPietColor("ptr", h, b);
			col = getPietColor(h, b, this);

			PietUtil::setColor(ptr + dx * (x + 2), col, C);
		}

		return;
	}

};

#endif
