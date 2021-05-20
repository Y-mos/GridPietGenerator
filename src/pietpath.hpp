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
