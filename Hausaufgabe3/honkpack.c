#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int read_byte(FILE* input, uint8_t* b)
{
	int result = fgetc(input);

	if (result == EOF)
	{
		if (!feof(input))
		{
			fprintf(stderr, "Failed to read from input.\n");
			exit(EXIT_FAILURE);
		}

		return 0;
	}

	*b = (uint8_t)result;
	return 1;
}

static void write_byte(FILE* output, uint8_t b)
{
	if (fputc(b, output) == EOF)
	{
		fprintf(stderr, "Failed to write to output.\n");
		exit(EXIT_FAILURE);
	}
}

typedef enum __state_t__
{
	STATE_IND,
	STATE_HOM,
	STATE_HET
} state_t;

static void compress(FILE* input, FILE* output) {

	uint8_t b1 = 0; //das erste Byte, welches eingelesen wird
	uint8_t b2 = 0;	//das zweite Byte, welches eingelesen wird
	uint8_t sb = 0; //das Statusbyte
	uint8_t puffer[127]; //Der Puffer, in den die Bytes einer heterogenen Gruppe geschoben werden, bis diese zu Ende ist
	size_t count = 0; 
	state_t state = STATE_IND;

	read_byte(input, &b1);
	read_byte(input, &b2);
	
	//Start der Kompression mit den ersten 2 Bytes des Bitstroms zur Initialisierung
	if (b1 == b2) {
		state = STATE_HOM;
		count = 2;
		
	}
	else {
		state = STATE_HET;
		count = 1;
		puffer[count-1] = b1;
	}

	b1 = b2;
	
	//Kompression der restlichen Daten
	while (read_byte(input, &b2)) {
		
		switch (state) {

			case STATE_HOM:

				if (b1 == b2) { //Homogene Gruppe setzt sich fort
					if (count < 127) {
						count++;
					}
					else { //Falls Grenze von 127 Byte erreicht: "Zwischenschreiben"
						sb |= (1 << 7);
						sb |= (uint8_t)count;
						write_byte(output, sb);
						write_byte(output, b1);
						count = 0;
						state = STATE_HOM;
					}
				}
				else { //Nun folgt entweder eine andere homogene Gruppe oder eine heterogene Gruppe
					sb |= (1 << 7);
					sb |= (uint8_t)count;
					write_byte(output, sb);
					write_byte(output, b1);
					count = 0;
					state = STATE_IND;
				}
			
			case STATE_HET:

				if (b1 == b2) { //Heterogene Gruppe beendet, also in output schreiben. Es folgt eine homogene Gruppe.
					sb &= ~ (1 << 7);
					sb |= (uint8_t)count;
					write_byte(output, sb);
					for (int i = 0; i < count; i++) {
						write_byte(output, puffer[i]);
					}
					count = 2;
					state = STATE_HOM;
				}
				else {
					if (count < 127) { //Heterogene Gruppe setzt sich fort
						puffer[count] = b1;
						count++;
					}
					else { //Falls Grenze von 127 Byte erreicht: "Zwischenschreiben"
						sb &= ~ (1 << 7);
						sb |= (uint8_t)count;
						write_byte(output, sb);
						for (int i = 0; i < count; i++) {
							write_byte(output, puffer[i]);
						}
						count = 0;
					}
					
				}

			case STATE_IND:

				if (b1 == b2) { 
					count = 2;
					state = STATE_HOM;
				}
				else {
					puffer[count] = b1;
					count++;
					state = STATE_HET;
				}
		}	

		//vorderes Byte zum hinteren Byte machen
		b1 = b2;
		
	}
}

static void decompress(FILE* input, FILE* output) {

	// Init the state machine.
	uint8_t b = 0;
	size_t count = 0;
	state_t state = STATE_IND;

	while (read_byte(input, &b))
	{
		switch (state)
		{
		case STATE_IND:

			count = (size_t)(b & 0x7F); // Binary: 0b01111111
			state = (b >> 7) ? STATE_HOM : (count > 0) ? STATE_HET : STATE_IND;

			break;

		case STATE_HOM:

			for (size_t i = 0; i < count; i++)
			{
				write_byte(output, b);
			}

			state = STATE_IND;
			break;

		case STATE_HET:

			write_byte(output, b);

			if (--count == 0)
			{
				state = STATE_IND;
			}

			break;
		}
	}

	// Validate the end state.
	if (state != STATE_IND)
	{
		fprintf(stderr, "Bad format!\n");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char** argv)
{
	
	FILE* input = stdin;
	FILE* output = stdout;
	
	//Auswahl, ob Kompression oder Dekompression
	if (strcmp(argv[1], "-d") == 0) {

		decompress(input, output);
	}
	else if (strcmp(argv[1], "-c") == 0){

		compress(input, output);
	}
	else {
		fprintf(stderr, "Kompression oder Dekompression? Verwenden Sie -c oder -d\n");
	}

	// Close the input / output stream.
	fclose(input);
	fclose(output);

	return 0;
}

