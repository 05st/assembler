#include <fstream>
#include <iostream>
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

struct ISet { int val; };

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
struct IJmp { Location tt; };
struct IJmz { Location tt, ss; };
struct ISkp { Location ss; };

using Instruction = std::variant<ISet, IInc, IAdd, ISub, ICpy, IInz, IGet, ISto, IOut, IInp, IHlt, ILbl, IJmp, IJmz, ISkp>;

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 3) return 0;
    std::string inputPath = argv[1], outputPath = argv[2];

    std::ifstream input(inputPath);
    std::ofstream output(outputPath);

    std::vector<Instruction> instrs;

    std::string lnbuf, spbuf;
    while (std::getline(input, lnbuf)) {
        if (lnbuf.empty()) continue;

        std::stringstream lnstream(lnbuf);
        std::vector<std::string> splits;

        while (std::getline(lnstream, spbuf, ' '))
            splits.push_back(spbuf);

        std::string fst = splits[0];

        if (fst == "set") {
            instrs.push_back(ISet { 0 });
        } else if (fst == "inc") {
        } else if (fst == "add") {
        } else if (fst == "sub") {
        } else if (fst == "cpy") {
        } else if (fst == "inz") {
        } else if (fst == "get") {
        } else if (fst == "sto") {
        } else if (fst == "out") {
        } else if (fst == "inp") {
        } else if (fst == "hlt") {
        } else if (fst == "jmp") {
        } else if (fst == "jmz") {
        } else if (fst == "skp") {
        }
    }
    
    output << sets.size() + 1 << '\n';

    for (std::string s : sets)
        output << s << std::endl;

    for (std::string s : outs)
        output << s << std::endl;

    input.close();
    output.close();

    return 0;
}