#include <stdio.h>
#include <stdint.h>
#include "lp9khzFilter.h"

#include <stdio.h>
#include <assert.h>
 
#include "make_wav.h"
 
void intToFloat( int16_t *input, double *output, int length )
{
    int i;
 
    for ( i = 0; i < length; i++ ) {
        output[i] = (double)input[i];
    }
}
 
void floatToInt( double *input, int16_t *output, int length )
{
    int i;
 
    for ( i = 0; i < length; i++ ) {
        if ( input[i] > 32767.0 ) {
            input[i] = 32767.0;
        } else if ( input[i] < -32768.0 ) {
            input[i] = -32768.0;
        }
        // convert
        output[i] = (int16_t)input[i];
    }
}

lp9khzFilter LPF;
#define GRANULARITY 4096

int main( void )
{
    FILE   *in_fid;
    FILE   *out_fid;
	int size;
    int16_t input[GRANULARITY];
    int16_t output[GRANULARITY];
    double floatInput[GRANULARITY];
    double floatOutput[GRANULARITY];

 
    // open the input waveform file
    in_fid = fopen( "signal.wav", "rb" );
    if ( in_fid == 0 ) {
        printf("couldn't open signal.wav\n");
        return -1;
    }
 
    // open the output waveform file
    out_fid = fopen( "filtered-signal.wav", "wb" );
    if ( out_fid == 0 ) {
        printf("couldn't open filtered-signal.wav\n");
        return -1;
    }
 
    // initialize the filter
    lp9khzFilter_init(&LPF);
 
    // process all of the samples
    do {
		int i;
        // read samples from file
        size = fread( input, sizeof(int16_t), GRANULARITY, in_fid );
        // convert to doubles
        intToFloat( input, floatInput, size );
        // perform the filtering
		
		for(i=0; i<GRANULARITY; i++)
		{
			lp9khzFilter_put(&LPF, floatInput[i]);
			floatOutput[i] = lp9khzFilter_get(&LPF);
		}
        // convert to ints
        floatToInt( floatOutput, output, size );
        // write samples to file
        fwrite( output, sizeof(int16_t), size, out_fid );
    } while ( size != 0 );
 
    fclose( in_fid );
    fclose( out_fid );
 
    return 0;
}
