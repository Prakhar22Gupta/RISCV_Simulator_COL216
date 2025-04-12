# RISC-V Complete Pipeline Simulator
## Authors

- **Name:** Prakhar Gupta  
  **Entry No.:** 2023CS10493

- **Name:** Ajinkya Dharme  
  **Entry No.:**  2023CS10761

## Design Decisions

1. **Modular Structure**
  - The project is divided into several modules:
     - **Machine Code Translator:** Converts machine code into human-readable RISC-V assembly instructions.
     - **Parser:** Tokenizes each instruction into operands, registers, and special characteristics like offsets.
    - **Simulator Module:** Simulates the pipeline execution, integrating all modules and running with or without forwarding while storing the results.
    - **Register File:** A custom data structure implemented for efficient register management.

2. **Two Pipeline Variants**
  - **No Forwarding:** Detects RAW hazards and inserts stalls (displayed as "*") to maintain correct data dependencies.
  - **With Forwarding:** Implements bypassing to forward results from later pipeline stages (EX/MEM) directly to earlier stages, reducing stalls and improving throughput.

3. **Instruction Input & Output**
  - The executable accepts the following command-line arguments:
    - A RISC-V assembly machine code file (e.g., `inputfiles/strlen.txt`).
    - The number of cycles to simulate.
    - A flag to enable or disable forwarding.
  - The pipeline diagram is output to `outputfiles/strlen_no_forward.txt` in a human-readable table format, including parsed instructions.

4. **Cycle count**
  - We have implemented a dynamic cycle count, which automatically decides the number of cycles and runs the simulator for that number of cycles. If some infinite loop occurs, we break the simulation after 50 main loop iterations.
  
5. **Instruction Support**
  - Supports most of the RISC-V instructions given in the reference, along with extra instructions.

6. **Control Hazard Handling**
  - Control hazards (e.g., branch instructions) are handled simplistically. Control Hazard is resolved by halting the pipeline till the ID stage of the branch and jump instructions, after which we get whether to jump to the required destination.

7. **Testing**
  - The simulator has been tested on given inputs and edge test cases to handle the simulator gracefully.

## Logic of the code

1. No forwarding design

    The source register values are checked from the previous register values after the WB stage until we get bubbles in the pipeline for the instructions which writes to a register.

2. Forwarding design

    After the EX stage, the source register values are checked from the previous register values. Until then, bubbles are introduced in the pipeline for instructions that are written to a register. For instructions that write to memory, the values are checked from the MEM stage. A special case arises with the `sw rs1 imm rs2` instruction, where forwarding is applied to both `rs1` and `rs2`. If the value of `rs1` or `rs2` depends on the previous instruction, it is forwarded to the EX stage, as implemented in the RIPES simulator.

## How to Build and Run

1. **Clone the Repository**
  ```bash
  git clone https://github.com/Prakhar22Gupta/RISCV_Simulator_COL216.git
  cd RISCV_Simulator_COL216
  ```
2. **Manual testing**
  ```bash
  make compile
  make run_bypass_forbid
  make run_bypass
  ```
3. **To run the test cases**
  ```bash
  cd src
  ./run_simulation.sh file.txt option
  ```
## Reproducing this assignment
   If this code is one of your assignments, I strongly recommend you try it out yourself first. Otherwise, feel free to use it if you want to expand the simulator or for any other purpose.
