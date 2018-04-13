#ifndef FILTER_H_
#define FILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 31250 Hz

* 0 Hz - 9000 Hz
  gain = 1
  desired ripple = 1 dB
  actual ripple = 0.6818121735379787 dB

* 10000 Hz - 15625 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = -40.85046256179286 dB

*/

#define FILTER_TAP_NUM 50

typedef struct {
  double history[FILTER_TAP_NUM];
  unsigned int last_index;
} Filter;

void Filter_init(Filter* f);
void Filter_put(Filter* f, double input);
double Filter_get(Filter* f);

#endif

