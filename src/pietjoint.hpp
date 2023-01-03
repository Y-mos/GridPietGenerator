#ifndef PIETJOINT_HPP_
#define PIETJOINT_HPP_

#include <iostream>
#include <sstream>
#include <string>

class PietJoint
{
private:
	int x;
	int y;
	int dirBefore;
	int dirAfter;
	std::string from;
	std::string to;

	class Err_NoTurn {};

	void init(int x, int y, int dirBefore, int dirAfter, std::string from, std::string to)
	{
		this->x = x;
		this->y = y;
		this->dirBefore = dirBefore;
		this->dirAfter = dirAfter;
		this->from = from;
		this->to = to;
	}
	void copyTo(PietJoint& dst) const
	{
		dst.x = x;
		dst.y = y;
		dst.dirBefore = dirBefore;
		dst.dirAfter = dirAfter;
		dst.from = from;
		dst.to = to;
	}
public:
	PietJoint()
	{
		init(-1, -1, 0, 1, "", "");
	}
	PietJoint(int x, int y, int dirBefore, int dirAfter, std::string from="", std::string to="")
	{
		init(x, y, dirBefore, dirAfter, from, to);
	}
	PietJoint(const PietJoint& obj)
	{
		obj.copyTo(*this);
	}
	~PietJoint(){}

	std::string str(std::string pad="") const
	{
		std::stringstream ss;
		ss << pad << "*JOINT [" << static_cast<const void*>(this) << "] @(" << x << "," << y << ")" << " '" << from << "(" << dirBefore << ")'->'" << to << "(" << dirAfter << ")'";

		return ss.str();
	}
    std::string getList(char delim=',', bool isHeader=true) const
    {
        std::stringstream ss;
        if(isHeader)    //  output PietJoint Header or not
        {
            ss << "#type:JOINT" << delim;
            ss << "address" << delim;
            ss << "x" << delim;
            ss << "y" << delim;
            ss << "from" << delim;
            ss << "dirBefore" << delim;
            ss << "to" << delim;
            ss << "dirAfter" << delim;
            ss << std::endl;
        }
        ss << "JOINT" << delim;
        ss << static_cast<const void*>(this) << delim;
        ss << x << delim;
        ss << y << delim;
        ss << from << delim;
        ss << dirBefore << delim;
        ss << to << delim;
        ss << dirAfter << delim;
        ss << std::endl;

        return ss.str();
    }
    
	template <typename T>
	void draw(T* data, int W, int H, int C, int _x0, int _y0, T blank, const T* (*getPietColor)(int h, int b, const void*)) const
	{
		bool isRightTurn;
		switch ((dirAfter + 4 - dirBefore) % 4)
		{
		case 3:isRightTurn = false; break;
		case 1:isRightTurn = true; break;
		default:throw Err_NoTurn(); break;
		}

		int dir = 0;
		if (isRightTurn) { dir = dirBefore; } else { dir = (dirAfter + 2) % 4; }

		size_t ori = ((_x0 + x) + (_y0 + y) * W)*C;
		int dx = 0;
		int dy = 0;
		switch (dir)
		{
		case 0:dx = C; dy = W * C; break;
		case 1:dx = W * C; dy = -C; break;
		case 2:dx = -C; dy = -W * C; break;
		case 3:dx = -W * C; dy = C; break;
		}

		T* const ptr = data + ori;
		const T* col = nullptr;

		int h = 0; int b = 0;
		//	joint
		col = getPietColor(h, b, nullptr);
		PietUtil::setColor(ptr, col, C);
		if (isRightTurn)
		{
			col = getPietColor(-1, -1, nullptr);
			PietUtil::setColor(ptr + dx, col, C);
		}
		else
		{
			PietUtil::setColor(ptr + dx + dy, col, C);
			PietUtil::setColor(ptr + dx, col, C);
			PietUtil::setColor(ptr + dx - dy, col, C);
			col = getPietColor(-1, -1, nullptr);
			PietUtil::setColor(ptr + dx - 2 * dy, col, C);
			PietUtil::setColor(ptr + dx + 2 * dy, col, C);
			PietUtil::setColor(ptr - dy + 2 * dx, col, C);
			PietUtil::setColor(ptr + dy + 2 * dx, col, C);
		}
	}

	bool draw_ascii(char*& data, const int pathSize[2], const int heights[4], int& W, int& H, char blank) const
	{
		static char markers[4] = { '>','v','<','^' };
		int x = this->x + heights[2];
		int y = this->y + heights[3];

		for (int yy = y-2; yy <= y + 2; yy++)
		{
			for (int xx = x-2; xx <= x + 2; xx++)
			{
				char& c = data[yy*W + xx];
				c = (c != blank ? '*' : markers[dirAfter]);
			}
		}

		return true;
	}
};

#endif
