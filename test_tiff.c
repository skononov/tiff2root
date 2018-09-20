#include <stdio.h>
#include "tiffio.h"

int main(int argc, char* argv[])
{
    TIFF* tif = TIFFOpen(argv[1], "r");
    if (tif) {
		uint32 imageWidth=0, imageLength=0;
		uint32 config=0, nsamples=0, nbits=0;
		uint32 row, col;
		uint64 scanLineSize;
		tdata_t buf;
		float *data;

		TIFFReadDirectory(tif); //skip to the next frame

		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);
		TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &nsamples);
		TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
		TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &nbits);
		scanLineSize = TIFFScanlineSize(tif);
		printf("Image width: %u\n",imageWidth);
		printf("Image length: %u\n",imageLength);
		printf("Planar configuration: %u\n",config);
		printf("Scan line size: %ld\n",scanLineSize);
		printf("Samples per pixel: %u\n",nsamples);
		printf("Bits per sample: %u\n",nbits);

		if(scanLineSize!=nbits/8*imageWidth) {
			fprintf(stderr,"Error: scan line size %lu is not equal to bytes-per-sample times image width %u\n",scanLineSize,nbits/8*imageWidth);
			return 1;
		}

		buf = _TIFFmalloc(scanLineSize);
		data = (float*)buf;

		for (row = 0; row < imageLength; row++) {
			TIFFReadScanline(tif, buf, row, 0);
			for(col = 0; col < imageWidth; row++) {
				printf("%5.3f ",data[col]);
			}
			printf("\n");
		}

		_TIFFfree(buf);

		TIFFClose(tif);
    }

	return 0;
}
