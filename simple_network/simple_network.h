#ifndef SIMPLE_NETWORK
#define SIMPLE_NETWORK
#include"../data/data_retrieval.h"

class NVector{
public:
	NVector(){};
	NVector(int);
	NVector(const NVector&);
	NVector(NVector&&);
	~NVector();
	NVector& operator=(NVector);
	float& operator[](int) const;
	NVector operator*(float) const;
	NVector operator+(const NVector&) const;
	NVector operator-(const NVector&) const;
	NVector& operator+=(const NVector&);
	NVector& operator-=(const NVector&);
	void setSize(int);
	int getSize() const;
	void setZeros();
private:
	float* values = NULL;
	int size = 0;
};

class NMatrix{
public:
	NMatrix(){};
	NMatrix(int, int);
	NMatrix(const NMatrix&);
	NMatrix(NMatrix&&);
	~NMatrix();
	NMatrix& operator=(NMatrix);
	float& operator()(int, int) const;
	NMatrix operator+(const NMatrix&) const;
	NMatrix operator-(const NMatrix&) const;
	NMatrix& operator+=(const NMatrix&);
	NMatrix& operator-=(const NMatrix&);
	NMatrix operator*(float) const;
	void setSize(int, int);
	void setZeros();
	int getRowNum() const;
	int getColNum() const;
private:
	bool isValid();

	float* values = NULL;
	int row_number = 0;
	int col_number = 0;
};

NVector operator*(const NMatrix&, const NVector&);
NVector operator*(const NVector&, const NMatrix&);

class SimpleNetwork{
public:
	SimpleNetwork(){};
	SimpleNetwork(const SimpleNetwork&) = delete;
	SimpleNetwork(SimpleNetwork&&) = delete;
	~SimpleNetwork();
	SimpleNetwork& operator=(const SimpleNetwork&) = delete;
	SimpleNetwork& operator=(SimpleNetwork&&) = delete;
	void initialize(int);
	void setEta(float);
	void setTrainingBatchSize(int);
	void setTrainingData(DataRetriever*);
	void train();
	char perceive(Data* const );
private:
	void initPointers();
	void initStructure(int);
	void initWeights();
	int getMaxVecIndex(NVector*);
	void operate(Data*);
	NVector activate(NVector);
	float activate(float);
	float activate_derivative(float);
	float getRandom();
	void setCostByZOfOutput(Data *);
	void backprop();
	void updateCostByZ(int);
	void updateCostByW(int);

	NVector* neurons = NULL;
	int number_of_layers = 3;
	NMatrix* weights = NULL;
	NVector* bias = NULL;
	NMatrix* cost_by_weights = NULL;
	NVector* cost_by_bias = NULL;
	NVector* cost_by_zs = NULL;
	NVector* input = NULL;
	NVector* output = NULL;

	float eta = 0.1;
	int batchsize = 10;
	DataRetriever* dr = NULL;
};

#endif
