#include"data_retrieval.h"
#include<cstring>

Data::Data(){
	image = new BYTE[IMAGESIZE];
	label = 0;
}

Data::Data(const Data& _data){
	label = _data.label;
	memcpy(image, _data.image, IMAGESIZE);
}

Data::Data(Data&& _data){
	label = _data.label;
	image = _data.image;
	_data.image = NULL;
}

Data& Data::operator=(const Data& _data){
	label = _data.label;
	memcpy(image, _data.image, IMAGESIZE);
	return *this;
}

Data& Data::operator=(Data&& _data){
	label = _data.label;
	image = _data.image;
	_data.image = NULL;
	return *this;
}

Data::~Data(){
	delete [] image;
}

void Data::setImage(const BYTE* newimage){
	memcpy(image, newimage, IMAGESIZE);
}

void Data::setLabel(const char newlabel){
	label = newlabel;
}

const BYTE* Data::getImage(){
	return image;
}

char Data::getLabel(){
	return label;
}

DataRetriever::DataRetriever(const char* filename){
	fimage = initializeFile(filename, "-images");
	flabel = initializeFile(filename, "-labels");
	index = 0;
}

DataRetriever::~DataRetriever(){
	fimage->close();
	flabel->close();
	delete fimage;
	delete flabel;
}

Data DataRetriever::getData(){
	Data data;
	BYTE* arrayimage = readImage();
	data.setImage(arrayimage);
	data.setLabel(readLabel());
	delete [] arrayimage;
	return data;
}

void DataRetriever::next(){
	index++;
}

void DataRetriever::setIndex(int newindex){
	index = newindex;
}

fstream* DataRetriever::initializeFile(const char* filename, const char* cat){
	char filenamecat[40];
	strcpy(filenamecat, filename);
	return new fstream( strcat(filenamecat, cat), ios_base::in|ios_base::binary);
}

BYTE* DataRetriever::readImage(){
	int position = 16 + IMAGESIZE*index;
	fimage->seekg(position, fimage->beg);

	BYTE* imagedata = new BYTE[IMAGESIZE];
	fimage->read((char*)imagedata, IMAGESIZE);
	return imagedata;
}

char DataRetriever::readLabel(){
	int position = 8 + index;
	flabel->seekg(position, flabel->beg);

	char labeldata;
	flabel->read(&labeldata, 1);
	return labeldata;
}
