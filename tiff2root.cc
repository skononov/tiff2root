#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "TH2F.h"
#include "TString.h"
extern "C" {
    #include "tiffio.h"
}

using std::endl;
using std::cout;
using std::cerr;

static const char * progname = "tiff2root";

static void Usage(int status=0) 
{
	cout << "Usage: " << progname << " file.tif [file.root]\n"
         << "\nConvert a TIFF file written by the RayCi software to\na ROOT 2D histogram (TH2F) and write it to ROOT file.\n";
	exit(status);
}

int main(int argc, char* argv[])
{
    uint32 imageWidth=0, imageLength=0;
	uint32 config=0, nsamples=0, nbits=0;
	uint64 scanLineSize=0;

	tdata_t buf=0;
	float *data=0;

	progname = argv[0];

	if (argc < 2) Usage();

	const char* filename = argv[1];
    const char* outfilename = 0;
    if(argc>2) outfilename = argv[2];

	//Open TIFF file
    TIFF* tif = TIFFOpen(filename, "r");
    if (!tif) {
		cerr << progname << ": Can not open TIFF file " << filename << endl;
		return 1;
	}

	cout << "File " << filename << " successfully opened" << endl;

	//Skip to the second IFD/frame
	if (!TIFFReadDirectory(tif)) {
		cerr << progname << ": No second IFD" << endl;
		return 2;
	}

	//Get frame properties
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageLength);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &nsamples);
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &nbits);
	scanLineSize = TIFFScanlineSize(tif);
	cout << "Image width: " << imageWidth << "\n"
		 << "Image length: " << imageLength << "\n"
		 << "Planar configuration: " << config << "\n"
		 << "Scan line size: " << scanLineSize << "\n"
	     << "Samples per pixel: " << nsamples << "\n"
	     << "Bits per sample: " << nbits << endl;

	//Check number if samples 
	if (nsamples != 1) {
		cerr << progname << ": number of samples per pixel (" << nsamples << ") is not 1" << endl;
		return 10;
	}

	//Check scan line size
	if (scanLineSize != nbits/8*imageWidth) {
		cerr << progname << ": scan line size " << scanLineSize 
             << " is not equal to bytes-per-sample times image width " << nbits/8*imageWidth << endl;
		return 11;
	}

	//Allocate buffer memmory
	buf = _TIFFmalloc(scanLineSize);
	data = (float*)buf;

	//Create 2D histogram to plot frame data
	TH2F* himg = new TH2F("himg",filename,imageWidth,0.,1.*imageWidth,imageLength,0.,1.*imageLength);

	//Read frame data
    cout << "Reading TIFF frame data..." << endl;
	for (uint32 row = 0; row < imageLength; row++) {
		TIFFReadScanline(tif, buf, row, 0);
		for(uint32 col = 0; col < imageWidth; col++)
			himg->SetBinContent(col+1,row+1,data[col]);
	}

    _TIFFfree(buf);
    TIFFClose(tif);
    
    TString rootFilename;
    if (outfilename) 
        rootFilename = outfilename;
    else
        rootFilename = filename;
	    rootFilename.ReplaceAll("#","N");
	    rootFilename.ReplaceAll(".tif",".root");
        if (rootFilename==filename)
            rootFilename += ".root";

	himg->SaveAs(rootFilename);

    cout << "TH2F histogram written to " << rootFilename << endl;

	return 0;
}
