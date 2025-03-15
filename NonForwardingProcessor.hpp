#ifndef NOFORWARDINGPROCESSOR_HPP
#define NOFORWARDINGPROCESSOR_HPP

#include "Processor.hpp"
#include <vector>
#include <string>

class NonForwardingProcessor : public Processor {
public:
    NonForwardingProcessor() : Processor() {}
    void runSimulation(int simCycles);

private:
    // Extra info for each instruction
    struct InstructionInfo {
        uint32_t code;
        std::string asmStr;
        int rd;
        std::vector<int> src;
    };

    // Pipeline register
    struct PipelineReg {
        bool valid;
        bool bubble;      // flush or stall bubble
        int instrIndex;   // index in instructionsInfo
        int aluResult;
        bool firstCycle;  // true => first cycle in ID

        PipelineReg()
            : valid(false), bubble(false), instrIndex(-1),
              aluResult(0), firstCycle(false) {}
    };

    // The 5 pipeline stages
    struct PipelineState {
        PipelineReg IF;
        PipelineReg ID;
        PipelineReg EX;
        PipelineReg MEM;
        PipelineReg WB;
    };

    // For each instruction, we store a list of stage labels (one per cycle).
    // pipelineDiagram[i][c] => label in cycle (c+1).
    std::vector<std::vector<std::string>> pipelineDiagram;

    // The final set of instructions used for pipeline simulation
    std::vector<InstructionInfo> instructionsInfo;

private:
    InstructionInfo buildInfo(uint32_t code, const std::string &asmStr);
    int executeALU(const InstructionInfo &info);
    bool hazardDetected(const PipelineState &state, const InstructionInfo &idInstr);
};

#endif
