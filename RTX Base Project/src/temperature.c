#include "temperature.h"

float voltage2temp(float voltage) {
		// Convert to temperature
		float v_sense = (voltage*step_size);
		return (((v_sense - v_25) / avg_slope) + 25);
}
