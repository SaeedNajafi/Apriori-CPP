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
#include <iomanip>

using namespace std;

#define cut_off_to_not_save_all_transactions_in_ram 50000

/*
	 lk maps each set, which represents an itemset, to a counter.
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
int number_of_strong_rules=0;


/*
	If the number of transactions is large (greater than cut_off) then, do not keep them in memory and each should be read from file for each pass of Apriori algorithm,
	otherwise all transactions are saved internally to speed up next steps.
*/
bool can_store_in_ram=true;
vector<set<int>> transactions;


clock_t t1,t2;

string fileName;
string programName;
string displayOption="?";

// A vector to keep all frequent itemsets of size k as an lk. 
vector<lk> all_ls;

void initialize();
void generate_candidates(lk &new_c, lk &previous_l);
int combinations(int d, int k);
void subset(set<int> &sset, int size, int left_size, set<int>::iterator index, set<int> &v,lk &c);
void keep_frequent_candidates(lk &c);
void print_results();
void print_frequent_items();
void print_strong_rules();
void find_print_subset(set<int> &sset, int size, int left_size, set<int>::iterator index, set<int> &v, ofstream &outfile, int count_f);

// A to_string function to print double values with 2 decimal point precision.
template <typename T>
string to_string_with_precision(const T value){
    ostringstream out;
    out <<fixed<<setprecision(2) << value;
    return out.str();}


int main(int argc, char **argv){

	// To get arguments of the program.
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

    // to capture execution time of the program.
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
	
	// Print results in files!
	print_results();


    t2=clock();
    double diff=((double)t2-(double)t1);
    double seconds = diff / CLOCKS_PER_SEC;
    cout<<"\nResult files were created!"<<endl;
    cout<<"Execution time: ";
    cout<<round(seconds);
    cout<<" seconds"<<endl;

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


// Go to each tracsaction and find subsets of size k and increment their counter in c. This algorithm is implemented in function subset().
// If the number of candidates in c is less than the number of subsets of size k of one trasaction, then loop over each candidate itemset and check whether it is inside the transaction or not, then increment its counter if exists.
void keep_frequent_candidates(lk &c){

	int temp_count;
    bool to_count;

	set<int> temp_set;
    set<int> t_set;
    set<int> help_set;
     

    if(can_store_in_ram){
    	// when all transactions are in main memory.
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
    else{
    	// when we should read each transaction from the input file.
    	ifstream infile(fileName);
    	string line;
    	int n;
    	while (getline(infile,line)){
        	istringstream iss(line);
        	t_set.clear();
        	while( iss >> n ){
            	t_set.insert(n);
        	}

        	// transaction read and is in t_set. Now, same as the above loop.
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


void print_results(){

		string text;
		lk l_temp;
    	
		cout<<endl;
    	for(int i=0;i<all_ls.size();i++){
			l_temp=all_ls[i];
			text="Number of frequent " + to_string(i+1) + "_itemsets: " + to_string(l_temp.counter.size());
			cout<<text<<endl;
		}

		if(displayOption=="f" || displayOption=="a"){
			print_frequent_items();
		}

    	print_strong_rules();

		text="Number of association rules: " + to_string(number_of_strong_rules);
		cout<<text<<endl;
		cout<<endl;    

    	return;}


void print_frequent_items(){

	int count;
	string text;
	set<int> temp;
	lk l_temp;
	ofstream outfile;

	for(int i=0;i<all_ls.size();i++){
		l_temp=all_ls[i];
		outfile.open(to_string(i+1)+"_itemsets.txt");
    	for (auto it = l_temp.counter.begin(); it != l_temp.counter.end(); ++it){
		    temp=it->first;
		    count=it->second;

		    for(auto itt=temp.begin(); itt!=temp.end();){
		    	outfile<<*itt;
		       	if(++itt!=temp.end()){
		        	outfile<<", ";
		        }
		        else{
		        	outfile<<" ";
		        }
		    }
		    
		    text="(" + to_string(count) + ")";
		    outfile<<text;
		    outfile<<endl;
		}
		outfile.close();	
	}}


void print_strong_rules(){

	
	int count_f;
	set<int> itemset;
	lk l_temp;
	ofstream outfile;
	if(displayOption=="a" || displayOption=="r"){
		outfile.open("association_rules.txt");
	}
	set<int> v;


	
	for(int i=1;i<all_ls.size();i++){
		l_temp=all_ls[i];
    	for (auto it = l_temp.counter.begin(); it != l_temp.counter.end(); ++it){
		    itemset=it->first;
		    count_f=it->second;
		    for(int j=1;j<itemset.size();j++){
		    	
		    	find_print_subset(itemset,itemset.size(),j,itemset.begin(),v,outfile,count_f);
		    }
		}	
	}
	if(displayOption=="a" || displayOption=="r"){
		outfile.close();
	}	}


void find_print_subset(set<int> &sset, int size, int left_size, set<int>::iterator index, set<int> &v, ofstream &outfile, int count_f){
    if(left_size==0){
    	
        double count_s=all_ls[v.size()-1].counter[v];
        double conf = (double)count_f/count_s;
        
        string text="";
        if(conf >= minConfidence){
        	vector<int> temp_set(sset.size());
        	vector<int>::iterator it=set_difference(sset.begin(),sset.end(),v.begin(),v.end(),temp_set.begin());
        	temp_set.resize(it-temp_set.begin());

        	if(displayOption=="r" || displayOption=="a"){
	        	for(auto itt=v.begin(); itt!=v.end();){
			    	text += to_string(*itt);
			       	if(++itt!=v.end()){
			        	text += ", ";
			        }
			        else{
			        	text += " ";
			        }
			    }
			    text += "-> ";
			    for(auto itt=temp_set.begin(); itt!=temp_set.end();){
			    	text += to_string(*itt);
			       	if(++itt!=temp_set.end()){
			        	text += ", ";
			        }
			        else{
			        	text += " ";
			        }
			    }
			    
			    text += "(" + to_string(count_f) + ", " + to_string_with_precision(conf) + ")";

			    outfile<<text<<endl;
			}
		    number_of_strong_rules +=1;
        }
    	
        return;
    }
    for(set<int>::iterator it=index; it!=sset.end(); ++it){
        v.insert(*it);
        find_print_subset(sset,size,left_size-1,++index,v,outfile,count_f);
        v.erase(*it);
    }
    return;}
