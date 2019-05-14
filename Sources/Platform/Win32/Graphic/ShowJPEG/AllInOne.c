#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jpeglib.h>
//
typedef unsigned char byte;
typedef struct {
	int Width, Height;
	byte *Data;
} Image;

typedef struct {
	int length;
	char** strs;
} vecstr;
//
int num=0;
Image img;
vecstr vs;
//
void initiate()
{
	char filename[FILENAME_MAX]="test.txt";
	vs.length=0;
	vs.strs=NULL;
	FILE* fp=fopen(filename,"rt");
	while(!feof(fp)) {
		char* line=(char*)malloc(BUFSIZ);
		memset(line,0,BUFSIZ);
		fgets(line,BUFSIZ,fp);
		line[strlen(line)-1]='\0';//ȥ��\n
		//puts(line);
		vs.strs=(char**)realloc(vs.strs,++(vs.length)*sizeof(char*));
		vs.strs[vs.length-1]=line;
	}
	fclose(fp);
	//printf("%d\n",vs.length);
}
void swap(byte* a, byte* b)
{
	byte tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}
void readjpeg(const char filename[], Image* img)
{
	//Read the file
	FILE *fp = fopen(filename, "rb");
	if (!fp) return;
	//
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPARRAY buffer;
	//unsigned char *src_buff;
	unsigned char *point;

	cinfo.err=jpeg_std_error(&jerr);    //һ��Ϊlibjpeg����������ο�����ĵ�
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo,fp);
	jpeg_read_header(&cinfo,TRUE);
	jpeg_start_decompress(&cinfo);

	unsigned long width=cinfo.output_width;
	unsigned long height=cinfo.output_height;
	unsigned short depth=cinfo.output_components;
	img->Data= (byte*)malloc(width*height*depth);
	img->Width=width,img->Height=height;
	byte* src_buff=img->Data;
	//src_buff=new unsigned char[width*height*depth];
	memset(src_buff,0,sizeof(unsigned char)*width*height*depth);
	buffer=(*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo,JPOOL_IMAGE,width*depth,1);
	point=src_buff;
	while (cinfo.output_scanline<height) {
		jpeg_read_scanlines(&cinfo,buffer,1);    //��ȡһ��jpgͼ�����ݵ�buffer
		memcpy(point,*buffer,width*depth);    //��buffer�е��������и�src_buff
		point+=width*depth;            //һ�θı�һ��
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(fp);
	//
	///Two Steps,From ppm-order to bmp-order.
	//RGB2BGR,swap B and R.
	byte*pimg=img->Data;
	int base;
	for (int i = 0; i<height; i++) {
		for (int j = 0; j<width; j++) {
			base=width*i*3+j*3;
			swap(pimg+base+0,pimg+base+2);
		}
	}
#ifdef _WIN32
	//Flip,swap Top and Bottom.
	int size=width*3;
	byte* line=(byte*)malloc(size);
	for(int k=0; k<height/2; k++) {
		memcpy(line,pimg+size*k,size);
		memcpy(pimg+size*k,pimg+size*(height-1-k),size);
		memcpy(pimg+size*(height-1-k),line,size);
	}
#endif
}
#ifdef _WIN32
#include <windows.h>
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
	HDC hdc = GetDC(hwnd);
	RECT rect;
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(1);
		break;
	case WM_PAINT: {
		//GetWindowRect(hwnd,&rect);
		//RedrawWindow(hwnd,&rect,0,0);
		if(num==vs.length-1)exit(0);
		readjpeg(vs.strs[num++], &img);
		bmi.bmiHeader.biWidth = img.Width;
		bmi.bmiHeader.biHeight = img.Height;
		SetDIBitsToDevice(hdc, 0, 0, img.Width, img.Height,
		                  0, 0, 0, img.Height, img.Data, &bmi, DIB_RGB_COLORS);
		free(img.Data);
	}
	break;
	default:
		return DefWindowProc(hwnd, msg, w, l);
	}
	return 0;
}
DWORD WINAPI ShowImage(LPVOID lp)
{
	static TCHAR szAppName[] = TEXT(" ");
	HWND         hwnd;
	MSG          msg;
	WNDCLASS     wndclass;
	int iCmdShow = 1;
	HINSTANCE hInstance = NULL; //GetModuleHandle(NULL);
	//
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	if (!RegisterClass(&wndclass)) {
		MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
		//return;
	}
	hwnd = CreateWindow(szAppName, TEXT(""), WS_OVERLAPPEDWINDOW^WS_THICKFRAME,
	                    300, 120, 640, 480,
	                    NULL, NULL, hInstance, NULL);
	//The message loop.
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
void ImageShow()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ShowImage(0), 0, 0, 0);
}

int main(int argc, char* argv[])
{
	initiate();
	//readjpeg("016.jpg", &img);
	ImageShow();
	return 0;
}
#endif
#ifdef __linux
//#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
char* addalpha(char* data,int w,int h)
{
	char*buffer=(char*)malloc(w*h*4*sizeof(char));
	for (int i = 0; i < w*h; i ++) {
		buffer[4*i+0] = data[3*i+0];
		buffer[4*i+1] = data[3*i+1];
		buffer[4*i+2] = data[3*i+2];

	}
	free(data);
	return buffer;
}

int main(int argc, char **argv)
{
	int win_b_color;
	int win_w_color;
	XEvent xev;
	Window window;
	GC gc;
	Display *display = XOpenDisplay(NULL);
	Visual *visual;
	XImage *ximage;
	win_b_color = BlackPixel(display, DefaultScreen(display));
	win_w_color = BlackPixel(display, DefaultScreen(display));
	window = XCreateSimpleWindow(display,DefaultRootWindow(display),0, 0, 640, 480, 0,win_b_color, win_w_color);
	visual = DefaultVisual(display, 0);
	//XSelectInput(display, window, ExposureMask | KeyPressMask);
	XMapWindow(display, window);
	XFlush(display);
	gc = XCreateGC(display, window, 0, NULL);
	//XEvent event;
	initiate();
	while (1) {
		if(num==vs.length-1)exit(0);
		readjpeg(vs.strs[num++],&img);
		char* image32=(char*)img.Data;
		int width=img.Width;
		int height=img.Height;
		image32=addalpha(image32,width,height);
		ximage=XCreateImage(display, visual, 24,ZPixmap, 0, image32,width, height, 32, 0);
		XPutImage(display, window,gc, ximage, 0, 0, 0, 0,640, 480);
		free(image32);
	}
	return 0;
}
#endif