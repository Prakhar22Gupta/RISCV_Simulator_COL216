# RISC-V Pipeline Simulator
## Authors

- **Name:** Prakhar Gupta  
  **Entry No.:** 2023CS10493

- **Name:** Ajinkya Dharme  
  **Entry No.:**  2023CS10761

## Design Decisions

1. **Modular Structure**
  - The project is divided into several modules:
    - **Parser Module:**
     - **Machine Code Translator:** Converts machine code into human-readable RISC-V assembly instructions.
     - **Parser:** Tokenizes each instruction into operands, registers, and special characteristics like offsets.
    - **Simulator Module:** Simulates the pipeline execution, integrating all modules and running with or without forwarding, while storing the results.
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
  - We have implemented a dynamic cycle count which decides the number of cycles automatically and runs  the siumaltor for that number of cycles. If there is some infinte loop coming we break the simualtion after 50 iterations of the main loop.
  
5. **Instruction Support**
  - Supports most of the RISC-V instructions given in the refernce along with extra instructions also.

6. **Control Hazard Handling**
  - Control hazards (e.g., branch instructions) are handled simplistically. Control Hazard is resolved by halting the pipeline till ID stage of branch and jump instructions after which we get whether to jump to required destination or not.

7. **Testing**
  - The simulator has been tested on a given inputs as well as edge test cases to handle the simulator gracefully.

## Logic of the code

1. No forwarding design

    The source register values are checked from the previous register values after the WB stage of them till that we get bubbles in the pipeline for the instructions which writes to a register.

2. Forwarding design

    The source register values are checked from the previous register values after the EX stage. Until then, bubbles are introduced in the pipeline for instructions that write to a register.
    For instructions that write to memory, the values are checked from the MEM stage.
    A special case arises with the `sw rs1 imm rs2` instruction, where forwarding is applied to both `rs1` and `rs2`. If the value of `rs1` or `rs2` depends on the previous instruction, it is forwarded to the EX stage, as implemented in the RIPES simulator.


## How to Build and Run

1. **Clone the Repository**
  ```bash
  git clone https://github.com/Prakhar22Gupta/RISCV_Simulator_COL216.git
  cd RISCV_Simulator_COL216
  ```
2. **To run the test cases**
  ```For manually running the code
  make compile
  make run_bypass_forbid
  make run_bypass
  ```
2. **To run the test cases**
  ```bash
  cd src
  ./run_simulation.sh file.txt option
  ```
