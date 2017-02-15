#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <time.h>

#include "types.h"


using namespace std;


int numItems=0;
int numTransactions=0;
int maxItemsetSize=0;
int minSupp;

double minSupport;
double minConfidence;

clock_t t1,t2;

string fileName;
string programName;
string displayOption="?";

lkArray l;

vector<unordered_set<int>> transactions;

void initialize();
void generate_candidates(lk* new_c, lk* previous_l);
int combinations(int d, int k);
string generate_key(itemset * ptr);
void increment(vector<int>* v,lk* c);
void subset(unordered_set<int>* set, int size, int left_size, unordered_set<int>::iterator index, vector<int> * v,lk* c);
void keep_frequent_candidates(lk* c);



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
    lk previous_l=l.lks[0];
    while(previous_l.itemsets.size()!=0){
    	generate_candidates(&c,&previous_l);
    	keep_frequent_candidates(&c);
    	c.counts.clear();
    	c.itemsets.clear();
    	previous_l=l.lks.back();
    }

	l.lks.pop_back();
    t2=clock();
    double diff=((double)t2-(double)t1);
    double seconds = diff / CLOCKS_PER_SEC;
    cout<<seconds<<endl;
    cout<<l.lks.size()<<endl;

    ofstream myfile;
    string hash;
    int count;
    itemset temp;
    lk l_temp;
	for(int i=0;i<l.lks.size();i++){
  		myfile.open ("output_"+to_string(i)+".txt");
    	l_temp=l.lks[i];
    	for (auto it = l_temp.counts.begin(); it != l_temp.counts.end(); ++it ){
        	hash=it->first;
        	count=it->second;
        	temp=l_temp.itemsets[hash];
        	myfile<<count<<endl;
        	myfile<<hash<<endl;
        	myfile<<"#"<<endl;
        	for (auto itt = temp.items.begin(); itt != temp.items.end(); ++itt ){
        		myfile<<*itt<<endl;
        	}
        	myfile<<"--------------------"<<endl;  
    	}
    	myfile<<l_temp.itemsets.size()<<endl;
    	myfile<<l_temp.counts.size()<<endl;
  		myfile.close();
  	}

  	
    /*
    lk l5=l.lks[4];
	string hash;
    int count;
    itemset temp;
    for (auto it = l5.counts.begin(); it != l5.counts.end(); ++it ){
        hash=it->first;
        count=it->second;
        temp=l5.itemsets[hash];
        cout<< count<<endl;
        cout<< hash<<endl;
        cout<<"#"<<endl;
        for (auto itt = temp.items.begin(); itt != temp.items.end(); ++itt ){
        	cout<<*itt<<endl;
        }
        cout<<"--------------------"<<endl;  
    }
    cout<<l5.itemsets.size()<<endl;
    cout<<l5.counts.size()<<endl;
	*/
	/*
	lk c1;
	lk l2=l.lks[1];
	generate_candidates(&c1,&l2);
	keep_frequent_candidates(&c1);
	*/
	/*
	lk c3;
	lk l3=l.lks[2];
	generate_candidates(&c3,&l3);
	keep_frequent_candidates(&c3);
	*/

    /*
    cout << numItems <<endl;
    cout << numTransactions <<endl;
    cout << minSupp <<endl;
    cout << maxItemsetSize <<endl;
    cout << minConfidence <<endl;
    cout << fileName <<endl;
    cout << programName <<endl;
    cout << displayOption <<endl;
    cout << minSupport <<endl;
    cout << transactions.size() <<endl;
    */

    return 0;
}

void initialize(){

    ifstream infile(fileName);
    string line;
    lk c;
    c.k=1;

    int n;
    int m;

    double r;
    double theta;
    string key;
    
    
    
    while (getline(infile,line)){
        istringstream iss(line);
        unordered_set<int> transaction;

        m=0;
        while( iss >> n ){
            m +=1;
            transaction.insert(n);
            r=(double)(n);
            theta=(double)(1.0);
            key=to_string(r) + to_string(theta);
            if(c.counts.count(key)>0){
                c.counts[key] +=1;
            }
            else{
                itemset newItemset;
                newItemset.items.insert(n);
                newItemset.r=r;
                newItemset.cosine_of_theta=theta;
                c.itemsets.insert(pair<string,itemset>(key,newItemset));
                c.counts.insert(pair<string,int>(key,1));
                numItems +=1;
            }
        }


        if (m > maxItemsetSize){
            maxItemsetSize=m;
        }

        numTransactions +=1;
        transactions.push_back(transaction);
    }

    minSupp=round(minSupport * numTransactions);

    lk l1;
    l1.k=1;

    string hash;
    int count;
    itemset temp;
    for (auto it = c.counts.begin(); it != c.counts.end(); ++it ){
        hash=it->first;
        count=it->second;
        if (count >= minSupp){
            l1.counts.insert(pair<string,int>(hash,count));
            temp=c.itemsets[hash];
            l1.itemsets.insert(pair<string,itemset>(hash,temp));   
        }
    }

    c.counts.clear();
    c.itemsets.clear();
    l.lks.push_back(l1);
    infile.close();
    return;}

void generate_candidates(lk* new_c, lk* previous_l){

	int k = previous_l->k;
    new_c->k = k + 1;
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

    unordered_set<string> checked_hashes;

    if(previous_l->itemsets.size() < l.lks[0].itemsets.size()){
	    
	    for (auto it_i= previous_l->itemsets.begin(); it_i != previous_l->itemsets.end(); ++it_i){
	        
	        temp_i = it_i->second;

	        for (auto it_j=it_i; it_j!= previous_l->itemsets.end(); ++it_j){
	            temp_j=it_j->second;
	            num=0;
	            
	            for (auto it=temp_j.items.begin(); it!= temp_j.items.end(); ++it){
	                if(temp_i.items.count(*it)==0){
	                    num += 1;
	                    item = *it;
	                    
	                }
	            }
	            
	            if(num==1){

	            	new_r = sqrt((temp_i.r * temp_i.r) + (item * item));
	            	new_theta = ((temp_i.cosine_of_theta * sqrt(k) * temp_i.r) + item) / (sqrt(newk) * new_r);
	                new_hash = to_string(new_r) + to_string(new_theta);
	                if(checked_hashes.count(new_hash)==0){
	                    to_generate =true;
	                    for (auto it=temp_i.items.begin(); it!= temp_i.items.end(); ++it){
	                    	temp_r = sqrt((temp_i.r * temp_i.r) - ((*it) * (*it)) + (item * item));
	                    	temp_theta = ((temp_i.cosine_of_theta * sqrt(k) * temp_i.r) - (*it) + item) / (sqrt(k) * temp_r);
	                        hash_to_check = to_string(temp_r) + to_string(temp_theta);
	                        if(previous_l->itemsets.count(hash_to_check)==0){
	                            to_generate=false;
	                            break;
	                        }
	                    }
	                    if(to_generate){
	                        itemset newItemset;
	                        newItemset.items.insert(temp_i.items.begin(),temp_i.items.end());
	                        newItemset.items.insert(item);
	                        newItemset.r = new_r;
	                        newItemset.cosine_of_theta= new_theta;
	                        new_c->itemsets.insert(pair<string,itemset>(new_hash,newItemset));
	                        new_c->counts.insert(pair<string,int>(new_hash,0));
	                    }
	                    checked_hashes.insert(new_hash);
	                }
	            }
	        }   
	    }
	}

	else{
		
		itemset temp;
		int temp_item;
		unordered_set<int> items_to_check;
		for (auto it = previous_l->itemsets.begin(); it!= previous_l->itemsets.end(); ++it){
	    
	    	temp = it->second;

	        for (auto itt=temp.items.begin(); itt!= temp.items.end(); ++itt){
	            temp_item=*itt;
	            if(items_to_check.count(temp_item)==0){
	            	items_to_check.insert(temp_item);
	            }
	        }
	    }

	    for (auto it_i = previous_l->itemsets.begin(); it_i != previous_l->itemsets.end(); ++it_i){
	        
	        temp_i = it_i->second;

	        for (auto it_j=items_to_check.begin(); it_j!=items_to_check.end(); ++it_j){
	            item=*it_j;
	            if(temp_i.items.count(item)==0){
		            new_r = sqrt((temp_i.r * temp_i.r) + (item * item));
	            	new_theta = ((temp_i.cosine_of_theta * sqrt(k) * temp_i.r) + item) / (sqrt(newk) * new_r);
	                new_hash = to_string(new_r) + to_string(new_theta);
		            if(checked_hashes.count(new_hash)==0){
		                to_generate=true;
		                for (auto it=temp_i.items.begin(); it!= temp_i.items.end(); ++it){
		                	temp_r = sqrt((temp_i.r * temp_i.r) - ((*it) * (*it)) + (item * item));
	                    	temp_theta = ((temp_i.cosine_of_theta * sqrt(k) * temp_i.r) - (*it) + item) / (sqrt(k) * temp_r);
	                        hash_to_check = to_string(temp_r) + to_string(temp_theta);
		                    if(previous_l->itemsets.count(hash_to_check)==0){
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
		                    new_c->itemsets.insert(pair<string,itemset>(new_hash,newItemset));
		                    new_c->counts.insert(pair<string,int>(new_hash,0));
		                }
		                checked_hashes.insert(new_hash);
		          	}
		        }
	       }
	 	}   
	}
	return;}

int combinations(int d, int k){
	if(k==1){
		return d;
	}
	if(k==d){
		return 1;
	}
	return combinations(d-1,k) + combinations(d-1,k-1);}

string generate_key(vector<int> * ptr){
	int k=ptr->size();
    double r=0.0;
    int sum=0;
    for (auto it=ptr->begin(); it!= ptr->end(); ++it){
        r += (*it) * (*it);
        sum += *it;
    }
    r=sqrt(r);
    double cosine_of_theta= sum / (sqrt(k) * r);
    string key =to_string(r) + to_string(cosine_of_theta);
    return key;}

void increment(vector<int>* v, lk* c){
    string key=generate_key(v);
    if(c->counts.count(key)>0){
    	c->counts[key] +=1;
	}
    return;   }

void subset(unordered_set<int>* set, int size, int left_size, unordered_set<int>::iterator index, vector<int> * v, lk* c){
    if(left_size==0){
        increment(v,c);
        return;
    }
    for(unordered_set<int>::iterator it=index; it!=set->end(); ++it){
        v->push_back(*it);
        subset(set,size,left_size-1,++index,v,c);
        v->pop_back();
    }
    return;}

void keep_frequent_candidates(lk* c){
	int i;
	string key;
    itemset temp;
    bool to_count;
    unordered_set<int> temp_set;

	for (i=0; i<transactions.size();i++){
		temp_set=transactions[i];
		if(temp_set.size() >= c->k){
			if(c->itemsets.size() < combinations(temp_set.size(),c->k)){
		    	for (auto it = c->itemsets.begin(); it!= c->itemsets.end(); ++it){
		        	key=it->first;
		        	temp=it->second;
		        	to_count=true;
		        	for (auto itt = temp.items.begin(); itt != temp.items.end(); ++itt){
		        		if(temp_set.count(*itt)==0){
		        			to_count=false;
		        			break;
		        		}
		        	}
		        	if(to_count){
		        		c->counts[key] +=1;
		        	}
				}
			}
			else{
				vector<int> temp_vector;
				subset(&temp_set,temp_set.size(),c->k,temp_set.begin(),&temp_vector,c);
			}
		}
	}

	lk l_new;
    l_new.k = c->k;

    
    int count;
    for (auto it = c->counts.begin(); it != c->counts.end(); ++it){
        key=it->first;
        count=it->second;
        if (count >= minSupp){
            l_new.counts.insert(pair<string,int>(key,count));
            temp=c->itemsets[key];
            l_new.itemsets.insert(pair<string,itemset>(key,temp));   
        }
    }

    l.lks.push_back(l_new);
    return;}
