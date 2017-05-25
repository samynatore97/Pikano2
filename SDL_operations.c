#include "SDL_operations.h"
#include <stdio.h>

SDL_Surface* generateEmptySurface(size_t width,size_t height)
{
  SDL_Surface *result ;
  int isBOIsBigEndian;
  isBOIsBigEndian = (SDL_BYTEORDER == SDL_BIG_ENDIAN);
  result = SDL_CreateRGBSurface(SDL_SWSURFACE,width,height,32,
     (isBOIsBigEndian ? 0xff000000 : 0x000000ff),
	   (isBOIsBigEndian ? 0x00ff0000 : 0x0000ff00),
	   (isBOIsBigEndian ? 0x0000ff00 : 0x00ff0000),
	   (isBOIsBigEndian ? 0x000000ff : 0xff000000));
	return result;
}

static inline
Uint8* pixelref(SDL_Surface *surf, size_t x, size_t y) {
  int bpp = surf->format->BytesPerPixel;
  return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}
 
Uint32 getpixel(SDL_Surface *surface, size_t x, size_t y) {
  Uint8 *p = pixelref(surface, x, y);
  switch(surface->format->BytesPerPixel) {
  case 1:
    return *p;
  case 2:
    return *(Uint16 *)p;
  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
      return p[0] << 16 | p[1] << 8 | p[2];
    else
      return p[0] | p[1] << 8 | p[2] << 16;
  case 4:
    return *(Uint32 *)p;
  }
  return 0;
}
 
void putpixel(SDL_Surface *surface, size_t x, size_t y, Uint32 pixel) {
  Uint8 *p = pixelref(surface, x, y);
  switch(surface->format->BytesPerPixel) {
  case 1:
    *p = pixel;
    break;
  case 2:
    *(Uint16 *)p = pixel;
    break;
  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
      p[0] = (pixel >> 16) & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = pixel & 0xff;
    } else {
      p[0] = pixel & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = (pixel >> 16) & 0xff;
    }
    break;
  case 4:
    *(Uint32 *)p = pixel;
    break;
  }
} 
SDL_Surface * genImgFromMat(struct s_matrix *mat)
{
	SDL_Surface *result;
	size_t width;
	size_t height;
	Uint32 pix = 0 ;
	width = mat->cols;
	height = mat->lines;
	result = generateEmptySurface(width, height);
	for (size_t x = 0 ; x < height; x++)
	{
		for (size_t y = 0 ; y < width;y++)
		{
			switch (mat->data[x*mat->cols + y])
			{
				case NOIR :
					pix = SDL_MapRGB(result->format,0,0,0);
					break;
				case BLANC :
					pix = SDL_MapRGB(result->format,255,255,255);
					break;
				case BLEU1 :
					pix = SDL_MapRGB(result->format, 0, 0, 255); 
					break;
				case ROUGE :
					pix = SDL_MapRGB(result->format,255,0,0);
					break;
				case VERT :
					pix = SDL_MapRGB(result->format,0,255,0);
					break;
				case VERT2 :
					pix = SDL_MapRGB(result->format,0,255,0);
					break;
				case BLEU2 :
					pix = SDL_MapRGB(result->format,67,125,255);
					break;
				case ROSE :
					pix = SDL_MapRGB(result->format,154,43,70);
					break;
			}
			putpixel(result, y,x ,pix);
		}
	}
	return result;
}

struct s_matrix *genMatFromImg(SDL_Surface *img)
{
	size_t lines;
	size_t columns;
	struct s_matrix *result;
	Uint32 pix;
	Uint8 r;
	Uint8 g;
	Uint8 b;

	lines = img->h;
	columns = img->w;
	result = malloc(sizeof(struct s_matrix));
	result->lines = lines;
	result->cols =  columns;
	result->data = malloc(( lines * columns) * sizeof(enum Couleur));

	for (size_t x = 0 ; x < lines ; x++)
	{
		for (size_t y = 0 ; y < columns; y++)
		{
			r = 0;
			g = 0;
			b = 0;
			pix = getpixel(img, y, x);
			SDL_GetRGB(pix, img->format, &r,&g,&b);
			result->data[x * columns +y] = (enum Couleur) (r/255);
		}
	}
	return result;
}

SDL_Surface* generateGrayScaleImage(SDL_Surface *img)
{
  size_t width;
  size_t height;
  int sum;
  SDL_Surface *result;
  Uint32 pix;
  Uint8 r;
  Uint8 g;
  Uint8 b;

  width = img->w;
  height = img->h;
  result = generateEmptySurface(width, height);
  for (size_t x = 0; x < width; ++x)
  {
    for (size_t y = 0; y < height; ++y)
    {
      pix = getpixel(img, x, y);
      SDL_GetRGB(pix, img->format, &r, &g, &b);
      sum = 0.30f * r + 0.59f * g + 0.11f * b;
      pix = SDL_MapRGB(img->format, sum, sum, sum);
      putpixel(result, x, y, pix);
    }
  }
  return result;
}

SDL_Surface* generateBlackAndWhiteFromGS(SDL_Surface *img)
{
  size_t width;
  size_t height;
  int val;
  SDL_Surface *result;
  Uint32 pix;
  Uint8 r;
  Uint8 g;
  Uint8 b;

  width = img->w;
  height = img->h;
  result = generateEmptySurface(width, height);
  for (size_t x = 0; x < width; ++x)
  {
    for (size_t y = 0; y < height; ++y)
    {
      pix = getpixel(img, x, y);
      SDL_GetRGB(pix, img->format, &r, &g, &b);
      val = (r > 127 ? 255 : 0);
      pix = SDL_MapRGB(img->format, val, val, val);
      putpixel(result, x, y, pix);
    }
  }
  return result;
}

SDL_Surface* genBWImgFromMat(struct s_matrix *mat)
{
  SDL_Surface *result;
  size_t width;
  size_t height;
  Uint32 pix;
  Uint8 pixCol;

  width = mat->lines;
  height = mat->cols;
  result = generateEmptySurface(width, height);
  for (size_t x = 0; x < width; ++x)
  {
    for (size_t y = 0; y < height; ++y)
    {
      pixCol = (Uint8)(mat->data[x * height + y] * 255);
      pix = SDL_MapRGB(result->format, pixCol, pixCol, pixCol);
      putpixel(result, x, y, pix);
    }
  }
  return result;
}

SDL_Surface* load_image(char *path) {
  SDL_Surface          *img;
  // Load an image using SDL_image with format detection
  img = IMG_Load(path);
  if (!img)
    // If it fails, die with an error message
    errx(3, "can't load %s: %s", path, IMG_GetError());
  return img;
} 

void display_image(SDL_Surface* screen, SDL_Surface* img) {
 
  /* Blit onto the screen surface */
  if(SDL_BlitSurface(img, NULL, screen, NULL) < 0)
    warnx("BlitSurface error: %s\n", SDL_GetError());
 
  // Update the screen
  SDL_UpdateRect(screen, 0, 0, img->w, img->h);
 
  // wait for a key
//  wait_for_keypressed();
 
  // return the screen for further uses
}  

//renvoie l'img de l'histo a partir de la matrice de l'histo
SDL_Surface * build_img_histo_hori(struct s_matrix * histo)
{
  SDL_Surface * img ;
  img = genImgFromMat(histo);
  return img; 
}

SDL_Surface * build_img_color_line(struct s_matrix * mat_color_line)
{
  SDL_Surface * img ;
  img = genImgFromMat(mat_color_line);
  return img ; 
}

int is_rect_in(SDL_Rect* box1, SDL_Rect* box2)
{
  if (box1->y >= box2->y 
      && box1->y + box1->w <= box2->y + box2->w
      && box1->x >= box2->x
      && box1->x +box1->h <= box2->x + box2->h)
    return 1;
  else
    return 0;
}
