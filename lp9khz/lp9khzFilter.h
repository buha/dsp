#ifndef LP9KHZFILTER_H_
#define LP9KHZFILTER_H_

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

#define LP9KHZFILTER_TAP_NUM 50

typedef struct {
  double history[LP9KHZFILTER_TAP_NUM];
  unsigned int last_index;
} lp9khzFilter;

void lp9khzFilter_init(lp9khzFilter* f);
void lp9khzFilter_put(lp9khzFilter* f, double input);
double lp9khzFilter_get(lp9khzFilter* f);

#endif

