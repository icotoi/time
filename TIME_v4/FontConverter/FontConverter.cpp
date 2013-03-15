
/*
FONT.TXT file field format description:

space
char:
first line
second line
third line
fourth line
fift line
-----

End of description.

Example :


A:
¦¦¦
¦ ¦
¦¦¦
¦ ¦
¦ ¦
-----

B:
¦¦ 
¦ ¦
¦¦ 
¦ ¦
¦¦¦ 
-----

The first field from font file is for char with ASCII code decimal 32 -   Space,
the seccond char is 33 - ! Exclamation mark, so on ...
Reference file for font completition is "uni2html.htm"
*/

#include <stdio.h>
#include <conio.h> 
#include <stdlib.h>
#include <string.h>


typedef struct {
    char l1[20];
    char l2[20];
    char l3[20];
    char l4[20];
    char l5[20];
    int width;
} character_item;

character_item buffer[65536];

int characterThreshold = 0;   // This keeps the number of the highest order character from unicode table found in the font file that must be converted
                              // Each time someone put a higher order character from unicode table the font buffer to be put in PROGMEM will increase  
int characterIndex = 0;

int counter;

void Initialize_Character_Buffer(void)
{
    int i;
    
    for (i = 0; i < 65535; i++)
    {
        buffer[i].l1[0] = '0'; 
        buffer[i].l2[0] = '0'; 
        buffer[i].l3[0] = '0'; 
        buffer[i].l4[0] = '0'; 
        buffer[i].l5[0] = '0'; 
        buffer[i].width = 0; 
    }
}

unsigned char Get_Line_Lenght(char l[20])
{
    unsigned char ch;
    unsigned char line_lenght_counter = 0;
    
    while (l[line_lenght_counter] != 0x0a) // 0x0a = LF, line terminator in unix
        line_lenght_counter++;
        
    return line_lenght_counter--;    
}

void ConvertLine1ToBinaryAndWrite2Buffer(int bufferPosition, char l[20])
{
    unsigned char i = 0;
    
    if (l[i] == 0x0a) for (unsigned char j = 0; j < 8; j++) buffer[bufferPosition].l1[j] = '0';
    else { 
        while (l[i] != 0x0a)
        {
            if (l[i] == '*') buffer[bufferPosition].l1[i] = '1';
            else buffer[bufferPosition].l1[i] = '0';
            i++;  
        }
        
        for (unsigned char j = i; j < 8; j++) buffer[bufferPosition].l1[j] = '0';
    }
}
void ConvertLine2ToBinaryAndWrite2Buffer(int bufferPosition, char l[20])
{
    unsigned char i = 0;
    
    if (l[i] == 0x0a)  for (unsigned char j = 0; j < 8; j++) buffer[bufferPosition].l2[j] = '0';
    else { 
        while (l[i] != 0x0a)
        {
            if (l[i] == '*') buffer[bufferPosition].l2[i] = '1';
            else buffer[bufferPosition].l2[i] = '0';
            i++;  
        }
        
        for (unsigned char j = i; j < 8; j++) buffer[bufferPosition].l2[j] = '0';
    }
}
void ConvertLine3ToBinaryAndWrite2Buffer(int bufferPosition, char l[20])
{
    unsigned char i = 0;
    
    if (l[i] == 0x0a)  for (unsigned char j = 0; j < 8; j++) buffer[bufferPosition].l3[j] = '0';
    else { 
        while (l[i] != 0x0a)
        {
            if (l[i] == '*') buffer[bufferPosition].l3[i] = '1';
            else buffer[bufferPosition].l3[i] = '0';
            i++;  
        }
        
        for (unsigned char j = i; j < 8; j++) buffer[bufferPosition].l3[j] = '0';
    }
}
void ConvertLine4ToBinaryAndWrite2Buffer(int bufferPosition, char l[20])
{
    unsigned char i = 0;
    
    if (l[i] == 0x0a)  for (unsigned char j = 0; j < 8; j++) buffer[bufferPosition].l4[j] = '0';
    else { 
        while (l[i] != 0x0a)
        {
            if (l[i] == '*') buffer[bufferPosition].l4[i] = '1';
            else buffer[bufferPosition].l4[i] = '0';
            i++;  
        }
        
        for (unsigned char j = i; j < 8; j++) buffer[bufferPosition].l4[j] = '0';
    }
}
void ConvertLine5ToBinaryAndWrite2Buffer(int bufferPosition, char l[20])
{
    unsigned char i = 0;
    
    if (l[i] == 0x0a)  for (unsigned char j = 0; j < 8; j++) buffer[bufferPosition].l5[j] = '0';
    else { 
        while (l[i] != 0x0a)
        {
            if (l[i] == '*') buffer[bufferPosition].l5[i] = '1';
            else buffer[bufferPosition].l5[i] = '0';
            i++;  
        }
        
        for (unsigned char j = i; j < 8; j++) buffer[bufferPosition].l5[j] = '0';
    }
}


int main(void)
{
    FILE *in, *out;
    char line[20], current_line_lenght, pre_line_lenght;
    unsigned long char_position = 32;    
    
    if ((in = fopen("FONT.TXT", "r")) == NULL) 
    {
        printf("\n Input file can't be opened!\n");
        return 0;
    }
    
    if ((out = fopen("FONT.H", "w")) == NULL) 
    {
        printf("\n Output file can't be opened!\n"); 
        return 0;
    }
    
    // Clear all data in buffer to 0
    Initialize_Character_Buffer();
    
    
    while (!feof(in))
    {
        pre_line_lenght = 0;
        
        fgets(line, 20, in);   // Get the space      
        fgets(line, 20, in);   // Get the character index in unicode table (string format now)
        
        characterIndex = atoi(line);   // Convert the character index to int
        printf("%d \n", characterIndex);
        if (characterIndex > characterThreshold) characterThreshold = characterIndex;   // Check if the index is higher then the current highest threshold             
              
        fgets(line, 20, in);   // first line
        ConvertLine1ToBinaryAndWrite2Buffer(characterIndex, line);   // write current line to file in binary format
        current_line_lenght = Get_Line_Lenght(line);
        
        fgets(line, 20, in);   // second line
        ConvertLine2ToBinaryAndWrite2Buffer(characterIndex, line);   // write current line to file in binary format
        if ((current_line_lenght = Get_Line_Lenght(line)) > pre_line_lenght) 
            pre_line_lenght = current_line_lenght; 
        
        fgets(line, 20, in);   // third line
        ConvertLine3ToBinaryAndWrite2Buffer(characterIndex, line);   // write current line to file in binary format
        if ((current_line_lenght = Get_Line_Lenght(line)) > pre_line_lenght) 
            pre_line_lenght = current_line_lenght;       
        
        fgets(line, 20, in);   // fourth line
        ConvertLine4ToBinaryAndWrite2Buffer(characterIndex, line);   // write current line to file in binary format
        if ((current_line_lenght = Get_Line_Lenght(line)) > pre_line_lenght) 
            pre_line_lenght = current_line_lenght; 
        
        fgets(line, 20, in);   // fift line
        ConvertLine5ToBinaryAndWrite2Buffer(characterIndex, line);   // write current line to file in binary format
        if ((current_line_lenght = Get_Line_Lenght(line)) > pre_line_lenght) 
            pre_line_lenght = current_line_lenght;   // now we have the maximum width of current char 
        
        buffer[characterIndex].width = pre_line_lenght;  

        fgets(line, 20, in);   // -----
    }


    fputs("static const unsigned char FontLookup [][6] = { \n", out);   
    
    // Here we dump all data from buffer to output file
    for (counter = 1; counter <= characterThreshold; counter++) {
        if (buffer[counter].width == 0) {
            fputs("    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0", out);
        }    
        else {
            fputs("    {", out);
            fputs("0b", out); fputs(buffer[counter].l1, out); fputs(", ", out);
            fputs("0b", out); fputs(buffer[counter].l2, out); fputs(", ", out);
            fputs("0b", out); fputs(buffer[counter].l3, out); fputs(", ", out);
            fputs("0b", out); fputs(buffer[counter].l4, out); fputs(", ", out);
            fputs("0b", out); fputs(buffer[counter].l5, out); fputs(", ", out);
            fprintf(out, "%d", buffer[counter].width);
        }
        
        if (buffer[counter].width == 0) {
            fputs("},// CHARACTER UNDEFINED \n", out);
        }
        else {
            fputs("},// ", out); 
            fprintf(out, "%d", counter);
            fputs("\n", out); 
        }
    }
    
    fputs("}; \n", out);
    
    printf("\n Font converted.\n");
    printf("%d \n", characterThreshold);
    fclose(in);
    fclose(out);
    getch();
    return 0;
}
