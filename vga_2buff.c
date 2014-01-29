//vga_part_a and b
//has 2 buffer
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_up_avalon_video_character_buffer_with_dma.h"
#include <stdio.h>
#include <system.h>

int main() {
	alt_up_pixel_buffer_dma_dev* pixel_buffer;

	// Use the name of your pixel buffer DMA core
	pixel_buffer = alt_up_pixel_buffer_dma_open_dev("/dev/pixel_buffer_dma");
	unsigned int pixel_buffer_addr1 = PIXEL_BUFFER_BASE;
	unsigned int pixel_buffer_addr2 = PIXEL_BUFFER_BASE + PIXEL_BUFFER_SPAN/2; //320*240*2

	// Set the 1st buffer address
	alt_up_pixel_buffer_dma_change_back_buffer_address(pixel_buffer,pixel_buffer_addr1);
	// Swap buffers – we have to swap because there is only an API function
	// to set the address of the background buffer.
	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
	while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));
	// Set the 2nd buffer address
	alt_up_pixel_buffer_dma_change_back_buffer_address(pixel_buffer,pixel_buffer_addr2);
	// Clear both buffers (this makes all pixels black)

	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 0);
	alt_up_pixel_buffer_dma_clear_screen(pixel_buffer, 1);

	// Draw a white line to the background buffer
	alt_up_pixel_buffer_dma_draw_line(pixel_buffer, 120, 120, 140, 120, 0xFFFF,1);
	alt_up_pixel_buffer_dma_draw_line(pixel_buffer, 120, 120, 120, 140, 0xaaaa,1);
	alt_up_pixel_buffer_dma_draw_line(pixel_buffer, 140, 120, 140, 140, 0xFFFF,1);
	alt_up_pixel_buffer_dma_draw_line(pixel_buffer, 120, 140, 140, 140, 0xaaaa,1);
	
	alt_up_pixel_buffer_dma_swap_buffers(pixel_buffer);
	while (alt_up_pixel_buffer_dma_check_swap_buffers_status(pixel_buffer));

	// Initialize
	alt_up_char_buffer_dev *char_buffer;
	char_buffer = alt_up_char_buffer_open_dev("/dev/char_drawer");


	alt_up_char_buffer_init(char_buffer);

	// Write some text
	alt_up_char_buffer_string(char_buffer, "EECE 381", 40, 30);
	alt_up_char_buffer_string(char_buffer, "EECE 381", 10, 30);
	alt_up_char_buffer_string(char_buffer, "EECE 381", 60, 30);

	return 0;
}
