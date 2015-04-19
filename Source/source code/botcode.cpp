
#define F_CPU 14745600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.c"
#include <math.h>
#include "ADC.h"

//Data variables initialisation

volatile unsigned char datahigh=256;                       // To store second bit of recieved data 
volatile unsigned char datalow=0;		           // To store first bit of recieved data 
volatile int flag = 1;   				   // For SIGNAL() interrupt logic implementation	
volatile unsigned char data=0; 		
volatile unsigned int distance = 0;       //To store the value of distance recieved after decrypting the recieved data
volatile unsigned int theta = 0;          //To store the value of distance recieved after decrypting the recieved data
volatile unsigned char sharp[5];          // To store the readings of the sharp IR sensors		
volatile unsigned char maxSharpValue=0;   // To store the Maximum value amongst the 5 Sharp sensors readings
volatile unsigned int counters = 0;             // To store the count of balls reached till then
volatile unsigned long int ShaftCountLeft = 0;  // To store the number of interrupt calls for ISR(INT4_vect)
volatile unsigned long int ShaftCountRight =0;  // To store number of interrupt calls for ISR(INT5_vect)


/*
Function: void buzzer_init(void)
Inputs: void
Outputs: Initialises buzzer
Logic: Using DDR to set 4th bit as output bit and others as input bits. 
Example: buzzer_init();
*/
void buzzer_init(void)
{
	DDRC= DDRC |0x08;
	PORTC = PORTC & 0xF7;
}

/*
Function: void buzzer_on (void)
Inputs: void
Outputs: Starts the buzzer by setting the buzzer on PORTC PIN3 bit high as soon as the function is called. 
Logic: Sets PINC 3 bit as high(i.e. on) and remaining bits unchanged using masking.
Examples: buzzer_on ();
*/
void buzzer_on (void)
{
	unsigned char port_restore = 0;
	port_restore = PINC;
	port_restore = port_restore | 0x08;
	PORTC = port_restore;
}

/*
Function: void buzzer_off (void)
Inputs: void
Outputs: Stops the buzzer by setting the buzzer on PORTC PIN3 bit low as soon as the function is called.
Logic: Sets PINC 3 bit as low(i.e. low) and remaining bits unchanged  using masking.
Examples: buzzer_off ();
*/
void buzzer_off (void)
{
	unsigned char port_restore = 0;
	port_restore = PINC;
	port_restore = port_restore & 0xF7;
	PORTC = port_restore;
}

/*
Function:void  lcd_port_config();
Inputs: void
Outputs: Configures the port which is connected to the LCD 
Logic: Sets the direction of all the pins of the LCD as output and initializes the all LCD pins to logic 0
Example: lcd_port_config();
*/
void lcd_port_config (void)
{
	DDRC = DDRC | 0xF7;             //all the LCD pin's direction set as output except PIN 3 which is used for buzzer.
	PORTC = PORTC & 0x80;           // all the LCD pins are set to logic 0 except PORTC 7
}

/*
Function: void motion_pin_config();
Inputs: void
Outputs: Configures the port A for motion control  
Logic: Sets the direction lower nibble of port A as output 
       and write a logical 0 to the port A to keep the bot stationary initially.
Example: motion_pin_config();
*/
void motion_pin_config (void)
{
	DDRA = DDRA | 0x0F;
	PORTA = PORTA & 0xF0;
}


/*
Function:void left_encoder_pin_config();
Inputs: void
Outputs: Configures the port which is connected to the encoder on the left wheel
Logic: Sets the direction of all the pin connected to the encoder as output and enables internal pull up 
       for the pin (PORT E pin 4) for taking reading.
Example: left_encoder_pin_config();
*/
void left_encoder_pin_config (void)
{
	DDRE  = DDRE & 0xEF;  //Set the direction of the PORTE 4 pin as input
	PORTE = PORTE | 0x10; //Enable internal pull-up for PORTE 4 pin
}

/*
Function: void right_encoder_pin_config();
Inputs: void
Outputs: Configures the port which is connected to the encoder on the right wheel
Logic: Sets the direction of all the pin connected to the encoder as output and enables internal pull up 
       for the pin (PORT E pin 5 ).
Example: right_encoder_pin_config();
*/
void right_encoder_pin_config (void)
{
	DDRE  = DDRE & 0xDF;  //Set the direction of the PORTE 5 pin as input
	PORTE = PORTE | 0x20; //Enable internal pull-up for PORTE 5 pin
}


/*
Function: void motion_set (unsigned char Direction);
Inputs:  one variable of datatype unsigned char 
Outputs: Sets the motion of both the wheels in a specific direction
Logic: The lower nibble of the port A is replaced by that of the parameter.
       The parameter has a specific pattern of the lower nibble for specific movement. 
       The 4 bits represent “left wheel forward” , “left wheel backward” , “right wheel forward” 
       and “right wheel backward” in the order of the most significant bit to the least significant bit.
       A high on a specific bit means the kind of motion desired.  
Example: motion_set(0x06); It would mean both wheels would be moving forward
*/
void motion_set (unsigned char Direction)
{
	unsigned char PortARestore = 0;

	Direction &= 0x0F; 	        // removing upper nibble for the protection
	PortARestore = PORTA; 		// reading the PORTA original status
	PortARestore &= 0xF0; 		// making lower direction nibble to 0
	PortARestore |= Direction;      // adding lower nibble for forward command and restoring the PORTA status
	PORTA = PortARestore; 		// executing the command
}

/*
Function: void forward();
Inputs: void
Outputs: Makes the bot move forward
Logic: Uses the motion_set(unsigned char) function above with the parameter to make both the wheels move forward.
Example: forward();
*/

void forward (void)
{
        motion_set (0x06);
}


/*
Function: void left();
Inputs: void
Outputs: Makes the bot move left
Logic: Uses the motion_set(unsigned char) function above with the parameter to make the left wheel move
       backward and the right wheel forward in order to achieve a left turn using differential mechanism.
Example: left();
*/

void left (void)
{
	motion_set(0x05);
}

/*
Function:void right();
Inputs: void
Outputs: Makes the bot move left
Logic: Uses the motion_set(unsigned char) function above with the parameter to make the right wheel move 
       backward and the left wheel forward in order to achieve a right turn using differential mechanism.
Example: right();
*/
void right (void) 
{
	motion_set(0x0A);
}


/*
Function: void stop (void)
Inputs: void
Outputs: Stops the bot
Logic: Calls function motion_set(...) to set PORTA to 0x00, i.e. all bits low to stop the bot. 
Examples: stop();
*/
void stop (void) //hard stop
{
	motion_set(0x00);
}



/*
Function: void port_init()
Inputs: void
Outputs: Initializes all the ports by calling the necessary functions which are defined above
Logic: Call all necessary functions for initialisation of the PORTs used in the program.
Examples: port_init();
*/
void port_init()
{
	motion_pin_config();        //robot motion pins config
	buzzer_init();
	left_encoder_pin_config();  //left encoder pin config
	right_encoder_pin_config(); //right encoder pin config
	lcd_port_config();
	
}


/*
Function: void left_position_encoder_interrupt_init (void) 
Inputs: void
Outputs: Interrupt 4 is enabled
Logic: INT4 is set to trigger with the falling edge by masking EICRB with 0x02 and 
       enable interrupt INT4 for left position encoder.
Examples: left_position_encoder_interrupt_init ();
*/
void left_position_encoder_interrupt_init (void) 
{
	cli();                                 //Clears the global interrupt
	EICRB = EICRB | 0x02;                  // INT4 is set to trigger with falling edge
	EIMSK = EIMSK | 0x10;                  // Enable Interrupt INT4 for left position encoder
	sei();                                 // Enables the global interrupt
}


/*
Function: void right_position_encoder_interrupt_init (void) 
Inputs: void
Outputs: Interrupt 5 is enabled
Logic: INT5 is set to trigger with the falling edge by masking EICRB with 0x02
       and enable interrupt INT5 for right position encoder.
Examples: right_position_encoder_interrupt_init ();
*/
void right_position_encoder_interrupt_init (void) //Interrupt 5 enable
{
	cli();                //Clears the global interrupt
	EICRB = EICRB | 0x08; // INT5 is set to trigger with falling edge
	EIMSK = EIMSK | 0x20; // Enable Interrupt INT5 for right position encoder
	sei();                // Enables the global interrupt
}

/*
The Interrupt Service Routine will redirect the control here if an interrupt vector is generated 
by INT4 and increment the shaft count of the encoder on the left wheel.
*/

// Interrupt 4 ISR definition, keeps track of number of pulses in the reading of the position encoder. 
ISR (INT4_vect)
{
	ShaftCountLeft++;
}


/*
Function: angle_rotate(unsigned int Degrees);
Inputs: one variable of datatype unsigned int
Outputs: Makes the bot rotate by a specific angle which is passed as the parameter
Logic: The degrees to be rotated are mapped to a required shaft count and the reading 
       of the position encoder is checked (i.e., shaft count) and the bot moves as long as
       the shaft count is less than the required value.
Example: angle_rotate(30); It will rotate the bot by 30 degrees
*/
void angle_rotate(unsigned int Degrees)
{
	float ReqdShaftCount = 0;
	unsigned long int ReqdShaftCountInt = 0;

	ReqdShaftCount = (float) Degrees/ 4.090;   // division by resolution to get shaft count
	ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
	ShaftCountRight = 0;
	ShaftCountLeft = 0;

	while(1)
	{
		if((ShaftCountRight >= ReqdShaftCountInt) | (ShaftCountLeft >= ReqdShaftCountInt))
		break;
	}
	stop(); //Stop robot
}


/*
Function: void linear_distance_mm(unsigned int DistanceInMM)
Inputs: unsigned int DistanceInMM, i.e. distance to be moved.
Outputs: Moves robot forward by specified distance entered in millimeters
Logic: Compares ShaftCountRight value, which corresponds to the number of the number of interrupt calls, 
       with a value ReqdShaftCount which is calibrated for DistanceInMM.
Examples: linear_distance_mm(100);
*/
void linear_distance_mm(unsigned int DistanceInMM)
{
	float ReqdShaftCount = 0;
	unsigned long int ReqdShaftCountInt = 0;

	ReqdShaftCount = DistanceInMM / 5.338; // division by resolution to get shaft count
	ReqdShaftCountInt = (unsigned long int) ReqdShaftCount;
	
	ShaftCountRight = 0;
	while(1)
	{
		//ShaftCountRight ++;
		if(ShaftCountRight > ReqdShaftCountInt)
		{
			break;
		}
	}
	stop(); //Stop robot
}
/*
The Interrupt Service Routine will redirect the control here if an interrupt vector is generated by INT5 and
increment the shaft count of the encoder on the right wheel, i.e. the number of pulses in the reading of the
position encoder.
*/
ISR(INT5_vect)
{
	ShaftCountRight++;  //increment right shaft position count
}

/*
Function: print_values(unsigned int distance)
Inputs: one variable of unsigned int datatype (distance)
Outputs: It will print the parameter passed to function on the LCD
Logic: The function uses the lcd_print() function in lcd.h to print the data passed on the specific coordinates
Example: print_values(1,2,7,3);  Prints 7 on the LCD at the first row and second column. 
*/
 
void print_values(unsigned int distance)
{
	lcd_print(1, 2, distance, 3);
}

/*
Function: void forward_mm(unsigned int DistanceInMM)
Inputs: one variable of datatype unsigned int (DistanceInMM)
Outputs: Moves robot forward by specified distance entered in millimeters
Logic: Calls functions forward() and linear_distance_mm(DistanceInMM) to move forward till 
       ShaftCountRight reaches the required value corresponding to the given distance to be moved..
Examples: forward_mm(500); Moves the bot forward by half a metre.
*/
void forward_mm(unsigned int DistanceInMM)
{
	forward();
	linear_distance_mm(DistanceInMM);
}

/*
Function: left_degree(unsigned int Degrees);
Inputs: one variable of datatype unsigned int viz the angle to be rotated
Outputs: Makes the bot rotate by a specific angle to the left (+) which is passed as the parameter
Logic: The degrees to be rotated are mapped to a required shaft count and the reading of 
       the position encoder is checked (i.e., shaft count). Functions left() and angle_rotate(Degrees) 
       are called to rotate the bot to left till Required shaft count is reached.
       The calibration of device is such that- 88 pulses for 360 degrees rotation 4.090 degrees per count.
Example: left_degree(30); It will rotate the bot by 30 degrees to left.
*/
void left_degrees(unsigned int Degrees)
{
	                                           // 88 pulses for 360 degrees rotation 4.090 degrees per count
	left();                                    //Turn left
	angle_rotate(Degrees);
}

/*
Function: right_degree(unsigned int Degrees);
Inputs: one variable of datatype unsigned int viz the angle to be rotated
Outputs: Makes the bot rotate by a specific angle to the right (-) which is passed as the parameter
Logic: The degrees to be rotated are mapped to a required shaft count and the reading of 
       the position encoder is checked (i.e., shaft count). Functions right() and angle_rotate(Degrees) 
       are called to rotate the bot to right till Required shaft count is reached.
       The calibration of device is such that- 88 pulses for 360 degrees rotation 4.090 degrees per count.
Example: right_degree(30); It will rotate the bot by 30 degrees to right.
*/
void right_degrees(unsigned int Degrees)
{
	                                        // 88 pulses for 360 degrees rotation 4.090 degrees per count
	right();                                //Turn right
	angle_rotate(Degrees);
}

/*
Function: void uart0_init(void)
Inputs: void
Outputs: Function To Initialize UART0
Logic:  Initialises various registers, and following some specifications:
      	desired baud rate:9600
        actual baud rate:9600 (error 0.0%)
        char size: 8 bit
        parity: Disabled
Example: uart0_init();
*/
void uart0_init(void)
{
	UCSR0B = 0x00;                                           //disable while setting baud rate
	UCSR0A = 0x00;
	UCSR0C = 0x06;
	// UBRR0L = 0x47;                                        //11059200 Hz
	UBRR0L = 0x5F;                                           // 14745600 Hzset baud rate lo
	UBRR0H = 0x00;                                           //set baud rate hi
	UCSR0B = 0x98;
}

/*
Function :  turntoCorrectAngle()
Inputs :  None
Outputs : None, just makes the bot align itself in the direction of the ball
Logic : To reduce the error in rotation,  the bot is first rotated by the required angle 
        in the main function and then the bot is rotated to the right by 10 degrees.
        One by one the bot turns to the left   5 degrees, each time and 
        stores the maxima detected by the sharp sensor and from the left most position, 
        the bot is rotated to this position, so that bot is always directed towards the next bal, 
        to which it has to reach to.
Example : turntoCorrectAngle();        //called this way in the main() function
*/
void turnToCorrectAngle()
{  
   int i, pos=0;
   
   right_degrees(10);
   
   sharp[0]=ADC_Conversion(11);
   maxSharpValue=sharp[0];
   
   for(i=1; i<5; i++)
   {  
	  _delay_ms(500); 
	  left_degrees(5);
	  sharp[i]=ADC_Conversion(11);
	  
	  if(maxSharpValue<sharp[i])
	  {  
		  pos=i;
		  maxSharpValue=sharp[i];
	  }	   	  
   } 
   
   right_degrees((unsigned int)(25-5*pos)); 	
   
}


/*
Function : int convert_to_binary(int )
Inputs :  int x , i.e.  number whose binary equivalent is to be calculated
Outputs : binary equivalent of input - decimal number
Logic : binary equivalent is given by the remainders obtained after successive division by 2
Example : long int binaryDistance= convert_to_binary(data);
*/
long int convert_to_binary(int x)
{
	long int num=0, i=1;
	while (x!=0)
	{
		int dig = x%2;
		num = num + dig*i;
		x/=2;
		i=10*i;
	}
	return num;
}


/*
Function : int convert_to_int(long int )
Inputs :  long int x , i.e.  number whose decimal equivalent is to be calculated
Outputs : decimal equivalent of input - binary number
Logic : decimal equivalent is given by the series D= b0 + b1*2 + b2*4 +b3*8 + ...
Example : int distance= convert_to_int(data);
*/
int convert_to_int(long int x)
{
	int result=0, powerOf2=1;
	while (x!=0)
	{
		result+=(x%10)*powerOf2;
		powerOf2*=2;
		x/=10;
	}
	return result;
}


//ISR(Interrupt Service Routine) for receiving the x-bee transmission from the laptop system 
SIGNAL(SIG_USART0_RECV) 		
{    
	
	data = UDR0; 				             //making copy of data from UDR0 in 'data' variable
	
	flag = flag*(-1);                                    //To toggle the value, to keep track of which bit is to be recieved
	                                                     // datalow (-1) or datahigh(1)
	if (flag == 1)	datahigh = data;
	else 
	{
		datalow = data;
		UDR0 = data; 			   	     //echo data back to PC so the x-bee transmitter on the laptop 
		                                             //to signall it to send the the next bit , i.e. datahigh
	}	 
	
}	
		

/*
Function: init_devices();
Inputs: void
Outputs: Initializes all the ports and devices used in the program.
Logic: By calling the functions defined above to initialize the specific ports. 
Example: init_devices();
*/
void init_devices()
{
	cli();                           //Clears the global interrupt
	uart0_init();                    //Initializes the UART0 Port which is connected for the X-bee communication
	port_init();                     //Initializes all the ports
	buzzer_init();                   //initialises buzzer
	adc_init();                      //Initializes the port for ADC
	timer5_init();
	left_position_encoder_interrupt_init();
	right_position_encoder_interrupt_init();
	sei();                           // Enables the global interrupt
}


int main()
{
	init_devices();
	lcd_set_4bit();
	lcd_init();
	while(1)
	{
		if (flag == 1) 		
		{
			
		    if (datahigh ==255 && datalow ==0)
		        continue;                                // if condition is true, complete data, i.e. both bits have not been recieved. 
		    if(datahigh==255) continue;
			
			if ((convert_to_binary(datalow))%100==11)
			{	distance= (volatile unsigned int) (convert_to_int(convert_to_binary(datalow)/100))+ (volatile unsigned int) ((datahigh)*64);  
				                                 //decrypting value of distance from data sent 
				                                 /*Last two bits of datalow are for sign and data-type identifiction.
				                                   The 8 bits of datahigh followed by the 6 bits of datalow 
				                                   store the actual data. 
				                                   datahigh contains decimal equivalent of 13-6 bits of data.
				                                   datalow contains decimal equivalent of 5-0 bits of data followed by sign and data type bit*/
				print_values(distance);          // printing the distance to be moved to get to next ball   
				_delay_ms(2000);
				counters++;                      // increase counters by 1, as another ball is reached
				forward_mm(distance-180);      
				stop();
				buzzer_on();                     // set buzzer on to alert user that ball is reached
				print_values(counters);          // print the number of balls collected till now
				_delay_ms(2000);
				buzzer_off();   
				forward_mm(180);                 // stop the buzzer and wait for next command
				datahigh = 255;                  // datahigh set to default value = 255 (all bits high)
				datalow = 0;                     // datalow set to default value = 0
			}
			
			else if ((convert_to_binary(datalow))%10==0)
			{	theta= (volatile unsigned int) (convert_to_int(convert_to_binary(datalow)/100))+ (volatile unsigned int) ((datahigh)*64);
				print_values(theta);
				_delay_ms(2000);
				
				if ((convert_to_binary(datalow))%100==00)  //left is positive
			  		left_degrees(theta);               // since theta>0 rotate to left
						
				else
				        right_degrees(theta);              // since theta<0 rotate to right  
				stop();
				print_values(counters);
				_delay_ms(2000);
				datahigh = 255;                            // datahigh set to default value = 255(all bits high)  
				datalow = 0;                               // datalow set to default value = 0
				turnToCorrectAngle();                      // call to function turnToCorrectAngle() which uses sharp sensors 
			}
				
				UDR0 = 0x00;                               // echo back to PC
		}		

	}
}



