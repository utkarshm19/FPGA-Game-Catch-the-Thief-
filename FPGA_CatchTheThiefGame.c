	#define SDRAM_BASE 0xC0000000
#define FPGA_ONCHIP_BASE 0xC8000000
#define FPGA_CHAR_BASE 0xC9000000

/*Cyclone V FPGA devices */
#define LEDR_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX5_HEX4_BASE 0xFF200030
#define SW_BASE 0xFF200040
#define KEY_BASE 0xFF200050
#define TIMER_BASE 0xFF202000
#define PIXEL_BUF_CTRL_BASE 0xFF203020
#define CHAR_BUF_CTRL_BASE 0xFF203030

/*VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00
#define ABS(x)(((x) > 0) ? (x) : -(x))

/*Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/*Constants for animation */
#define BOX_LEN 2
#define NUM_BOXES 8
#define FALSE 0
#define TRUE 1
#include <stdlib.h>
#include <stdio.h>

// Begin part1.s for Lab 7
void clear_screen();
void plot_pixel(int x, int y, short int line_color);
void draw_line(int x0, int y0, int x1, int y1, short color);
void swap(int *x, int *y);
void wait_ofr_vsync();
void write_char(int x, int y, char *c);
void display_caught();
void text(int x, int y, char *text_ptr);
void fill_position_thief(int x_pos, int y_pos);
void fill_position_cop(int x_pos, int y_pos);
int caught();
void null_thief();
void null_cop();
void play_hex();
int loop_check(int SW_value);
void display_error();
void drawBlock(int xInit, int yInit, int size, short color);

volatile int pixel_buffer_start;	// global variable
volatile char *character_buffer = (char *) 0xC9000000;
void set_A9_IRQ_stack(void);
void config_GIC(void);
void config_KEYs(void);
void enable_A9_interrupts(void);
void pushbutton_ISR(void);
void move(void);

#define USER_MODE 0b10000
#define FIQ_MODE 0b10001
#define IRQ_MODE 0b10010
#define SVC_MODE 0b10011
#define ABORT_MODE 0b10111
#define UNDEF_MODE 0b11011
#define SYS_MODE 0b111111

#define INT_ENABLE 0b01000000
#define INT_DISABLE 0b11000000
#define ENABLE 0b1

#define emptyPixel 0
	
/* This files provides address values that exist in the system */

#define BOARD                 "DE1-SoC"

/* Memory */
#define DDR_BASE              0x00000000
#define DDR_END               0x3FFFFFFF
#define A9_ONCHIP_BASE        0xFFFF0000
#define A9_ONCHIP_END         0xFFFFFFFF
#define SDRAM_BASE            0xC0000000
#define SDRAM_END             0xC3FFFFFF
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_ONCHIP_END       0xC803FFFF
#define FPGA_CHAR_BASE        0xC9000000
#define FPGA_CHAR_END         0xC9001FFF

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define JP1_BASE              0xFF200060
#define JP2_BASE              0xFF200070
#define PS2_BASE              0xFF200100
#define PS2_DUAL_BASE         0xFF200108
#define JTAG_UART_BASE        0xFF201000
#define JTAG_UART_2_BASE      0xFF201008
#define IrDA_BASE             0xFF201020
#define TIMER_BASE            0xFF202000
#define AV_CONFIG_BASE        0xFF203000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030
#define AUDIO_BASE            0xFF203040
#define VIDEO_IN_BASE         0xFF203060
#define ADC_BASE              0xFF204000

/* Cyclone V HPS devices */
#define HPS_GPIO1_BASE        0xFF709000
#define HPS_TIMER0_BASE       0xFFC08000
#define HPS_TIMER1_BASE       0xFFC09000
#define HPS_TIMER2_BASE       0xFFD00000
#define HPS_TIMER3_BASE       0xFFD01000
#define FPGA_BRIDGE           0xFFD0501C

/* ARM A9 MPCORE devices */
#define   PERIPH_BASE         0xFFFEC000    // base address of peripheral devices
#define   MPCORE_PRIV_TIMER   0xFFFEC600    // PERIPH_BASE + 0x0600

/* Interrupt controller (GIC) CPU interface(s) */
#define MPCORE_GIC_CPUIF      0xFFFEC100    // PERIPH_BASE + 0x100
#define ICCICR                0x00          // offset to CPU interface control reg
#define ICCPMR                0x04          // offset to interrupt priority mask reg
#define ICCIAR                0x0C          // offset to interrupt acknowledge reg
#define ICCEOIR               0x10          // offset to end of interrupt reg
/* Interrupt controller (GIC) distributor interface(s) */
#define MPCORE_GIC_DIST       0xFFFED000    // PERIPH_BASE + 0x1000
#define ICDDCR                0x00          // offset to distributor control reg
#define ICDISER               0x100         // offset to interrupt set-enable regs
#define ICDICER               0x180         // offset to interrupt clear-enable regs
#define ICDIPTR               0x800         // offset to interrupt processor targets regs
#define ICDICFR               0xC00         // offset to interrupt configuration regs

struct position{
	int x;
	int y;
	
};
	
	
	//struct position thief_pos[100];
	//struct position cop_pos[100];

	int y0 = 120;
	int y1 = 30;
	int x0 = 150;
	int x1 = 30;
	int true = 1;
	int move_y;
	int move_x;
	int move_y1;
	int move_x1;
	
	int x1_speed =1;
	
	int y1_speed =1;
	
int main(void)
{
	/*
  	set_A9_IRQ_stack();
    config_GIC();
    config_KEYs(); // configure pushbutton KEYs to generate interrupts
    enable_A9_interrupts(); // enable interrupts
	
	*/
	int catch = 0;
	volatile int *pixel_ctrl_ptr = (int*) 0xFF203020;
	volatile int *sw_ptr = (int*)SW_BASE;
	volatile int * KEY_ptr = (int *)KEY_BASE; // pushbutton KEY address
	int SW_value, KEY_value;
	/*Read location of the pixel buffer from the pixel buffer controller */
	pixel_buffer_start = *pixel_ctrl_ptr;
	volatile int* ps2_port = (int*)0xFF200100;
	unsigned char ps2_data = 0;

	clear_screen();
	
	
	
	while (true)
	{
		//display_caught();
		
		ps2_data = *ps2_port & 0xff;
		if(catch == 0){
			play_hex();
		}
		
		draw_line(10, 20, 309, 20, GREEN);
		draw_line(10, 20, 10, 229, GREEN);
		draw_line(10, 229, 309, 229, GREEN);
		draw_line(309, 20, 309, 229, GREEN);
		char* title = "THIEF AND POLICE";

		text(160, 1, title);
		SW_value = *(sw_ptr);
		KEY_value =*(KEY_ptr);
		
		if(KEY_value ==0b0001 && y1_speed <=6&&x1_speed <=6){
			y1_speed  = y1_speed*2;
			x1_speed  = x1_speed*2;
		}
		if(KEY_value ==0b0010 && y1_speed >=3&&x1_speed >=3){
			y1_speed  = y1_speed/2;
			x1_speed  = x1_speed/2;
		}
		if(ps2_data != 0){
		
		drawBlock(x0, y0, 10, RED);
		//drawBlock(x1, y1, 10, BLUE);

		if(ps2_data == 0x75){//up
			if (y1 >30){
			y1 = y1 - y1_speed;
			}
			drawBlock(x1, y1+y1_speed, 10, BLUE);
			wait_ofr_vsync();
			drawBlock(x1, y1+y1_speed, 10, 0x0000);
		}else if(ps2_data == 0x72){//down
			if (y1 <200){
			y1 = y1 + y1_speed;
			}
			drawBlock(x1, y1-y1_speed, 10, BLUE);
			wait_ofr_vsync();
			drawBlock(x1, y1-y1_speed, 10, 0x0000);
		}else if(ps2_data == 0x6B){
			if(x1>30){
			x1 = x1 - x1_speed;
			}
			drawBlock(x1+x1_speed, y1, 10, BLUE);
			wait_ofr_vsync();
			drawBlock(x1+x1_speed, y1, 10, 0x0000);
		}else if(ps2_data == 0x74){
			if(x1<300){
			x1 = x1 + x1_speed;
			}
			drawBlock(x1-x1_speed, y1, 10, BLUE);
			wait_ofr_vsync();
			drawBlock(x1-x1_speed, y1, 10, 0x0000);
		}
		
		else{
			display_error();
			int delay;
			while(loop_check(ps2_data) == 1){
				ps2_data = *(ps2_port);
				delay++;
			}
			continue;
		}
		
	
			//caught condition
		if(x1-10 <= x0 && x0 <= x1+10 && y1-10 <= y0 && y0 <= y1+10){
			display_caught();
			catch = 1;
			break;
		}
		/*fill_position_thief(x0, y0);
		fill_position_cop(x1, y1);
		if(caught() == 1){
			display_caught();
			break;
		}*/
		//wait_ofr_vsync();
		drawBlock(x0, y0, 10, 0x0000);
		
		
	
		int randMov = rand() % 2;
		int randMov1 = rand() % 2;
		
		//random movement within bound
		if(y0 > 30 && x0 > 30 && y0 < 200 && x0 < 300){
			int randMov = rand() % 2;
			int direction = rand() % 2;
		
			
		if(y1-20 <=y0 && y0 <=y1+20 && x0 >x1+30){//thief at the right
			if(direction == 0){//cant move left
				move_y = -4;
				move_x = 4;
			}else{
				move_y = 4;
			}
		}
			
			//prevent thief run into police
			else if(y1-20 <=y0 && y0 <=y1+20 && x0 >x1-30){//thief at the left
			if(direction == 0){//cant move right
				move_y = -3;
				move_x = -3;
			
			}else{
				move_y = 3;
				move_x = -3;
				
			}
		}
			else if(x1-20 <=x0 && x0 <=x1+20 && y0 >y1+30){//thief at the top
			if(direction == 0){//cant move down
				move_x = -3;
				move_y = 3;
				
			}else{
				
				move_y = 3;
				move_x = 3;
			}
		}
			
			else if(x1-20 <=x0 && x0 <=x1+20 && y0 >y1-30){//thief at the bottom
			if(direction == 0){//cant move up
				
				move_y = -3;
				move_x = -3;
			}else{

				move_x = 3;
				move_y = -3;
			}
		}
			
			//move thief randomly
			if(randMov == 0){
				x0 = x0 + ((move_x));
			}else{
				y0 = y0 + ((move_y));
			}
			
		}
		
		if (y0 <= 30){ //boundary condition for thief
			move_y= 10 ;
			y0 = y0 + move_y;
		}
		 if (y0 >= 200){
			move_y= -10 ;
			 y0 = y0 + move_y;
		}
		if (x0 <= 30){
			move_x= 10 ;
			x0 = x0 + move_x;
		}
		if (x0 >= 300){
			move_x= -10 ;
			x0 = x0 + move_x;
		}
		}
	else{
			drawBlock(x0, y0, 10, 0xF800);
		drawBlock(x1, y1, 10, BLUE);
		}
		
	}
	
	return 0;
}

// code not shown for clear_screen() and draw_line() subroutines

void wait_ofr_vsync()
{
	volatile int *pixel_ctrl_ptr = (int*) 0xFF203020;	//pixel controller
	register int status;

	*pixel_ctrl_ptr = 1;	//start the synchronization process

	status = *(pixel_ctrl_ptr + 3);	//point to ...2c
	while ((status & 0x01) != 0)
	{
		//check s bit
		status = *(pixel_ctrl_ptr + 3);	//keeping reading until s is 1
	}
}

void clear_screen()
{
	int x_clear;
	int y_clear;
	for (x_clear = 0; x_clear <= 319; x_clear++)
	{
		for (y_clear = 0; y_clear <= 240; y_clear++)
		{
			plot_pixel(x_clear, y_clear, 0x0000);
		}
	}
}

void swap(int *x, int *y)
{
	int *temp;
	*temp = *x;
	*x = *y;
	*y = *temp;
}

void plot_pixel(int x, int y, short int line_color)
{
	*(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void draw_line(int x0, int y0, int x1, int y1, short color)
{
	int is_steep = abs(y1 - y0) > abs(x1 - x0);
	if (is_steep != 0)
	{
		swap(&x0, &y0);
		swap(&x1, &y1);
	}
	if (x0 > x1)
	{
		swap(&x0, &x1);
		swap(&y0, &y1);
	}

	int delta_x = x1 - x0;
	int delta_y = abs(y1 - y0);
	int error = -(delta_x / 2);
	int y = y0;
	int y_step = 0;
	if (y0 < y1)
	{
		y_step = 1;
	}
	else
	{
		y_step = -1;
	}

	int x;

	for (x = x0; x <= x1; x++)
	{
		if (is_steep != 0)
		{
			plot_pixel(y, x, color);
		}
		else
		{
			plot_pixel(x, y, color);
		}
		error = error + delta_y;

		if (error > 0)
		{
			y = y + y_step;
			error = error - delta_x;
		}
	}
}

void drawBlock(int xInit, int yInit, int size, short color){
	int x;
	int y;
    for(x = xInit; x < xInit + size; x++){
        for( y = yInit; y < yInit + size; y++){
            plot_pixel(x, y, color);//show cursor
        }
      }
}

void write_char(int x, int y, char *c) {
  // VGA character buffer
  volatile char * character_buffer = (char *) (0x09000000 + (y<<7) + x);
  *character_buffer = *c;
}

void text(int x, int y, char *text_ptr) {
    int offset = (y << 7) + x;

    while (*(text_ptr)) 
    {
        
        *(character_buffer + offset) = *(text_ptr);
        ++text_ptr;
        ++offset;
    }
}


void display_caught(){
	volatile int *hex3_0_ptr = (int*)HEX3_HEX0_BASE;
	volatile int *hex5_4_ptr = (int*)HEX5_HEX4_BASE;
	
	*hex3_0_ptr = 0x78 | 0x76 <<8 | 0x3D <<16 | 0x3E <<24;
	*hex5_4_ptr = 0x77 | 0x39 <<8;
	
}

void display_error(){
	volatile int *hex3_0_ptr = (int*)HEX3_HEX0_BASE;
	volatile int *hex5_4_ptr = (int*)HEX5_HEX4_BASE;
	
	*hex3_0_ptr = 0x50 | 0x5C <<8 | 0x50 <<16 | 0x50 <<24;
	*hex5_4_ptr = 0x79;
	
}

void play_hex(){

	volatile int *hex3_0_ptr = (int*)HEX3_HEX0_BASE;
	volatile int *hex5_4_ptr = (int*)HEX5_HEX4_BASE;
	
	*hex3_0_ptr = 0x00 | 0x00 <<8 | 0x6E <<16 | 0x77 <<24;
	*hex5_4_ptr = 0x38 | 0x73 <<8;
	
}



int loop_check(int ps2_data){
	
	if(ps2_data != 0x75 && ps2_data != 0x72 && ps2_data != 0x6B && ps2_data != 0x74){
		return 1;		
	}
	
	return 0;
	
}
	
	
	