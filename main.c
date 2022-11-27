#pragma pack(push)
#pragma pack(1)
#include "mlx.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>


void make_table(char table[766])
{
	char *syms = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
	//char *syms = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
	double len = (double)strlen(syms);

	for (int i=0; i<766; ++i)
	{
		table[i] = syms[(int)(i / 766.0 * len)];
	}
}

void    my_mlx_pixel_put(char*addr, int line_length,int bpp, int x, int y,
		int color)
{
    char    *dst;

    dst = addr + (y * line_length + x * (bpp / 8));
    *(unsigned int *)dst = color;
}

int close_win(void*)
{
	exit(0);
	return (0);
}
typedef struct {
    // Size of self
    uint32_t size;

    // Image dimensions in pixels
    int32_t width;
    int32_t height;

    // Image settings
    uint16_t color_planes;
    uint16_t color_depth;
    uint32_t compression;
    uint32_t raw_size; // This is generally unused

    // Resolution in pixels per metre
    int32_t horizontal_ppm;
    int32_t vertical_ppm;

    // Other settings
    uint32_t color_table;
    uint32_t important_colors;
} info_t;

typedef struct {
    // File signature
    uint16_t signature;

    // File size
    uint32_t size;

    // Reserved data
    uint16_t reservedA;
    uint16_t reservedB;

    // Location of the first pixel
    uint32_t data_offset;
} header_t;

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} pixel_t;

int color(pixel_t *p)
{
	int color = p->red << 16 | p->green << 8 | p->blue;
	//printf("%d\n",color);
	return (color);
}

void draw(info_t info, pixel_t pxs[info.height][info.width])
{
	//void *mlx=mlx_init();
	//void *win=mlx_new_window(mlx, 800, 800, "gend");
	//void *img=mlx_new_image(mlx, 1920, 1080);
	//int bpp;
	//int line_length;
	//int endian;
	char table[766];
	make_table(table);
	//char *addr=mlx_get_data_addr(img, &bpp, &line_length, &endian);
	int buffer_size = info.height * (1 + info.width) + 1;
	char buf[buffer_size];
	int current = -1;
	for (int i=0; i<info.height; ++i)
	{
		for (int j=0; j<info.width; ++j)
		{
			//my_mlx_pixel_put(addr, line_length, bpp, j, i, color(&pxs[i][j]));
			buf[++current] = table[pxs[i][j].red + pxs[i][j].green
									+ pxs[i][j].blue];
		}
		buf[++current] = '\n';
	}
	buf[++current] = 0;
	write(1, buf, current);
	//mlx_put_image_to_window(mlx, win, img, 0, 0);
	//mlx_hook(win, 17, 0, close_win, NULL);
	//mlx_loop(mlx);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    	exit(1);
    FILE* p_bmp = fopen(argv[1], "rb");

    header_t header;
    info_t info;

    fread(&header, sizeof(header), 1, p_bmp);
    fread(&info, sizeof(info), 1, p_bmp);

	int index = 0;
	int h = info.height - 1;
	int w = 0;
	pixel_t pxs[info.height][info.width];
	fseek(p_bmp, header.data_offset, SEEK_SET);
    // Read every pixel
    while(1){
        if(fread(&pxs[h][w], 3, 1, p_bmp) == 0) break;
        ++w;
        if (w == info.width)
        {
        	w = 0;
        	--h;
        	if (h == -1)
        		break;
        }
    }
	draw(info, pxs);
    return 0;
}

