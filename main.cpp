#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

/*
each line is an instruction

<int>
<int>
<int>
...

instructions:
set <num>           sets num in memory

inc <tt>            [tt] += 1
add <tt> <ss>       [tt] += [ss]
sub <tt> <ss>       [tt] = max([tt] - [ss], 0)
cpy <tt> <ss>       [tt] = [ss]
inz <tt> <ss>       [tt] += 1 if [ss] == 0
get <tt> <ss>       [tt] = [[ss]]
sto <tt> <ss>       [[tt]] = [ss]
out <ss>            output [ss]
inp <tt>            read [tt]
hlt                 halt

lbl <str>           label next address
jmp <tt>            jump to tt by setting instr ptr
jmz <tt> <ss>       jump to tt if [ss] == 0
skp <ss>            skip the next instr if [ss] == 0

*/

struct LAddr { int addr; };
struct LLabel { std::string name; };

using Location = std::variant<LAddr, LLabel>;

struct IDef { Location val; };

struct IInc { Location tt; };
struct IAdd { Location tt, ss; };
struct ISub { Location tt, ss; };
struct ICpy { Location tt, ss; };
struct IInz { Location tt, ss; };
struct IGet { Location tt, ss; };
struct ISto { Location tt, ss; };
struct IOut { Location ss; };
struct IInp { Location tt; };
struct IHlt { };

struct ILbl { std::string name; };
struct IJmp { Location tt; std::string mem; };
struct IJmz { Location tt, ss; std::string mem; };
struct ISkp { Location ss; };

using Instruction = std::variant<IDef, IInc, IAdd, ISub, ICpy, IInz, IGet, ISto, IOut, IInp, IHlt, ILbl, IJmp, IJmz, ISkp>;

std::vector<Instruction> instrs;
std::map<std::string, int> labelMap;

inline Location parseLoc(std::string str) {
    if (str[0] == '@')
        return LLabel { str };
    else
        return LAddr { std::stoi(str) };
}

inline std::string fixAddr(int addr) {
    std::string str = std::to_string(addr);
    return (str.length() < 2 ? "0" + str : str);
}

std::string toAddr(Location loc) {
    if (LAddr* laddr = std::get_if<LAddr>(&loc)) {
        return fixAddr(laddr->addr);
    } else if (LLabel* llabel = std::get_if<LLabel>(&loc)) {
        return fixAddr(labelMap[llabel->name]);
    }
    return "XX";
}

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 3) return 0;
    std::string inputPath = argv[1], outputPath = argv[2];

    std::ifstream input(inputPath);
    std::ofstream output(outputPath);

    std::string lnbuf, spbuf;
    while (std::getline(input, lnbuf)) {
        if (lnbuf.empty()) continue;

        std::stringstream lnstream(lnbuf);
        std::vector<std::string> splits;

        while (std::getline(lnstream, spbuf, ' '))
            splits.push_back(spbuf);

        std::string fst = splits[0];

        if (fst == "def") {
            instrs.push_back(IDef { parseLoc(splits[1]) });
        } else if (fst == "inc") {
            instrs.push_back(IInc { parseLoc(splits[1]) });
        } else if (fst == "add") {
            instrs.push_back(IAdd { parseLoc(splits[1]), parseLoc(splits[2]) });
        } else if (fst == "sub") {
            instrs.push_back(ISub { parseLoc(splits[1]), parseLoc(splits[2] )});
        } else if (fst == "cpy") {
            instrs.push_back(ISub { parseLoc(splits[1]), parseLoc(splits[2] )});
        } else if (fst == "inz") {
            instrs.push_back(IInz { parseLoc(splits[1]), parseLoc(splits[2] )});
        } else if (fst == "get") {
            instrs.push_back(IGet { parseLoc(splits[1]), parseLoc(splits[2] )});
        } else if (fst == "sto") {
            instrs.push_back(ISto { parseLoc(splits[1]), parseLoc(splits[2] )});
        } else if (fst == "out") {
            instrs.push_back(IOut { parseLoc(splits[1]) });
        } else if (fst == "inp") {
            instrs.push_back(IInp { parseLoc(splits[1]) });
        } else if (fst == "hlt") {
            instrs.push_back(IHlt { });
        } else if (fst == "jmp") {
            instrs.push_back(IJmp { parseLoc(splits[1]) });
        } else if (fst == "jmz") {
            instrs.push_back(IJmz { parseLoc(splits[1]) });
        } else if (fst == "skp") {
            instrs.push_back(ISkp { parseLoc(splits[1]) });
        } else if (fst == "lbl") {
            if (splits[1][0] != '@') std::cout << "ERROR: Labels should start with '@'" << std::endl;
            instrs.push_back(ILbl { splits[1] });
        }
    }

    int curAddr = 0;
    for (Instruction& instr : instrs) {
        if (IJmp* jmp = std::get_if<IJmp>(&instr)) {
            jmp->mem = fixAddr(curAddr + 1);
            curAddr += 3;
        } else if (IJmz* jmz = std::get_if<IJmz>(&instr)) {
            jmz->mem = fixAddr(curAddr + 1);
            curAddr += 5;
        }

        if (ILbl* lbl = std::get_if<ILbl>(&instr)) {
            labelMap[lbl->name] = curAddr;
        } else {
            curAddr++;
        }
    }

    for (Instruction i : instrs) {
        if (IDef* def = std::get_if<IDef>(&i)) {
            output << toAddr(def->val) << '\n';
        } else if (IInc* ins = std::get_if<IInc>(&i)) {
            output << "0" << toAddr(ins->tt) << "00\n";
        } else if (IAdd* ins = std::get_if<IAdd>(&i)) {
            output << "1" << toAddr(ins->tt) << toAddr(ins->ss) << '\n';
        } else if (ISub* ins = std::get_if<ISub>(&i)) {
            output << "2" << toAddr(ins->tt) << toAddr(ins->ss) << '\n';
        } else if (ICpy* ins = std::get_if<ICpy>(&i)) {
            output << "3" << toAddr(ins->tt) << toAddr(ins->ss) << '\n';
        } else if (IInz* ins = std::get_if<IInz>(&i)) {
            output << "4" << toAddr(ins->tt) << toAddr(ins->ss) << '\n';
        } else if (IGet* ins = std::get_if<IGet>(&i)) {
            output << "5" << toAddr(ins->tt) << toAddr(ins->ss) << '\n';
        } else if (ISto* ins = std::get_if<ISto>(&i)) {
            output << "6" << toAddr(ins->tt) << toAddr(ins->ss) << '\n';
        } else if (IOut* ins = std::get_if<IOut>(&i)) {
            output << "700" << toAddr(ins->ss) << '\n';
        } else if (IInp* ins = std::get_if<IInp>(&i)) {
            output << "8" << toAddr(ins->tt) << "00\n";
        } else if (IHlt* ins = std::get_if<IHlt>(&i)) {
            output << "90000\n";
        } else if (IJmp* ins = std::get_if<IJmp>(&i)) {
            output << "00000\n";
            output << toAddr(ins->tt) << '\n';
            output << "300" << ins->mem << '\n';
        } else if (IJmz* ins = std::get_if<IJmz>(&i)) {
            output << "00000\n";
            output << "400" << toAddr(ins->ss) << '\n';
            output << "00000\n";
            output << "300" << ins->mem << '\n';
        } else if (ISkp* ins = std::get_if<ISkp>(&i)) {
            output << "400" << toAddr(ins->ss) << '\n';
        }
    }
    
    input.close();
    output.close();

    return 0;
}