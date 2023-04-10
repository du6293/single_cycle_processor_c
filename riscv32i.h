#ifndef __RV32I_H__
#define __RV32__H__

// headers
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>  


// defines
#define REG_WIDTH 32
#define IMEM_DEPTH 1024
#define DMEM_DEPTH 1024

// configs
#define CLK_NUM 45

//
//typedef union{
//	uint32_t inst; // 32bit instruction
//	struct {
//		uint8_t	opcode:7; 
//		uint8_t 	rd:5;
//		uint8_t	funct3:3;
//		uint8_t	rs1:5;
//		uint8_t	rs2:5;
//		uint8_t    funct7:7;  
//	}r;  // r-format 
//	struct{
//		uint8_t	opcode:7;
//		uint8_t	rd:5;
//		uint8_t	funct3:3;
//		uint8_t	rs1:5;
//		uint32_t	imm:12;
//	} i; // i-format, jalr, load
//	struct {
//		uint8_t	opcode:7;  
//		uint8_t 	imm1:5;
//		uint8_t	funct3:3;
//		uint8_t	rs1:5;
//		uint8_t	rs2:5;
//		uint32_t    imm2:7;  // 7비트만 사용
//	}sb;   // sb , store 
//	struct{
//		uint8_t	opcode:7 ;
//		uint8_t	rd:5;
//		uint32_t	imm:20;
//	}uj;  //uj, u
//}rv_format;







/*   instruction memory   */

typedef struct imem_input_t {
	uint32_t imem_addr;
	uint32_t *imem_data;
}Imem_in;

typedef struct imem_output_t {
	uint32_t inst;
}Imem_out;


Imem_out imem ( Imem_in, uint32_t *imem_data);



/*   register  */
typedef struct regfile_input_t{
	uint8_t rs1;
	uint8_t rs2;
	uint8_t rd;
	uint32_t rd_din;
	uint8_t reg_write;
	uint32_t *reg_data;
}Reg_in;

typedef struct regfile_output_t{
	uint32_t rs1_dout;
	uint32_t rs2_dout;
	
}Reg_out;

Reg_out regfile( Reg_in, uint32_t *reg_data);





/*    alu    */
typedef struct alu_input_t{
	uint32_t alu_in1;
	uint32_t alu_in2;
	uint8_t alu_control;
}Alu_in;


typedef struct alu_output_t{
	uint32_t alu_result;
	uint8_t alu_zero;
	uint8_t alu_sign;

}Alu_out;

Alu_out alu(Alu_in);




/*    data memory    */
typedef struct dmem_input_t{
	uint32_t dmem_din;
	uint32_t dmem_addr;
	//uint32_t dmem_sz;
	uint8_t mem_write;
	uint8_t mem_read;
	uint32_t *dmem_data;
}Dmem_in;


typedef struct dmem_output_t{
	uint32_t dmem_dout;
}Dmem_out;

Dmem_out dmem( Dmem_in, uint32_t *dmem_data);



#endif

