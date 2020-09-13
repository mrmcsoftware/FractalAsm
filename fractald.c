/* Sample assembly language program for the x86 CPU by
   Mark Craig (mandelbrot and julia fractals)
   https://www.youtube.com/MrMcSoftware */

/* I chose to use inline assembly because I didn't want to code the Windows
   GUI stuff in assembly.  My original assembly language fractal program was
   written for a CPU I designed.  That version was all assembly and was the
   basis for this x86 version. */

#include <windows.h>
#include <stdio.h>
#include "dialog.h"

#define MANDELBROT 0
#define JULIA 1

	/* Windows GUI variables */

HINSTANCE hin;				/* instance handle */
WNDCLASS wc;				/* window class */
HWND w = NULL;				/* window */
HDC hdc, hdcimg = NULL;		/* window DC handle, fractal bitmap DC handle */
HBITMAP himg = NULL;		/* fractal bitmap handle */
POINTS pt0, pts, pte;		/* zoom: old point, start point, end point */

	/* Variables used in assembly part */

	/* some of these variables are vestiges of my original assembly language
	   fractal program for my CPU design which this x86 version is based on.
	   That CPU design was simpler than x86, so these vars. were necessary. */

unsigned char im[512*512*3];	/* Image buffer */
double aedge1 = -2.0, bedge1 = -1.25, xdelta1 = 3.0, ydelta1 = 2.5;
double aedge2 = -1.061955193, bedge2 = -.264645193;
double xdelta2 = .000230386, ydelta2 = .000230386;
double aedge3 = -1.2125216, bedge3 = -.3355226;
double xdelta3 = .1620652, ydelta3 = .1620652;
double aedge4 = -1.5, bedge4 = -1.4, xdelta4 = 3.0, ydelta4 = 2.8;
double aedge5 = 0.33285461273437479100, bedge5 = 0.57068114093749985900;
double xdelta5 = 0.1395625000000000000e-04, ydelta5 = 0.1395625000000000000e-04;
double a1 = -.8, b1 = .156, a2 = -.742, b2 = .1, a3 = -.4, b3 = .6;
double a4 = .285, b4 = .01, a5 = 0.0, b5 = -.8, a6 = -.835, b6 = -.2321;
double a7 = -.1, b7 = .651, a8 = .3, b8 = 0.0, a = -.8, b = .156;
double xres1 = 512.0, yres1 = 512.0, xres = 512.0, yres = 512.0;
double xres2 = 256.0, yres2 = 256.0;
double one = 1.0, two = 2.0, converge = 4.0, popped;
double m, n, xgap, ygap, ac, bc, az, bz, sizev, temp, tempb;
double aedge, bedge, xdelta, ydelta;
int numpixels = 512*512*3, numpixels1 = 512*512*3, numpixels2 = 512*256*3;
int off = 0, ncols = 255, count, limit = 1023;
int pixel, cyinc = 2, cydir = 1;
int resadd = 0, resadd1 = 0, resadd2 = 256*3;
int imaged[512*512];		/* "count" buffer used for color cycling */

	/* Rainbow colortable generated by HSV to RGB conversion (hue: 0 to 360) */

unsigned char Colors[256*3] = {
	0,0,0,
	255,6,0,
	255,12,0,
	255,18,0,
	255,24,0,
	255,30,0,
	255,36,0,
	255,42,0,
	255,48,0,
	255,54,0,
	255,60,0,
	255,66,0,
	255,72,0,
	255,78,0,
	255,84,0,
	255,90,0,
	255,96,0,
	255,102,0,
	255,108,0,
	255,114,0,
	255,120,0,
	255,126,0,
	255,132,0,
	255,138,0,
	255,144,0,
	255,150,0,
	255,156,0,
	255,162,0,
	255,168,0,
	255,174,0,
	255,180,0,
	255,186,0,
	255,192,0,
	255,198,0,
	255,204,0,
	255,210,0,
	255,216,0,
	255,222,0,
	255,228,0,
	255,234,0,
	255,240,0,
	255,246,0,
	255,252,0,
	252,255,0,
	246,255,0,
	239,255,0,
	233,255,0,
	227,255,0,
	221,255,0,
	215,255,0,
	209,255,0,
	203,255,0,
	197,255,0,
	191,255,0,
	185,255,0,
	179,255,0,
	173,255,0,
	167,255,0,
	161,255,0,
	155,255,0,
	149,255,0,
	143,255,0,
	137,255,0,
	131,255,0,
	125,255,0,
	119,255,0,
	113,255,0,
	107,255,0,
	101,255,0,
	95,255,0,
	89,255,0,
	83,255,0,
	77,255,0,
	71,255,0,
	65,255,0,
	59,255,0,
	53,255,0,
	47,255,0,
	41,255,0,
	35,255,0,
	29,255,0,
	23,255,0,
	17,255,0,
	11,255,0,
	5,255,0,
	0,255,0,
	0,255,6,
	0,255,12,
	0,255,18,
	0,255,24,
	0,255,30,
	0,255,36,
	0,255,42,
	0,255,48,
	0,255,54,
	0,255,60,
	0,255,66,
	0,255,72,
	0,255,78,
	0,255,84,
	0,255,90,
	0,255,96,
	0,255,102,
	0,255,108,
	0,255,114,
	0,255,120,
	0,255,126,
	0,255,132,
	0,255,138,
	0,255,144,
	0,255,150,
	0,255,156,
	0,255,162,
	0,255,168,
	0,255,174,
	0,255,180,
	0,255,186,
	0,255,192,
	0,255,198,
	0,255,204,
	0,255,210,
	0,255,216,
	0,255,222,
	0,255,228,
	0,255,234,
	0,255,240,
	0,255,246,
	0,255,252,
	0,251,255,
	0,245,255,
	0,239,255,
	0,233,255,
	0,227,255,
	0,221,255,
	0,215,255,
	0,209,255,
	0,203,255,
	0,197,255,
	0,191,255,
	0,185,255,
	0,179,255,
	0,173,255,
	0,167,255,
	0,161,255,
	0,155,255,
	0,149,255,
	0,143,255,
	0,137,255,
	0,131,255,
	0,125,255,
	0,119,255,
	0,113,255,
	0,107,255,
	0,101,255,
	0,95,255,
	0,89,255,
	0,83,255,
	0,77,255,
	0,71,255,
	0,65,255,
	0,59,255,
	0,53,255,
	0,47,255,
	0,41,255,
	0,35,255,
	0,29,255,
	0,23,255,
	0,17,255,
	0,11,255,
	0,5,255,
	0,0,255,
	6,0,255,
	12,0,255,
	18,0,255,
	24,0,255,
	30,0,255,
	36,0,255,
	42,0,255,
	48,0,255,
	54,0,255,
	60,0,255,
	66,0,255,
	72,0,255,
	78,0,255,
	84,0,255,
	90,0,255,
	96,0,255,
	102,0,255,
	108,0,255,
	114,0,255,
	120,0,255,
	126,0,255,
	132,0,255,
	138,0,255,
	144,0,255,
	150,0,255,
	156,0,255,
	162,0,255,
	168,0,255,
	174,0,255,
	180,0,255,
	186,0,255,
	192,0,255,
	198,0,255,
	204,0,255,
	210,0,255,
	216,0,255,
	222,0,255,
	228,0,255,
	234,0,255,
	240,0,255,
	246,0,255,
	252,0,255,
	255,0,251,
	255,0,245,
	255,0,239,
	255,0,233,
	255,0,227,
	255,0,221,
	255,0,215,
	255,0,209,
	255,0,203,
	255,0,197,
	255,0,191,
	255,0,185,
	255,0,179,
	255,0,173,
	255,0,167,
	255,0,161,
	255,0,155,
	255,0,149,
	255,0,143,
	255,0,137,
	255,0,131,
	255,0,125,
	255,0,119,
	255,0,113,
	255,0,107,
	255,0,101,
	255,0,95,
	255,0,89,
	255,0,83,
	255,0,77,
	255,0,71,
	255,0,65,
	255,0,59,
	255,0,53,
	255,0,47,
	255,0,41,
	255,0,35,
	255,0,29,
	255,0,23,
	255,0,17,
	255,0,11,
	255,0,5,
	255,0,5};

	/* Variables used in C part */

double aedgeUndo, bedgeUndo, xdeltaUndo, ydeltaUndo;	/* zoom undo vars */
int zoomgetpoint = 0, zoomtype, keepitsquare = 1;		/* zoom vars */
int speed = 0, dspeed = 10, pause = 0, cycle = 0;		/* color cycle vars */
int fractaltype;

	/* Now the good stuff.  BTW, I purposely chose to use basic x86 assembly -
	   no SSE*, no AVX-512, etc. */

void Mandelbrot()
{
__asm{
Mandelbrot:	mov pixel,0
			fld xdelta
			fdiv xres
			fstp xgap			; xgap=xdelta/xres
			fld ydelta
			fdiv yres
			fstp ygap			; ygap=ydelta/yres
			fldz
Loopm:		fst m				; for (m=0;m<yres;m++)
			fmul ygap
			fadd bedge
			fstp bc				; bc=m*ygap+bedge
			fldz
Loopn:		fst n				; for (n=0;n<xres;n++)
			fmul xgap
			fadd aedge
			fstp ac				; ac=n*xgap+aedge
			fldz
			fst az				; az=0
			fst bz				; bz=0
			fstp sizev			; sizev=0 (size must be reserved word)
			mov count,0			; count=0
Loopw:		fld sizev
			fcomp converge
			fnstsw ax
			test ah,65
			je Exitw
			mov eax,limit
			cmp count,eax		; was cmp count,limit when limit was #define
			jg Exitw			; while (size<=converge && count<=limit)
			fld az
			fst temp			; temp=az
			fmul az
			fadd ac
			fstp az
			fld bz
			fmul bz
			fstp tempb
			fld az
			fsub tempb
			fstp az				; az=az*az-bz*bz+ac
			fld bz
			fmul temp
			fmul two
			fadd bc
			fstp bz				; bz=temp*bz*2+bc
			add count,1			; count++
			fld az
			fmul az
			fstp tempb
			fld bz
			fmul bz
			fadd tempb
			fstp sizev			; size=(az*az+bz*bz)
			jmp Loopw			; end while
Exitw:		mov eax,limit
			cmp count,eax
			jle ok
			mov count,0			; if (count>limit) count=0
			jmp ok2
ok:			mov eax,off
			add count,eax
			mov eax,count		;
			mov edx,0			;
			idiv ncols			; MODI 255
			mov count,edx		;
			add count,1			; else count=(count+off)%255+1
ok2:		mov eax,count
			mov ebx,[pixel]
			mov imaged[ebx],eax	; save count for color cycling (not aligned)
			imul eax,3
			add eax,2
			mov cl,Colors[eax]
			sub eax,1
			mov ebx,[pixel]
			mov im[ebx],cl
			add pixel,1
			mov cl,Colors[eax]
			sub eax,1
			mov ebx,[pixel]
			mov im[ebx],cl
			add pixel,1
			mov cl,Colors[eax]
			mov ebx,[pixel]
			mov im[ebx],cl
			add pixel,1
			fld n
			fadd one
			fst n
			fcom xres
			fnstsw ax
			test ah,1
			jne Loopn			; end Loop n
			mov eax,resadd
			add pixel,eax
			fstp popped
			fld m
			fadd one
			fst m
			fcom yres
			fnstsw ax
			test ah,1
			jne Loopm			; end Loop m
			fstp popped
			;call Cycle
	}
ChangeBitmap(himg, im);
BitBlt(hdc, 0, 0, 512, 512, hdcimg, 0, 0, SRCCOPY);
}
			
void Julia()
{
__asm{
Julia:		mov pixel,0
			fld xdelta
			fdiv xres
			fstp xgap			; xgap=xdelta/xres
			fld ydelta
			fdiv yres
			fstp ygap			; ygap=ydelta/yres
			fldz
Loopmj:		fst m				; for (m=0;m<yres;m++)
			fmul ygap
			fadd bedge
			fstp bc				; bc=m*ygap+bedge
			fldz
Loopnj:		fst n				; for (n=0;n<xres;n++)
			fmul xgap
			fadd aedge
			fst ac				; ac=n*xgap+aedge
			fstp az				; az=ac
			fld bc
			fstp bz				; bz=bc
			fldz
			fstp sizev			; sizev=0 (size must be reserved word)
			mov count,0			; count=0
Loopwj:		fld sizev
			fcomp converge
			fnstsw ax
			test ah,65
			je Exitwj
			mov eax,limit
			cmp count,eax
			jg Exitwj			; while (size<=converge && count<=limit)
			fld az
			fst temp			; temp=az
			fmul az
			fadd a
			fstp az
			fld bz
			fmul bz
			fstp tempb
			fld az
			fsub tempb
			fstp az				; az=az*az-bz*bz+a
			fld bz
			fmul temp
			fmul two
			fadd b
			fstp bz				; bz=temp*bz*2+b
			add count,1			; count++
			fld az
			fmul az
			fstp tempb
			fld bz
			fmul bz
			fadd tempb
			fstp sizev			; size=(az*az+bz*bz)
			jmp Loopwj			; end while
Exitwj:		mov eax,limit
			cmp count,eax
			jle okj
			mov count,0			; if (count>limit) count=0
			jmp ok2j
okj:		mov eax,off
			add count,eax
			mov eax,count		;
			mov edx,0			;
			idiv ncols			; MODI 255
			mov count,edx		;
			add count,1			; else count=(count+off)%255+1
ok2j:		mov eax,count
			mov ebx,[pixel]
			mov imaged[ebx],eax	; save count for color cycling (not aligned)
			imul eax,3
			add eax,2
			mov cl,Colors[eax]
			sub eax,1
			mov ebx,[pixel]
			mov im[ebx],cl
			add pixel,1
			mov cl,Colors[eax]
			sub eax,1
			mov ebx,[pixel]
			mov im[ebx],cl
			add pixel,1
			mov cl,Colors[eax]
			mov ebx,[pixel]
			mov im[ebx],cl
			add pixel,1
			fld n
			fadd one
			fst n
			fcom xres
			fnstsw ax
			test ah,1
			jne Loopnj			; end Loop n
			mov eax,resadd
			add pixel,eax
			fstp popped
			fld m
			fadd one
			fst m
			fcom yres
			fnstsw ax
			test ah,1
			jne Loopmj			; end Loop m
			fstp popped
			;call Cycle
	}
ChangeBitmap(himg, im);
BitBlt(hdc, 0, 0, 512, 512, hdcimg, 0, 0, SRCCOPY);
}
			
	/* Original menu options */

void Choice1()
{
__asm{
Choice1:	fld aedge1
			fstp aedge
			fld bedge1
			fstp bedge
			fld xdelta1
			fstp xdelta
			fld ydelta1
			fstp ydelta
			mov fractaltype,MANDELBROT
			call Mandelbrot
	}
}

void Choice2()
{
__asm{
Choice2:	fld aedge2
			fstp aedge
			fld bedge2
			fstp bedge
			fld xdelta2
			fstp xdelta
			fld ydelta2
			fstp ydelta
			mov fractaltype,MANDELBROT
			call Mandelbrot
	}
}

void Choice3()
{
__asm{
Choice3:	fld aedge3
			fstp aedge
			fld bedge3
			fstp bedge
			fld xdelta3
			fstp xdelta
			fld ydelta3
			fstp ydelta
			mov fractaltype,MANDELBROT
			call Mandelbrot
	}
}

void Choice4()
{
__asm{
Choice4:	fld aedge4
			fstp aedge
			fld bedge4
			fstp bedge
			fld xdelta4
			fstp xdelta
			fld ydelta4
			fstp ydelta
			fld a1
			fstp a
			fld b1
			fstp b
			mov fractaltype,JULIA
			call Julia
	}
}

void Choice5()
{
__asm{
Choice5:	fld aedge4
			fstp aedge
			fld bedge4
			fstp bedge
			fld xdelta4
			fstp xdelta
			fld ydelta4
			fstp ydelta
			fld a2
			fstp a
			fld b2
			fstp b
			mov fractaltype,JULIA
			call Julia
	}
}

void Choice6()
{
__asm{
Choice6:	fld aedge4
			fstp aedge
			fld bedge4
			fstp bedge
			fld xdelta4
			fstp xdelta
			fld ydelta4
			fstp ydelta
			fld a3
			fstp a
			fld b3
			fstp b
			mov fractaltype,JULIA
			call Julia
	}
}

void Choice7()
{
__asm{
Choice7:	fld aedge4
			fstp aedge
			fld bedge4
			fstp bedge
			fld xdelta4
			fstp xdelta
			fld ydelta4
			fstp ydelta
			fld a4
			fstp a
			fld b4
			fstp b
			mov fractaltype,JULIA
			call Julia
	}
}

void Choice8()
{
__asm{
Choice8:	fld aedge4
			fstp aedge
			fld bedge4
			fstp bedge
			fld xdelta4
			fstp xdelta
			fld ydelta4
			fstp ydelta
			fld a5
			fstp a
			fld b5
			fstp b
			mov fractaltype,JULIA
			call Julia
	}
}

void Choice9()
{
__asm{
Choice9:	fld aedge4
			fstp aedge
			fld bedge4
			fstp bedge
			fld xdelta4
			fstp xdelta
			fld ydelta4
			fstp ydelta
			fld a6
			fstp a
			fld b6
			fstp b
			mov fractaltype,JULIA
			call Julia
	}
}

void Choice0()
{
__asm{
Choice0:	fld aedge4
			fstp aedge
			fld bedge4
			fstp bedge
			fld xdelta4
			fstp xdelta
			fld ydelta4
			fstp ydelta
			fld a7
			fstp a
			fld b7
			fstp b
			mov fractaltype,JULIA
			call Julia
	}
}

void Choicea()
{
__asm{
Choicea:	fld aedge4
			fstp aedge
			fld bedge4
			fstp bedge
			fld xdelta4
			fstp xdelta
			fld ydelta4
			fstp ydelta
			fld a8
			fstp a
			fld b8
			fstp b
			mov fractaltype,JULIA
			call Julia
	}
}

void Choiceb()
{
__asm{
Choiceb:	fld xres
			fcomp xres1
			fnstsw ax
			test ah,64
			jne To128
			fld xres1
			fstp xres
			fld yres1
			fstp yres
			mov eax,numpixels1
			mov numpixels,eax
			mov eax,resadd1
			mov resadd,eax
			jmp resend
To128:		fld xres2
			fstp xres
			fld yres2
			fstp yres
			mov eax,numpixels2
			mov numpixels,eax
			mov eax,resadd2
			mov resadd,eax
resend:
	}
}

void Choicem()
{
__asm{
Choicem:	fld aedge5
			fstp aedge
			fld bedge5
			fstp bedge
			fld xdelta5
			fstp xdelta
			fld ydelta5
			fstp ydelta
			mov fractaltype,MANDELBROT
			call Mandelbrot
	}
}

	/* Colortable cycling */

void Cycle()
{
__asm{
Cycle:
Loopc0:		mov eax,cyinc
			mul cydir
			add [off],eax
			mov pixel,0
Loopc:		mov ebx,pixel
			mov ecx,imaged[ebx]
			mov count,ecx
			cmp count,0
			je ok3
			mov eax,off
			add count,eax
			mov eax,count
			mov edx,0
			idiv ncols
			mov count,edx
			add count,1
ok3:		mov eax,count
			imul eax,3
			add eax,2
			mov cl,Colors[eax]
			sub eax,1
			mov ebx,[pixel]
			mov im[ebx],cl
			add pixel,1
			mov cl,Colors[eax]
			sub eax,1
			mov ebx,[pixel]
			mov im[ebx],cl
			add pixel,1
			mov cl,Colors[eax]
			mov ebx,[pixel]
			mov im[ebx],cl
			add pixel,1
			mov eax,numpixels
			cmp pixel,eax
			jl Loopc
	}
ChangeBitmap(himg, im);
BitBlt(hdc, 0, 0, 512, 512, hdcimg, 0, 0, SRCCOPY);
}

	/* Windows GUI stuff */

ChangeBitmap(HBITMAP b, unsigned char *bits)
{
BITMAP bm;
BITMAPINFOHEADER bi;

/* would be better to have bi be a global variable and only set once.
   Then could call SetDIBits directly instead of ChangeBitmap. */

GetObject(b, sizeof(bm), &bm);
bi.biSize = sizeof(BITMAPINFOHEADER);
bi.biWidth = bm.bmWidth;
bi.biHeight = -bm.bmHeight;		// Negative to fix upside-down BMP orientation
bi.biPlanes = 1;
bi.biBitCount = 24;
bi.biCompression = BI_RGB;
bi.biSizeImage = 0;
bi.biXPelsPerMeter = 0;
bi.biYPelsPerMeter = 0;
bi.biClrUsed = 0;
bi.biClrImportant = 0;
SetDIBits(hdc, b, 0, bm.bmHeight, bits, &bi, DIB_RGB_COLORS);
}

QuitProgram()
{
if (w) { DestroyWindow(w); w = NULL; }
if (himg) { DeleteObject(himg); }
if (hdcimg) { DeleteDC(hdcimg); }
}

ClearScreen()
{
SelectObject(hdc, wc.hbrBackground);
PatBlt(hdc, 0, 0, 512, 512, PATCOPY);
memset(im, 0, 512*512*3);
memset(imaged, 0, 512*512*4);
ChangeBitmap(himg, im);
}

int GetFilename(char *str)
{
OPENFILENAME fna;
char path[1024],cwd[256];
int i;

GetCurrentDirectory(256, cwd);
str[0] = '\0';
memset(&fna, 0, sizeof(OPENFILENAME));
fna.lStructSize = sizeof(OPENFILENAME);
fna.hwndOwner = w;
fna.lpstrFilter = "All Files\0*.*\0PPM Files\0*.ppm\0";
fna.lpstrInitialDir = cwd;
fna.nFilterIndex = 1;
fna.lpstrFile = str;
fna.nMaxFile = MAX_PATH;
fna.lpstrTitle = "Save To:";
fna.Flags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
GetSaveFileName(&fna);
if (str[0] == '\0') return(1);
if (fna.nFileExtension == 0) { strcat(str, ".ppm"); }
return(0);
}

SaveImage()
{
FILE *fp;
char str[256];
unsigned char *p = im;

if (GetFilename(str)) { return; }
if ((fp = fopen(str,"wb")) == NULL)
	{
	MessageBox(w, "Can't save to desired file", "Error", MB_ICONERROR|MB_APPLMODAL);
	return;
	}
fprintf(fp, "P6\n512 512\n255\n");
for (n=0; n<512; n++)
	for (m=0; m<512; m++)
		{
		fputc(*(p+2), fp);
		fputc(*(p+1), fp);
		fputc(*(p), fp);
		p+=3;
		}
fclose(fp);
}

int PASCAL DialogParams(HWND w0, unsigned msg, WPARAM wp, LPARAM lp)
{
char str[256];

switch (msg)
	{
	case WM_INITDIALOG:
		sprintf(str, "%.10lf", aedge); SetDlgItemText(w0, IDD_AEDGE, str);
		sprintf(str, "%.10lf", bedge); SetDlgItemText(w0, IDD_BEDGE, str);
		sprintf(str, "%.10lf", xdelta); SetDlgItemText(w0, IDD_XDELTA, str);
		sprintf(str, "%.10lf", ydelta); SetDlgItemText(w0, IDD_YDELTA, str);
		sprintf(str, "%.10lf", a); SetDlgItemText(w0, IDD_A, str);
		sprintf(str, "%.10lf", b); SetDlgItemText(w0, IDD_B, str);
		sprintf(str, "%d", limit); SetDlgItemText(w0, IDD_LIMIT, str);
		CheckDlgButton(w0, IDD_MANDELBROT, !fractaltype);
		CheckDlgButton(w0, IDD_JULIA, fractaltype);
		if (fractaltype == MANDELBROT) { SendDlgItemMessage(w0, IDD_A, EM_SETREADONLY, 1, 0); SendDlgItemMessage(w0, IDD_B, EM_SETREADONLY, 1, 0); }
		return(TRUE);
	case WM_COMMAND: 
		switch (LOWORD(wp))
			{
			case IDOK: EndDialog(w0, TRUE); if (fractaltype == MANDELBROT) { Mandelbrot(); } else { Julia(); } return(TRUE); break;
			case IDCANCEL: EndDialog(w0, TRUE); return(TRUE); break;
			case IDD_AEDGE: GetDlgItemText(w0, IDD_AEDGE, str, 256); sscanf(str, "%lf", &aedge); break;
			case IDD_BEDGE: GetDlgItemText(w0, IDD_BEDGE, str, 256); sscanf(str, "%lf", &bedge); break;
			case IDD_XDELTA: GetDlgItemText(w0, IDD_XDELTA, str, 256); sscanf(str, "%lf", &xdelta); break;
			case IDD_YDELTA: GetDlgItemText(w0, IDD_YDELTA, str, 256); sscanf(str, "%lf", &ydelta); break;
			case IDD_A: GetDlgItemText(w0, IDD_A, str, 256); sscanf(str, "%lf", &a); break;
			case IDD_B: GetDlgItemText(w0, IDD_B, str, 256); sscanf(str, "%lf", &b); break;
			case IDD_LIMIT: GetDlgItemText(w0, IDD_LIMIT, str, 256); sscanf(str, "%d", &limit); break;
			case IDD_MANDELBROT: fractaltype = MANDELBROT;
				SendDlgItemMessage(w0, IDD_A, EM_SETREADONLY, 1, 0); SendDlgItemMessage(w0, IDD_B, EM_SETREADONLY, 1, 0);
				break;
			case IDD_JULIA: fractaltype = JULIA;
				SendDlgItemMessage(w0, IDD_A, EM_SETREADONLY, 0, 0); SendDlgItemMessage(w0, IDD_B, EM_SETREADONLY, 0, 0);
				break;
			}
		return(TRUE);
	}
return(FALSE);
}

long FAR PASCAL WindowProcedure(HWND w, unsigned msg, WPARAM wp, LPARAM lp)
{
PAINTSTRUCT paints;
DLGPROC getdata;
HMENU smenu;
WORD wplow;
HDC h;
POINTS pt;
double ae,be,ae2,be2;
int dx,dy;
char str[256];

switch (msg)
	{
	case WM_CREATE:
		hdc = GetDC(w);
		hdcimg = CreateCompatibleDC(hdc);
		himg = CreateBitmap(512, 512, 1, 32, NULL);
		SelectObject(hdcimg, himg);
		smenu = GetSystemMenu(w, 0);
		InsertMenu(smenu, -1, MF_BYPOSITION|MF_SEPARATOR, 0, 0);
		InsertMenu(smenu, -1, MF_BYPOSITION|MF_STRING, IDM_SAVE, "Save image as PPM");
		Choice1();
		break;
	case WM_PAINT:
		h = BeginPaint(w, &paints);
		BitBlt(h, 0, 0, 512, 512, hdcimg, 0, 0, SRCCOPY);
		EndPaint(w, &paints);
		return(FALSE);
	case WM_KEYDOWN:
		wplow = LOWORD(wp);
		switch (wplow)
			{
			case '0': Choice0(); break;
			case '1': Choice1(); break;
			case '2': Choice2(); break;
			case '3': Choice3(); break;
			case '4': Choice4(); break;
			case '5': Choice5(); break;
			case '6': Choice6(); break;
			case '7': Choice7(); break;
			case '8': Choice8(); break;
			case '9': Choice9(); break;
			case 'A': Choicea(); break;
			case 'B': Choiceb(); ClearScreen(); break;
			case 189: Choicem(); break; // -
			case 'Q': DestroyWindow(w); w = NULL; break;
			case 'C': cycle = !cycle; break;
			case 'D': if (cydir == 1) { cydir = -1; } else { cydir = 1; } break;
			case ' ': pause = !pause; break;
			case 188: cyinc-=2; break; // ,
			case 190: cyinc+=2; break; // .
			case VK_LEFT: speed-=dspeed; if (speed < 0) { speed = 0; } break;
			case VK_RIGHT: speed+=dspeed; break;
			case 'Z': zoomgetpoint = 1; zoomtype = 0; pt0.x = -1; break;
			case 'X': zoomgetpoint = 1; zoomtype = 1; pt0.x = -1; break;
			case 'U': aedge = aedgeUndo; bedge = bedgeUndo;
				xdelta = xdeltaUndo; ydelta = ydeltaUndo;
				if (fractaltype == MANDELBROT) { Mandelbrot(); } else { Julia(); }
				break;
			case 'K': keepitsquare = !keepitsquare; break;
			case 'P':
				getdata = MakeProcInstance((DLGPROC)DialogParams, hin);
				DialogBox(hin, "Params", w, getdata);
				FreeProcInstance(getdata);
				break;
			case 'R': if (fractaltype == MANDELBROT) { Mandelbrot(); } else { Julia(); } break;
			case 'H':
				MessageBox(w,
					"\
1 - Mandelbrot location 1\n\
2 - Mandelbrot location 2\n\
3 - Mandelbrot location 3\n\
4 - Julia location 1\n\
5 - Julia location 2\n\
6 - Julia location 3\n\
7 - Julia location 4\n\
8 - Julia location 5\n\
9 - Julia location 6\n\
0 - Julia location 7\n\
a - Julia location 8\n\
b - Toggle size\n\
- - Mandelbrot location 4\n\
r - Recalculate/redraw (use after 'b', for example)\n\
z - Zoom by opposing corners\n\
x - Zoom by center to corner\n\
u - Undo zoom\n\
k - Toggle zoom square/free\n\
c - Toggle color cycle\n\
d - Change cycle direction\n\
, - Decrease cycle delta\n\
. - Increase cycle delta\n\
{left} - Increase cycle speed\n\
{right} - Decrease cycle speed\n\
{space} - Pause cycle\n\
h - Help\n\
q - Quit\n\
					", "Keyboard Controls", MB_ICONINFORMATION|MB_APPLMODAL);
				break;
			}
		break;
	case WM_MOUSEMOVE:
		if (zoomgetpoint == 1)
			{
			pt = MAKEPOINTS(lp);
			if (pt0.x >= 0)
				{
				PatBlt(hdc, 0, pt0.y, 512, 1, DSTINVERT);
				PatBlt(hdc, pt0.x, 0, 1, 512, DSTINVERT);
				}
			PatBlt(hdc, 0, pt.y, 512, 1, DSTINVERT);
			PatBlt(hdc, pt.x, 0, 1, 512, DSTINVERT);
			ae = aedge+xdelta*(double)(pt.x)/(double)(512);
			be = bedge+ydelta*(double)(pt.y)/(double)(512);
			sprintf(str, "%.10lf %.10lf", ae, be);
			SetWindowText(w, str);
			pt0 = pt;
			}
		else if (zoomgetpoint == 2)
			{
			if (zoomtype)	/* zoom by center to corner */
				{
				if (pte.x >= 0)
					{
					dx = abs(pte.x-pts.x); dy = abs(pte.y-pts.y);
					PatBlt(hdc, pts.x-dx, pts.y-dy, dx*2, 1, DSTINVERT);
					PatBlt(hdc, pts.x+dx, pts.y-dy+1, 1, dy*2-2, DSTINVERT);
					PatBlt(hdc, pts.x-dx, pts.y-dy+1, 1, dy*2-2, DSTINVERT);
					PatBlt(hdc, pts.x-dx, pts.y+dy, dx*2, 1, DSTINVERT);
					}
				pte = MAKEPOINTS(lp); if (keepitsquare) { pte.y = pts.y+(pte.x-pts.x); }
				dx = abs(pte.x-pts.x); dy = abs(pte.y-pts.y);
				PatBlt(hdc, pts.x-dx, pts.y-dy, dx*2, 1, DSTINVERT);
				PatBlt(hdc, pts.x+dx, pts.y-dy+1, 1, dy*2-2, DSTINVERT);
				PatBlt(hdc, pts.x-dx, pts.y-dy+1, 1, dy*2-2, DSTINVERT);
				PatBlt(hdc, pts.x-dx, pts.y+dy, dx*2, 1, DSTINVERT);
				ae = aedge+xdelta*(double)(pts.x-dx)/(double)(512);
				be = bedge+ydelta*(double)(pts.y-dy)/(double)(512);
				ae2 = aedge+xdelta*(double)(pts.x+dx)/(double)(512);
				be2 = bedge+ydelta*(double)(pts.y+dy)/(double)(512);
				}
			else			/* zoom by corner to opposing corner */
				{
				if (pte.x >= 0)
					{
					PatBlt(hdc, pts.x, pts.y, (pte.x-pts.x), 1, DSTINVERT);
					PatBlt(hdc, pts.x, pte.y, 1, -(pte.y-pts.y), DSTINVERT);
					PatBlt(hdc, pte.x-1, pts.y, 1, (pte.y-pts.y), DSTINVERT);
					PatBlt(hdc, pte.x, pte.y-1, -(pte.x-pts.x), 1, DSTINVERT);
					}
				pte = MAKEPOINTS(lp); if (keepitsquare) { pte.y = pts.y+(pte.x-pts.x); }
				PatBlt(hdc, pts.x, pts.y, (pte.x-pts.x), 1, DSTINVERT);
				PatBlt(hdc, pts.x, pte.y, 1, -(pte.y-pts.y), DSTINVERT);
				PatBlt(hdc, pte.x-1, pts.y, 1, (pte.y-pts.y), DSTINVERT);
				PatBlt(hdc, pte.x, pte.y-1, -(pte.x-pts.x), 1, DSTINVERT);
				ae = aedge+xdelta*(double)(pts.x)/(double)(512);
				be = bedge+ydelta*(double)(pts.y)/(double)(512);
				ae2 = aedge+xdelta*(double)(pte.x)/(double)(512);
				be2 = bedge+ydelta*(double)(pte.y)/(double)(512);
				}
			sprintf(str, "%.10lf %.10lf to %.10lf %.10lf (%.10lf %.10lf)", ae, be, ae2, be2, ae2-ae, be2-be);
			SetWindowText(w, str);
			}
		return(FALSE);
	case WM_LBUTTONDOWN:
		if (zoomgetpoint == 1)
			{
			if (pt0.x >= 0)
				{
				PatBlt(hdc, 0, pt0.y, 512, 1, DSTINVERT);
				PatBlt(hdc, pt0.x, 0, 1, 512, DSTINVERT);
				}
			pts = MAKEPOINTS(lp); zoomgetpoint = 2; pte.x = -1;
			break;
			}
		break;
	case WM_LBUTTONUP:
		if (zoomgetpoint == 2)
			{
			aedgeUndo = aedge; bedgeUndo = bedge;
			xdeltaUndo = xdelta; ydeltaUndo = ydelta;
			if (zoomtype)	/* zoom by center to corner */
				{
				dx = abs(pte.x-pts.x); dy = abs(pte.y-pts.y);
				ae = aedge+xdelta*(double)(pts.x-dx)/(double)(512);
				be = bedge+ydelta*(double)(pts.y-dy)/(double)(512);
				ae2 = aedge+xdelta*(double)(pts.x+dx)/(double)(512);
				be2 = bedge+ydelta*(double)(pts.y+dy)/(double)(512);
				}
			else			/* zoom by corner to opposing corner */
				{
				ae = aedge+xdelta*(double)(pts.x)/(double)(512);
				be = bedge+ydelta*(double)(pts.y)/(double)(512);
				ae2 = aedge+xdelta*(double)(pte.x)/(double)(512);
				be2 = bedge+ydelta*(double)(pte.y)/(double)(512);
				}
			aedge = ae; bedge = be;
			xdelta = ae2-ae; ydelta = be2-be;
			zoomgetpoint = 0;
			if (fractaltype == MANDELBROT) { Mandelbrot(); } else { Julia(); }
			SetWindowText(w, "Mark's x86 Asm. Fractals");
			}
		break;
	case WM_CLOSE: DestroyWindow(w); w = NULL; break;
	case WM_DESTROY: PostQuitMessage(0); break;
	case WM_SYSCOMMAND:
		wplow = LOWORD(wp);
		if (wplow == IDM_SAVE) { SaveImage(); }
		else { return(DefWindowProc(w, msg, wp, lp)); }
		break;
	default: return(DefWindowProc(w, msg, wp, lp));
	}
return(0L);
}

int PASCAL WinMain(HINSTANCE hi, HINSTANCE hpi, LPSTR cmd, int scmd)
{
int i;
MSG msg;
DWORD dwStyle = WS_OVERLAPPEDWINDOW;
RECT rc={0, 0, 512, 512};

wc.style = 0;
wc.lpfnWndProc = (WNDPROC)WindowProcedure;
wc.cbClsExtra = 0; wc.cbWndExtra = 0; wc.hInstance = hi;
wc.hIcon = LoadIcon(hi, "Icon");
wc.hCursor = LoadCursor(NULL, IDC_ARROW);
wc.hbrBackground = GetStockObject(BLACK_BRUSH);
wc.lpszMenuName = NULL;
wc.lpszClassName = "FractClass";
if (!RegisterClass(&wc)) { QuitProgram(); }
hin=hi;
AdjustWindowRect(&rc, dwStyle, FALSE);
w = CreateWindow("FractClass", "Mark's x86 Asm. Fractals", dwStyle, 50, 50,
	(rc.right-rc.left), (rc.bottom-rc.top), NULL, NULL, hi, NULL);
if (!w) { QuitProgram(); }
ShowWindow(w, scmd);
UpdateWindow(w);
for (i=0; i<strlen(cmd); i++) { SendMessage(w, WM_KEYDOWN, cmd[i], 0); }
while (1)
	{
	if ((cycle)&&(!pause)) { Cycle(); } if (speed > 1) { Sleep(speed); }
	if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE)
		{
		if (GetMessage(&msg, NULL, 0, 0))
			{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			}
		else { return(TRUE); }
		}
	}
QuitProgram();
}