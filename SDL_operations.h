# ifndef SDL_OPERATIONS_H_
# define SDL_OPERATIONS_H_
# include <stdlib.h>
# include <SDL/SDL.h>
# include <SDL/SDL_image.h>

# include "matrix.h"

/* 
** generateEmptySurface(width, height)
** size_t, size_t -> SDL_Surface*
** Returns an empty surface 
*/
SDL_Surface* generateEmptySurface(size_t width, size_t height);

/*
** generateGrayScaleImage(img)
** SDL_Surface* -> SDL_Surface*
** Return GrayScaledImage from <img>
*/
SDL_Surface* generateGrayScaleImage(SDL_Surface *img);

/*
** generateBlackAndWhiteFromGS(img)
** SDL_Surface* -> SDL_Surface*
** Return Black And White Image from GrayScaled Image
*/
SDL_Surface* generateBlackAndWhiteFromGS(SDL_Surface *img);

/*
** 
**
**
*/
SDL_Surface* copySurface(SDL_Surface *img);

/*
** genImgFromMat(mat)
** struct matrix* -> SDL_Surface*
** Returns the image generated from a matrix
*/
SDL_Surface* genImgFromMat(struct s_matrix *mat);

/*
** load_image(path)
** char* -> SDL_Surface*
** Returns the image generate from file
*/
SDL_Surface* load_image(char *path);

/*
** display_image(img)
** SDL_Surface* -> SDL_Surface*
** Displays the image on the screen
*/
void display_image(SDL_Surface* screen, SDL_Surface* img);

/*
** getpixel(img, x, y)
** SDL_Surface*, size_t, size_t -> Uint32;
** Returns the image's pixel on (x,y)
*/
Uint32 getpixel(SDL_Surface *img, size_t x, size_t y);

/*
** putpixel(img, x, y, pixel)
** SDL_Surface*, size_t, size_t, Uint32 -> void
** Puts the pixel on the image at (x,y)
*/
void putpixel(SDL_Surface *img, size_t x, size_t y, Uint32 pixel);

struct s_matrix *genMatFromImg(SDL_Surface *img);
int is_rect_in(SDL_Rect* box1, SDL_Rect* box2);

#endif
