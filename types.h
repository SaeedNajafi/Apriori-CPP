#ifndef TYPES_H	
#define TYPES_H

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

typedef struct{
	std::unordered_set<int> items;
    	float r;
    	float cosine_of_theta;
}itemset;

typedef struct{
	std::unordered_map<string,itemset> itemsets;
	std::unordered_map<string,int> counts;
	int k;
}lk;

typedef struct{
	std::vector<lk> lks;
}lkArray;

#endif
