#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <unordered_set>


using namespace std;

#define cut_off_to_not_save_all_transactions_in_ram 25000

/*
	 
*/
typedef struct{
	std::map<std::set<int>,int> counter;
	int k;
}lk;


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
vector<set<int>> transactions;

void initialize();
void generate_candidates(lk &new_c, lk &previous_l);
int combinations(int d, int k);
void subset(set<int> &sset, int size, int left_size, set<int>::iterator index, set<int> &v,lk &c);
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
    

    /*
    	Apriori Algorithm!
    */
    initialize();
    lk c;
    lk previous_l=all_ls[0];

    while(previous_l.counter.size()!=0){
    	generate_candidates(c,previous_l);
    	keep_frequent_candidates(c);
    	previous_l=all_ls.back();	
    }
	all_ls.pop_back();
	


    t2=clock();
    double diff=((double)t2-(double)t1);
    double seconds = diff / CLOCKS_PER_SEC;
    cout<<seconds<<endl;
    cout<<all_ls.size()<<endl;

    ofstream myfile;

    int count;
    set<int> temp;
    lk l_temp;

	for(int i=0;i<all_ls.size();i++){
  		myfile.open("output_"+to_string(i)+".txt");
    	l_temp=all_ls[i];
    	
        for (auto it = l_temp.counter.begin(); it != l_temp.counter.end(); ++it){
            temp=it->first;
            count=it->second;
            for(auto itt=temp.begin(); itt!=temp.end(); ++itt){
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

    // Create candidates of size 1 and fill it while reading the transaction file.
    lk c;
    c.k=1;

    while (getline(infile,line)){
        istringstream iss(line);


        set<int> transaction;
        
        set<int> n_set;

        m=0;
        while( iss >> n ){

        	// m captures the number of items in one transaction (one line of file).
            m +=1;
            
            n_set.insert(n);
            if(can_store_in_ram){
            	transaction.insert(n);
            	
        	}

            if(c.counter.count(n_set)>0){
            	
                c.counter[n_set] +=1;
            }
            else{
                c.counter.insert(pair<set<int>,int>(n_set,1));
                numItems +=1;
            }

            n_set.erase(n);
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
    set<int> temp_set;
    for (auto it = c.counter.begin(); it != c.counter.end(); ++it){
        temp_set=it->first;
        count=it->second;
        if (count >= minSupp){
            l1.counter.insert(pair<set<int>,int>(temp_set,count));
        }
    }

    c.counter.clear();

    all_ls.push_back(l1);

    infile.close();

    return;}



/*
    This function generates new candidates of size k + 1 from previous frequent itemsets of size k.
*/
void generate_candidates(lk &new_c, lk &previous_l){

    new_c.k = previous_l.k +1;
    

    set<int> temp_i;

    int num;
    int item;
    bool to_generate;

	set<int> temp;
	set<int> items_to_check;
	

	for (auto it = previous_l.counter.begin(); it!= previous_l.counter.end(); ++it){
        temp = it->first;
	    items_to_check.insert(temp.begin(),temp.end());
    }

	for (auto it_i = previous_l.counter.begin(); it_i != previous_l.counter.end(); ++it_i){

        temp_i = it_i->first;

        for (auto it_j=items_to_check.begin(); it_j!=items_to_check.end(); ++it_j){
            item=*it_j;

            if(temp_i.count(item)==0){

                temp_i.insert(item);
                
                if(new_c.counter.count(temp_i)==0){

                    temp=temp_i;
                    to_generate=true;
                    	                    
                    	for (auto it=temp_i.begin(); it!= temp_i.end(); ++it){

	                        temp.erase(*it);

	                        if(previous_l.counter.count(temp)==0){

	                            to_generate=false;
	                            temp.insert(*it);
	                            
	                            break;
	                        }

	                        temp.insert(*it);
	                    }
                	
	                    if(to_generate){
	                        new_c.counter.insert(pair<set<int>,int>(temp_i,0));
	                    }
                	
                }

                temp_i.erase(item);
            }
        }
	}
	
	items_to_check.clear();
	temp.clear();
	temp_i.clear();
	

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

// find subsets of one transaction (transaction is stored in sset) with size "size" and increment their counters in candidate itemsets "c" if exist.
void subset(set<int> &sset, int size, int left_size, set<int>::iterator index, set<int> &v, lk &c){
    if(left_size==0){
        if(c.counter.count(v)>0){
            c.counter[v] +=1;
        }
        return;
    }
    for(set<int>::iterator it=index; it!=sset.end(); ++it){
        v.insert(*it);
        subset(sset,size,left_size-1,++index,v,c);
        v.erase(*it);
    }
    return;}

void keep_frequent_candidates(lk &c){

	set<int> temp_set;
    int temp_count;
    bool to_count;
    set<int> t_set;
    set<int> help_set;
     

    if(can_store_in_ram){

    	for (int i=0; i<transactions.size(); i++){
    		t_set=transactions[i];
    		
    		if(t_set.size() >= c.k){
    			if(c.counter.size() < combinations(t_set.size(),c.k)){

    				
    		    	for (auto it = c.counter.begin(); it!= c.counter.end(); ++it){
    		        	temp_set=it->first;
    		        	
    		        	to_count=true;

    		        	
    		        	for (auto itt = temp_set.begin(); itt != temp_set.end(); ++itt){
    		        		if(t_set.count(*itt)==0){
    		        			to_count=false;
    		        			break;
    		        		}
    		        	}

    		        	if(to_count){
    		        		c.counter[temp_set] +=1;
    		        	}
    				}
    				
    			}
    			else{
                    subset(t_set,t_set.size(),c.k,t_set.begin(),help_set,c);
    			}
    		}
    	}
    }

	lk l_new;
    l_new.k = c.k;


    // Copy frequent items into l_new.
    int count;
    
    for (auto it = c.counter.begin(); it != c.counter.end(); ++it){
        help_set=it->first;
        count=it->second;
        if (count >= minSupp){
            l_new.counter.insert(pair<set<int>,int>(help_set,count));
        }
    }

    c.counter.clear();

    all_ls.push_back(l_new);

    return;}
