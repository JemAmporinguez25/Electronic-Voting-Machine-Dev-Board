#include "reg52.h"    // Include the 8051 microcontroller header file
#include "lcd.h"
#include <string.h>
#include "key.h"

#define u8 unsigned char
#define u16 unsigned int
u8 a[16] = " Congratulations";
u8 b[27] = "Winner A                   ";
#define GPIO_DIG P0
#define GPIO_KEY P1
sbit PUSH_BUTTON = P2^0;
sbit CANDIDATE1=P2^1;
sbit CANDIDATE2=P2^2;
sbit CANDIDATE3=P2^7;
sbit CANDIDATE4=P2^4;

sbit CANDIDATE1_LED=P1^0;
sbit CANDIDATE2_LED=P1^1;
sbit CANDIDATE3_LED=P1^2;
sbit CANDIDATE4_LED=P1^3;

u8 key_num;
u16 votes[4] = {0, 0, 0, 0}; // Votes for A, B, C, D

xdata u8 Disp[16];  // Display buffer in XDATA

// Function prototypes
void delay_ms(unsigned int ms);
void printdisp();
void display_candidates();
void update_display();
void int_to_str(u16 num, char* str);
void declare_winner();
void Displaywinner(const char* winner);
void tie_breaker();
void CountdownBeforeDisplayResults();
/*******************************************************************************
* Function    : main
* Description : Main function
* Input       : None
* Output      : None
*******************************************************************************/
void main(void)
{
    LcdInit();
    KeyDown();

    // Display the welcome message
		while(PUSH_BUTTON==0){
    strcpy(Disp, " Welcome to EVM ");
    LcdWriteCom(0x80);
    printdisp();

    // Display "VOTE WISELY"
    LcdWriteCom(0xC0);
    strcpy(Disp, "  VOTE WISELY   ");
    printdisp();
		}
		PUSH_BUTTON=0;
    // Clear display and start displaying candidates
    LcdWriteCom(0x01);
    display_candidates();
CANDIDATE2=0;
		CANDIDATE4=0;
while (1) {
    // Continuously scan for key presses and update votes
    // if ((key_num = KeyDown()) != 0xFF) {
    //     if (key_num >= 1 && key_num <= 4) {
    //         votes[key_num - 1]++;
    //         update_display();
    //     }
    // }

    // Example condition to stop voting and declare a winner
    // You can replace this with your own condition to end voting
    if (PUSH_BUTTON == 1) {
        declare_winner();
			while(PUSH_BUTTON==1){
			}
        break;
    } else if (CANDIDATE1 == 1) {
			CANDIDATE1_LED=1;
        votes[0]++;
       // update_display();
        CANDIDATE1 = 0;
						while(CANDIDATE1==1){
			}
			//continue;
    } else if (CANDIDATE2 == 1) {
				CANDIDATE2_LED=1;
        votes[1]++;
        //update_display();
									while(CANDIDATE2==1){
			}
        CANDIDATE2 = 0;
			//continue;
    } else if (CANDIDATE3 == 1) {
				CANDIDATE3_LED=1;
        votes[2]++;
			  //update_display();
        CANDIDATE3 = 0;
												while(CANDIDATE3==1){
			}
			//continue;
    } else if (CANDIDATE4 == 1) {
				CANDIDATE4_LED=1;
        votes[3]++;
        //update_display();
        CANDIDATE4 = 0;
															while(CANDIDATE4==1){
			}
			//continue;
    }
		        update_display();
}
}

/*******************************************************************************
* Function    : delay_ms
* Description : Delay function
* Input       : ms (milliseconds)
* Output      : None
*******************************************************************************/
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 1275; j++);
}

/*******************************************************************************
* Function    : printdisp
* Description : Prints the display buffer to the LCD
* Input       : None
* Output      : None
*******************************************************************************/
void printdisp()
{
    u8 i;
    for (i = 0; i < 16; i++) {
        LcdWriteData(Disp[i]);
    }
}

/*******************************************************************************
* Function    : display_candidates
* Description : Displays candidates A, B, C, and D
* Input       : None
* Output      : None
*******************************************************************************/
void display_candidates()
{
    LcdWriteCom(0x80); // Move to the first line
    strcpy(Disp, "A:00000 B:00000 ");
    printdisp();
    LcdWriteCom(0xC0); // Move to the second line
    strcpy(Disp, "C:00000 D:00000 ");
    printdisp();
}

/*******************************************************************************
* Function    : update_display
* Description : Updates the display with current vote counts
* Input       : None
* Output      : None
*******************************************************************************/
void update_display()
{
    char buffer[16];
    char temp[6];

    // Format the string for the first line
    strcpy(buffer, "A:");
    int_to_str(votes[0], temp);
    strcat(buffer, temp);
    strcat(buffer, " B:");
    int_to_str(votes[1], temp);
    strcat(buffer, temp);

    // Update the first line of the display
    LcdWriteCom(0x80); // Move to the first line
    strcpy(Disp, buffer);
    printdisp();

    // Format the string for the second line
    strcpy(buffer, "C:");
    int_to_str(votes[2], temp);
    strcat(buffer, temp);
    strcat(buffer, " D:");
    int_to_str(votes[3], temp);
    strcat(buffer, temp);

    // Update the second line of the display
    LcdWriteCom(0xC0); // Move to the second line
    strcpy(Disp, buffer);
    printdisp();
}

/*******************************************************************************
* Function    : int_to_str
* Description : Converts an integer to a string with leading zeros
* Input       : num (integer), str (output string)
* Output      : None
*******************************************************************************/
void int_to_str(u16 num, char* str)
{
    u8 i;
    for (i = 0; i < 5; i++) {
        str[4 - i] = (num % 10) + '0';
        num /= 10;
    }
    str[5] = '\0';  // Null-terminate the string
}

/*******************************************************************************
* Function    : declare_winner
* Description : Declares the winner based on the highest votes
* Input       : None
* Output      : None
*******************************************************************************/
void declare_winner()
{
    u8 i, winner = 0;
    u16 max_votes = votes[0];
    u8 tie = 0; // To track if there's a tie
    char winner_msg[16];

    // Determine the winner
    for (i = 1; i < 4; i++) {
        if (votes[i] > max_votes) {
            max_votes = votes[i];
            winner = i;
            tie = 0; // Reset tie because a new maximum was found
        } else if (votes[i] == max_votes) {
            tie = 1; // A tie is detected
        }
    }

    // Clear the display
    LcdWriteCom(0x01);
    delay_ms(2);

    // Display the winner message or tie message
    if (tie) {
				CountdownBeforeDisplayResults();
        strcpy(winner_msg, "It's a Tie      ");
        LcdWriteCom(0x80); // Move to the first line
        strcpy(Disp, winner_msg);
        printdisp();
        delay_ms(2000);
        tie_breaker();
    } else {
        switch (winner) {
            case 0:
                strcpy(winner_msg, " Winner: Michael");
                break;
            case 1:
                strcpy(winner_msg, " Winner: Reham C");
                break;
            case 2:
                strcpy(winner_msg, " Winner: Jemuel A");
                break;
            case 3:
                strcpy(winner_msg, " Winner: Sim D  ");
                break;
        }

        LcdWriteCom(0x80); // Move to the first line
        //strcpy(Disp, winner_msg);
        //printdisp();
				CountdownBeforeDisplayResults();
										while(1){
					
        Displaywinner(winner_msg);
				}
    }

}

/*******************************************************************************
* Function    : Displaywinner
* Description : Displays the winner message
* Input       : winner (winner message string)
* Output      : None
*******************************************************************************/
void Displaywinner(const char* winner)
{
    u8 i;

    // Copy the winner name to the b array
    strncpy((char*)b, winner, sizeof(b) );
    b[sizeof(b) - 1] = '\0'; // Ensure null termination

    LcdWriteCom(0x00 + 0x80);
    for (i = 0; i < 16; i++)
    {
        LcdWriteData(a[i]);
    }
    LcdWriteCom(0x40 + 0x80);
    for (i = 0; i < strlen((char*)b); i++)
    {
        LcdWriteData(b[i]);
    }
    LcdWriteCom(0x07);

    while (1)
    {
        LcdWriteCom(0x00 + 0x80);
        for (i = 0; i < 16; i++)
        {
            LcdWriteData(a[i]);
            delay_ms(80); // Delay to show each character
        }
    }
}

/*******************************************************************************
* Function    : tie_breaker
* Description : Resets the votes and conducts a re-vote in case of a tie
* Input       : None
* Output      : None
*******************************************************************************/
void tie_breaker()
{
    u8 i;
    char tie_msg[16];

    // Clear the display
    LcdWriteCom(0x01);
    delay_ms(2
	);

    // Display "Re-vote needed"
    strcpy(tie_msg, "Re-vote needed  ");
    LcdWriteCom(0x80); // Move to the first line
    strcpy(Disp, tie_msg);
    printdisp();
    delay_ms(500);

    // Reset the votes
    for (i = 0; i < 4; i++) {
        votes[i] = 0;
    }

    // Display candidates again for re-voting
    display_candidates();

    // Continue re-voting
while (1) {
    // Continuously scan for key presses and update votes
    // if ((key_num = KeyDown()) != 0xFF) {
    //     if (key_num >= 1 && key_num <= 4) {
    //         votes[key_num - 1]++;
    //         update_display();
    //     }
    // }

    // Example condition to stop voting and declare a winner
    // You can replace this with your own condition to end voting
    if (PUSH_BUTTON == 1) {
        declare_winner();
						while(PUSH_BUTTON==1){
			}
        break;
    } else if (CANDIDATE1 == 1) {
			CANDIDATE1_LED=1;
        votes[0]++;
       // update_display();
        CANDIDATE1 = 0;
						while(CANDIDATE1==1){
			}
			//continue;
    } else if (CANDIDATE2 == 1) {
				CANDIDATE2_LED=1;
        votes[1]++;
        //update_display();
			       CANDIDATE1 = 0;
						while(CANDIDATE2==1){
			}
        CANDIDATE2 = 0;
			//continue;
    } else if (CANDIDATE3 == 1) {
				CANDIDATE3_LED=1;
        votes[2]++;
									while(CANDIDATE3==1){
			}
			  //update_display();
        CANDIDATE3 = 0;
			//continue;
    } else if (CANDIDATE4 == 1) {
				CANDIDATE4_LED=1;
        votes[3]++;
        //update_display();
        CANDIDATE4 = 0;
												while(CANDIDATE4==1){
			}
			//continue;
    }
		        update_display();
}
}
void CountdownBeforeDisplayResults()
{
    u8 i,j;
    char message[] = "Counting results";
    char countdown[] = "3";

    for ( j = 3; j > 0; j--)
    {
        // Clear the display (assuming 16 characters wide)
        LcdWriteCom(0x00 + 0x80);
        for (i = 0; i < 16; i++)
        {
            LcdWriteData(' ');
        }

        // Display the message "Calculating results"
        LcdWriteCom(0x00 + 0x80);
        for (i = 0; i < strlen(message) && i < 16; i++)
        {
            LcdWriteData(message[i]);
        }

        // Update countdown number
        countdown[0] = '0' + j;

        // Display the countdown number on the second line
        LcdWriteCom(0x40 + 0x80);
        LcdWriteData(countdown[0]);

        // Wait for 1 second
        delay_ms(250);
    }

    // Clear the second line after the countdown
    LcdWriteCom(0x40 + 0x80);
    for (i = 0; i < 16; i++)
    {
        LcdWriteData(' ');
    }
}