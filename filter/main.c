#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <libgen.h>
#include "Filter.h"
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

int main(int argc, char * argv[])
{
	char prefix[] = "filtered-";	
	char * ifname = argv[1];
	
	if( argc < 1 || argc > 2 || access( ifname, F_OK ) == -1 )
	{
		printf("Usage: filter path/to/file.wav\n");
		return -1;
	} 
	
	char * ofname = (char *)malloc(strlen(prefix) + strlen(ifname) + 1);
    if (ofname == NULL) 
	{
		printf("Could not allocate memory.\n");
		return -1;
    } 
	strcpy(ofname, prefix);
	strcat(ofname, basename(ifname));

    // initialize the filter
	Filter LPF;
    Filter_init(&LPF);

	// open the input file
	drwav* wavIn = drwav_open_file(ifname);
    if (wavIn == NULL) 
	{
		printf("Could not open %s\n", ifname);
		return 0;
    } 

	// allocate memory for input/output buffers
	int16_t * input = (int16_t*)malloc((size_t)wavIn->totalSampleCount * sizeof(int16_t));
	int16_t * output = (int16_t*)malloc((size_t)wavIn->totalSampleCount * sizeof(int16_t));
    if (input == NULL || output == NULL) 
	{
		printf("Could not allocate memory.\n");
		return -1;
    } 

	printf("Total sample count: %ld\n", wavIn->totalSampleCount);	
	drwav_read_s16(wavIn, wavIn->totalSampleCount, input);

	// apply filter
	for(int i = 0; i < wavIn->totalSampleCount; i++)
	{
		Filter_put(&LPF, input[i]);
		output[i] = Filter_get(&LPF);
	}

	// open, configure and write output file
	drwav_data_format format;
	format.container = drwav_container_riff;
	format.format = DR_WAVE_FORMAT_PCM;
	format.channels = wavIn->fmt.channels;
	format.sampleRate = wavIn->fmt.sampleRate;
	format.bitsPerSample = wavIn->fmt.bitsPerSample;
	printf("ofname=%s\n", ofname);
	drwav * wavOut = drwav_open_file_write(ofname, &format);
	drwav_uint64 samplesWritten = drwav_write(wavOut, wavIn->totalSampleCount, output);
	printf("Wrote %ld samples\n", samplesWritten);

	// close files
	drwav_close(wavIn);
	drwav_close(wavOut);

    return 0;
}
