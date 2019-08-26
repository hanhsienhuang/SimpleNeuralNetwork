#include"simple_network.h"
#include<cmath>
#include<cstdlib>
#include<utility>

/////  NVector  ///////

NVector::NVector(int _size){
	values = NULL;
	setSize(_size);
}

NVector::NVector(const NVector& _nvector){
	setSize(_nvector.size);
	for(int i=0; i<size; i++){
		values[i] = _nvector.values[i];
	}
}

NVector::NVector(NVector&& rvl){
	size = rvl.size;
	values = rvl.values;
	rvl.values = NULL;
}

NVector::~NVector(){
	delete [] values;
}

NVector& NVector::operator=(NVector rvl){
	std::swap(values, rvl.values);
	std::swap(size, rvl.size);
	return *this;
}

float& NVector::operator[](int index) const{
	int rearranged_index = index % size;
	if(rearranged_index < 0){
		rearranged_index += size;
	}
	return values[rearranged_index];
}

NVector NVector::operator*(float c) const{
	NVector product(size);
	for(int i=0; i<size; i++){
		product.values[i] = values[i]*c;
	}
	return product;
}

NVector NVector::operator+(const NVector& rval) const{
	NVector sum(size);
	for(int i=0; i<size; i++){
		sum.values[i] = values[i]+rval.values[i];
	}
	return sum;
}

NVector NVector::operator-(const NVector& rval) const{
	NVector result(size);
	for(int i=0; i<size; i++){
		result.values[i] = values[i]-rval.values[i];
	}
	return result;
}

NVector& NVector::operator+=(const NVector& rval){
	*this = *this + rval;
	return *this;
}

NVector& NVector::operator-=(const NVector& rval){
	*this = *this - rval;
	return *this;
}

void NVector::setSize(int _size){
	size = _size;
	delete [] values;
	if(size <= 0){
		values = NULL;
	}else{
		values = new float [size];
	}
}

int NVector::getSize() const{
	return size;
}

void NVector::setZeros(){
	for(int i=0; i!=size; i++){
		values[i] = 0.0;
	}
}

////  NMatrix  ///////

NMatrix::NMatrix(int _row_number, int _col_number){
	setSize(_row_number, _col_number);
}

NMatrix::NMatrix(const NMatrix& _nmatrix){
	values = NULL;
	setSize(_nmatrix.row_number, _nmatrix.col_number);
	if(isValid()){
		for(int i=0; i!=row_number*col_number; i++){
			values[i] = _nmatrix.values[i];
		}
	}
}

NMatrix::NMatrix(NMatrix && rvl){
	row_number = rvl.row_number;
	col_number = rvl.col_number;
	values = rvl.values;
	rvl.values = NULL;
}

NMatrix::~NMatrix(){
	delete [] values;
}

NMatrix& NMatrix::operator=(NMatrix _nmatrix){
	std::swap(row_number, _nmatrix.row_number);
	std::swap(col_number, _nmatrix.col_number);
	std::swap(values, _nmatrix.values);
	return *this;
}

float& NMatrix::operator()(int row, int col) const{
	// no error handling //
	return values[row*col_number + col];
}

NMatrix NMatrix::operator+(const NMatrix& rval) const{
	NMatrix result(row_number, col_number);
	for(int i=0; i!= row_number*col_number; i++){
		result.values[i] = values[i] + rval.values[i];
	}
	return result;
}

NMatrix NMatrix::operator-(const NMatrix& rval) const{
	NMatrix result(row_number, col_number);
	for(int i=0; i!= row_number*col_number; i++){
		result.values[i] = values[i] - rval.values[i];
	}
	return result;
}

NMatrix& NMatrix::operator+=(const NMatrix& _nmatrix){
	*this = *this + _nmatrix;
	return *this;
}

NMatrix& NMatrix::operator-=(const NMatrix& _nmatrix){
	*this = *this - _nmatrix;
	return *this;
}

NMatrix NMatrix::operator*(float c) const{
	NMatrix product(row_number, col_number);
	for(int i=0; i!=row_number*col_number; i++){
		product.values[i] = values[i] * c;
	}
	return product;
}

void NMatrix::setSize(int _row_number, int _col_number){
	row_number = _row_number;
	col_number = _col_number;
	delete [] values;
	if(!isValid()){
		values = NULL;
	}else{
		values = new float [row_number*col_number];
	}
}

void NMatrix::setZeros(){
	for(int i=0; i!=row_number*col_number; i++){
		values[i] = 0.0;
	}
}

int NMatrix::getRowNum() const{
	return row_number;
}

int NMatrix::getColNum() const{
	return col_number;
}

bool NMatrix::isValid(){
	return (row_number>0) && (col_number>0);
}

/// operator * /////

NVector operator*(const NMatrix& nmatrix, const NVector& nvector){
	//no error handling //
	int rownum = nmatrix.getRowNum();
	NVector product(rownum);
	for(int i=0; i<rownum; i++){
		float sum = 0;
#pragma omp parallel for reduction(+:sum)
		for(int j=0; j<nmatrix.getColNum(); j++){
			sum += nmatrix(i, j)*nvector[j];
		}
		product[i] = sum;
	}
	return product;
}

NVector operator*(const NVector& nvector, const NMatrix& nmatrix){
	//no error handling //
	int colnum = nmatrix.getColNum();
	NVector product(colnum);
	for(int i=0; i<colnum; i++){
		float sum = 0;
#pragma omp parallel for reduction(+:sum)
		for(int j=0; j<nmatrix.getRowNum(); j++){
			sum += nvector[j]*nmatrix(j, i);
		}
		product[i] = sum;
	}
	return product;
}
////  SimpleNetwork  /////

SimpleNetwork::~SimpleNetwork(){
	delete [] neurons;
	delete [] weights;
	delete [] bias;
	delete [] cost_by_zs;
	delete [] cost_by_bias;
	delete [] cost_by_weights;
}

void SimpleNetwork::initialize(int hidden_layer){
	initPointers();
	initStructure(hidden_layer);
	initWeights();
}

void SimpleNetwork::setEta(float _eta){
	eta = _eta;
}

void SimpleNetwork::setTrainingBatchSize(int _batchsize){
	batchsize = _batchsize;
}

void SimpleNetwork::setTrainingData(DataRetriever* _dr){
	dr = _dr;
}

void SimpleNetwork::train(){
	for(int i=0; i!=number_of_layers-1; i++){
		cost_by_weights[i].setZeros();
		cost_by_bias[i].setZeros();
	}
	for(int i=0; i!=batchsize; i++){
		Data data = dr->getData();
		operate(&data);
		setCostByZOfOutput(&data);
		backprop();
		dr->next();
	}
	for(int j=0; j!=number_of_layers-1; j++){
		weights[j] -= cost_by_weights[j]*eta;
		bias[j] -= cost_by_bias[j]*eta;
	}
}

char SimpleNetwork::perceive(Data* const pdata){
	operate(pdata);
	return (char)getMaxVecIndex(output);
}

void SimpleNetwork::initPointers(){
	neurons = new NVector[number_of_layers];
	cost_by_zs = new NVector[number_of_layers-1];
	weights = new NMatrix[number_of_layers-1];
	bias = new NVector[number_of_layers-1];
	cost_by_weights = new NMatrix[number_of_layers-1];
	cost_by_bias = new NVector[number_of_layers-1];
	input = neurons;
	output = neurons + number_of_layers-1;
}

void SimpleNetwork::initStructure(int hidden_layer){
	neurons[0].setSize(IMAGESIZE);
	neurons[1].setSize(hidden_layer);
	neurons[2].setSize(10);

	cost_by_zs[0].setSize(hidden_layer);
	cost_by_zs[1].setSize(10);

	weights[0].setSize(hidden_layer, IMAGESIZE);
	weights[1].setSize(10, hidden_layer);

	bias[0].setSize(hidden_layer);
	bias[1].setSize(10);

	cost_by_weights[0].setSize(hidden_layer, IMAGESIZE);
	cost_by_weights[1].setSize(10, hidden_layer);

	cost_by_bias[0].setSize(hidden_layer);
	cost_by_bias[1].setSize(10);
}

void SimpleNetwork::initWeights(){
	for(int i=0; i!=number_of_layers-1; i++){
		NVector* pb = &(bias[i]);
		for(int j=0; j!=pb->getSize(); j++){
			(*pb)[j] = getRandom();
		}
		NMatrix* pw = &(weights[i]);
		for(int row=0; row != pw->getRowNum(); row++){
			for(int col=0; col != pw->getColNum(); col++){
				(*pw)(row, col) = getRandom();
			}
		}
	}
}

int SimpleNetwork::getMaxVecIndex(NVector* pvector){
	float test_max = (*pvector)[0];
	int index = 0;
	for(int i=1 ;i != pvector->getSize(); i++){
		if(test_max < (*pvector)[i] ){
			test_max = (*pvector)[i];
			index = i;
		}
	}
	return index;
}

void SimpleNetwork::operate(Data* pdata){
	const BYTE* pdataimage = pdata->getImage();
	for(int i=0; i != IMAGESIZE; i++){
		neurons[0][i] = (float)pdataimage[i]/255.0;
	}

	for(int j=1; j!= number_of_layers; j++){
		neurons[j] = activate(bias[j-1]+weights[j-1]*neurons[j-1]);
	}
}

NVector SimpleNetwork::activate(NVector in){
	NVector out(in.getSize());
	for(int i=0; i!=in.getSize(); i++){
		out[i] = activate(in[i]);
	}
	return out;
}

float SimpleNetwork::activate(float x){
	return 1.0/(1.0+exp(-x));
}

float SimpleNetwork::activate_derivative(float y){
	return y*(1.0-y);
}

float SimpleNetwork::getRandom(){
	static float x,y;
	static bool oddtimes = true;
	float * returnp;
	if(oddtimes){
		float u = (float)rand()/RAND_MAX;
		float v = (float)(1L+rand())/(2L+RAND_MAX);

		float theta = 2*M_PI*u;
		float r = sqrt(-log(v));

		x = r*cos(theta);
		y = r*sin(theta);

		returnp = &x;
	}else{
		returnp = &y;
	}
	
	oddtimes = !oddtimes;
	return *returnp;
}

void SimpleNetwork::setCostByZOfOutput(Data * pdata){
	int answer = (int)pdata->getLabel();
	for(int i=0; i!=output->getSize(); i++){
		float yi = (*output)[i];
		if(i==answer){
			cost_by_zs[number_of_layers-2][i] = -activate_derivative(yi)/yi /batchsize;
		}else{
			cost_by_zs[number_of_layers-2][i] = activate_derivative(yi)/(1-yi) /batchsize;
		}
	}
}

void SimpleNetwork::backprop(){
	for(int i=number_of_layers-2; i!=-1; i--){
		updateCostByZ(i);
		updateCostByW(i);
	}
}

void SimpleNetwork::updateCostByZ(int layer){
	if(layer == number_of_layers-2){
		return;
	}
	NVector c_z_times_w = cost_by_zs[layer+1]*weights[layer+1];
	for(int i=0; i!=cost_by_zs[layer].getSize(); i++){
		(cost_by_zs[layer])[i] = c_z_times_w[i] * activate_derivative( (neurons[layer+1])[i]);
	}
}

void SimpleNetwork::updateCostByW(int layer){
	for(int i=0; i!=cost_by_weights[layer].getRowNum(); i++){
		for(int j=0; j!=cost_by_weights[layer].getColNum(); j++){
			cost_by_weights[layer](i,j) += cost_by_zs[layer][i]*neurons[layer][j];
		}
	}
	cost_by_bias[layer] += cost_by_zs[layer];
}
