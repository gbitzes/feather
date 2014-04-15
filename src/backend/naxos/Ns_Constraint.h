#ifndef FEATHER_BACKEND_NSCONSTRAINT_H
#define FEATHER_BACKEND_NSCONSTRAINT_H

#include <backend/naxos/NsIntVar.h>
#include <base/utils.h>

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
		NsIntVar *varX, *varY;
	public:
		Ns_ConstrXlessthanY(NsIntVar *x, NsIntVar *y) : varX(x), varY(y) {
			FEATHER_ASSERT(  &x->manager() == &y->manager() );
		}

		virtual int varsInvolvedIn() const { return 2; }
		virtual void ArcCons();
		virtual void LocalArcCons(Ns_QueueItem&);

};

class Ns_ConstrXlesseqthanY : public Ns_Constraint {
	private:
		NsIntVar *varX, *varY;
	public:
		Ns_ConstrXlesseqthanY(NsIntVar *x, NsIntVar *y) : varX(x), varY(y) {
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








} // namespace feather

#endif