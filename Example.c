//Make F1 output and turn it on

TRISF=0b1111111111111101 //Set F1 to output
TRISFbits.TRISF1=0 //Set F1 to output

LATF=0b10 //Set the value to 1 (turn it on)
LATFbits.LATF1=1 //Set the value to 1 //Try to use this 

LATFINV=0b10 //Inverts all the F pins
LATFSET=0b10 //MAkes the F0 = 0 and F1=1
LATFCLR=0b10 //MAkes all the F1 = 0