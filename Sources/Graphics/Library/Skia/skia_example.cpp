// 

#include <iostream>
#include <stdexcept>

#include <SkData.h>
#include <SkImage.h>
#include <SkStream.h>
#include <SkSurface.h>
#include <SkCanvas.h>
#include <SkFont.h>

void raster(int width, int height, void (*draw)(SkCanvas&), const char* path)
{
	sk_sp<SkSurface> rasterSurface = SkSurface::MakeRasterN32Premul(width, height);
	if (!rasterSurface)
		throw std::runtime_error("can't create surface");

	SkCanvas* rasterCanvas = rasterSurface->getCanvas();
	if (!rasterCanvas)
		throw std::runtime_error("can't get canvas");

	draw(*rasterCanvas);

	sk_sp<SkImage> img(rasterSurface->makeImageSnapshot());
	if (!img)
		throw std::runtime_error("can't make image snapshot");

	sk_sp<SkData> png(img->encodeToData());
	if (!png)
		throw std::runtime_error("can't encode png");

	SkFILEWStream out(path);
	out.write(png->data(), png->size());
}

void drawTest(SkCanvas& canvas)
{
	canvas.clear(SK_ColorWHITE);

	SkPaint paint;
	paint.setAntiAlias(true);
	paint.setColor(SK_ColorCYAN);
	paint.setStyle(SkPaint::kFill_Style);
	SkFont font;
	font.setSize(48*4);
	font.setTypeface(SkTypeface::MakeFromName("Consolas", SkFontStyle::Italic()));
	canvas.drawString("Skia Test", 20, 75*4,font, paint);
}

#pragma comment(lib,"skia")

int main()
{
	const char* testFileName = "test.png";
	const int WIDTH = 1000;
	const int HEIGHT = 500;

	std::cout << "Writing test output to " << testFileName << std::endl;
	raster(WIDTH, HEIGHT, drawTest, testFileName);
}