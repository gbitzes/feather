#ifndef FEATHER_BACKEND_NSCONSTRAINT_H
#define FEATHER_BACKEND_NSCONSTRAINT_H

#include <backend/naxos/NsIntVar.h>
#include <feather/utils.h>
#include <algorithm>
#include "NsIntVarArray.h"
#include "naxos.h"

namespace feather {

class Ns_QueueItem;

class Ns_Constraint {

	public:
		/* 
		 * The timestamp (current removal ID) when the last check
		 * of this constraint took place
		 */

		unsigned long lastConstraintCheckTime;

		Ns_Constraint() {
			lastConstraintCheckTime = 0;
			revisionType = BOUNDS_CONSISTENCY;
		}

		virtual void ArcCons() = 0;
		virtual void LocalArcCons(Ns_QueueItem&) = 0;

		/* The number of the variables involved in the constraint */
		virtual int varsInvolvedIn() const = 0;
		/*
		 * Description of the consistency type that a revision function
		 * for a constraint can impose
		 */

		enum ConsistencyType {
			/*
			 * The revision function needs to know the value that has been
			 * removed from the domain (see NsConstrInverse)
			 */

			VALUE_CONSISTENCY = 1,

			/*
			 * The revision function does not need to know the removed values
			 */

			BOUNDS_CONSISTENCY,

			/*
			 * Like BOUNDS_CONSISTENCY, but revision is imposed in both directions
			 */

			BIDIRECTIONAL_CONSISTENCY
		};

		ConsistencyType revisionType;

		virtual ~Ns_Constraint() {}
};

class Ns_ConstrXlessthanY : public Ns_Constraint {
	private:
		NsIntVar *VarX, *VarY;
	public:
		Ns_ConstrXlessthanY(NsIntVar *x, NsIntVar *y) : VarX(x), VarY(y) {
			FEATHER_ASSERT(  &x->manager() == &y->manager() );
		}

		virtual int varsInvolvedIn() const { return 2; }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);

};

class Ns_ConstrXlesseqthanY : public Ns_Constraint {
	private:
		NsIntVar *VarX, *VarY;
	public:
		Ns_ConstrXlesseqthanY(NsIntVar *x, NsIntVar *y) : VarX(x), VarY(y) {
			FEATHER_ASSERT( &x->manager() == &y->manager() );
		}

		virtual int varsInvolvedIn() const { return 2; }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};

class Ns_ConstrXeqYplusC : public Ns_Constraint { 
	private:
		NsIntVar *VarX, *VarY;
		Int C;

	public:
		Ns_ConstrXeqYplusC(NsIntVar *x, NsIntVar *y, Int c_) : VarX(x), VarY(y), C(c_) {
			revisionType = BIDIRECTIONAL_CONSISTENCY;
			FEATHER_ASSERT( &x->manager() == &y->manager() );
		}

		virtual int varsInvolvedIn() const { return 2; }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};

class Ns_ConstrXeqCminusY : public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY;
		Int C;
	public:
		Ns_ConstrXeqCminusY(NsIntVar *X, const Int C_init, NsIntVar *Y)
		 : VarX(X), VarY(Y), C(C_init)
		{
			revisionType = BIDIRECTIONAL_CONSISTENCY;
			FEATHER_ASSERT(&VarX->manager() == &VarY->manager());
		}

		virtual int varsInvolvedIn() const { return 2; }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqYtimesC : public Ns_Constraint {
	private:
		NsIntVar *VarX, *VarY;
		Int C;
	public:
		Ns_ConstrXeqYtimesC(NsIntVar *X, NsIntVar *Y, const Int C_init)
		 : VarX(X), VarY(Y), C(C_init)
		{
			revisionType = BIDIRECTIONAL_CONSISTENCY;
			FEATHER_ASSERT(&VarX->manager() == &VarY->manager());
			FEATHER_ASSERT(C != 0);
		}

		virtual int varsInvolvedIn() const { return 2; }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem& Qitem);
};

//  The following constraint is somehow `stronger' than the simple `X == Y + C*Z'.
//   It requires some special conditions, that allow the efficient application of
//   the pure arc-consistency--i.e. not only bounds consistency.

class Ns_ConstrXeqYplusCZspecial : public Ns_Constraint {
	private:
		NsIntVar *VarX, *VarY, *VarZ;
		Int C;
	public:
		Ns_ConstrXeqYplusCZspecial (NsIntVar *X, NsIntVar *Y, const Int C_init, NsIntVar *Z)
		 : VarX(X), VarY(Y), VarZ(Z), C(C_init)
		{
			revisionType  =  VALUE_CONSISTENCY;


			FEATHER_ASSERT( &VarX->manager() == &VarY->manager()  &&  &VarY->manager() == &VarZ->manager());

			FEATHER_ASSERT(X->min() >= 0);
			FEATHER_ASSERT(0 <= Y->min()  &&  Y->max() < C);
			FEATHER_ASSERT(C > 0);
		}

		virtual int varsInvolvedIn() const {  return 3;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};




class Ns_ConstrXeqYplusZ: public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY, *VarZ;


	public:
		Ns_ConstrXeqYplusZ (NsIntVar *X, NsIntVar *Y, NsIntVar *Z)
		 : VarX(X), VarY(Y), VarZ(Z)
		{
			revisionType  =  BIDIRECTIONAL_CONSISTENCY;


			FEATHER_ASSERT( &VarX->manager() == &VarY->manager()  &&  &VarY->manager() == &VarZ->manager());
		}

		virtual int varsInvolvedIn() const {  return 3;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};




class Ns_ConstrXeqYtimesZ: public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY, *VarZ;
	public:
		Ns_ConstrXeqYtimesZ (NsIntVar *X, NsIntVar *Y, NsIntVar *Z)
		 : VarX(X), VarY(Y), VarZ(Z)
		{
			revisionType  =  BIDIRECTIONAL_CONSISTENCY;


			FEATHER_ASSERT( &VarX->manager() == &VarY->manager()  &&  &VarY->manager() == &VarZ->manager());
		}

		virtual int varsInvolvedIn() const {  return 3;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};




class Ns_ConstrXeqYdivC : public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY;
		Int C;


	public:
		Ns_ConstrXeqYdivC (NsIntVar *X, NsIntVar *Y, const Int C_init)
		 : VarX(X), VarY(Y), C(C_init)
		{
			revisionType  =  BIDIRECTIONAL_CONSISTENCY;


			FEATHER_ASSERT( &VarX->manager() == &VarY->manager());
			FEATHER_ASSERT(C != 0);
		}

		virtual int varsInvolvedIn() const {  return 2;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};




class Ns_ConstrXeqCdivY : public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY;
		Int C;


	public:
		Ns_ConstrXeqCdivY (NsIntVar *X, const Int C_init, NsIntVar *Y)
		 : VarX(X), VarY(Y), C(C_init)
		{
			revisionType  =  BIDIRECTIONAL_CONSISTENCY;


			FEATHER_ASSERT( &VarX->manager() == &VarY->manager() );
			FEATHER_ASSERT(Y->min() >= 0);
			FEATHER_ASSERT(C > 0);
			VarY->remove( 0 );
		}

		virtual int varsInvolvedIn() const {  return 2;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};




class Ns_ConstrXeqYmodC : public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY;
		Int C;


	public:
		Ns_ConstrXeqYmodC (NsIntVar *X, NsIntVar *Y, const Int C_init)
		 : VarX(X), VarY(Y), C(C_init)
		{
			revisionType  =  VALUE_CONSISTENCY;


			FEATHER_ASSERT( &VarX->manager() == &VarY->manager());
			FEATHER_ASSERT(Y->min() >= 0);
			FEATHER_ASSERT(C > 0);
		}

		virtual int varsInvolvedIn() const {  return 2;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};




class Ns_ConstrMetaXeqYlessthanC : public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY;
		Int C;


	public:
		Ns_ConstrMetaXeqYlessthanC (NsIntVar *X, NsIntVar *Y, const Int C_init)
		 : VarX(X), VarY(Y), C(C_init)
		{
			FEATHER_ASSERT( &VarX->manager() == &VarY->manager());
		}

		virtual int varsInvolvedIn() const {  return 2;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};




class Ns_ConstrMetaXeqYlessthanZ : public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY, *VarZ;


	public:
		Ns_ConstrMetaXeqYlessthanZ (NsIntVar *X, NsIntVar *Y, NsIntVar *Z)
		 : VarX(X), VarY(Y), VarZ(Z)
		{
			FEATHER_ASSERT( &VarX->manager() == &VarY->manager()  &&  &VarY->manager() == &VarZ->manager());
		}

		virtual int varsInvolvedIn() const {  return 3;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};




class Ns_ConstrMetaXeqYlesseqthanC : public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY;
		Int C;


	public:
		Ns_ConstrMetaXeqYlesseqthanC (NsIntVar *X, NsIntVar *Y, const Int C_init)
		 : VarX(X), VarY(Y), C(C_init)
		{
			FEATHER_ASSERT( &VarX->manager() == &VarY->manager());
		}

		virtual int varsInvolvedIn() const {  return 2;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};




class Ns_ConstrMetaXeqYlesseqthanZ : public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY, *VarZ;
	public:
		Ns_ConstrMetaXeqYlesseqthanZ (NsIntVar *X, NsIntVar *Y, NsIntVar *Z)
		 : VarX(X), VarY(Y), VarZ(Z)
		{
			FEATHER_ASSERT( &VarX->manager() == &VarY->manager()  &&  &VarY->manager() == &VarZ->manager());
		}

		virtual int varsInvolvedIn() const {  return 3;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};




class Ns_ConstrMetaXeqYgreaterthanC : public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY;
		Int C;
	public:
		Ns_ConstrMetaXeqYgreaterthanC (NsIntVar *X, NsIntVar *Y, const Int C_init)
		 : VarX(X), VarY(Y), C(C_init)
		{
			FEATHER_ASSERT( &VarX->manager() == &VarY->manager());
		}

		virtual int varsInvolvedIn() const {  return 2;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};




class Ns_ConstrMetaXeqYgreatereqthanC : public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY;
		Int C;
	public:
		Ns_ConstrMetaXeqYgreatereqthanC (NsIntVar *X, NsIntVar *Y, const Int C_init)
		 : VarX(X), VarY(Y), C(C_init)
		{
			FEATHER_ASSERT( &VarX->manager() == &VarY->manager());
		}

		virtual int varsInvolvedIn() const {  return 2;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};




class Ns_ConstrMetaXeqYeqC : public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY;
		Int C;


	public:
		Ns_ConstrMetaXeqYeqC (NsIntVar *X, NsIntVar *Y, const Int C_init)
		 : VarX(X), VarY(Y), C(C_init)
		{
			FEATHER_ASSERT( &VarX->manager() == &VarY->manager());
		}

		virtual int varsInvolvedIn() const {  return 2;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};




class Ns_ConstrMetaXeqYeqZ : public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY, *VarZ;
		const bool  neg;
		// If `neg==true' the constraint becomes 'Ns_ConstrMetaXeqY neq Z'.


	public:
		Ns_ConstrMetaXeqYeqZ (NsIntVar *X, NsIntVar *Y, NsIntVar *Z, const bool pos)
		 : VarX(X), VarY(Y), VarZ(Z), neg(!pos)
		{
			FEATHER_ASSERT( &VarX->manager() == &VarY->manager()  &&  &VarY->manager() == &VarZ->manager());
		}

		virtual int varsInvolvedIn() const {  return 3;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};




class Ns_ConstrMetaXeqYneqC : public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY;
		Int C;


	public:
		Ns_ConstrMetaXeqYneqC (NsIntVar *X, NsIntVar *Y, const Int C_init)
		 : VarX(X), VarY(Y), C(C_init)
		{
			FEATHER_ASSERT( &VarX->manager() == &VarY->manager());
		}

		virtual int varsInvolvedIn() const {  return 2;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};




//class Ns_ConstrMetaXeqYneqZ : public Ns_Constraint  {
//  private:
//		NsIntVar *VarX, *VarY, *VarZ;
//
//	public:
//		Ns_ConstrMetaXeqYneqZ (NsIntVar *X, NsIntVar *Y, NsIntVar *Z)
//			: VarX(X), VarY(Y), VarZ(Z)
//		{
//			FEATHER_ASSERT( VarX->manager() == VarY->manager()  &&  VarY->manager() == VarZ->manager());
//		}
//
//		virtual int varsInvolvedIn() const {  return 3;  }
//
//		virtual void  ArcCons      (void)  const;
//		virtual void  LocalArcCons (Ns_QueueItem& Qitem)  const;
//};

class Ns_ConstrXeqYandZ : public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY, *VarZ;
		const bool  neg;
		// If `neg==true' the constraint becomes 'Ns_ConstrXeqY nand Z'.


	public:
		Ns_ConstrXeqYandZ (NsIntVar *X, NsIntVar *Y, NsIntVar *Z, const bool pos)
		 : VarX(X), VarY(Y), VarZ(Z), neg(!pos)
		{
			FEATHER_ASSERT( &VarX->manager() == &VarY->manager()  &&  &VarY->manager() == &VarZ->manager());
			FEATHER_ASSERT( 0 <= VarX->min() && VarX->max() <= 1  &&  0 <= VarY->min() && VarY->max() <= 1  &&  0 <= VarZ->min() && VarZ->max() <= 1);
		}

		virtual int varsInvolvedIn() const {  return 3;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};




class Ns_ConstrXeqYorZ : public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY, *VarZ;
		const bool  neg;
		// If `neg==true' the constraint becomes 'Ns_ConstrXeqY nor Z'.


	public:
		Ns_ConstrXeqYorZ (NsIntVar *X, NsIntVar *Y, NsIntVar *Z, const bool pos)
		 : VarX(X), VarY(Y), VarZ(Z), neg(!pos)
		{
			FEATHER_ASSERT( &VarX->manager() == &VarY->manager()  &&  &VarY->manager() == &VarZ->manager());
			FEATHER_ASSERT( 0 <= VarX->min() && VarX->max() <= 1  &&  0 <= VarY->min() && VarY->max() <= 1  &&  0 <= VarZ->min() && VarZ->max() <= 1);
		}

		virtual int varsInvolvedIn() const {  return 3;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};




class Ns_ConstrXorY : public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY;
		const bool  neg;
		// If `neg==true' the constraint becomes 'Constr not X and not Y'.


	public:
		Ns_ConstrXorY (NsIntVar *X, NsIntVar *Y, const bool pos)
		 : VarX(X), VarY(Y), neg(!pos)
		{
			FEATHER_ASSERT( &VarX->manager() == &VarY->manager());
			FEATHER_ASSERT( 0 <= VarX->min() && VarX->max() <= 1  &&  0 <= VarY->min() && VarY->max() <= 1);
		}

		virtual int varsInvolvedIn() const {  return 2;  }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);
};

class Ns_ConstrXeqY : public Ns_Constraint  {

	private:

		NsIntVar  *VarX, *VarY;


	public:

		Ns_ConstrXeqY (NsIntVar *X, NsIntVar *Y)
		 : VarX(X), VarY(Y)
		{
			revisionType  =  BIDIRECTIONAL_CONSISTENCY;


			FEATHER_ASSERT( &VarX->manager() == &VarY->manager());
		}

		virtual int   varsInvolvedIn (void)  const    {  return 2;  }
		virtual void  ArcCons      (void);
		virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrXneqY : public Ns_Constraint  {
	private:
		NsIntVar *VarX, *VarY;
	public:
		Ns_ConstrXneqY (NsIntVar *X, NsIntVar *Y)
		 : VarX(X), VarY(Y)
		{
			FEATHER_ASSERT( &VarX->manager() == &VarY->manager());
		}

		virtual int   varsInvolvedIn (void)  const    {  return 2;  }
		virtual void  ArcCons      (void);
		virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqAbsY : public Ns_Constraint  {
	private:
		NsIntVar  *VarX, *VarY;
	public:
		Ns_ConstrXeqAbsY (NsIntVar *X, NsIntVar *Y)
		 : VarX(X), VarY(Y)
		{
			revisionType  =  BIDIRECTIONAL_CONSISTENCY;


			FEATHER_ASSERT( &VarX->manager() == &VarY->manager() );
		}

		virtual int   varsInvolvedIn (void)  const    {  return 2;  }
		virtual void  ArcCons      (void);
		virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrAllDiff : public Ns_Constraint {
	private:
		NsIntVarArray *VarArr;
	public:
		Ns_ConstrAllDiff(NsIntVarArray *VarArr_init);

		virtual int varsInvolvedIn() const { return VarArr->size(); }

		virtual void  ArcCons      (void);
		virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrAllDiffStrong : public Ns_Constraint  {

	public:

		//  `groupedNsIntVar', as the name suggests, is a class that
		//   extends `NsIntVar', by adding the information concerning
		//   the id of the group taht the constrained variable belongs to.

		class  groupedNsIntVar  {

			public:

				NsIntVar&  Var;

			private:

				NsIntVar  vGroup;


			public:

				typedef  Int  group_t;

				static const group_t  FIRST_GROUP  =  kMinusInf + 1;


				groupedNsIntVar (NsIntVar& Var_init)
				 : Var(Var_init), vGroup(Var.manager(), FIRST_GROUP, kPlusInf-1)	{	}

					group_t
				group (void)
				{
					return  vGroup.min();
				}


					void
				setGroup (const group_t groupVal)
				{
					assert_Ns( vGroup.contains(groupVal-1),  "groupedNsIntVar::setGroup: `groupVal-1' is not contained in`vGroup'" );
					assert_Ns( vGroup.removeRange(kMinusInf, groupVal-1, 0),  "groupedNsIntVar::setGroup: Could not change group to `groupVal'" );
					assert_Ns( group() == groupVal,  "groupedNsIntVar::setGroup: Not succesful change of group to `groupVal'" );
				}


				bool  removeDomain (const NsIntVar& V, const Ns_Constraint *c);
		};



	private:

		NsDeque<groupedNsIntVar>  VarArr;

		typedef  Ns_UNORDERED_MAP<Ns_pointer_t,groupedNsIntVar*>  VarPointerGroup_t;

		VarPointerGroup_t  VarPointerGroup;

		//NsIntVarArray  VarArrGroup;
		//NsIntVar       vGroupCounter;

		const unsigned long   Capacity;


	public:

		Ns_ConstrAllDiffStrong (NsIntVarArray *VarArr_init, const unsigned long Cap);

		virtual int   varsInvolvedIn (void)  const    {  return VarArr.size();  }


		//	virtual void
		//toGraphFile (std::ofstream& fileConstraintsGraph)  const
		//{
		//	Ns_arrayConstraintToGraphFile(
		//		fileConstraintsGraph,
		//		&VarArr, this, "!= strong");
		//}


		virtual void  ArcCons      (void);
		virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrXeqSum : public Ns_Constraint  {
	private:
		NsIntVar  *VarX;
		NsIntVarArray  *VarArr;
		NsIndex  start, length;
	public:
		Ns_ConstrXeqSum (NsIntVar *X, NsIntVarArray *VarArr_init);
		Ns_ConstrXeqSum (NsIntVar *X, NsIntVarArray *VarArr_init,
				const NsIndex start_init, const NsIndex length_init);
		virtual int varsInvolvedIn() const {  return  (1 + length);  }

		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem& Qitem);
};


class Ns_ConstrElement : public Ns_Constraint  {
	private:
		NsIntVar *VarIndex;
		const NsDeque<Int>&  intArray;
		NsIntVar  *VarValue;
	public:
		Ns_ConstrElement (NsIntVar *VarIndex_init,
				const NsDeque<Int>& intArray_init,
				NsIntVar *VarValue_init)
			: VarIndex(VarIndex_init),
			  intArray(intArray_init),
			  VarValue(VarValue_init)
		{
			revisionType  =  VALUE_CONSISTENCY;
			assert_Ns( &VarIndex->manager() == &VarValue->manager(),  "Ns_ConstrElement::Ns_ConstrElement: All the variables of a constraint must belong to the same NsProblemManager");
			assert_Ns( ! intArray.empty() ,  "Ns_ConstrElement::Ns_ConstrElement: Empty element array" );
		}
	
		virtual int   varsInvolvedIn (void)  const    {  return 2;  }

		virtual void  ArcCons      (void);
		virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};

class Ns_ConstrCount : public Ns_Constraint  {
	private:
		NsIntVarArray  *VarArr;
		//const NsDeque<NsInt>&  Values;
		//const NsDeque<NsInt>&  Occurrences;

		/////  vCount[i].max() is the number of the occurrences of Values[i] in VarArr.
		//NsIntVarArray  vCount;

		/////  vMinValueIndex.min() is the minimum index i in a sorted array Values[i], with the property that the occurrences of the corresponding value is greater than zero.
		//NsIntVar  vMinValueIndex;

		/////  vMaxValueIndex.max() is the vMinValueIndex.min() maximum equivalent.
		//NsIntVar  vMaxValueIndex;

		typedef  Ns_UNORDERED_MAP<Ns_pointer_t,NsIndex>  VarIndex_t;

		VarIndex_t  VarIndex;


	public:
		typedef  Ns_UNORDERED_MAP<Int,NsIndex>  ValueIndex_t;

	private:
		ValueIndex_t  ValueIndex;


	public:
		struct  ValueOccurrence_t  {

			Int  value;

			Int  occurrence;

			const NsDeque<NsIndex>  *split_positions;

			NsIntVarArray  vCount;


			ValueOccurrence_t (const Int value_init)
				: value(value_init)
			{
			}


			ValueOccurrence_t (const Int value_init,
				const Int occurrence_init,
				Naxos& pm)
				: value(value_init),
				  occurrence(occurrence_init),
				  split_positions(0)
			{
				assert_Ns( occurrence >= 0 , "Ns_ConstrCount::ValueOccurrence_t::ValueOccurrence_t: negative value in `occurrence'");

				vCount.push_back( *(pm.makeVar(0, occurrence)) );
			}


			ValueOccurrence_t (const Int value_init,
				const Int occurrence_init,
				Naxos& pm,
				const NsDeque<NsIndex>& split_positions_init,
				const NsIndex Split)
				: value(value_init),
				  occurrence(occurrence_init),
				  split_positions(&split_positions_init)
			{
				assert_Ns( occurrence >= 0 , "Ns_ConstrCount::ValueOccurrence_t::ValueOccurrence_t: negative value in `occurrence'");

				for (NsIndex i=0;  i < split_positions->size();  ++i)
					vCount.push_back(  *(pm.makeVar(0, Split)) );

				vCount.push_back(  *(pm.makeVar(0, occurrence%Split)) );

				if ( split_positions->size()  ==  0 )
					split_positions  =  0;
			}


				NsIndex
			splitIndexForPosition (NsIndex i)
			{
				if ( split_positions  ==  0 )
					return  0;

				NsDeque<NsIndex>::const_iterator  cit =
					lower_bound(
						split_positions->begin(),
						split_positions->end(),
						i);

				return  ( cit - split_positions->begin() );
			}


				bool
			operator  <  (const ValueOccurrence_t& Y)  const
			{
				return  ( value  <  Y.value );
			}
		};


	private:
		NsDeque<ValueOccurrence_t>  ValuesOccurrences;
		const NsIndex  Split;
		const NsIndex  Dwin;
	public:

		Ns_ConstrCount (NsIntVarArray *VarArr_init,
			const NsDeque<Int>& Values,
			const NsDeque<Int>& Occurrences,
			const NsDeque< NsDeque<NsIndex> >& SplitPositions,
			const NsIndex Split_init,
			const NsIndex Dwin_init);

		virtual int   varsInvolvedIn (void)  const    {  return VarArr->size();  }

		virtual void  ArcCons      (void);
		virtual void  LocalArcCons (Ns_QueueItem& Qitem);
};



} // namespace feather

#endif
