# Apriori-CPP
Implementation of Apriori algorithm in C++ 

CMPUT 690 – Assignment 2
Saeed Najafi 1509106 – Winter 2017 – University of Alberta

Time and Count measures

Experiments were done on my laptop with the following specifications.

•	Processor: 2.7 GHz Intel Core i5, only one core was used.
•	Memory: 8 Giga Byte DDR3.
•	Hard Disk: 128 Giga Byte SSD Disk.
•	Operating System: macOS Sierra


First Experiment:
Number of Transactions: 100 (first 100 transactions were selected)
Minimum Support: 10 (10% - 0.1)
Minimum Confidence: 80% (0.8)
Compile: g++ -std=c++11 main.cpp -o main
Run: ./main test1.txt 0.1 0.8 a

Results:
Number of frequent 1_itemsets: 10
Number of association rules: 0

Execution time: 0 seconds


Second Experiment:
Number of Transactions: 1000 (first 1000 transactions were selected)
Minimum Support: 10 (1% - 0.01)
Minimum Confidence: 80% (0.8)
Compile: g++ -std=c++11 main.cpp -o main
Run: ./main test2.txt 0.01 0.8 a

Results:
Number of frequent 1_itemsets: 334
Number of frequent 2_itemsets: 174
Number of association rules: 0

Execution time: 1 seconds

Third Experiment:
Number of Transactions: 10000
Minimum Support: 10 (0.1% - 0.001)
Minimum Confidence: 80% (0.8)
Compile: g++ -std=c++11 main.cpp -o main
Run: ./main test3.txt 0.001 0.8 a

Results:
Memory Usage: at most 160 Mega Byte.
Execution Time: 3 minutes.
Number of frequent 1_itemsets: 472
Number of frequent 2_itemsets: 24085
Number of frequent 3_itemsets: 8049
Number of frequent 4_itemsets: 3786
Number of frequent 5_itemsets: 2329
Number of frequent 6_itemsets: 1115
Number of frequent 7_itemsets: 407
Number of frequent 8_itemsets: 111
Number of frequent 9_itemsets: 21
Number of frequent 10_itemsets: 2
Number of association rules: 161856

Execution time: 179 seconds
