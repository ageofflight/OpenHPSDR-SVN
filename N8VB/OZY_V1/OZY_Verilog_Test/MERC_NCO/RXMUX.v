// megafunction wizard: %LPM_MUX%
// GENERATION: STANDARD
// VERSION: WM1.0
// MODULE: lpm_mux 

// ============================================================
// File Name: RXMUX.v
// Megafunction Name(s):
// 			lpm_mux
// ============================================================
// ************************************************************
// THIS IS A WIZARD-GENERATED FILE. DO NOT EDIT THIS FILE!
//
// 6.0 Build 202 06/20/2006 SP 1 SJ Web Edition
// ************************************************************


//Copyright (C) 1991-2006 Altera Corporation
//Your use of Altera Corporation's design tools, logic functions 
//and other software and tools, and its AMPP partner logic 
//functions, and any output files any of the foregoing 
//(including device programming or simulation files), and any 
//associated documentation or information are expressly subject 
//to the terms and conditions of the Altera Program License 
//Subscription Agreement, Altera MegaCore Function License 
//Agreement, or other applicable license agreement, including, 
//without limitation, that your use is for the sole purpose of 
//programming logic devices manufactured by Altera and sold by 
//Altera or its authorized distributors.  Please refer to the 
//applicable agreement for further details.


// synopsys translate_off
`timescale 1 ps / 1 ps
// synopsys translate_on
module RXMUX (
	clock,
	data0x,
	data1x,
	data2x,
	data3x,
	sel,
	result);

	input	  clock;
	input	[15:0]  data0x;
	input	[15:0]  data1x;
	input	[15:0]  data2x;
	input	[15:0]  data3x;
	input	[1:0]  sel;
	output	[15:0]  result;

	wire [15:0] sub_wire0;
	wire [15:0] sub_wire5 = data3x[15:0];
	wire [15:0] sub_wire4 = data1x[15:0];
	wire [15:0] sub_wire3 = data0x[15:0];
	wire [15:0] result = sub_wire0[15:0];
	wire [15:0] sub_wire1 = data2x[15:0];
	wire [63:0] sub_wire2 = {sub_wire5, sub_wire1, sub_wire4, sub_wire3};

	lpm_mux	lpm_mux_component (
				.sel (sel),
				.clock (clock),
				.data (sub_wire2),
				.result (sub_wire0)
				// synopsys translate_off
				,
				.aclr (),
				.clken ()
				// synopsys translate_on
				);
	defparam
		lpm_mux_component.lpm_pipeline = 1,
		lpm_mux_component.lpm_size = 4,
		lpm_mux_component.lpm_type = "LPM_MUX",
		lpm_mux_component.lpm_width = 16,
		lpm_mux_component.lpm_widths = 2;


endmodule

// ============================================================
// CNX file retrieval info
// ============================================================
// Retrieval info: CONSTANT: LPM_PIPELINE NUMERIC "1"
// Retrieval info: CONSTANT: LPM_SIZE NUMERIC "4"
// Retrieval info: CONSTANT: LPM_TYPE STRING "LPM_MUX"
// Retrieval info: CONSTANT: LPM_WIDTH NUMERIC "16"
// Retrieval info: CONSTANT: LPM_WIDTHS NUMERIC "2"
// Retrieval info: USED_PORT: clock 0 0 0 0 INPUT NODEFVAL clock
// Retrieval info: USED_PORT: data0x 0 0 16 0 INPUT NODEFVAL data0x[15..0]
// Retrieval info: USED_PORT: data1x 0 0 16 0 INPUT NODEFVAL data1x[15..0]
// Retrieval info: USED_PORT: data2x 0 0 16 0 INPUT NODEFVAL data2x[15..0]
// Retrieval info: USED_PORT: data3x 0 0 16 0 INPUT NODEFVAL data3x[15..0]
// Retrieval info: USED_PORT: result 0 0 16 0 OUTPUT NODEFVAL result[15..0]
// Retrieval info: USED_PORT: sel 0 0 2 0 INPUT NODEFVAL sel[1..0]
// Retrieval info: CONNECT: @clock 0 0 0 0 clock 0 0 0 0
// Retrieval info: CONNECT: result 0 0 16 0 @result 0 0 16 0
// Retrieval info: CONNECT: @data 0 0 16 48 data3x 0 0 16 0
// Retrieval info: CONNECT: @data 0 0 16 32 data2x 0 0 16 0
// Retrieval info: CONNECT: @data 0 0 16 16 data1x 0 0 16 0
// Retrieval info: CONNECT: @data 0 0 16 0 data0x 0 0 16 0
// Retrieval info: CONNECT: @sel 0 0 2 0 sel 0 0 2 0
// Retrieval info: LIBRARY: lpm lpm.lpm_components.all
// Retrieval info: GEN_FILE: TYPE_NORMAL RXMUX.v TRUE
// Retrieval info: GEN_FILE: TYPE_NORMAL RXMUX.inc FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL RXMUX.cmp FALSE
// Retrieval info: GEN_FILE: TYPE_NORMAL RXMUX.bsf TRUE
// Retrieval info: GEN_FILE: TYPE_NORMAL RXMUX_inst.v TRUE
// Retrieval info: GEN_FILE: TYPE_NORMAL RXMUX_bb.v TRUE
