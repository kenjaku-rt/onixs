#include <stdlib.h>
#include <avr/io.h>

#include <onixs.h>

int main(void) {
	/*
	static const onixs_device_t device = {
		
		.display = {
			.e_pin = 1,
			.rs_pin = 2,
			.rw_pin = 3,
			.bus_port_reg = &PORTD,
			.control_port_reg = &PORTC,
		},
		
		
	};
	TERMINATE_IN_FAILURE(onixs_init(device));
	*/
	usart_init(9600);
	usart_write_str("Start:\n");
	
	const static max6675_t config = {
		.port = &PORTB,
		.miso_pin = PB6,
		.mosi_pin = PB5,
		.sck_pin = PB7,
		.ss_pin = PB4,
	};
	
	max6675_init(config);
	
	char buffer[10];
	memset(buffer, '\0', 10 * sizeof(char));
	
	while (1) {
		usart_write_str("\nT = ");
		max6675_update(config);
		if (max6675_temp == INFINITY)
			usart_write_str("ERROR!");
		else
			usart_write_str(dtostrf(max6675_temp, 6, 3, buffer));
		_delay_ms(250);
	}

	
	return 0;
}
