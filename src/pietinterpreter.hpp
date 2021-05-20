#ifndef PIETINTERPRETER_HPP_
#define PIETINTERPRETER_HPP_

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "pietutil.hpp"

class PietInterpreter
{
private:
	const size_t STACK_SIZE = 30000;	///	stack size
	int* stack;	///	stack
	int ptr;	///	position where the next value will be pushed
	int ppc;		///	program counter
	int pc;		///	program counter
	int cnt;	///	number of operations
	bool isRunning;	///	flag (program is running or not)
	std::vector<std::string> cmds;	/// commands
	std::vector<std::string> lines;	/// commands
	std::stringstream output; ///	output


	class Err_LabelNotFound {};
	class Err_StackOverflow {};
	class Err_CommandNotFound {};

	void init()
	{
		stack = nullptr;
		stack = new int[STACK_SIZE];
		ptr = 0;
		isRunning = false;
		cmds.clear();
		lines.clear();
		ppc = -1;
		pc = 0;
		cnt = 0;
		output.str("");
		output.clear(std::stringstream::goodbit);
	}

	void copyTo(PietInterpreter& dst) const
	{
		for (int i = 0; i < ptr; i++)
		{
			dst.stack[i] = stack[i];
		}
		dst.ptr = ptr;

		dst.cmds.clear();
		for (auto itr = cmds.begin(); itr != cmds.end(); itr++)
		{
			dst.cmds.push_back(*itr);
		}
		dst.lines.clear();
		for (auto itr = lines.begin(); itr != lines.end(); itr++)
		{
			dst.lines.push_back(*itr);
		}

		dst.ppc = -1;
		dst.pc = 0;
		dst.cnt = 0;
		dst.isRunning = false;
		dst.output.str("");
		dst.output.clear(std::stringstream::goodbit);
	}
public:
	PietInterpreter()
	{
		init();
	}
	PietInterpreter(const PietInterpreter& obj)
	{
		init();
		obj.copyTo(*this);
	}
	~PietInterpreter()
	{
		if (stack != nullptr)
		{
			delete[] stack;
		}
	}

	size_t appendCommand(std::string cmd, std::string line="")
	{
		isRunning = false;
		cmds.push_back(cmd);
		lines.push_back(line);
		return cmds.size();
	}
	size_t clearCommands()
	{
		isRunning = false;
		cmds.clear();
		lines.clear();
		return cmds.size();
	}
	bool step()
	{
		if (!isRunning)
		{
			isRunning = true;
			ptr = 0;
			ppc = -1;
			pc = 0;
			cnt = 0;
			output.str("");
			output.clear(std::stringstream::goodbit);
		}

		std::string cmd = cmds[pc];
		ppc = pc;
		if (PietUtil::isLabel(cmd))
		{
			pc++;
		}
		else if (PietUtil::isCommand(cmd))
		{
			int arg;
			std::string outT;
			std::string outF;
			if (PietUtil::isEndCommand(cmd))
			{
				isRunning = false;
			}
			else if (PietUtil::isGotoCommand(cmd, &outT))
			{
				pc = -1;
				for (size_t i = 0; i < cmds.size(); i++)
				{
					if (cmds[i] == outT)
					{
						pc = i;
						break;
					}
				}
				if (pc==-1)
				{
					throw Err_LabelNotFound();
				}
			}
			else if (PietUtil::isIfCommand(cmd, &outT, &outF))
			{
				ptr--;
				int v = stack[ptr];
				std::string nlabel;
				nlabel = ((v == 0) ? (outF) : (outT));
				if (nlabel == ":")
				{
					pc++;
				}
				else
				{
					pc = -1;
					for (size_t i = 0; i < cmds.size(); i++)
					{
						if (cmds[i] == nlabel)
						{
							pc = i;
							break;
						}
					}
					if (pc == -1)
					{
						throw Err_LabelNotFound();
					}

				}
			}
			else if (PietUtil::isPushCommand(cmd, &arg))
			{
				stack[ptr] = arg;
				ptr++;
				pc++;
			}
			else
			{
				if (cmd == "pop")
				{
					if (ptr > 0)
					{
						ptr--;
					}
				}
				else if (cmd == "add")
				{
					if (ptr > 1)
					{
						int v1 = stack[ptr - 1];
						int v2 = stack[ptr - 2];
						stack[ptr - 2] = v2 + v1;
						ptr--;
					}
				}
				else if (cmd == "sub")
				{
					if (ptr > 1)
					{
						int v1 = stack[ptr - 1];
						int v2 = stack[ptr - 2];
						stack[ptr - 2] = v2 - v1;
						ptr--;
					}
				}
				else if (cmd == "mul")
				{
					if (ptr > 1)
					{
						int v1 = stack[ptr - 1];
						int v2 = stack[ptr - 2];
						stack[ptr - 2] = v2 * v1;
						ptr--;
					}
				}
				else if (cmd == "div")
				{
					if (ptr > 1)
					{
						int v1 = stack[ptr - 1];
						int v2 = stack[ptr - 2];
						if (v1 != 0)
						{
							stack[ptr - 2] = v2 / v1;
							ptr--;
						}
					}
				}
				else if (cmd == "mod")
				{
					if (ptr > 1)
					{
						int v1 = stack[ptr - 1];
						int v2 = stack[ptr - 2];
						if (v1 != 0)
						{
							stack[ptr - 2] = v2 - v1 * std::floor(static_cast<double>(v2) / static_cast<double>(v1));
							ptr--;
						}
					}
				}
				else if (cmd == "not")
				{
					stack[ptr - 1] = ((stack[ptr - 1] == 0) ? (1) : (0));
				}
				else if (cmd == "gt")
				{
					if (ptr > 1)
					{
						int v1 = stack[ptr - 1];
						int v2 = stack[ptr - 2];
						stack[ptr - 2] = ((v2 > v1) ? (1) : (0));
						ptr--;
					}
				}
				else if (cmd == "dup")
				{
					if (ptr == STACK_SIZE)
					{
						throw Err_StackOverflow();
					}
					stack[ptr] = stack[ptr - 1];
					ptr++;
				}
				else if (cmd == "roll")
				{
					int n = stack[ptr - 1];
					int d = stack[ptr - 2];
					if (d > 0)
					{
						if (d <= ptr - 2)
						{
							ptr--;
							ptr--;
							while (n < 0) { n += d; }
							for (int j = 0; j < n; j++)
							{
								int tmp = stack[ptr - 1];
								for (int i = 1; i < d; i++)
								{
									stack[ptr - i] = stack[ptr - i - 1];
								}
								stack[ptr - d] = tmp;
							}
						}
					}
				}
				else if (cmd == "inc")
				{
					if (ptr == STACK_SIZE)
					{
						throw Err_StackOverflow();
					}
					char v;
					std::cin >> v;
					stack[ptr] = static_cast<int>(v);
					ptr++;
				}
				else if (cmd == "inn")
				{
					if (ptr == STACK_SIZE)
					{
						throw Err_StackOverflow();
					}
					int v;
					std::cin >> v;
					stack[ptr] = v;
					ptr++;
				}
				else if (cmd == "outc")
				{
					if (ptr > 0)
					{
						char v = static_cast<char>(stack[ptr - 1]);
						ptr--;
						output << v;
					}
				}
				else if (cmd == "outn")
				{
					if (ptr > 0)
					{
						int v = stack[ptr - 1];
						ptr--;
						output << v;
					}
				}
				else
				{
					throw Err_CommandNotFound();
				}
				pc++;
			}
		}
		cnt++;

		return isRunning;
	}

	std::string str(std::string pad="") const
	{
		std::stringstream ss;
		ss << pad << "===PietInterpreter [" << static_cast<const void*>(this) << "]" << std::endl;
		const int ss2d = 32;	//	stack size to display 
		const int sslb = 16;	//	stack size between line breaks
		ss << pad << "cnt :" << cnt << std::endl;
		ss << pad << "pc  :" << pc << std::endl;
		if (ppc>=0)
		{
			ss << pad << "cur : " << cmds[ppc] << ((lines[ppc] != "") ? (" @ l." + lines[ppc]) : ("")) << std::endl;
		}
		if (0 <= pc && pc < cmds.size())
		{
			ss << pad << "next: " << cmds[pc] << ((lines[pc] != "") ? (" @ l." + lines[pc]) : ("")) << std::endl;
		}
		ss << pad << "stack: ";
		for (int p = std::max(0, ptr - ss2d); p < ptr; p++)
		{
			ss << pad << "[" << std::setfill('0') << std::setw(5) << p << " : ";
			ss << std::setfill(' ') << std::setw(5) << stack[p] << "] ";
			if ((p + 1) % sslb == 0) { ss << std::endl << pad << "      "; }
		}
		ss << std::endl;
		ss << pad << "output: " << std::endl << output.str() << std::endl;
		return ss.str();
	}

	std::string str_commands(std::string pad = "") const
	{
		std::stringstream ss;
		ss << pad << "===PietInterpreter - Commands [" << static_cast<const void*>(this) << "] " << std::endl;
		for (size_t i = 0; i < cmds.size(); i++)
		{
			ss << pad << lines[i] << " | " << cmds[i] << std::endl;
		}
		return ss.str();
	}


};
#endif
