#ifndef PIETPATH_HPP_
#define PIETPATH_HPP_

class PietPath
{
private:
	int pos;
	int dir;
	std::string name;
	std::string to;

	void init(int pos, int dir, std::string from="", std::string to="")
	{
		this->pos = pos;
		this->dir = dir;
		this->name = from;
		this->to = to;
	}
	void copyTo(PietPath& dst) const
	{
		dst.pos = pos;
		dst.dir = dir;
		dst.name = name;
		dst.to = to;
	}

public:
	PietPath()
	{
		init(0, 0);
	}
	PietPath(int pos, int dir, std::string name = "", std::string to="")
	{
		init(pos, dir, name, to);
	}
	PietPath(const PietPath& obj)
	{
		obj.copyTo(*this);
	}
	~PietPath(){}

	int getPos() const { return pos; }
	int setPos(int pos) { return (this->pos = pos); }
	int getDir() const { return dir; }
	int setDir(int dir) { return (this->dir = dir); }
	std::string getName() const { return name; }
	std::string getTo() const { return to; }
	std::string setTo(std::string to) { return (this->to = to); }

	std::string str(std::string pad = "") const
	{
		std::stringstream ss;
		ss << pad << "*PATH [" << static_cast<const void*>(this) << "] (" << name << ")==>(" << to << ") (pos, dir)=("
			<< pos << "," << dir2str(dir) << ")";

		return ss.str();
	}

	static const char* getPathColor_ascii(int dir, const void*)
	{
		static char _[] = { '>','v','<','^','+' };
		return (_ + dir);
	}
	static const unsigned char* getPathColor(int dir, const void*)
	{
		static unsigned char _[][3] = { {64,64,64},{96,96,96},{128,128,128},{160,160,160},{192,192,192} };
		return _[dir];
	}


	template <typename T>
	void draw(T* data, int W, int H, int C, int x0, int y0, int& BPX, int& BPY, int _EP, T blank, const T* (*getPathColor)(int dir, const void*)) const
	{
		int ori = (BPY*W + BPX)*C;
		int dptr = 0;
		int BP = 0;
		int EP = 0;

		int nextBPX = -1;
		int nextBPY = -1;

		switch (dir)
		{
		case 0: { BP = BPX; EP = _EP+x0; nextBPX = EP; nextBPY = BPY; dptr = C; break; }
		case 1: { BP = BPY; EP = _EP+y0; nextBPX = BPX; nextBPY = EP; dptr = W * C; break; }
		case 2: { BP = BPX; EP = _EP+x0; nextBPX = EP; nextBPY = BPY; dptr = -C; break; }
		case 3: { BP = BPY; EP = _EP+y0; nextBPX = BPX; nextBPY = EP; dptr = -W * C; break; }
		}

		if (EP < BP) { int tmp = BP; BP = EP; EP = tmp; }
		for (int i = BP; i != EP; i++)
		{
			if (PietUtil::isColor(data + ori, blank, C))
			{
				PietUtil::setColor(data+ori, getPathColor(dir, nullptr), C);
			}
			ori += dptr;
		}

		BPX = nextBPX;
		BPY = nextBPY;

		return;
	}
private:
	static char dir2str(int d)
	{
		switch (d)
		{
		case 0: {return 'E'; break; }
		case 1: {return 'S'; break; }
		case 2: {return 'W'; break; }
		case 3: {return 'N'; break; }
		default: {return '?'; break; }
		}
		return '?';
	}

};
#endif
