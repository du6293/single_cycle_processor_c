#include "rv32i.h"
/* instruction memory */
Imem_in imem_in;
Imem_out imem_out;
/* register */
Reg_in reg_in;
Reg_out reg_out;
/* alu */
Alu_in alu_in;
Alu_out alu_out;
/* data memory */
Dmem_in dmem_in;
Dmem_out dmem_out;

Imem_out imem ( Imem_in imem_in, uint32_t *imem_data){
	Imem_out result;
	result.inst = imem_in.imem_data[imem_in.imem_addr];
	return result;
}

Reg_out regfile( Reg_in reg_in, uint32_t *reg_data){
	Reg_out result;
	if (reg_in.reg_write ) reg_in.reg_data[reg_in.rd] = reg_in.rd_din;  // load
	
	result.rs1_dout =  (reg_in.rs1 != 0) ? reg_in.reg_data[reg_in.rs1] : 0;
	result.rs2_dout =  (reg_in.rs2!= 0 ) ? reg_in.reg_data[reg_in.rs2] : 0;  

	return result;
}


Alu_out alu(Alu_in alu_in){
	Alu_out result;
	if		(alu_in.alu_control == 0) result.alu_result = alu_in.alu_in1 & alu_in.alu_in2;  // and/
	else if (alu_in.alu_control == 1) result.alu_result = alu_in.alu_in1 | alu_in.alu_in2;  // or/
	else if (alu_in.alu_control == 2) result.alu_result = (int32_t)alu_in.alu_in1 + (int32_t)alu_in.alu_in2;  // add/
	else if (alu_in.alu_control == 3) result.alu_result = alu_in.alu_in1 ^ alu_in.alu_in2;  // xor/
	else if (alu_in.alu_control == 4) result.alu_result = alu_in.alu_in1 << alu_in.alu_in2;  // sll, slli unsigned shift
	else if (alu_in.alu_control == 5) result.alu_result = alu_in.alu_in1 >> alu_in.alu_in2;  // srli  unsigned shift
	else if (alu_in.alu_control == 6) result.alu_result = (int32_t)alu_in.alu_in1 >> alu_in.alu_in2;  // sra,srai  signed shift
	else if (alu_in.alu_control == 7) result.alu_result = (int32_t)alu_in.alu_in1 - (int32_t)alu_in.alu_in2;  // sub
	else if (alu_in.alu_control == 8) result.alu_result = ((int32_t)alu_in.alu_in1 < (int32_t)alu_in.alu_in2) ? 1 : 0;  // slt, slti. signed
	else if (alu_in.alu_control == 9) result.alu_result = (alu_in.alu_in1 < alu_in.alu_in2) ? 1 : 0;  // sltu, sltiu, unsigned
	else if (alu_in.alu_control == 10) result.alu_result = alu_in.alu_in1 + alu_in.alu_in2;  // lui -> 0 + imm32
	else 								result.alu_result = alu_in.alu_in1 + alu_in.alu_in2;
	

	
	if ((int32_t) result.alu_result == 0) result.alu_zero = 1;
	else 								  result.alu_zero = 0;
	
	if ((int32_t) result.alu_result < 0) result.alu_sign = 1;
	else								result.alu_sign = 0;
	return result;
	
}

Dmem_out dmem( Dmem_in dmem_in, uint32_t *dmem_data){  // load, store
	Dmem_out result;
	
	if (dmem_in.mem_write) dmem_in.dmem_data[dmem_in.dmem_addr] = dmem_in.dmem_din; // store
	
	result.dmem_dout =  (dmem_in.mem_read ) ? dmem_in.dmem_data[dmem_in.dmem_addr] : 0; // load
	return result;
}
