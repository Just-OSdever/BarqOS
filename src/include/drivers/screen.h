#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <font16x16.h>


extern volatile struct limine_framebuffer_request framebuffer_request ;
extern struct limine_framebuffer *framebuffer; //Forward declaration

extern uint32_t cursor_x;
extern uint32_t cursor_y;
extern int current_y ;

void cls(uint32_t color);
void put_pixel(uint32_t x , uint32_t y , uint32_t color);
void framebuffer_init();
void draw_vertical_line( uint32_t X ,uint32_t color );
void draw_horizental_line( uint32_t Y ,uint32_t color ) ;
void print_char(char c , uint32_t color , int scale);
void print(const char *str , uint32_t color , int scale) ;
void hcf(void);
void print_centered(const char* text, int y, uint32_t color, int scale);
void print_step(char* text, uint32_t color, int scale);
void uint_to_string(uint64_t num, char* out_str);
void barqos_boot_splash();

extern int current_y;

#endif