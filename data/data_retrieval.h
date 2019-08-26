#ifndef DATA_RETRIEMAL_H
#define DATA_RETRIEMAL_H

#include<fstream>
using namespace std;
typedef unsigned char BYTE;

#define IMAGESIZE 784

class Data{
public:
	Data();
	Data(const Data&);
	Data(Data&&);
	~Data();
	Data& operator=(const Data&);
	Data& operator=(Data&&);
	void setImage(const BYTE *);
	void setLabel(const char);
	const BYTE* getImage();
	char getLabel();
private:
	BYTE* image; 
	char label;
};

class DataRetriever{
public:
	DataRetriever(const char*);
	~DataRetriever();
	DataRetriever(const DataRetriever&) = delete;
	DataRetriever& operator=(const DataRetriever&) = delete;
	DataRetriever(DataRetriever&&) = delete;
	DataRetriever& operator=(DataRetriever&&) = delete;
	Data getData();
	void next();
	void setIndex(int );
private:
	fstream* initializeFile(const char*, const char*);
	BYTE* readImage();
	char readLabel();
	int index;
	fstream *fimage;
	fstream *flabel;
};
#endif
