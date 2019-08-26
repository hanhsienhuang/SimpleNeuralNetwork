#include"data/data_retrieval.h"
#include"simple_network/simple_network.h"
#include<iostream>


int main(){
	DataRetriever dr("data/train-ubyte");
	DataRetriever testdr("data/t10k-ubyte");

	SimpleNetwork sn;
	sn.setEta(0.5);
	sn.setTrainingBatchSize(10);
	sn.setTrainingData(&dr);
	sn.initialize(200);

	for(int i=0;i!=20;i++){
		dr.setIndex(0);
		for(int trainj=0; trainj!=5000; trainj++){
			sn.train();
		}
		testdr.setIndex(0);
		int num_of_correct = 0;
		for(int testj=0; testj!=10000; testj++){
			Data testdata = testdr.getData();
			if(sn.perceive(&testdata) == testdata.getLabel()){
				num_of_correct ++;
			}
			testdr.next();
		}
		cout << "epoch " << i << " : " 
                    << "Testing accuracy " << (float)num_of_correct /10000*100
                    << "%" << endl;
	}
	
	return 0;
}
