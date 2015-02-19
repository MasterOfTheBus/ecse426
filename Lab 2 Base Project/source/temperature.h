// Variables to convert voltage to temperature
#define step_size (float)((3.0)/4096)
#define v_25 (float)0.76
#define avg_slope (float)0.0025

float voltage2temp(float voltage);
