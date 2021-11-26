/*----------------------------------------------------------------------------
 Newhaven NHD0216HZ LCD C/C++ file
 *----------------------------------------------------------------------------*/

#include "mbed.h"
#include "ctype.h"
#include "NHD_0216HZ.h"

#define SPI_MOSI D11
#define SPI_SCLK D13
#define SPI_CS D10


//I2C PART
I2C i2c(I2C_SDA, I2C_SCL);
const int addr = 0x90;      
char cmd1 = 0x51; //start the conversion
char cmd2 = 0xAA; //read the value
char data[2];

//SPI LCD PART
DigitalOut SS(SPI_CS);     //slave select a.k.a. cs or latch for shift reg
SPI spi(SPI_MOSI, SPI_MISO, SPI_SCLK);

//UART PART
Serial pc(USBTX, USBRX);

//--------------------FUNCTIONS----------------------------------------------
//Initialise SPI
void init_spi(void) {
    SS = 1;
    
    spi.format(8,3);         //8bit spi mode 2
    spi.frequency(100000);    //100 kHz spi clock
}

//Initialise LCD
void init_lcd(void) {
    /*
	Implement the LCD initialisation using the information 
	from the ST7066U LCD driver datasheet (pages 25-26)
	*/
	//Write your code here
    wait_ms(40);
    write_cmd(0x30);
    wait_us(37);

    write_cmd(0x20);
    wait_us(37);

    write_cmd(0x20);
    wait_us(37);

    write_cmd(0x0C);
    wait_us(37);

    write_cmd(0x01);
    wait_ms(52);

    write_cmd(0x06);
}

//Write 4bits to the LCD
void write_4bit(int nibble, int mode) {
    SS = 0;
    spi.write(nibble | ENABLE | mode);
    SS = 1;
    wait_us(1);
    SS = 0;
    spi.write(nibble & ~ENABLE);
    SS = 1;
}

//Write a command to the LCD
void write_cmd(int data) {
    int hi_n;
    int lo_n;
    
    hi_n = hi_n = (data & 0xF0);
    lo_n = ((data << 4) &0xF0);
    
    write_4bit(hi_n, COMMAND_MODE);
    write_4bit(lo_n, COMMAND_MODE);
}

//Write data to the LCD
void write_data(char c) {
    int hi_n;
    int lo_n;
    
    hi_n = hi_n = (c & 0xF0);
    lo_n = ((c << 4) &0xF0);
    
    write_4bit(hi_n, DATA_MODE);
    write_4bit(lo_n, DATA_MODE);
}

//Set cursor position
void set_cursor(int column, int row) {
    int addr;
    
    addr = (row * LINE_LENGTH) + column;
    addr |= TOT_LENGTH;
    write_cmd(addr);
}

//Print strings to the LCD
void print_lcd(const char *string) {
    while(*string){
        write_data(*string++);
    }
}

//---------------------------------------------------------------------------


int main() {

    //I2C/TEMPERATURE
    float tmp = 0.0;
    while(1){
    
        i2c.write(addr, &cmd1, 1); //start the conversion
        ThisThread::sleep_for(1000); //wait for 0.5s
        i2c.write(addr, &cmd2, 1); //read the last converted
        i2c.read(addr, data, 2);
        tmp = (float((data[0] << 8) | data[1]) / 256);
        printf("\n");
        printf("T = %.2f °C", tmp);
    }

    //SPI/LCD
    char msg1[] = "I hate for loops";
    char msg2[] = "but I love CS435";
    init_spi(); // Intialise the SPI 
    init_lcd(); // and the LCD 
    print_lcd(msg1); // Print the first string
    set_cursor(0,1); // Set the cursor to (0,1)
    print_lcd(msg2); // Print the second string
  
    //UART/COOLTERM
    pc.printf("Hello to the world of mbed!\n");
    while(1) {
        char letter = pc.getc();
        pc.putc(toupper(letter));

        
    }
    
    

}

