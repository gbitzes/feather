#include <feather/feather.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include "strtok.h"

#include "gtest/gtest.h"


using namespace std;
using namespace feather;

int solutionCostNikos(string filename, unsigned NCl, unsigned NVe, unsigned Timeout) {

    try  {

  //  Checking input arguments.


  //  Opening input filename, e.g. `hcvrp_data.pl'.

  ifstream  file(filename);

  if ( ! file )   {

    cerr << "Could not open `" << filename << "'\n";
    return  1;
  }


  string  input, tok;
  Solver slv(new Naxos());
  // if ( Timeout  !=  0 )
  //   pm.timeLimit(Timeout);


  NsDeque<Int>  clientX, clientY, clientDemand, vehicles;


  //  Depot coordinates.

  clientX.push_back(0);
  clientY.push_back(0);
  clientDemand.push_back(0);


  //  Reading input file line by line.

  while ( getline(file,input) )    {

    StrTokenizer  tokens(input, "([,]). ");

    tok  =  tokens.next();

    if ( tok  ==  "vehicles" )    {

      while ( ! (tok = tokens.next()).empty() )
        vehicles.push_back(atoi(tok.c_str()));

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

      cerr << "Unknown predicate `" << tok << "'\n";
      return  1;
    }
  }

  file.close();


  //  Compute all the distances.
  unsigned  i, j;
  NsDeque< NsDeque<Int> >  edge_weight(clientX.size());
  for (i=0;  i < edge_weight.size();  ++i)    {
    for (j=0;  j < edge_weight.size();  ++j)   {
      edge_weight[i].push_back( sqrt(
        pow((clientX[i]-clientX[j])*1000.0, 2) +
        pow((clientY[i]-clientY[j])*1000.0, 2)) + 0.5 );
      //  0.5 is added in order to round to the closest integer.
    }
  }



  if ( NCl  <  1  ||  NCl >= edge_weight.size() )   {

    cerr << "Wrong NCl\n";
    return  1;
  }

  if ( NVe  <  1  ||  NVe > vehicles.size() )   {

    cerr << "Wrong NVe\n";
    return  1;
  }



  //  Print problem representation.
  
/*  for (i=0;  i < NVe;  ++i)
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
  }*/
  


  /**   ADD YOUR CODE HERE   **/



  unsigned  routesNum = NVe;
  NsDeque<Int>  &routeCapacity = vehicles;


  unsigned  r, c, c1, c2;


  IntVarArray vClientRoute(slv), vClientOrder(slv), vAllVars(slv);
  for (c=0;  c < NCl;  ++c)   {
    vClientRoute.push_back( IntVar(slv,0,routesNum-1) );
    vClientOrder.push_back( IntVar(slv,0,NCl-1) );
  }


  for (c=0;  c < NCl;  ++c)
    vAllVars.push_back( vClientRoute[c] );
  for (c=0;  c < NCl;  ++c)
    vAllVars.push_back( vClientOrder[c] );


  IntVarArray vAllClientRoutes(slv);
  for (c=0;  c < NCl;  ++c) {
    vAllClientRoutes.push_back( vClientRoute[c]*NCl + vClientOrder[c] );
  }
  slv.add( AllDiff(vAllClientRoutes) );


  std::deque<IntVarArray*> vClientIsInRoute;
  IntVarArray vRouteSize(slv);
  for (r=0;  r < routesNum;  ++r)    {
    vClientIsInRoute.push_back( new IntVarArray(slv) );
    for (c=0;  c < NCl;  ++c) {
      vClientIsInRoute[r]->push_back( vClientRoute[c] == r );
    }
    vRouteSize.push_back(Sum(*(vClientIsInRoute[r])) );

    for (c=0;  c < NCl;  ++c) {
      slv.add(IfThen(vClientRoute[c]==r, vClientOrder[c]<vRouteSize[r]) );
    }
  }


  std::deque<IntVarArray*>  vRouteDemandTerms;
  IntVarArray vRouteDemandsSum(slv);
  for (r=0;  r < routesNum;  ++r)    {
    vRouteDemandTerms.push_back( new IntVarArray(slv) );
    for (c=0;  c < NCl;  ++c)
      vRouteDemandTerms[r]->push_back( (*vClientIsInRoute[r])[c] * clientDemand[c+1] );

    vRouteDemandsSum.push_back( Sum(*vRouteDemandTerms[r]) );

    slv.add( vRouteDemandsSum[r] <= routeCapacity[r] );
  }


  IntVarArray vClientIsLast(slv);
  std::deque<IntVarArray*> vClientIsLastTerms;

  IntVarArray  vRouteCostTerms(slv);

    for (c2=0;  c2 < NCl;  ++c2)   {
      vRouteCostTerms.push_back(
        (0==vClientOrder[c2]) *
        edge_weight[0][c2+1] );
    }
  for (c1=0;  c1 < NCl;  ++c1)   {
    for (c2=0;  c2 < NCl;  ++c2)   {
      if ( c1  !=  c2 )   {
        vRouteCostTerms.push_back(
        (vClientRoute[c1]==vClientRoute[c2]) *
        (vClientOrder[c1]+1==vClientOrder[c2]) *
        edge_weight[c1+1][c2+1] );
      }
    }

    vClientIsLastTerms.push_back( new IntVarArray(slv) );

    for (r=0;  r < routesNum;  ++r)    {

      vClientIsLastTerms[c1]->push_back(
        (vClientRoute[c1]==r) *
        (vClientOrder[c1]+1==vRouteSize[r]) );
    }

    vClientIsLast.push_back( Sum(*vClientIsLastTerms[c1]) );

    vRouteCostTerms.push_back(
      vClientIsLast[c1] * edge_weight[c1+1][0] );
  }

  //  The sum of all the weights is the solution cost.
  IntVar vCost = Sum(vRouteCostTerms);
  slv.minimize( vCost );


  ////  Symmetries breaking.
  //for (c1=0;  c1 < NCl;  ++c1)   {
  //  for (c2=c1+1;  c2 < NCl;  ++c2)   {
  //    // If c1 and c2 are in the same route and c1
  //    //  is first and c2 is last, then c1 < c2.
  //    pm.add( vClientRoute[c1]!=vClientRoute[c2]
  //        || vClientOrder[c2] != 0
  //        || vClientIsLast[c1] != 1 );

  //    pm.add( NsIfThen(
  //      vClientOrder[c1]==0 && vClientOrder[c2]==0 ,
  //      vClientRoute[c1] < vClientRoute[c2] ) );
  //  }
  //}

  for (r=1;  r < routesNum-1;  ++r)
    slv.add( IfThen(vRouteSize[r]==0, vRouteSize[r+1]==0) );


  slv.addGoal(Labeling(vAllVars));

  Int  Cost=-1;

  while ( slv.nextSolution()  !=  false )    {

/*    for (r=0;  r < routesNum;  ++r)    {
      cout << "  Route:  ";
      unsigned  previous = 0;
      for (Int order=0;  order < vRouteSize[r].value();  ++order)   {
        for (c=0;  c < NCl;  ++c)   {
          if ( vClientRoute[c].value()==static_cast<Int>(r)
            && vClientOrder[c].value()==order)
          {
            if ( previous == 0)
              cout << 0;

            cout << "  --";
            cout << edge_weight[previous][c+1];
            realcost += edge_weight[previous][c+1];

            cout << "-->  ";

            cout << c+1;

            cout << "{";
            cout << clientDemand[c+1];
            cout << "}";

            previous = c+1;
          }
        }
      }
      if ( previous  ==  0 )   {
        cout << "-\n";
      }  else  {
        cout << "  --";
        cout << edge_weight[previous][0];
        realcost += edge_weight[previous][0];
        cout << "-->  ";
        cout << 0;
        cout << ".\n";

        cout << "          Demands={" << vRouteDemandsSum[r].value() << "}, ";
        cout << "Capacity={" << routeCapacity[r] << "}.\n";
      }
    }*/
    Cost  =  vCost.value();
    // cout << "Found a solution with cost " << Cost << "\n";

  }

  return Cost;

  // cout << "Found no solution with cost " << vCost << "\n";

  // cout << "\n";

  // cout << "Solution = " << "\n";

  // cout << "Cost = " << Cost << "\n";

  // cout << "Time = " << (double)clock() / CLOCKS_PER_SEC << "\n";



    }  catch (exception& exc)  {

  cerr << exc.what() << "\n";

    }  catch (...)  {

  cerr << "Unknown exception" << "\n";
    }
}

string pathNikos = "tests/examples/data/hcvrp_data.pl";

/* Warning - long, long tests lay ahead... */

/* Should run in ~7sec */
TEST(hcvrpNikos, T1) {
  int cost = solutionCostNikos(pathNikos, 5, 3, 0);
  ASSERT_EQ(488492, cost);
}

/* Should run in ~16sec */
TEST(hcvrpNikos, T2) {
  int cost = solutionCostNikos(pathNikos, 6, 3, 0);
  ASSERT_EQ(634714, cost);
}

/* Shoud run in ~3min */
TEST(hcvrpNikos, T3) {
  int cost = solutionCostNikos(pathNikos, 8, 4, 0);
  ASSERT_EQ(702248, cost);
}

/* Should run in ~600ms */
TEST(hcvrpNikos, T4) {
  int cost = solutionCostNikos(pathNikos, 2, 2, 0);
  ASSERT_EQ(303768, cost);
}

/* Should run in ~100ms */
TEST(hcvrpNikos, T5) {
  int cost = solutionCostNikos(pathNikos, 1, 1, 0);
  ASSERT_EQ(247694, cost);
}

