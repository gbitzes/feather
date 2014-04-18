#include <backend/naxos/Ns_Constraint.h>
#include <backend/naxos/naxos.h>
#include <backend/naxos/internal.h>
#include <base/utils.h>
#include <cstdlib>
#include <utility>

using namespace std;

namespace feather {

	void
array_min_minmax (const NsIntVarArray *VarArr, Int& min, Int& minmax)
{
	NsIntVarArray::const_iterator V = VarArr->begin();
	min = V->min();
	minmax = V->max();
	++V;
	for ( ;   V != VarArr->end();   ++V)   {
		if ( V->min() < min)
			min = V->min();
		if ( V->max() < minmax)
			minmax = V->max();
	}
}

	void
array_maxmin_max (const NsIntVarArray *VarArr, Int& maxmin, Int& max)
{
	NsIntVarArray::const_iterator  V = VarArr->begin();
	maxmin = V->min();
	max = V->max();
	++V;
	for ( ;   V != VarArr->end();   ++V)   {
		if (V->min() > maxmin)
			maxmin = V->min();
		if (V->max() > max)
			max = V->max();
	}
}

	void
array_sum_min_max (const NsIntVarArray *VarArr, const NsIndex start, const NsIndex length,
			Int& sumMin, Int& sumMax)
{
	sumMin = sumMax = 0;
	for (NsIndex  i=start;   i < start+length;   ++i)   {
		const NsIntVar&  V = (*VarArr)[i];
		sumMin += V.min();
		sumMax += V.max();
	}
	//for (NsIntVarArray::iterator V = VarArr->begin();   V != VarArr->end();   ++V)   {
	//	sumMin += V->min();
	//	sumMax += V->max();
	//}
}

//#ifndef Ns_AllDiff_Test


Ns_ConstrAllDiff::Ns_ConstrAllDiff (NsIntVarArray *VarArr_init)
	: VarArr(VarArr_init)
{
	assert_Ns( VarArr->size() >= 2 ,
			"Ns_ConstrAllDiff::Ns_ConstrAllDiff: Condition required:  VarArr.size() >= 2");

	NsIntVarArray::iterator  X = VarArr->begin();
	Naxos&  pm = X->manager();
	++X;

	for ( ;   X != VarArr->end();   ++X)  {
		assert_Ns( &pm == &X->manager(),  "Ns_ConstrAllDiff::Ns_ConstrAllDiff: All the variables of a constraint must belong to the same NsProblemManager");
	}
}




namespace  {

		void
	allDiffArcCons (NsIntVarArray *VarArr,
			NsQueue<const NsIntVar*>& newBoundVars,
			const Ns_Constraint *constraint)
	{
		while ( ! newBoundVars.empty() )    {

			Int val = newBoundVars.front()->value();

			for (NsIntVarArray::iterator X = VarArr->begin();
					X != VarArr->end();
					++X)
			{
				if ( &*X  !=  newBoundVars.front() )    {

					bool  wasBound = X->isBound();

					X->removeSingle( val, constraint );

					if ( ! wasBound  &&  X->isBound() )
						newBoundVars.push( &*X );
				}
			}

			newBoundVars.pop();
		}
	}

} // namespace



	void
Ns_ConstrAllDiff::ArcCons (void)
{
	NsIntVarArray::iterator  X, Y;

	// Dovecote Theorem (to spot a dead-end earlier) //
	Int min_of_arr, max_of_arr;

	X = VarArr->begin();
	min_of_arr = X->min();
	max_of_arr = X->max();
	++X;

	for ( ;   X != VarArr->end();   ++X)  {
		if (X->min() < min_of_arr)
			min_of_arr = X->min();
		if (X->max() > max_of_arr)
			max_of_arr = X->max();
	}

	if ( static_cast<UInt>(max_of_arr - min_of_arr) + 1  <  VarArr->size() )    {

		VarArr->begin()->removeAll();	// all values are inconsistent
		return;
	}


	// Classic ArcCons //

	NsQueue<const NsIntVar*>  newBoundVars;

	for (X = VarArr->begin();   X != VarArr->end();   ++X)    {

		if ( X->isBound() )
			newBoundVars.push( &*X );
	}

	allDiffArcCons(VarArr, newBoundVars, this);

	//for (X = VarArr->begin();   X != VarArr->end();   ++X)  {
	//	for (Y = VarArr->begin();   Y != VarArr->end();   ++Y)  {
	//		if ( Y != X   &&   Y->isBound() )
	//			X->removeSingle( Y->value(), 0 );
	//		//  `0' is used in order not to record this constraint
	//		//   that provoked the removal, because
	//		//   Ns_ConstrAllDiff::LocalArcCons() should be called
	//		//   to check again the constraint AllDiff (against
	//		//   this removal).
	//	}
	//}
}


	void
Ns_ConstrAllDiff::LocalArcCons (Ns_QueueItem& Qitem)
{
	if ( Qitem.getVarFired()->isBound() )    {

		NsQueue<const NsIntVar*>  newBoundVars;

		newBoundVars.push( Qitem.getVarFired() );

		allDiffArcCons(VarArr, newBoundVars, this);
	}


	//if ( Qitem.getVarFired()->isBound() )    {

	//	Int Value = Qitem.getVarFired()->value();

	//	for (NsIntVarArray::iterator X = VarArr->begin();   X != VarArr->end();   ++X)  {
	//		if ( &*X  !=  Qitem.getVarFired() )
	//			X->removeSingle( Value, 0 );
	//		//  `0' is used in order not to record this constraint...
	//		//   (see the previous comment).
	//	}
	//}
}


//#endif  // ! Ns_AllDiff_Test






	bool
Ns_ConstrAllDiffStrong::groupedNsIntVar::removeDomain (const NsIntVar& V, const Ns_Constraint *constraint)
{
	if ( Var.max()  <=  V.max() )   {

		//  In this case, and when we have bounds-consistency mode (i.e. a
		//   constrained variable is represented by its minimum and maximum),
		//   we `eat' the values from `right' to `left', in order not to avoid
		//   (if possible) removing a value from the middle of the domain.

		for (NsIntVar::const_reverse_iterator val = V.rbegin();  val != V.rend();   ++val)    {

			//if ( Var.contains(*val) )   {

				if ( ! Var.removeSingle( *val , constraint ) )
					return  false;
			//}
		}


	} else   {

	      for (NsIntVar::const_iterator val = V.begin();  val != V.end();   ++val)    {

	      	//if ( Var.contains(*val) )   {

	      		if ( ! Var.removeSingle(*val, constraint) )
	      			return  false;
	      	//}
	      }
	}

	return  true;
}











//struct less_max_mmin_size : public binary_function<NsIntVar, NsIntVar, bool>  {
//
//	bool  operator ()  (const MaxLopez& X, const MaxLopez& Y)
//	{
//		return  ( X.max() < Y.max()
//				|| ( X.max() == Y.max()  &&  X.min() >  Y.min() )
//				|| ( X.max() == Y.max()  &&  X.min() == Y.min()  &&  X.size() < Y.size() ) );
//	}
//
//};
//
//
//struct less_min           : public binary_function<NsIntVar, NsIntVar, bool>  {
//
//	bool  operator ()  (const MaxLopez& X, const MaxLopez& Y)
//	{
//		return  ( X.min() >  Y.min() );
//	}
//
//};



//#ifdef Ns_AllDiff_Test


Ns_ConstrAllDiffStrong::Ns_ConstrAllDiffStrong (NsIntVarArray *VarArr_init, unsigned long Cap)
	: Capacity(Cap)
{
	assert_Ns( VarArr_init->size() > Capacity,
			"Ns_ConstrAllDiffStrong::Ns_ConstrAllDiffStrong: Condition required:  VarArr.size() >= 2");
	assert_Ns( Capacity > 0,
			"Ns_ConstrAllDiffStrong::Ns_ConstrAllDiffStrong: Condition required:  Capacity > 0");

	NsIntVarArray::iterator  V = VarArr_init->begin();
	Naxos& pm = V->manager();

	for ( ;   V != VarArr_init->end();   ++V)   {

		assert_Ns( &pm == &V->manager(),  "Ns_ConstrAllDiffStrong::Ns_ConstrAllDiffStrong: All the variables of a constraint must belong to the same NsProblemManager");

		VarArr.push_back( groupedNsIntVar(*V) );
		VarPointerGroup.insert( make_pair( (Ns_pointer_t)&*V  ,  &VarArr.back() ) );
	}

	//for (X = VarArr->begin();   X != VarArr->end();   ++X)
	//	VarArrGroup.push_back( NsIntVar(pm, NsMINUS_INF+1, NsPLUS_INF-1) );
}



namespace  {

		int
	less_function_MaxMMinSize  (const void *X, const void *Y)
	{
		NsIntVar&  VarX = (*static_cast<Ns_ConstrAllDiffStrong::groupedNsIntVar* const *>( X ))->Var;
		NsIntVar&  VarY = (*static_cast<Ns_ConstrAllDiffStrong::groupedNsIntVar* const *>( Y ))->Var;

		if ( VarX.max()  <  VarY.max()
			|| ( VarX.max() == VarY.max()  &&  ( -VarX.min() < -VarY.min()
					||  ( VarX.min() == VarY.min()  &&  VarX.size()  <  VarY.size() ) ) ) )
		{
			return  -1;
		}

		if ( VarX.max() == VarY.max()  &&  VarX.min() == VarY.min()  &&  VarX.size() == VarY.size() )
			return  0;

		return  +1;
	}





		void
	allDiffBoundsConsistency (NsDeque<Ns_ConstrAllDiffStrong::groupedNsIntVar>& VarArr,
			const unsigned long Capacity,
			const Ns_ConstrAllDiffStrong::groupedNsIntVar::group_t group,
			const Ns_Constraint* constraint)
	{
	//	cout << &VarArr << ": ";
	//	for (NsIndex k=0;   k < VarArr.size();   ++k)
	//		cout << "  " << (VarArr[k].group() - groupedNsIntVar::FIRST_GROUP) << ">" << VarArr[k].Var;
	//	cout << endl;

	//for (groupedNsIntVar::group_t group = groupedNsIntVar::FIRST_GROUP;   /* VOID */ ;   ++group)   {

		//  At first, we gather all the variables with group-id
		//   equal to `group' into the array `VarArrSortedList'.

		NsDeque<Ns_ConstrAllDiffStrong::groupedNsIntVar*>  VarArrSortedList;

		NsIndex  i;

		Ns_ConstrAllDiffStrong::groupedNsIntVar::group_t  nGroups =
			Ns_ConstrAllDiffStrong::groupedNsIntVar::FIRST_GROUP;
		for (i=0;   i < VarArr.size();   ++i)   {

			if ( VarArr[i].group()  ==  group )
				VarArrSortedList.push_back( &VarArr[i] );

			if ( VarArr[i].group()  >   nGroups )
				nGroups = VarArr[i].group();
		}

		assert_Ns( ! VarArrSortedList.empty() ,  "allDiffBoundsConsistency: Empty `group' of variables" );
			//break;	// We have processed all the separate groups of variables.



		//  Copying `VarArrSortedList' to `VarArrSorted' which is friendlier
		//   to the `qsort()' function that it is used bellow.

		Ns_ConstrAllDiffStrong::groupedNsIntVar  **VarArrSorted =
						new Ns_ConstrAllDiffStrong::groupedNsIntVar*[VarArrSortedList.size()];

		for (i=0;  i < VarArrSortedList.size();  ++i)
			VarArrSorted[i] = VarArrSortedList[i];


		//  ... Then we sort the variables in `VarArrSorted', by ascending maximum,
		//   descending minimum, and ascending size.  I.e., if the domain of VarX
		//   is more possible to be a subset of domain of VarY, then VarX precedes
		//   VarY in the array `VarArrSorted'.

		qsort(VarArrSorted, VarArrSortedList.size(), sizeof(Ns_ConstrAllDiffStrong::groupedNsIntVar*),
											less_function_MaxMMinSize);



		//  We gather all the different maxima into the array `Max' by ascending order.

		NsDeque<Int>  Max;

		for (i=0;  i < VarArrSortedList.size();  ++i)   {

			if ( i == 0   ||   ( i > 0  &&  VarArrSorted[i]->Var.max() != VarArrSorted[i-1]->Var.max() ) )
				Max.push_back( VarArrSorted[i]->Var.max() );
		}



		NsDeque<Ns_ConstrAllDiffStrong::groupedNsIntVar*>  VarLtMax;
		NsDeque<Int>             VarLtMaxMins;

		i = 0;
		for (NsIndex max_i = 0;   max_i < Max.size();   ++max_i)    {

			//  `VarEqMax' consists of all the variables of the
			//   array `VarArr' with `VarArr[i].max() == Max[max_i]'.

			NsDeque<Ns_ConstrAllDiffStrong::groupedNsIntVar*>  VarEqMax;

			for ( ;   i < VarArrSortedList.size()  &&  VarArrSorted[i]->Var.max() == Max[max_i];   ++i)
				VarEqMax.push_back( VarArrSorted[i] );


			//  `VarLeMax' is constructed by merging its previous contents,
			//   represented by `VarLtMax' (initially empty), and `VarEqMax'.
			//   `VarLeMax' contains the constrained variables of `VarArr'
			//   with `VarArr[i].max() <= Max[max_i]'.

			NsDeque<Ns_ConstrAllDiffStrong::groupedNsIntVar*>  VarLeMax;

			NsIndex  i1, i2;

			for (i1=i2=0;  i1 < VarLtMax.size()  &&  i2 < VarEqMax.size();   )     {

				if ( VarLtMax[i1]->Var.min()  >=  VarEqMax[i2]->Var.min() )   {

					VarLeMax.push_back( VarLtMax[i1] );
					++i1;

				} else   {

					VarLeMax.push_back( VarEqMax[i2] );
					++i2;
				}
			}

			//  One of the two arrays (`VarLtMax' and `VarEqMax') is
			//   exhausted, so we append the contents of the other
			//   array to `VarLeMax', and the merging is completed.

			for ( ;   i1 < VarLtMax.size();   ++i1)
				VarLeMax.push_back( VarLtMax[i1] );

			for ( ;   i2 < VarEqMax.size();   ++i2)
				VarLeMax.push_back( VarEqMax[i2] );

			VarLtMax = VarLeMax;	// `VarLtMax' will be used at the next iteration.



			//  `VarLeMaxMins' contains the minima of the constrained variables
			//   included in `VarLeMax', in descending order (without duplicates).
			//   It is constructed by merging the array `VarLtMaxMins' (i.e., the
			//   previous contents of `VarLeMaxMins', that have no duplicates, by
			//   definition) and `VarEqMax'.mins (i.e., the minima of the
			//   constrained variables in `VarEqMax' that may contain duplicates,
			//   so we should avoid them).

			NsDeque<Int>  VarLeMaxMins;

			for (i1=i2=0;  i1 < VarLtMaxMins.size()  &&  i2 < VarEqMax.size();   )     {

				if ( VarLtMaxMins[i1]  >  VarEqMax[i2]->Var.min() )   {

					VarLeMaxMins.push_back( VarLtMaxMins[i1] );
					++i1;

				} else if ( VarLtMaxMins[i1]  <  VarEqMax[i2]->Var.min() )   {

					VarLeMaxMins.push_back( VarEqMax[i2]->Var.min() );
					do {
						++i2;	// avoiding duplicates

					} while ( i2 < VarEqMax.size()
							&&  VarEqMax[i2]->Var.min() == VarEqMax[i2-1]->Var.min() );

				} else   {	// The two minima are equal.

					VarLeMaxMins.push_back( VarLtMaxMins[i1] );
					++i1;

					do {
						++i2;	// avoiding duplicates

					} while ( i2 < VarEqMax.size()
							&&  VarEqMax[i2]->Var.min() == VarEqMax[i2-1]->Var.min() );
				}
			}

			//  We complete the merging by appending the contents of the non-
			//   exhausted array (`VarLtMaxMins' or `VarEqMax') to `VarLeMaxMins'.

			for ( ;   i1 < VarLtMaxMins.size();   )     {

				VarLeMaxMins.push_back( VarLtMaxMins[i1] );
				++i1;
			}

			for ( ;   i2 < VarEqMax.size();   )     {

				VarLeMaxMins.push_back( VarEqMax[i2]->Var.min() );
				do {
					++i2;	// avoiding duplicates

				} while ( i2 < VarEqMax.size()
						&&  VarEqMax[i2]->Var.min() == VarEqMax[i2-1]->Var.min() );
			}

			VarLtMaxMins = VarLeMaxMins;	// `VarLtMaxMins' will be used at the next iteration.




			//  `vUnion' is a domain representing the union of the
			//   domains that we have examined so far.  Inintially,
			//   it equals to the domain of `VarLeMax[0]'.

			NsIntVar  *vUnion = new NsIntVar(VarLeMax[0]->Var.manager(), VarLeMax[0]->Var.min(), VarLeMax[0]->Var.max());

			// Does not need to be stored for backtracking, because it is temporary.
			vUnion->transparent();

			NsIntVar::const_gap_iterator  gapVal;

			for (gapVal = VarLeMax[0]->Var.gap_begin();  gapVal != VarLeMax[0]->Var.gap_end();  ++gapVal)   {

				vUnion->remove(*gapVal);

				//assert_Ns( vUnion->removeSingle(*gapVal, constraint),
				//		"Ns_ConstrAllDiffStrong::ArcCons: `vUnion->removeSingle(*gapVal)' failed" );
			}




			//  Iterating through the `VarLeMaxMins', which
			//   has been renamed for readability as `Min'.

			NsDeque<Int>  &Min = VarLeMaxMins;

			NsIndex  nVars = 0;
			NsIndex  j = 0;

			for (NsIndex min_i=0;   min_i < Min.size();   ++min_i)   {

				// Iterating through `VarLeMax'...

				for ( ;   j < VarLeMax.size();   ++j)    {

					if ( VarLeMax[j]->Var.min() < Min[min_i] )
						break;	// End of `Min', no propagation.

					++nVars;

					//  We produce the union of the domains `vUnion' and `VarLeMax[j]'.  I.e.,
					//      vUnion  <---  vUnion  UNION  VarLeMax[j].domain.

					NsIntVar  vUnionPrevious = *vUnion;

					delete  vUnion;

					vUnion = new NsIntVar(
							vUnionPrevious.manager(),
							min(vUnionPrevious.min(), VarLeMax[j]->Var.min()),
							max(vUnionPrevious.max(), VarLeMax[j]->Var.max()));


					// Does not need to be stored for backtracking, because it is temporary.
					vUnion->transparent();

					//  Removing the values that neither domain contains.

					for (gapVal = vUnionPrevious.gap_begin();
							gapVal != vUnionPrevious.gap_end();
							++gapVal)
					{
						if ( ! VarLeMax[j]->Var.contains( *gapVal ) )   {
							vUnion->remove(*gapVal);
							//assert_Ns( vUnion->removeSingle(*gapVal,constraint),  "Ns_ConstrAllDiffStrong::ArcCons: `vUnion->removeSingle(*gapVal)' failed" );
						}
					}

					for (gapVal = VarLeMax[j]->Var.gap_begin();
							gapVal != VarLeMax[j]->Var.gap_end();
							++gapVal)
					{
						if ( ! vUnionPrevious.contains( *gapVal ) )
						{
							vUnion->remove(*gapVal);
							//assert_Ns( vUnion->Single(*gapVal,constraint),  "Ns_ConstrAllDiffStrong::ArcCons: `vUnion->removeSingle(*gapVal)' failed" );
						}
					}




					NsIndex  sizeXcap  =  vUnion->size() * Capacity;

					if ( sizeXcap  <  nVars )    {

						//  The number of the variables is too big to take
						//   `sizeXcap' different values.   ==>   Failure

						VarLeMax[0]->Var.removeAll();

						delete  vUnion;
						delete  []  VarArrSorted;

						return;
					}


					if ( sizeXcap  ==  nVars
							&&  ( j < VarLeMax.size()-1  ||  i < VarArrSortedList.size() ) )
					{

						//  The number of the variables is the minimum needed to take
						//   `sizeXcap' different values (contained in `vUnion').
						//   Each variable has to be assigned a value from `vUnion', and
						//   all the values will be used.  So no other variable should be
						//   assigned a value from `vUnion'.  Thus, it is possible to
						//   subtract this domain from all the other variables, and to
						//   separate them from the current group, by changing their group.
						//   (This domain is in other words a `Hall Interval'.)

						++nGroups;

//						bool changed=false;
						for (++j;  j < VarLeMax.size();  ++j)   {
//							changed = true;

							if ( ! VarLeMax[j]->removeDomain(*vUnion, constraint) )    {

								delete  vUnion;
								delete  []  VarArrSorted;

								return;
							}

							VarLeMax[j]->setGroup( nGroups );
						}

						for ( ;  i < VarArrSortedList.size();  ++i)   {
//							changed = true;

							if ( ! VarArrSorted[i]->removeDomain(*vUnion, constraint) )    {

								delete  vUnion;
								delete  []  VarArrSorted;

								return;
							}

							VarArrSorted[i]->setGroup( nGroups );
						}

						allDiffBoundsConsistency(VarArr, Capacity, nGroups, constraint);

						max_i = Max.size();

//						if ( changed )   {
//	cout << "           ";
//	for (NsIndex k=0;   k < VarArr.size();   ++k)
//		cout << "  " << (VarArr[k].group() - groupedNsIntVar::FIRST_GROUP) << ">" << VarArr[k].Var;
//	cout << endl;
//						}
					}


					if ( sizeXcap  >  VarLeMax.size() )    {

						//  The number of the values `sizeXcap' to be assigned to be assigned
						//   to different variables of the array `VarLeMax' is already greater
						//   than the minimum needed, so there is no need to examine the rest
						//   of the variables of `VarLeMax'.  Thus, we override them (`early
						//   cut-off') using the following statements.

						min_i = Min.size();
						break;
					}

				}
			}

			delete  vUnion;

		}

		delete  []  VarArrSorted;

	//} // groups iteration
	}

} // namespace





// bounds-consistency only
	void
Ns_ConstrAllDiffStrong::ArcCons (void)
{
	allDiffBoundsConsistency(VarArr, Capacity, groupedNsIntVar::FIRST_GROUP, this);
}


// bounds-consistency only
	void
Ns_ConstrAllDiffStrong::LocalArcCons (Ns_QueueItem& Qitem)
{
	//if (Qitem.getVarFired()->min() < Qitem.getW()  &&  Qitem.getW() < Qitem.getVarFired()->max())
	//	return;	// bounds-consistency does not care

	VarPointerGroup_t::const_iterator  cit = VarPointerGroup.find( (Ns_pointer_t)Qitem.getVarFired() );
	groupedNsIntVar::group_t  groupFired = cit->second->group();

	//ArcCons();
	allDiffBoundsConsistency(VarArr, Capacity, groupFired, this);
}










}