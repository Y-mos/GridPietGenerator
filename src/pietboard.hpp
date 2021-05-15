#ifndef PIETBOARD_HPP_
#define PIETBOARD_HPP_

#include "pietutil.hpp"
#include "pietblock.hpp"
#include "pietpath.hpp"
#include "pietjoint.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <queue>

#include <cstring>
#include <limits.h>

class PietBoard
{
private:
	std::map<std::string, PietBlock> blocks;
	std::map<std::string, PietPath> paths;
	std::vector<PietJoint> joints;
	int lastID;
	std::string lastLabel;
	int heights[4];
	int pathSize[2];
	int locs[4];	//	temporary member(not copied)

	class Err_AlreadyRegisteredLabel {};
	class Err_ParentBlockNotFound {};
	class Err_LastLabelNotFound {};
	class Err_PathNotFound {};
	class Err_InvalidBuffer {};

	void init()
	{
		blocks.clear();
		joints.clear();
		paths.clear();

		lastID = 0;
		PietBlock pb_err(getAutomaticLabel());
		blocks[getAutomaticLabel()] = pb_err;

		lastID = 1;
		PietBlock pb_begin(getAutomaticLabel());
		blocks[getAutomaticLabel()] = pb_begin;

		lastLabel = getAutomaticLabel();

		lastID = 2;

		for (int i = 0; i < 2; i++)
		{
			pathSize[i] = 0;
		}
		for (int i = 0; i < 4; i++)
		{
			heights[i] = 0;
		}
		return;
	}
	void copyTo(PietBoard& dst) const
	{
		dst.blocks.clear();
		for (auto itr = blocks.begin(); itr != blocks.end(); itr++)
		{
			dst.blocks[itr->first] = itr->second;
		}
		dst.joints.clear();
		for (auto itr = joints.begin(); itr != joints.end(); itr++)
		{
			dst.joints.push_back(*itr);
		}
		dst.paths.clear();
		for (auto itr = paths.begin(); itr != paths.end(); itr++)
		{
			dst.paths[itr->first]=itr->second;
		}
		dst.lastID = lastID;
		dst.lastLabel = lastLabel;
		for (int i = 0; i < 2; i++)
		{
			dst.pathSize[i] = pathSize[i];
		}
		for (int i = 0; i < 4; i++)
		{
			dst.heights[i] = heights[i];
		}

		return;
	}
	std::string getAutomaticLabel() const
	{
		return ":" + std::to_string(lastID);
	}
public:
	PietBoard()
	{
		init();
	}
	PietBoard(const PietBoard& obj)
	{
		obj.copyTo(*this);
	}
	~PietBoard()
	{

	}

	bool appendCommand(std::string cmd)
	{
		if (blocks.find(lastLabel) == blocks.end())
		{
			throw Err_LastLabelNotFound();
		}
		PietBlock& pb = blocks[lastLabel];
		std::string nextLabel = lastLabel;
		bool isAdded = pb.appendCommand(cmd, nextLabel, lastID);
		if (!isAdded)
		{
			blocks[":0"].appendErrorCommand(cmd);
		}
		else
		{
			if (nextLabel != lastLabel)
			{
				PietBlock npb(nextLabel);
				blocks[nextLabel] = npb;
				lastLabel = nextLabel;
			}

		}
		return true;
	}
	bool clearCommands()
	{
		init();
		return true;
	}

public:
	void put()
	{
		paths.clear();
		joints.clear();
		//	grouping blocks
		int widths[4] = { 0,0,0,0 };
		for (int i = 0; i < 4; i++)
		{
			heights[i] = 0;
		}
		for (int i = 0; i < 2; i++)
		{
			pathSize[i] = 0;
		}

		for (auto itr = blocks.begin(); itr != blocks.end(); itr++)
		{
			PietBlock& pb=itr->second;
			if (pb.getName() == ":0") { continue; }
			pb.resetBuffers();
			pb.setPos(0);
			pb.setDir(3);

			int minw = INT_MAX;
			int mini = 0;

			for (int i = 0; i < 4; i++)
			{
				if (minw > widths[i])
				{
					minw = widths[i];
					mini = i;
				}
			}
			pb.setDir(mini);
			widths[mini] += pb.getW();

		}
		//	rotate to locate ":1" at top-left
		int dirOrigin = blocks[":1"].getDir();
		for (auto itr = blocks.begin(); itr != blocks.end(); itr++)
		{
			PietBlock& pb = itr->second;
			if (pb.getName() == ":0") { continue; }
			int dir = (pb.getDir() - dirOrigin + 3 + 4) % 4;
			pb.setDir(dir);
			heights[dir] = std::max(heights[dir], pb.getH());
		}
		for (int i = 0; i < 4; i++)
		{
			heights[i]++;
		}
		//	locate each block
		for (int i = 0; i < 4; i++)
		{
			locs[i]=0;
		}
		for (auto itr = blocks.begin(); itr != blocks.end(); itr++)
		{
			PietBlock& pb = itr->second;
			if (pb.getName() == ":0") { continue; }
			pb.resetFlag();
		}
		for (auto itr = blocks.begin(); itr != blocks.end(); itr++)
		{
			PietBlock& pb = itr->second;
			if (pb.getName() == ":0") { continue; }
			int dir = pb.getDir();
			pb.setPos(locs[dir]);
			pb.setFlag();

			_putPath(pb.getInPos(), pb.getDir(), ">" + pb.getName());
			if (pb.getOutF() != "")
			{
				_putPath(pb.getOutFPos(), (pb.getDir() + 2) % 4, pb.getName() + "/F>");
			}
			if (pb.getOutT() != "")
			{
				_putPath(pb.getOutTPos(), (pb.getDir() + 2) % 4, pb.getName() + "/T>");
			}

		}
		//	bridge path
		for (auto itr = blocks.begin(); itr != blocks.end(); itr++)
		{
			PietBlock& pb = itr->second;
			std::string name = pb.getName();
			if (name == ":0") { continue; }
			std::string outT = pb.getOutT();
			std::string outF = pb.getOutF();
			if (outT != "")
			{
				_bridgePath(name + "/T>", ">" + outT);
			}
			if (outF != "")
			{
				_bridgePath(name + "/F>", ">" + outF);
			}
		}

		//	calculating pathSize
		for (int i = 0; i < 2; i++)
		{
			pathSize[i] = 0;
		}
		for (auto itr = paths.begin(); itr != paths.end(); itr++)
		{
			PietPath& pp = itr->second;
			int dir = (pp.getDir() + 1) % 2;
			pathSize[dir] = std::max(pathSize[dir], pp.getPos() + 3);
		}
		for (auto itr = blocks.begin(); itr != blocks.end(); itr++)
		{
			PietBlock& pb = itr->second;
			int dir = (pb.getDir() + 1) % 2;
			pathSize[dir] = std::max(pathSize[dir], pb.getPos() + pb.getW()+3);
		}

		//	add joints
		for (auto itr = blocks.begin(); itr != blocks.end(); itr++)
		{
			PietBlock& pb = itr->second;
			std::string name = pb.getName();
			if (name == ":0") { continue; }
			
			std::string outs[2] = { pb.getOutT(), pb.getOutF() };
			std::string suffixes[2] = { "/T>","/F>" };
			for (int i = 0; i < 2; i++)
			{
				std::string out = outs[i];
				//std::string out = pb.getOutT();
				if (out != "")
				{
					std::string prevName = "";
					std::string curName = name + suffixes[i];

					while (curName != "")
					{
						if (paths.find(curName) == paths.end())
						{
							throw Err_PathNotFound();
						}
						PietPath& curpp = paths[curName];
						if (prevName != "")
						{
							PietPath& prevpp = paths[prevName];
							int x = ((prevpp.getDir() % 2 == 0) ? curpp.getPos() : prevpp.getPos());
							int y = ((prevpp.getDir() % 2 != 0) ? curpp.getPos() : prevpp.getPos());
							PietJoint pj(x, y, prevpp.getDir(), curpp.getDir(), prevName, curName);
							joints.push_back(pj);
						}
						prevName = curName;
						curName = curpp.getTo();
					}

				}

			}

		}


	}
private:
	bool _bridgePath(std::string src, std::string dst)
	{
		if (paths.find(src) == paths.end())
		{
			throw Err_PathNotFound();
		}
		PietPath& ppfrom = paths[src];
		if (paths.find(dst) == paths.end())
		{
			throw Err_PathNotFound();
		}
		PietPath& ppto = paths[dst];

		if ((ppfrom.getDir() + 4 - ppto.getDir()) % 2 == 0)
		{
			int dir = (ppfrom.getDir() + 1) % 2 + (ppfrom.getPos() >= ppto.getPos()) * 2;
			int pos= (ppfrom.getPos() + ppto.getPos()) / 2;	//	tekito
			_putPath(pos, dir, src + dst, dst);
			ppfrom.setTo(src + dst);
		}
		else
		{
			ppfrom.setTo(dst);
		}
		return true;
	}
	bool _putPath(int pos, int dir, std::string name, std::string to = "")
	{
		PietPath newpp(pos,dir,name,to);
		const int P = 5;
		int minpos = INT_MAX;
		int maxpos = -1;
		for (auto itr = paths.begin(); itr != paths.end(); itr++)
		{
			PietPath& pp = itr->second;
			if ((pp.getDir() - newpp.getDir()) % 2 != 0) { continue; }
			if (newpp.getPos() <= pp.getPos() && pp.getPos() < minpos)
			{
				minpos = pp.getPos();
			}
			if (pp.getPos() < newpp.getPos() && maxpos < pp.getPos())
			{
				maxpos = pp.getPos();
			}
		}
		int D1 = ((minpos == INT_MAX) ? 0 : std::max(0, P - (minpos - newpp.getPos())));
		int D2 = ((maxpos < 0) ? 0 : std::max(0, P - (newpp.getPos() - maxpos)));
		for (auto itr = paths.begin(); itr != paths.end(); itr++)
		{
			PietPath& pp = itr->second;
			if ((pp.getDir() - newpp.getDir()) % 2 != 0) { continue; }
			if (minpos <= pp.getPos())
			{
				pp.setPos(pp.getPos() + D1 + D2);
			}
		}
		newpp.setPos(newpp.getPos() + D2);
		paths[name] = newpp;

		checkBlockBuffers();
		updateLocs();
		
		return true;
	}
	void updateLocs()
	{
		for (int i = 0; i < 4; i++)
		{
			locs[i] = 0;
		}
		for (auto itr = blocks.begin(); itr != blocks.end(); itr++)
		{
			PietBlock& pb = itr->second;
			if (pb.getName() == ":0") { continue; }
			if (pb.getFlag() == false) { continue; }
			locs[pb.getDir()] = std::max(locs[pb.getDir()],pb.getPos()+pb.getW());
		}
	}
	void checkBlockBuffers()
	{
		for (auto itr = blocks.begin(); itr != blocks.end(); itr++)
		{
			PietBlock& pb = itr->second;
			if (pb.getName() == ":0") { continue; }
			if (pb.getFlag() == false) { continue; }
			std::string name = pb.getName();
			std::string outT = pb.getOutT();
			std::string outF = pb.getOutF();

			if (paths.find(">" + name) == paths.end())
			{
				continue;
			}
			int pos = paths[">" + name].getPos();
			int posOutT = -1;
			if (outT != "" && paths.find(name + "/T>") != paths.end())
			{
				posOutT=paths[name + "/T>"].getPos();
			}
			int posOutF = -1;
			if (outF != "" && paths.find(name + "/F>") != paths.end())
			{
				posOutF = paths[name + "/F>"].getPos();
			}
			if (!pb.calcBuffer(pos, posOutT, posOutF))
			{
				throw Err_InvalidBuffer();
			}

		}
		return;
	}
public:
	std::string str(std::string pad = "") const
	{
		std::stringstream ss;
		ss << pad << "*PietBoard [" << static_cast<const void*>(this) << std::endl;
		ss << pad << " - lastID = " << lastID << std::endl;
		for (auto itr = blocks.begin(); itr != blocks.end(); itr++)
		{
			ss << pad << itr->second.str(" ") << std::endl;
		}
		for (auto itr = joints.begin(); itr != joints.end(); itr++)
		{
			ss << pad << itr->str(" ") << std::endl;
		}
		for (auto itr = paths.begin(); itr != paths.end(); itr++)
		{
			ss << pad << itr->second.str(" ") << std::endl;
		}
		return ss.str();
	}


	template <typename T, int C, T BLANK>
	void draw(T*& data, int& W, int& H, const T* (*getPietColor)(int h, int b)) const
	{
		W = heights[2] + pathSize[0] + heights[0];
		H = heights[3] + pathSize[1] + heights[1];
		data = new T[W*H*C];
		memset(data, BLANK, W*H*C);
		for (auto itr = blocks.begin(); itr != blocks.end(); itr++)
		{
			if (itr->first == ":0") { continue; }
			itr->second.draw(data, W, H, C, heights[2], heights[3], BLANK, getPietColor);
		}
		for (auto itr = joints.begin(); itr != joints.end(); itr++)
		{
			itr->draw(data, W, H, C, heights[2], heights[3], BLANK, getPietColor);
		}
	}


};
#endif