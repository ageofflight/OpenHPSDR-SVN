// V1.0 31 August  2009  
//
// Copyright 2009 Phil Harman VK6APH
//
//  HPSDR - High Performance Software Defined Radio
//
//  Ozy11 Test code .
//
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//


// Built with Quartus II v9.0 Build 178
//
// Change log:  
//	31 Aug 2009  - started coding - read EP2 and send to EP6


//
////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
//
//              Quartus V9.0  Notes
//
//////////////////////////////////////////////////////////////

/*
	In order to get this code to compile without timing errors under
	Quartus V9.0 I needed to use the following settings:
	
	- Analysis and Synthesis Settings\Power Up Dont Care [not checked]
	- Analysis and Synthesis Settings\Restructure Multiplexers  [OFF]
	- Fitter Settings\Optimise fast-corner timing [ON]
	
	Same setting for Quartus V8.0 and V8.1
	****** Synthesis options for State Machine Processing  = User Encoded ******
	
*/


//////////////////////////////////////////////////////////////
//
//                      Pin Assignments
//
/////////////////////////////////////////////////////////////
//
//
//       FX2 pin    to   FPGA pin connections
//
//       IFCLK                  - pin 24
//   	 FX2_CLK                - pin 23
//       FX2_FD[0]              - pin 56
//       FX2_FD[1]              - pin 57
//       FX2_FD[2]              - pin 58
//       FX2_FD[3]              - pin 59
//       FX2_FD[4]              - pin 60
//       FX2_FD[5]              - pin 61
//       FX2_FD[6]              - pin 63
//       FX2_FD[7]              - pin 64
//       FX2_FD[8]              - pin 208
//       FX2_FD[9]              - pin 207
//       FX2_FD[10]             - pin 206
//       FX2_FD[11]             - pin 205
//       FX2_FD[12]             - pin 203
//       FX2_FD[13]             - pin 201
//       FX2_FD[14]             - pin 200
//       FX2_FD[15]             - pin 199
//       FLAGA                  - pin 198
//       FLAGB                  - pin 197
//       FLAGC                  - pin 5
//       SLOE                   - pin 13
//       FIFO_ADR[0]            - pin 11
//       FIFO_ADR[1]            - pin 10
//       PKEND                  - pin 8
//       SLRD                   - pin 30
//       SLWR                   - pin 31
//
//
//   General FPGA pins
//
//       DEBUG_LED0             - pin 4
//       DEBUG_LED1             - pin 33
//       DEBUG_LED2             - pin 34
//       DEBUG_LED3             - pin 108
//		 FPGA_GPIO1				- pin 67
//		 FPGA_GPIO2				- pin 68
//		 FPGA_GPIO3				- pin 69
//		 FPGA_GPIO4				- pin 70
//		 FPGA_GPIO5				- pin 72
//		 FPGA_GPIO6				- pin 74
//		 FPGA_GPIO7				- pin 75
//		 FPGA_GPIO8				- pin 76
//		 FPGA_GPIO9				- pin 77
//		 FPGA_GPIO10			- pin 80
//		 FPGA_GPIO11			- pin 81
//		 FPGA_GPIO12			- pin 82
//		 FPGA_GPIO13			- pin 84
//		 FPGA_GPIO14			- pin 86
//		 FPGA_GPIO15			- pin 87
//		 FPGA_GPIO16			- pin 88
//		 FPGA_GPIO17			- pin 89
//
//
////////////////////////////////////////////////////////////


module Ozy11(
        IFCLK, FX2_FD, FLAGA, FLAGB, FLAGC, SLWR, SLRD, SLOE, PKEND, FIFO_ADR, 
        DEBUG_LED0,
		DEBUG_LED1, DEBUG_LED2,DEBUG_LED3,
		FX2_PE0, FX2_PE1, FX2_PE2, FX2_PE3,SDOBACK,TDO,TCK, TMS );

input IFCLK;                   // FX2 IFCLOCK - 48MHz
inout  [15:0] FX2_FD;           // bidirectional FIFO data to/from the FX2
//output  [15:0] FX2_FD;        // bidirectional FIFO data to/from the FX2
                                // ***** use this so simulation works
input FLAGA;
input FLAGB;
input FLAGC;
output SLWR;
output SLRD;
output SLOE;
output PKEND;
output [1:0] FIFO_ADR;
output DEBUG_LED0;              // LEDs on OZY board
output DEBUG_LED1;
output DEBUG_LED2;
output DEBUG_LED3;

// interface pins for JTAG programming via Atlas bus
input  FX2_PE0;		// Port E on FX2
output FX2_PE1;
input  FX2_PE2;
input  FX2_PE3;
output TDO;			// A27 on Atlas 
input  SDOBACK;		// A25 on Atlas
output TCK;			// A24 on Atlas
output TMS;			// A23 on Atlas

// link JTAG pins through
assign TMS = FX2_PE3;
assign TCK = FX2_PE2;
assign TDO = FX2_PE0;  // TDO on our slot ties to TDI on next slot  
assign FX2_PE1 = SDOBACK;


// set up FX2 Port A

assign PKEND = 1'b1;

reg [4:0] state_FX;             // state for FX2

reg data_flag;                  // set when data ready to send to Tx FIFO
reg [15:0] register;            // AK5394A A/D uses this to send its data to Tx FIFO
reg [15:0] Tx_data;             // Tx mic audio from TLV320
reg [6:0] loop_counter;         // counts number of times round loop
reg Tx_fifo_enable;             // set when we want to send data to the Tx FIFO EP6




//////////////////////////////////////////////////////////////
//
//                    Clocks 
//
//////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
//
//   Rx_fifo  (2048 words) Dual clock FIFO - Altera Megafunction (dcfifo)
//
/////////////////////////////////////////////////////////////

/*
        The write clock of the FIFO is SLRD and the read clock IFCLK.
        Data from the FX2_FIFO is written to the FIFO using !SLRD. Data from the
        FIFO is read on the positive edge of IFCLK when fifo_enable is high. The
        FIFO is 4096 words long.
        NB: The output flags are only valid after a read/write clock has taken place
*/


wire [15:0] Rx_data;
reg fifo_enable;                        // controls reading of dual clock fifo

wire Rx_full; 			 // set when write side full 

Rx_fifo Rx_fifo(.wrclk (~SLRD),.rdreq (fifo_enable),.rdclk (IFCLK),.wrreq (Rx_fifo_enable), 
                .data (Rx_FIFO),.q (Rx_data),.rdusedw(), .wrfull(Rx_full));

///////////////////////////////////////////////////////////////
//
//     Tx_fifo (4096 words) Dual clock FIFO  - Altera Megafunction - for EP6
//
//////////////////////////////////////////////////////////////
               
                
Tx_fifo Tx_fifo(.wrclk (!BCLK),.rdreq (Tx_read_clock),.rdclk (IFCLK),.wrreq(Tx_fifo_enable),
                .data (register),.q (Tx_register), .wrusedw(write_used), .aclr(Tx_aclr), .rdempty(Tx_empty));

wire [15:0] Tx_register;                // holds data from A/D to send to FX2
reg  Tx_read_clock;                     // when goes high sends data to Tx_register
wire [11:0] write_used;                 // indicates how may bytes in the Tx buffer
reg  [11:0] syncd_write_used;   		// ditto but synced to FX2 clock
reg Tx_aclr;							// async clear of FIFO 
wire Tx_empty; 							


//////////////////////////////////////////////////////////////
//
//   State Machine to manage FX2 USB interface
//
//////////////////////////////////////////////////////////////
/*
        The state machine checks if there are characters to be read
        in the FX2 Rx FIFO by checking 'EP2_has_data'  If set it loads the word
        read into the Rx_register. On the next clock it  checks if the Tx_data_fag is set
         - if so it sends the data in 'register'to the Tx FIFO. After the Tx data has been sent
        it checks 'EP2_has_data'again and repeats.
*/

reg SLOE;                             // FX2 data bus enable - active low
reg SLEN;                             // Put data on FX2 bus for EP6
reg SLEN_2;							  // Enable data for EP4
reg SLRD;                             // FX2 read - active low
reg SLWR;                             // FX2 write - active low
reg [1:0] FIFO_ADR;                   // FX2 register address
wire EP2_has_data = FLAGA;            // high when EP2 has data available
wire EP6_ready = FLAGC;               // high when we can write to End Point
reg [15:0] Rx_register;               // data from PC goes here
reg [12:0] send_count;


always @ (negedge IFCLK)
begin
syncd_write_used <= write_used;
case(state_FX)
// state 0 - set up to check for Rx data from EP2
4'd0:begin
		SLWR <= 1;                    // reset FX2 FIFO write stobe
        Tx_read_clock <= 1'b0;        // reset Tx fifo read strobe
        SLRD <= 1'b1;
        SLOE <= 1'b1;
        SLEN <= 1'b0;
        SLEN_2 <= 1'b0;
        FIFO_ADR <= 2'b00;            // select EP2
        state_FX <= state_FX + 1'b1;
   end
        
1: // delay so address is stable
		state_FX <= state_FX + 1'b1;
		
// delay 2 IFCLOCK cycle, this is necessary at 48MHZ to allow FIFO_ADR to settle
// check for Rx data by reading EP2. Only get data if Rx FIFO has room 
4'd2:begin
     send_count <= 0; 			  // reset send data counter
	if(EP2_has_data  && !Rx_full)begin
       state_FX <= state_FX + 1'b1;
       SLOE <= 1'b0;                  //assert SLOE
       end
    else begin
       FIFO_ADR <= 2'b10;             // select EP6
       state_FX <= 4'd6;              // No Rx data so check for Tx data
       end
    end
// Wait 2 IFCLK before we assert SLRD then load received data
4'd4:begin
        SLRD <= 1'b0;
        Rx_register[15:8] <= FX2_FD[7:0];  //  swap endian
        Rx_register[7:0]  <= FX2_FD[15:8];
        state_FX <= state_FX + 1'b1;
        end


// reset SLRD and SLOE
4'd5:begin
        SLRD <= 1'b1;
        SLOE <= 1'b1;
        //state_FX <= state_FX + 1'b1;
        //state_FX <= 2;			// keep reading EP2 data until it is empty
        FIFO_ADR <= 2'b10;             // select EP6
        state_FX <= 4'd6;   
        end
     
6: // delay so address is stable
		state_FX <= state_FX + 1'b1;
      
// check for Tx data - Tx fifo must be at least half full before we Tx
7:begin
        if (EP6_ready && !Tx_empty) begin // data available, so let's start the xfer...
            Tx_read_clock <= 1'b1;
            state_FX <= state_FX + 1'b1;
            end
     	else begin
			state_FX <= 0;                  // No EP6 data so back to EP2,
			end
    end                                     
8:begin
    	Tx_read_clock <= 1'b0;          	   
        SLEN <= 1'b1;
        state_FX <= state_FX + 1'b1;
    end
//  set SLWR
9: begin
       SLWR <= 1'b0;
       state_FX <= state_FX + 1'b1;
       end
//  reset SLWR and tristate SLEN
10: begin
        SLWR <= 1;
        SLEN <= 1'b0;
		state_FX <= 0;
			
    end		
       
default:  state_FX <= state_FX + 1'b1;
    endcase
end

// FX2_FD is tristate when SLEN  is low, otherwise it's the Tx_register value.
// Swap endian so data is correct at PC end

assign FX2_FD[15:8] = SLEN ? Tx_register[7:0]  : 8'bZ;  // For EP6
assign FX2_FD[7:0]  = SLEN ? Tx_register[15:8] : 8'bZ;




// Flash the LEDs to show something is working! - LEDs are active low

// DEBUG_LED0 - flashes if 12.288MHz Atlas clock selected but not present

//assign DEBUG_LED1 = ~halt_flag;	// LED on when EP6 available
//assign DEBUG_LED1 = ~conf[1];	// test config setting 
//assign DEBUG_LED2 = ~EP2_has_data; 	
//assign DEBUG_LED3 = ~have_sync; // lights when sync from PowerSDR detected 


endmodule

