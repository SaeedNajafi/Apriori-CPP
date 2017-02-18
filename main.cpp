#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <time.h>
#include <algorithm>

#include "types.h"


using namespace std;



#define cut_off_to_not_save_all_transactions_in_ram 30000

int numItems=0;
int numTransactions=0;
int maxItemsetSize=0;
int minSupp;

double minSupport;
double minConfidence;



/*	
	We check size of transactions that we currently have in an unordered_set, if this size is large enough,
	then this set of transactions is deleted, otherwise all transactions are saved internally to speed up next steps.
*/
bool can_store_in_ram=true;

clock_t t1,t2;

string fileName;
string programName;
string displayOption="?";

vector<lk> all_ls;
vector<unordered_set<int>> transactions;

void initialize();
void generate_candidates(lk &new_c, lk &previous_l);
int combinations(int d, int k);
string generate_key(vector<int> &ptr);
void increment(vector<int> &v,lk &c);
void subset(unordered_set<int> &set, int size, int left_size, unordered_set<int>::iterator index, vector<int> &v,lk &c);
void keep_frequent_candidates(lk &c);



int main(int argc, char **argv){

    if(argc==4){
        programName = argv[0];
        fileName = argv[1];
        minSupport = atof(argv[2]);
        minConfidence = atof(argv[3]);
    }
    else if(argc==5){

        programName = argv[0];
        fileName = argv[1];
        minSupport = atof(argv[2]);
        minConfidence = atof(argv[3]);
        displayOption = argv[4];
    }

    t1=clock();
    
    initialize();

    
    lk c;
    lk previous_l=all_ls[0];
    while(previous_l.itemsets.size()!=0){
    	generate_candidates(c,previous_l);
    	keep_frequent_candidates(c);
    	c.itemsets.clear();
    	previous_l=all_ls.back();
    }

	all_ls.pop_back();
    t2=clock();
    double diff=((double)t2-(double)t1);
    double seconds = diff / CLOCKS_PER_SEC;
    cout<<seconds<<endl;
    cout<<all_ls.size()<<endl;

    ofstream myfile;
    string hash;
    int count;
    itemset temp;
    lk l_temp;
    vector<int> v_temp;
	for(int i=0;i<all_ls.size();i++){
  		myfile.open("output_"+to_string(i)+".txt");
    	l_temp=all_ls[i];
    	for (auto it = l_temp.itemsets.begin(); it != l_temp.itemsets.end(); ++it ){
        	hash=it->first;
        	temp=it->second;

        	v_temp.clear();
        	for (auto itt = temp.items.begin(); itt != temp.items.end(); ++itt){
        		v_temp.push_back(*itt);
        	}
        	sort(v_temp.begin(),v_temp.end());
        	for (auto itt = v_temp.begin(); itt != v_temp.end(); ++itt){
        		myfile<<*itt;
        		myfile<<",";
        	}
        	myfile<<endl;	
    	}
  		myfile.close();
  	}

  	
    return 0;
}



/*
	Read the transaction file once and find the total number of transactions (numTransactions),
	maximum number of items in one transaction (maxItemsetSize),
	check whether we can store all transactions in RAM or not (can_store_in_ram),
	create candidate items of size 1 and count their occurrences in different transactions,
	Finally, create frequent itemsets of size 1 which their counter are not less than minSupp (minmum number of transactions for a frequent itemset).
*/
void initialize(){

    ifstream infile(fileName);
    string line;

    int n;
    int m;

    double r;
    double cosine_of_theta;
    string hash;

    // Create candidates of size 1 and fill it while reading the transaction file.
    lk c;
    c.k=1;

    while (getline(infile,line)){
        istringstream iss(line);

        
        unordered_set<int> transaction;
    	

        m=0;
        while( iss >> n ){

        	// m captures the number of items in one transaction (one line of file).
            m +=1;

            if(can_store_in_ram){
            	transaction.insert(n);
        	}

        	// "r" and "cosine_of_theta" for a set with a single integer n is: r = n, cosine_of_theta = 1.0
            r=(double)(n);
            cosine_of_theta=(double)(1.0);
            hash=to_string(r) + to_string(cosine_of_theta);

            if(c.itemsets.count(hash)>0){
                c.itemsets[hash].counter +=1;
            }
            else{

                itemset newItemset;
                newItemset.items.insert(n);
                newItemset.r=r;
                newItemset.cosine_of_theta=cosine_of_theta;
                newItemset.counter=1;
                c.itemsets.insert(pair<string,itemset>(hash,newItemset));
                numItems +=1;

            }
        }

        // find maximum number of items in one transaction.
        if (m > maxItemsetSize){
            maxItemsetSize=m;
        }

        numTransactions +=1;

        if(can_store_in_ram){
        	transactions.push_back(transaction);
    	}

        if((can_store_in_ram) && (numTransactions > cut_off_to_not_save_all_transactions_in_ram)){
        	can_store_in_ram=false;
        	transactions.clear();
        }

    }

    minSupp=round(minSupport * numTransactions);

    lk l1;
    l1.k=1;

    
    // Copy frequent items into l1.
    int count;
    itemset temp_itemset;
    for (auto it = c.itemsets.begin(); it != c.itemsets.end(); ++it){
        hash=it->first;
        temp_itemset=it->second;
        if (temp_itemset.counter >= minSupp){
            l1.itemsets.insert(pair<string,itemset>(hash,temp_itemset));   
        }
    }

    c.itemsets.clear();

    all_ls.push_back(l1);

    infile.close();

    return;}



/*
    This function generates new candidates of size k + 1 from previous frequent itemsets of size k.
*/
void generate_candidates(lk &new_c, lk &previous_l){

	int k = previous_l.k;
    new_c.k = k + 1;
    int newk= k + 1;

    string hash_to_check;
    string new_hash;

    itemset temp_i;
    itemset temp_j;

    int num;
    int item;

    bool to_generate;

    double new_r;
    double new_theta;
    double temp_r;
    double temp_theta;

    if(previous_l.itemsets.size() < all_ls[0].itemsets.size()){
	    
	    for (auto it_i= previous_l.itemsets.begin(); it_i != previous_l.itemsets.end(); ++it_i){
	        
	        temp_i = it_i->second;

	        for (auto it_j=it_i; it_j!= previous_l.itemsets.end(); ++it_j){
	            temp_j=it_j->second;
	            num=0;
	            
	            for (auto it=temp_j.items.begin(); it!= temp_j.items.end(); ++it){
	                if(temp_i.items.count(*it)==0){
	                    num += 1;
	                    item = *it;
	                    
	                }
	            }
	            
                // num is the number of different items between temp_i and temp_j.
	            if(num==1){

	            	new_r = sqrt((temp_i.r * temp_i.r) + (item * item));
	            	new_theta = ((temp_i.cosine_of_theta * sqrt(k) * temp_i.r) + item) / (sqrt(newk) * new_r);
	                new_hash = to_string(new_r) + to_string(new_theta);
	                
                    if(new_c.itemsets.count(new_hash)==0){

	                    to_generate =true;
	                    for (auto it=temp_i.items.begin(); it!= temp_i.items.end(); ++it){

	                    	temp_r = sqrt((temp_i.r * temp_i.r) - ((*it) * (*it)) + (item * item));
	                    	temp_theta = ((temp_i.cosine_of_theta * sqrt(k) * temp_i.r) - (*it) + item) / (sqrt(k) * temp_r);
	                        hash_to_check = to_string(temp_r) + to_string(temp_theta);
	                       	
	                        if(previous_l.itemsets.count(hash_to_check)==0){
	                            to_generate=false;
	                            break;
	                        }
	                    }

                        // If all subsets of the new itemset is in previous_l, then add that new itemset to new_c.
	                    if(to_generate){
	                        itemset newItemset;
	                        newItemset.items.insert(temp_i.items.begin(),temp_i.items.end());
	                        newItemset.items.insert(item);
	                        newItemset.r = new_r;
	                        newItemset.cosine_of_theta= new_theta;
                            newItemset.counter=0;
	                        new_c.itemsets.insert(pair<string,itemset>(new_hash,newItemset));
	                    }
	                }
	            }
	        }   
	    }
	}

    // This is an alternative to the previous algorithm where the number of itemsets in previous_l is greater than number of frequent items.
    // This algorithm can be faster.
	else{
		

        // Find all items in previous_l;
		itemset temp;
		unordered_set<int> items_to_check;
		for (auto it = previous_l.itemsets.begin(); it!= previous_l.itemsets.end(); ++it){
	    	temp = it->second;
	        for (auto itt=temp.items.begin(); itt!= temp.items.end(); ++itt){
	            	items_to_check.insert(*itt);
	        }
	    }

	    for (auto it_i = previous_l.itemsets.begin(); it_i != previous_l.itemsets.end(); ++it_i){
	        
	        temp_i = it_i->second;

	        for (auto it_j=items_to_check.begin(); it_j!=items_to_check.end(); ++it_j){
	            item=*it_j;

	            if(temp_i.items.count(item)==0){

		            new_r = sqrt((temp_i.r * temp_i.r) + (item * item));
	            	new_theta = ((temp_i.cosine_of_theta * sqrt(k) * temp_i.r) + item) / (sqrt(newk) * new_r);
	                new_hash = to_string(new_r) + to_string(new_theta);

                    if(new_c.itemsets.count(new_hash)==0){
		                to_generate=true;
		                for (auto it=temp_i.items.begin(); it!= temp_i.items.end(); ++it){

		                	temp_r = sqrt((temp_i.r * temp_i.r) - ((*it) * (*it)) + (item * item));
	                    	temp_theta = ((temp_i.cosine_of_theta * sqrt(k) * temp_i.r) - (*it) + item) / (sqrt(k) * temp_r);
	                        hash_to_check = to_string(temp_r) + to_string(temp_theta);
	                        
		                    if(previous_l.itemsets.count(hash_to_check)==0){
		                        to_generate=false;
		                        break;
		                    }
		                }
		                if(to_generate){
		                    itemset newItemset;
		                    newItemset.items.insert(temp_i.items.begin(),temp_i.items.end());
		                    newItemset.items.insert(item);
		                    newItemset.r = new_r;
		                    newItemset.cosine_of_theta=new_theta;
                            newItemset.counter=0;
		                    new_c.itemsets.insert(pair<string,itemset>(new_hash,newItemset));
		                }
		          	}
		        }
	       }
	 	}   
	}
    
	return;}



/*
	Calculate combinations of k items from a set of d items. (k is less than d)
	This function is used to check which algorithm should be used for counting transactions for each itemset
	in the function "keep_frequent_candidates". 
*/
int combinations(int d, int k){

	if(k==1){
		return d;
	}
	if(k==d){
		return 1;
	}

	if(k > d){
		return 0;
	}
	return combinations(d-1,k) + combinations(d-1,k-1);}


/*
	Function to generate a hash key for a vector of integers.
	"ptr" the vector of integers passed by reference to this function.
*/
string generate_key(vector<int> &ptr){
	int k=ptr.size();
    double r=0.0;
    int sum=0;
    for (auto it=ptr.begin(); it!= ptr.end(); ++it){
        r += (*it) * (*it);
        sum += *it;
    }
    r=sqrt(r);
    double cosine_of_theta= sum / (sqrt(k) * r);
    string key =to_string(r) + to_string(cosine_of_theta);
    return key;}

void increment(vector<int> &v, lk &c){

    string hash=generate_key(v);

    if(c.itemsets.count(hash)>0){

    	c.itemsets[hash].counter +=1;

	}
    return;}

// find subsets of one transaction (transaction is stored in set) with size "size" and increment their counters in candidate itemsets "c" if exist.
void subset(unordered_set<int> &set, int size, int left_size, unordered_set<int>::iterator index, vector<int> &v, lk &c){
    if(left_size==0){
        increment(v,c);
        return;
    }
    for(unordered_set<int>::iterator it=index; it!=set.end(); ++it){
        v.push_back(*it);
        subset(set,size,left_size-1,++index,v,c);
        v.pop_back();
    }
    return; }

void keep_frequent_candidates(lk &c){
	
	string hash;
    itemset temp;
    bool to_count;
    unordered_set<int> temp_set;
    

    if(can_store_in_ram){

    	for (int i=0; i<transactions.size(); i++){
    		temp_set=transactions[i];

    		if(temp_set.size() >= c.k){
    			if(c.itemsets.size() < combinations(temp_set.size(),c.k)){
    		    	for (auto it = c.itemsets.begin(); it!= c.itemsets.end(); ++it){
    		        	hash=it->first;
    		        	temp=it->second;
    		        	to_count=true;
    		        	for (auto itt = temp.items.begin(); itt != temp.items.end(); ++itt){
    		        		if(temp_set.count(*itt)==0){
    		        			to_count=false;
    		        			break;
    		        		}
    		        	}
    		        	if(to_count){
    		        		c.itemsets[hash].counter +=1;
    		        	}
    				}
    			}
    			else{
                    vector<int> temp_vector;
    				subset(temp_set,temp_set.size(),c.k,temp_set.begin(),temp_vector,c);
    			}
    		}
    	}  
    }

	lk l_new;
    l_new.k = c.k;

    // Copy frequent items into l_new.
    int count;
    itemset temp_itemset;
    for (auto it = c.itemsets.begin(); it != c.itemsets.end(); ++it){
        hash=it->first;
        temp_itemset=it->second;
        if (temp_itemset.counter >= minSupp){
            l_new.itemsets.insert(pair<string,itemset>(hash,temp_itemset));   
        }
    }

    c.itemsets.clear();

    all_ls.push_back(l_new);

    return;}
