#include <gtest/gtest.h>
#include <feather/feather.h>
#include <fstream>
#include "strtok.h"
#include <cmath>

using namespace std;
using namespace feather;

int hcvrp(string filename, unsigned NCl, unsigned NVe, unsigned Timeout) {
    try  {
	
	//  Opening input filename, e.g. `hcvrp_data.pl'.

	ifstream  file(filename);

	if (!file)   {
		cerr << "Could not open `" << filename << "'\n";
		return  -1;
	}


	string  input, tok;

	Solver slv(new Naxos());
	// if ( Timeout  !=  0 )
	// 	pm.timeLimit(Timeout);


	NsDeque<Int>  clientX, clientY, vehicles;
	IntDeque clientDemand(slv);

	//  Depot coordinates.

	clientX.push_back(0);
	clientY.push_back(0);
	clientDemand.push_back(0);


	//  Reading input file line by line.

	while ( getline(file,input) )    {

		StrTokenizer  tokens(input, "([,]). ");

		tok  =  tokens.next();

		if ( tok  ==  "vehicles" )    {
		
			while ( ! (tok = tokens.next()).empty() ) {
				vehicles.push_back(atoi(tok.c_str()));
				// cout << "Vehicle: " << atoi(tok.c_str()) << endl;
			}

		} else if ( tok == "clients"  ||  tok == "c" )    {

			if ( tok == "clients"  &&  tokens.next() != "c" )   {
				cerr << filename
				     << ": clients/1 not followed by c/3\n";
				return  1;
			}

			while ( ! (tok = tokens.next()).empty() )   {

				clientDemand.push_back(atoi(tok.c_str()));

				if ( (tok = tokens.next()).empty() )   {
					cerr << "c/3: Missing argument X\n";
					return  1;
				}

				clientX.push_back(atoi(tok.c_str()));

				if ( (tok = tokens.next()).empty() )   {
					cerr << "c/3: Missing argument Y\n";
					return  1;
				}

				clientY.push_back(atoi(tok.c_str()));

				if ( ! (tok = tokens.next()).empty()
						&& tok != "c" )
				{
					cerr << "clients/1: Unexpected `" << tok << "'\n";
					return  1;
				}
			}

		}  else if ( tok  !=  "" ) {

			cerr << "Unknown predicate " << tok << "\n";
			return  1;
		}
	}

	file.close();

	//  Compute all the distances.
	unsigned  i, j;
	std::deque< std::deque<Int> >  edge_weight(clientX.size());
	IntDeque distancesList(slv);
	for (i=0;  i < edge_weight.size();  ++i)    {
		for (j=0;  j < edge_weight.size();  ++j)   {
			edge_weight[i].push_back( sqrt(
				pow((clientX[i]-clientX[j])*1000.0, 2) +
				pow((clientY[i]-clientY[j])*1000.0, 2)) + 0.5 );
			//  0.5 is added in order to round to the closest integer.
		}
	}
	
	for(i = 0; i < NCl+1; i++)
	    for(j = 0; j < NCl+1; j++)
			distancesList.push_back( edge_weight[i][j]);


	if ( NCl  <  1  ||  NCl >= edge_weight.size() )   {

		cerr << "Wrong NCl\n";
		return  1;
	}

	if ( NVe  <  1  ||  NVe > vehicles.size() )   {

		cerr << "Wrong NVe\n";
		return  1;
	}



	//  Print problem representation.
	
	/*for (i=0;  i < NVe;  ++i)
		cout << "vehicles[" << i << "] = " << vehicles[i] << "\n";
	cout << "\n";

	for (i=0;  i < NCl+1;  ++i)
		cout << "clientDemand[" << i << "] = " << clientDemand[i] << "\n";
	cout << "\n";

	for (i=0;  i < NCl+1;  ++i)   {
		for (j=0;  j < NCl+1;  ++j)   {
			cout << "edge_weight[" << i << "][" << j << "] = "
				<< edge_weight[i][j] << "\n";
		}
		cout << "\n";
	} */
	

	// /**   ADD YOUR CODE HERE   **/
	IntVarArray FSolList(slv);
	
	// /* define variables */
	for(i = 1; i <= NVe; i++)
	    for(j = 1; j <= NCl+1; j++)
		    FSolList.push_back( IntVar(slv, 0, NCl) );
	
	// /* post capacities constraint */
	IntVarArray weightList(slv);
	
	int count = 0;
	for(i = 0; i < NVe; i++) {

		for(j= 0; j < NCl+1; j++) {
			
			if(j == 0)
			    weightList.push_back( clientDemand[ FSolList[count]  ] );
			else 
			    weightList.push_back( weightList[count-1] + clientDemand[ FSolList[count] ] );
				
			count++;
			//cout << count << endl;
		}
		
		slv.add( weightList[count-1] <= vehicles[i] ); 
	}
	
	// /* post unique */
	IntDeque values(slv, NCl+1);
	IntDeque occurrences(slv, NCl+1);
	
	for(i = 1; i <= NCl; i++) {
		values[i] = i;
		occurrences[i] = 1;
	}
	values[0] = 0;
	occurrences[0] = (NVe)*(NCl+1) - NCl;
	
	slv.add( Count( FSolList, values, occurrences ) );

	/* post zeroes at end */
	count = 0;
	for(i = 0; i < NVe; i++)
		for(j = 0; j < NCl+1; j++) {
			if( j != NCl)
				slv.add( IfThen( FSolList[count] == 0, FSolList[count+1] == 0 ) );

			count++;
		}
	
	/* eliminiate symmetries */
	int first;
	count = 0;
	for(i = 0; i < NVe; i++) {
		first = count;
		
		for(j = 0; j < NCl+1; j++) {
			
			if( j != NCl && count != first) 
			    slv.add( IfThen( FSolList[count+1] == 0, FSolList[first] >= FSolList[count]) );
			
			count++;
		}
	}
	
	// cout << "in create cost" << endl;
	
	// /* create cost */
	IntVarArray costList(slv);
	
	count = 0;
	for(i = 0; i < NVe; i++)
		for(j = 0; j < NCl+1; j++) {
			if( count == 0 )
				costList.push_back( distancesList[ (FSolList[count]*(NCl+1)) + 0 ]  );
			else				
				costList.push_back( distancesList[ (FSolList[count]*(NCl+1)) + FSolList[count-1] ] + costList[count-1] );
				
			count++;
		} 
		
	slv.minimize( costList[count-1] );  
	
	// // cout << "Done putting constraints. Searching for solution..." << endl;
	slv.addGoal(Labeling(FSolList));
	
	int *BestSolution = (int*) malloc( sizeof(int)*(FSolList.size()+1) );
	int bestCost;
	bool foundSolution = false;
	
	while (slv.nextSolution() != false) {
		foundSolution = true;
        for(i = 0; i < FSolList.size(); i++)
			BestSolution[i] = FSolList[i].value();
		
		bestCost = costList[count-1].value();
	// 	// cout << "Found a solution with cost " << costList[count-1].value() << endl; 
	// 	//cout << "Solution with cost" << costList[count-1] << " :" << FSolList << "\n";
	// 	//cout << costList << endl;
	// 	//cout << weightList << endl;
	}
	
	/* formidate solution */
	if(foundSolution) {
	// 	// cout << "Best solution: " << endl;
	// 	// i = 0;
	// 	// bool f = true;
	// 	// cout << "[[";
	// 	// while(i < FSolList.size() ) {
	// 	// 	if(BestSolution[i] != 0) {
	// 	// 		if(f && i == 0) {
	// 	// 			cout << BestSolution[i];
	// 	// 			f = false;
	// 	// 		}
	// 	// 		else if( f && i != 0) {
	// 	// 			cout << ", [" << BestSolution[i];
	// 	// 			f = false;
	// 	// 		}
	// 	// 		else 
	// 	// 			cout << ", " << BestSolution[i];
			
	// 	// 		i++;
	// 	// 	}
	// 	// 	else {
	// 	// 		cout << "]";
	// 	// 		f = true;
	// 	// 		while( BestSolution[i] == 0) 
	// 	// 			i++;
			
	// 	// 	}		
	// 	// }
	// 	// cout << "] " << endl;
	// 	// cout << "Cost = " << bestCost << endl;
		return bestCost;
	}
	else {
	// 	// cout << "No solution" << endl;
		return -1;
	}
	
	// // cout << "Time = " << (double)clock() / CLOCKS_PER_SEC << "\n";
	


    }  catch (exception& exc)  {

	cerr << exc.what() << "\n";

    }  catch (...)  {

	cerr << "Unknown exception" << "\n";
    }
}

string path = "tests/examples/data/hcvrp_data.pl";

/* Warning - long, long tests lay ahead... */


/* Should run in ~7sec */
TEST(hcvrp, T1) {
	int cost = hcvrp(path, 5, 3, 0);
	ASSERT_EQ(488492, cost);
}

/* Should run in ~16sec */
TEST(hcvrp, T2) {
	int cost = hcvrp(path, 6, 3, 0);
	ASSERT_EQ(634714, cost);
}

/* Shoud run in ~3min */
TEST(hcvrp, T3) {
	int cost = hcvrp(path, 8, 4, 0);
	ASSERT_EQ(702248, cost);
}

/* Should run in ~600ms */
TEST(hcvrp, T4) {
	int cost = hcvrp(path, 2, 2, 0);
	ASSERT_EQ(303768, cost);
}

/* Should run in ~100ms */
TEST(hcvrp, T5) {
	int cost = hcvrp(path, 1, 1, 0);
	ASSERT_EQ(247694, cost);
}
