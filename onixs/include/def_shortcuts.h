#ifndef DEF_SHORTCUTS_H_
#define DEF_SHORTCUTS_H_

#define MAKE_DDR(port_addr) (*(volatile uint8_t*)((port_addr - 1)))
#define MAKE_PORT(port_addr) (*(volatile uint8_t*)((port_addr)))
#define MAKE_PIN(port_addr) (*(volatile uint8_t*)((port_addr - 2)))

#endif /* DEF_SHORTCUTS_H_ */
