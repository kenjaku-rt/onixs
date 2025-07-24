#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

static inline void adc_init(void){
	ADMUX  |= (1 << REFS0);
	ADCSRA |= (1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0)|(1 << ADEN);
}

static inline uint16_t adc_read(uint8_t channel){
	ADMUX   = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1 << ADSC);
	while(ADCSRA & (1 << ADSC));
	return ADC;
}

#endif /* ADC_H_ */
