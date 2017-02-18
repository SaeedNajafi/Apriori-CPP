#ifndef TYPES_H	
#define TYPES_H

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

/*
 	Struct for each itemset. Items are stored as integers in an unordered_set ("items") for fast retrieval. 
 	Each itemset has an "r" and "cosine_of_theta" which create a hash value for this set of integers.
 	The variable "counter" keeps the number of transactions containing this set.

 	I have my idea to hash a set of integers. I also posted it on StackOverFlow. Hopefully, it is useful for implementing Apriori.
 	http://stackoverflow.com/a/42194596/7554647

 	"
 		Consider a set of k positive integers {n1, n2, n3, ... , nk} in a k-dimensional space. 
 			Each set represents a point for which we wish to find a hash.

 		If we compute r = sqrt(n1 * n1 + n2 * n2 + ... + nk * nk) and then,
 			calculate cosine of the angle between two vectors (n1, n2, n3, ... , nk) and (1, 1, 1, ... ,1) (denoted with cosine_of_theta), 
 			then string "r,cosine_of_theta" is a hash for this set.

		cosine_of_theta = (n1*1 + n2*1 + n3*1 + ... + nk*1) / ( sqrt(k) * r )

		The reason to choose (1,1,1,...,1) as a reference vector is that the order of integers in the set doesn't matter. 
		If we change the order of integers, the hash doesn't change.

		Even when we have a multiset of integers, the mentioned method can generate a hash in O(n).

		For deleting and inserting a new integer, it can be done in O(1) only if we store the r and cosine_of_theta for each set since
			we can calculate r and cosine_of_theta for a new multiset based on the previously created one.

		If integers in the set are big, r will be a large number. In this case, it's better to store Nth root of r in the hash string (2nd root, 3rd root, ... which depends on your application).

		This method generates hashes that take fewer bytes than concatenating all integers of the set to form a key.

		This method can have collisions only because we have a limited output precision for r and cosine_of_theta (at most, double datatype for both r and cosine_of_theta).
	"
*/
typedef struct{
	std::unordered_set<int> items;
    double r;
    double cosine_of_theta;
    int counter;
}itemset;


/*
	The struct "lk" is a set of all itemsets with size "k" (each contains "k" integers as items). 
	For fast retrieval and checking of itemsets, They are stored in an unordered_map which maps each hash value to its corresponding itemset. 
*/
typedef struct{
	std::unordered_map<std::string,itemset> itemsets;
	int k;
}lk;

#endif
