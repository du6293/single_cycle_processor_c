
#include "rv32i.h"

#include <stdlib.h>  
#include <stdint.h>


int main (int argc, char *argv[]) {  // argument cout 

	// get input arguments
	FILE *f_imem, *f_dmem; // 
	if (argc < 3) {
		printf("usage: %s imem_data_file dmem_data_file\n", argv[0]);
		exit(1);
	}

	if ( (f_imem = fopen(argv[1], "r")) == NULL ) {  // imem 
		printf("Cannot find %s\n", argv[1]);
		exit(1);
	}
	if ( (f_dmem = fopen(argv[2], "r")) == NULL ) {  // dmem
		printf("Cannot find %s\n", argv[2]);
		exit(1);
	}

	// memory data (global)
	uint32_t *reg_data;
	uint32_t *imem_data;
	uint32_t *dmem_data;

	reg_data = (uint32_t*)malloc(32*sizeof(uint32_t));  
	imem_data = (uint32_t*)malloc(IMEM_DEPTH*sizeof(uint32_t));
	dmem_data = (uint32_t*)malloc(DMEM_DEPTH*sizeof(uint32_t)); 

	// initialize memory data
	int i, j, k;
	for (i = 0; i < 32; i++) reg_data[i] = 0;
	for (i = 0; i < IMEM_DEPTH; i++) imem_data[i] = 0;
	for (i = 0; i < DMEM_DEPTH; i++) dmem_data[i] = 0;  

	uint32_t d, buf; 
	/* instruction memory */
	i = 0;
	printf("\n*** Reading %s ***\n", argv[1]);
	while (fscanf(f_imem, "%1d", &buf) != EOF) 
	 {
		d = buf << 31; 
		for (k = 30; k >= 0; k--) 
		{
			if (fscanf(f_imem, "%1d", &buf) != EOF)  
			{
				d |= buf << k;
			} 
			else 
			{
				printf("Incorrect format!!\n");
				exit(1);
			}
		}
		imem_data[i] = d;
		printf("imem[%03d]: %08X\n", i, imem_data[i]);
		i++;
	}
	/* data memory */
	i = 0;
	printf("\n*** Reading %s ***\n", argv[2]);
	while (fscanf(f_dmem, "%8x", &buf) != EOF) 
	{
		dmem_data[i] = buf;
		printf("dmem[%03d]: %08X\n", i, dmem_data[i]);
		i++;
	}

	fclose(f_imem);
	fclose(f_dmem);
	

	// processor model
	uint32_t pc_curr = 0;
	uint32_t pc_next =0;	
	
	/* instruction memory */
   	Imem_in imem_in = {0};
   	Imem_out imem_out= {0};
   	/* register */
   	Reg_in reg_in= {0};
   	Reg_out reg_out= {0};
   	/* alu */
   	Alu_in alu_in= {0};
   	Alu_out alu_out= {0};
   	/* data memory */
   	Dmem_in dmem_in= {0};
   	Dmem_out dmem_out= {0};
   	
   	
   	
   	// get melloc
   	imem_in.imem_data = imem_data ; 
   	reg_in.reg_data = reg_data;
   	dmem_in.dmem_data = dmem_data;
   	
   	
   	uint32_t cc = 2;	// clock count
	
	
	
	
	while (cc < CLK_NUM) {
		
		
		/*1.instruction fetch*//////////////////////////////////////////////////////////////////////////
		uint32_t imem_addr = pc_curr >> 2;
		imem_in.imem_addr = imem_addr;  // 1word
		imem_out = imem(imem_in, imem_data); //   
		uint32_t inst = imem_out.inst;   // get 32 bit instruction from instruction memory
		
	
		/* 2.instruction decode *///////////////////////////////////////////////////////////////////////////////
		
		uint32_t opcode = inst & 0x0000007f; 
		uint8_t funct3;// = (inst >> 12) & 0x00007;
		
		if (opcode == 0x37 || opcode == 0x17 ) funct3 =0;
		else 									funct3 = (inst >> 12) & 0x00007;
		
		uint8_t funct7 ;//= (inst >> 25) &0x7f;
		
		if (opcode == 0x33 ) funct7 = (inst >> 25) &0x7f;
		else 				funct7 = 0;
		
		
		
		
		
		/* control uint*/
		
		uint8_t branch[6] = {0};
		branch[0]  = (opcode == 0x63 && funct3 == 0x0)  ? 1 : 0; // beq
		branch[1]  = (opcode == 0x63 && funct3 == 0x1)  ? 1 : 0; // bne
		branch[2]  = (opcode == 0x63 && funct3 == 0x4)  ? 1 : 0; // blt
		branch[3]  = (opcode == 0x63 && funct3 == 0x5)  ? 1 : 0; // bge
		branch[4]  = (opcode == 0x63 && funct3 == 0x6)  ? 1 : 0; // bltu
		branch[5]  = (opcode == 0x63 && funct3 == 0x7)  ? 1 : 0; // bgeu
		
		uint8_t mem_read = (opcode == 0x3) ? 1  : 0 ;    // load
		uint8_t mem_write = (opcode == 0x23) ? 1 : 0;   // store
		uint8_t mem_to_reg = (opcode == 0x3) ? 1 : 0;   // load
		uint8_t reg_write = (opcode == 0x03 || opcode == 0x33 || opcode == 0x13 || opcode == 0x63 || opcode == 0x67 || opcode == 0x37 || opcode == 0x6f || opcode == 0x17) ? 1 : 0;  // except for store 
		uint8_t alu_src = (opcode == 0x3 || opcode == 0x23 || opcode == 0x13 || opcode == 0x67 || opcode == 0x6f || opcode == 0x37 || opcode == 0x17) ? 1 : 0 ;  // rs2_dout에 imm?  rs2
	           

		uint8_t alu_op = 0;         /// what is alu excute's operation
		uint8_t alu_control = 0;

		if      (opcode == 0x03) 		alu_op = 0;         //load -> add
		else if (opcode == 0x23) 		alu_op = 0;			//store -> add
		else if (opcode == 0x33) 		alu_op = 2;			//r-type  
		else if (opcode == 0x13) 		alu_op = 2;			// i-type
		else if (opcode == 0x63) 		alu_op = 1;          // sb   -> sub
		else if (opcode == 0x37) 		alu_op = 3;			// lui
		else 	                 		alu_op = 0;         // auipc, jal, jalr
		
		
		
		
		
		
		
		
		if      (alu_op == 0) 		alu_control = 2;  	 //load, store   
		else if (alu_op == 1) 		alu_control = 7;	 // sb-type 
		else if (alu_op == 3) 		alu_control = 10;    // lui
		else if (alu_op == 2) {						 // i-type, r-type
			
			if (funct3 == 0x01) 						alu_control = 4;  //sll,sllli
			else if(funct3 == 0x05 && funct7 == 0x0) 	alu_control = 5 ; //srl,srli
			else if(funct3 == 0x05 && funct7 == 0x20) 	alu_control = 6 ;  // sra, srai signed right shift
			else if(funct3 == 0x00) {
				if(funct7 == 0x20) 					alu_control = 7 ; //sub
				else 								alu_control = 2 ; //add, addi
			}
			else if (funct3 == 0x04) 		alu_control = 3; //xor,xori
			else if (funct3 == 0x06) 		alu_control = 1; //or,ori
			else if (funct3 == 0x07)		alu_control = 0; //and,andi
			else if (funct3 == 0x02) 		alu_control = 8; //slt,slti
			else if (funct3 == 0x03)		alu_control = 9; //sltu,sltiu
		}
		
	
		/*Immediate generator*/
		uint32_t imm = 0; // imm12 or imm20
		if (opcode == 0x13){                /////// i-type 
		    imm = (inst >> 20) & 0xfff;  // inst[31:20]->imm[11:0] 
		}
		else if (opcode == 0x3){              ////load
			imm = (inst >> 20) &0xfff; // inst[31:20]->imm[11:0]
		}
		
		else if (opcode == 0x23){  ///// store
			imm = (((inst >> 25) & 0x7f)<<5 )| (inst>> 7) &0x1f ;  //imm[11:5] imm[4:0]
		}
		
		else if (opcode == 0x63){        ///////// sb-type  
			uint8_t	imm1 = ((inst >> 8) & 0xf);        // imm[4:1]
			uint8_t imm2 = ((inst >> 25) & 0x3f)<<4;   // imm[10:5]
			uint8_t imm3 = ((inst >> 7) & 0x01)<<10;   // imm[11]
			uint8_t imm4 = ((inst >> 31) & 0x1)<<11;    //imm[12]
			imm = imm1 | imm2 | imm3 | imm4;
		}
		else if (opcode == 0x6f){            // jal, uj-type 
			uint32_t	imm1 = ((inst >> 12) & 0xff)<<11; // imm[19:12]
			uint32_t	imm2 = ((inst >> 20) &  0x1)<<10;// imm[11]
			uint32_t	imm3 = ((inst >> 21) & 0x3ff)<<1;// imm[10:1]
			uint32_t	imm4 = ((inst >> 31) & 0x1)<<19;// imm[20]
			imm = imm1 | imm2 | imm3 | imm4;
			}
			
		else if (opcode == 0x67){     /////////  jalr
			imm = (inst >> 20) & 0xfff;
		}
		else if (opcode == 0x37){           // lui	
			imm = (inst >> 12) & 0xfffff;  // imm[31:12]
		}
		else if (opcode == 0x17){           // auipc
			imm = (inst >> 12) & 0xfffff;  // imm[31:12]
		}
		else imm = 0;
		
		
		
		/*  extension*/
		
		uint32_t imm32 = 0;
		
		if(opcode == 0x03 || opcode == 0x67){  // load,  jalr imm12 -> imm32
			if (((int32_t)imm >> 11) &1== 1)
				imm32 = (imm&0xfff) | 0xfffff000;
			else imm32 = imm & 0x00000fff;
		}	
		else if (opcode == 0x23){  // store imm12 -> imm32
			if (((int32_t)imm >> 11) &1== 1)
				imm32 = (imm&0xfff) | 0xfffff000;
			else imm32 = (imm) & 0x00000fff;
		}
		
		else if (opcode == 0x013){                                 // i-type imm12 -> imm32
			if (funct3 == 0x03) imm32 = imm & 0x00000fff;  // unsigned extension
			else {
				if (((int32_t)imm>> 11) &1== 1)  imm32 = ( imm&0xfff) | 0xfffff000;
				else imm32 = imm & 0x00000fff;
			}
		}
		else if (opcode == 0x63){                               // sb-type imm12 -> imm32
			if (funct3 == 0x06 || funct3 == 0x07) imm32 = imm & 0x00000fff;  // unsigned extension
			else {
				if (((int32_t)imm >> 11) &1== 1) imm32 =  ( imm&0xfff) | 0xfffff000;
				else imm32 = imm & 0x00000fff;
			}
		}
		else if (opcode == 0x6f){                                 // jal, imm20 -> imm32
			if (( (int32_t)imm >> 19) &1 == 1) imm32 = (imm& 0xfffff) | 0xfff00000;
			else imm32 = imm & 0x000fffff;
		}           
		else if (opcode == 0x37) imm32 = imm << 12;            // lui, imm20-> imm32
		else if (opcode == 0x17) imm32 = imm << 12;            // auipc, imm20 -> imm32

		uint32_t imm32_branch = 0;
		imm32_branch = imm32 << 1;




		/* instruction decode */
		uint8_t rs1=0;//= (opcode == (0x03|0x23|0x33|0x13|0x63|0x67)) ?( (inst >> 15) & 0x1f) : 0 ;
		uint8_t rs2=0 ;//= (opcode == (0x23|0x33|0x63)) ? ((inst >> 20) & 0x0001f) : 0 ;
		uint8_t rd=0 ;//= (opcode == (0x03|0x33|0x13|0x37|0x17|0x6f|0x67)) ? ((inst >>7) & 0x000001f) : 0;
		
		if (opcode == 0x23||opcode == 0x33||opcode == 0x13||opcode ==0x63||opcode == 0x67)		rs1 = ((inst >> 15) & 0x1f); // load, store, r-type, i-type, sb-type, jalr
		else if (opcode == 0x03)																rs1 = ((inst >> 15) & 0x1f);
		else 																									rs1 =0;
		
		if (opcode == 0x23||opcode ==0x33||opcode ==0x63)						rs2 = ((inst >> 20) & 0x0001f); // store, r-type, sb-type
		else																	rs2 = 0;
		
		if (opcode == 0x03||opcode == 0x33|| opcode == 0x13|| opcode ==0x37||opcode == 0x17||opcode == 0x6f|| opcode == 0x67)	rd = ((inst >>7) & 0x000001f); // load, r-type, i-type, lui,auipc,jal,jalr
		else																												rd = 0; 
		
		
		reg_in.rs1 = rs1;
		reg_in.rs2 = rs2;
		reg_in.rd = rd;
		reg_in.reg_write=0; 
		reg_out = regfile(reg_in, reg_data);   // get rs1_dout, rs2_dout
		
		
		
		/* 3. execution */
		uint32_t alu_in1 = 0;
		uint32_t alu_in2 = 0;
		uint32_t alu_result = 0;
		
		if (opcode == 0x03 || opcode == 0x23 ||opcode == 0x33 ||opcode == 0x13||opcode == 0x63||opcode == 0x67 ) alu_in1 = reg_out.rs1_dout; // load, store, r-type, i-type, sb-type, jalr
		else if (opcode == 0x17 || opcode == 0x6f) 																 alu_in1 = pc_curr;  // auipc, jal
		else if (opcode == 0x37) 																				 alu_in1 = 0; // for lui
		
		if (opcode == 0x3 ||opcode ==0x23 || opcode ==0x13 || opcode ==0x67  ||opcode == 0x37) 					alu_in2 = imm32; // load, store, i-type, jalr, lui
		else if (opcode == 0x33 || opcode == 0x63) 																alu_in2 = reg_out.rs2_dout;  // r-type, sb-type
		else if (opcode == 0x17|| opcode == 0x6f)  																alu_in2 = (imm32<<1); // for auipc
		
		alu_in.alu_in1 = alu_in1;
		alu_in.alu_in2 = alu_in2;
		alu_in.alu_control = alu_control;
		alu_out = alu(alu_in);  
		alu_result = alu_out.alu_result;
		
		
		 
		uint8_t pc_next_sel;
		uint32_t pc_next_plus4 ;
		uint32_t pc_next_branch ;
		
		pc_next_plus4= pc_curr + 4;
		//pc_curr = pc_next;
		
		
		 /* branch */
		if 		( branch[0] == 1 && alu_out.alu_zero == 1) 							pc_next_sel = 1; // beq
		else if ( branch[1] == 1 && alu_out.alu_zero == 0) 							pc_next_sel = 1; //bne
		else if ( branch[2] == 1 && alu_out.alu_sign == 1 && alu_out.alu_zero == 0) pc_next_sel = 1;    // blt
		else if ( branch[3] == 1 && alu_out.alu_sign == 0) 							pc_next_sel = 1;   // bge
		else if ( branch[4] == 1 && alu_out.alu_zero == 0 && alu_out.alu_sign == 1) pc_next_sel = 1;   // bltu
		else if ( branch[5] == 1 && alu_out.alu_sign == 0)						 	pc_next_sel = 1;   // bgeu
		else if (opcode ==0x6f || opcode == 0x67 )								 	pc_next_sel = 1; // jal, jarl
		
		else if (opcode == 0x3 || opcode == 0x23 || opcode == 0x33 || opcode == 0x13|| opcode == 0x37 || opcode == 0x17)	pc_next_sel = 0;  /// etc don't jump
		
		if (pc_next_sel == 1){
			if (opcode == 0x63)  { //sb-type
				pc_next_branch = pc_curr + ( imm32_branch) ; // conditional jump
			//	pc_next = pc_next_branch;			
			}   
			else if (opcode == 0x6f) { // jal
				pc_next_branch = pc_curr + ( imm32_branch); // unconditional jump
				//pc_next = pc_next_branch;
			}
			else if (opcode == 0x67) { // jalr
				pc_next_branch = alu_result; 
				//pc_next = pc_next_branch; // unconditional jump
			}
		}
		
		pc_next = (pc_next_sel ) ? pc_next_branch : pc_next_plus4;
		
		pc_curr = pc_next;   
		
		

		/*4.memory */

		dmem_in.dmem_addr = alu_out.alu_result >> 2 ;  // 1 word
		
		
		/* memory write - store */
		
		if 		(opcode == 0x23 && funct3 == 0x00)  	   dmem_in.dmem_din = (reg_out.rs2_dout & 0x000000ff); // sb R[rs2][7:0]
		else if (opcode == 0x23 && funct3 == 0x01) 		   dmem_in.dmem_din = (reg_out.rs2_dout & 0x0000ffff); // sh R[rs2][15:0]	
		else if (opcode == 0x23 && funct3 == 0x2)  	       dmem_in.dmem_din = (reg_out.rs2_dout); // sw R[rs2][31:0] 
		
		dmem_in.mem_write = mem_write;
		dmem_in.mem_read = mem_read;
		dmem_out = dmem(dmem_in, dmem_data);  //to transfer register
		
		
		
		
		/*  5.write-back */
		
		
		if(mem_to_reg == 1) {
			if (opcode == 0x03 && funct3 == 0x00) {                                        // lb 8bit
				if (((int32_t)dmem_out.dmem_dout >> 7) &1== 1) reg_in.rd_din  = ((int32_t)dmem_out.dmem_dout | 0xffffff00) ;  //dout[7:0]
				
				else                                reg_in.rd_din  = (dmem_out.dmem_dout & 0x000000ff);
			}
			
			else if (opcode == 0x03 && funct3 == 0x01 ){                                      // lh 16bit
				if (((int32_t)dmem_out.dmem_dout >> 15) &1== 1) reg_in.rd_din = ((int32_t)dmem_out.dmem_dout | 0xffff0000) ;  //dou[15:0]
				
				else                                   reg_in.rd_din = (dmem_out.dmem_dout & 0x0000ffff) ;
			
			}
			else if (opcode == 0x03 && funct3 == 0x02) reg_in.rd_din = (dmem_out.dmem_dout );    // lw 32bit
			else if (opcode == 0x03 && funct3 == 0x04) reg_in.rd_din = (dmem_out.dmem_dout & 0x000000ff);     // lbu
			else if (opcode == 0x03 && funct3 == 0x05) reg_in.rd_din = (dmem_out.dmem_dout & 0x0000ffff);     // lhu
        }
        else {
			if 		(opcode == 0x33)					reg_in.rd_din = alu_out.alu_result;  // r-type
			else if (opcode == 0x13) 					reg_in.rd_din = alu_out.alu_result;  // i-type
			else if (opcode == 0x63) 					reg_in.rd_din = pc_next_plus4; // sb-type
			else if (opcode == 0x6f) 					reg_in.rd_din = pc_next_plus4;  // jal
			else if (opcode == 0x67) 					reg_in.rd_din = pc_next_plus4;	//jalr
			else if (opcode == 0x37)  					reg_in.rd_din = alu_out.alu_result;     //lui
			else if (opcode == 0x17) 					reg_in.rd_din = alu_out.alu_result;  // auipc
			else										reg_in.rd_din = alu_out.alu_result;
	}
		reg_in.reg_write = reg_write;
		reg_out = regfile(reg_in, reg_data);   // write back to register
	
		
		

//		printf("cc : %d		opcode : %x		imm32 : %d \n",cc,opcode,imm32);
//		printf("pc_curr : %d	pc_next : %d \n", pc_curr, pc_next);
//		printf("rs1 : %d 		rs2 : %d		rd : %d \n", reg_in.rs1, reg_in.rs2, reg_in.rd);
//		printf("rs1_dout : %d 		rs2_dout : %d\n", reg_out.rs1_dout, reg_out.rs2_dout );
//		printf("alu_in1 : %d	alu_in2 : %d	alu_result: %d\n", alu_in.alu_in1, alu_in.alu_in2, alu_out.alu_result);
//		printf(" \n" );

		cc++;
	}
	
	
    for (i = 0; i < 32; i++) {
		printf("RF[%02d]: %08X \n", i, reg_data[i]);
	}
		
	for (i = 0; i < 9; i++) {
		printf( "DMEM[%02d]: %08X \n", i, dmem_data[i]);
	}
   
   // malloc 
	free(reg_data);
	free(imem_data);
	free(dmem_data);

	return 1;
}
