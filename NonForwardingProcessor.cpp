#include "NonForwardingProcessor.hpp"
#include <iostream>
#include <sstream>
using namespace std;

NonForwardingProcessor::InstructionInfo
NonForwardingProcessor::buildInfo(uint32_t code, const string &asmStr) {
    InstructionInfo info;
    info.code = code;
    info.asmStr = asmStr;

    uint32_t opcode = code & 0x7F;
    uint32_t rd     = (code >> 7) & 0x1F;
    uint32_t rs1    = (code >> 15) & 0x1F;
    uint32_t rs2    = (code >> 20) & 0x1F;

    // For demonstration, treat only addi or add as writing a register.
    // If rd=0 => treat as no write (rd=-1).
    info.rd = (rd == 0) ? -1 : rd;

    // If instruction reads registers, store them in src.
    info.src.clear();

    // Very basic logic: addi => read rs1
    //                  add  => read rs1, rs2
    if(opcode == 0x13) { // addi
        if(rs1 != 0) info.src.push_back(rs1);
    } else if(opcode == 0x33) { // add/sub
        if(rs1 != 0) info.src.push_back(rs1);
        if(rs2 != 0) info.src.push_back(rs2);
    }
    return info;
}

// Minimal ALU for demonstration
int NonForwardingProcessor::executeALU(const InstructionInfo &info) {
    // This can be more detailed. We'll just return 0 for simplicity.
    return 0;
}

// Hazard detection for no-forwarding pipeline:
// The ID-stage instruction must wait until the producing instruction
// has completed WB.
bool NonForwardingProcessor::hazardDetected(const PipelineState &state,
                                            const InstructionInfo &idInstr)
{
    // We check EX, MEM, WB. If they have a valid instruction that writes
    // a register 'd' and 'd' is in ID's src => hazard => stall.
    auto checkStage = [&](const PipelineReg &reg) {
        if(!reg.valid || reg.bubble) return false;
        int idx = reg.instrIndex;
        int d = instructionsInfo[idx].rd;
        if(d <= 0) return false; // no hazard if rd = -1 or 0
        // if the ID instr reads 'd'
        for(int s : idInstr.src) {
            if(s == d) {
                return true;
            }
        }
        return false;
    };
    // In a no-forwarding pipeline, we must stall as long as the producing
    // instruction is in EX, MEM, or WB. Because the updated register
    // is only available after WB completes.
    if(checkStage(state.EX))  return true;
    if(checkStage(state.MEM)) return true;
    if(checkStage(state.WB))  return true;

    return false;
}

void NonForwardingProcessor::runSimulation(int simCycles) {
    // 1) decode all instructions so that instruction_storage[i] is a "nice" string
    decodeAllInstructions();

    // 2) build instructionsInfo
    instructionsInfo.clear();
    instructionsInfo.resize(machine_code_storage.size());
    for(size_t i=0; i<machine_code_storage.size(); i++) {
        instructionsInfo[i] = buildInfo(machine_code_storage[i], instruction_storage[i]);
    }

    // 3) pipelineDiagram: each instruction gets its own row
    pipelineDiagram.clear();
    pipelineDiagram.resize(instructionsInfo.size());

    // 4) create pipeline state
    PipelineState current, next;
    int PC = 0;  // next instruction index to fetch

    // Preload the first instruction into IF so cycle1 shows "IF"
    if(PC < (int)instructionsInfo.size()) {
        current.IF.valid = true;
        current.IF.bubble = false;
        current.IF.instrIndex = PC;
        current.IF.firstCycle = false;
        PC++;
    }

    // 5) cycle loop
    for(int cycle=1; cycle <= simCycles; cycle++) {
        // reset next state
        next = PipelineState();

        // WB
        if(current.WB.valid && !current.WB.bubble) {
            int idx = current.WB.instrIndex;
            if((int)pipelineDiagram[idx].size() < cycle)
                pipelineDiagram[idx].resize(cycle);
            pipelineDiagram[idx][cycle-1] = "WB";

            // write back
            int rd = instructionsInfo[idx].rd;
            if(rd > 0) {
                registers[rd] = current.WB.aluResult;
            }
        }

        // MEM -> WB
        if(current.MEM.valid && !current.MEM.bubble) {
            int idx = current.MEM.instrIndex;
            if((int)pipelineDiagram[idx].size() < cycle)
                pipelineDiagram[idx].resize(cycle);
            pipelineDiagram[idx][cycle-1] = "MEM";
            next.WB = current.MEM;
        }

        // EX -> MEM
        if(current.EX.valid && !current.EX.bubble) {
            int idx = current.EX.instrIndex;
            if((int)pipelineDiagram[idx].size() < cycle)
                pipelineDiagram[idx].resize(cycle);
            pipelineDiagram[idx][cycle-1] = "EX";
            // do ALU
            current.EX.aluResult = executeALU(instructionsInfo[idx]);
            next.MEM = current.EX;
        }

        // ID -> EX (or stall)
        bool stall = false;
        if(current.ID.valid && !current.ID.bubble) {
            int idx = current.ID.instrIndex;
            if((int)pipelineDiagram[idx].size() < cycle)
                pipelineDiagram[idx].resize(cycle);

            // If it's the first cycle in ID, print "ID" and remain in ID for next cycle
            // (so that we can check hazard in the next cycle).
            if(current.ID.firstCycle) {
                pipelineDiagram[idx][cycle-1] = "ID";
                // remain in ID, but not firstCycle anymore
                PipelineReg hold = current.ID;
                hold.firstCycle = false;
                next.ID = hold;
            } else {
                // subsequent cycles in ID => check hazard
                if(hazardDetected(current, instructionsInfo[idx])) {
                    // stall => print "-"
                    pipelineDiagram[idx][cycle-1] = "-";
                    next.ID = current.ID; // remain in ID
                    stall = true;
                } else {
                    // no hazard => move to EX
                    // We do NOT re-print "ID" a second time
                    next.EX = current.ID;
                }
            }
        }

        // IF -> ID
        bool flushIF = false; // for branches, if needed
        if(!stall) {
            // if IF has a valid instruction
            if(current.IF.valid && !current.IF.bubble) {
                int idx = current.IF.instrIndex;
                if((int)pipelineDiagram[idx].size() < cycle)
                    pipelineDiagram[idx].resize(cycle);
                pipelineDiagram[idx][cycle-1] = "IF";

                // move it to ID (as firstCycle)
                PipelineReg newID = current.IF;
                newID.firstCycle = true;
                next.ID = newID;
            }

            // if not flushing, fetch the next instruction
            if(!flushIF) {
                if(PC < (int)instructionsInfo.size()) {
                    PipelineReg newIF;
                    newIF.valid = true;
                    newIF.bubble = false;
                    newIF.instrIndex = PC;
                    newIF.firstCycle = false;
                    PC++;
                    next.IF = newIF;
                }
            } else {
                // flush => bubble
                PipelineReg bubble;
                bubble.valid = true;
                bubble.bubble = true;
                next.IF = bubble;
            }
        } else {
            // stall => hold IF as is
            next.IF = current.IF;
        }

        // update
        current = next;
    }

    // 6) print pipeline diagram
    for(size_t i=0; i<pipelineDiagram.size(); i++) {
        if(pipelineDiagram[i].empty()) continue; // never fetched
        cout << instructionsInfo[i].asmStr;  // the textual instruction
        int last = pipelineDiagram[i].size();
        for(int c=0; c<last; c++) {
            // if empty, print ""
            string cell = pipelineDiagram[i][c];
            cout << ";" << cell;
        }
        cout << endl;
    }
}
