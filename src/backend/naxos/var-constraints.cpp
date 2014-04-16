#include <backend/naxos/Ns_Constraint.h>
#include <backend/naxos/naxos.h>
#include <backend/naxos/internal.h>
#include <base/utils.h>
#include <cstdlib>

using namespace std;

namespace feather {

	void
Ns_ConstrXlessthanY::ArcCons (void)
{
	VarX->removeRange(VarY->max(), kPlusInf, this);

	VarY->removeRange(kMinusInf, VarX->min(), this);
}


	void
Ns_ConstrXlessthanY::LocalArcCons (Ns_QueueItem& Qitem)
{
	if (VarX == Qitem.getVarFired())  {

		VarY->removeRange(kMinusInf, VarX->min(), this);

	}  else  {
		FEATHER_ASSERT( VarY == Qitem.getVarFired());
		VarX->removeRange(VarY->max(), kPlusInf, this);
	}
}



	void
Ns_ConstrXlesseqthanY::ArcCons (void)
{
	VarX->removeRange(VarY->max()+1, kPlusInf, this);

	VarY->removeRange(kMinusInf, VarX->min()-1, this);
}


	void
Ns_ConstrXlesseqthanY::LocalArcCons (Ns_QueueItem& Qitem)
{
	if (VarX == Qitem.getVarFired())  {

		VarY->removeRange(kMinusInf, VarX->min()-1, this);

	}  else  {
		FEATHER_ASSERT( VarY == Qitem.getVarFired());
		VarX->removeRange(VarY->max()+1, kPlusInf, this);
	}
}





namespace  {

	inline void
	makeBoolean (NsIntVar *VarX, const Ns_Constraint* constraint)
	{
		VarX->removeRange(kMinusInf, -1, constraint);
		VarX->removeRange(2, kPlusInf, constraint);
	}

} // namespace




	void
Ns_ConstrMetaXeqYlessthanC::ArcCons (void)
{
	makeBoolean(VarX, this);

	if ( VarY->max() < C )
		VarX->removeSingle( 0 , this);
	if ( VarY->min() >= C )
		VarX->removeSingle( 1 , this);

	if (VarX->isBound())  {

		if (VarX->value() == 0)  {

			VarY->removeRange(kMinusInf, C-1, this);

		}  else  {
			FEATHER_ASSERT( VarX->value() == 1);
			VarY->removeRange(C, kPlusInf, this);
		}
	}
}



	void
Ns_ConstrMetaXeqYlessthanC::LocalArcCons (Ns_QueueItem& Qitem)
{
	if (VarX == Qitem.getVarFired())  {

		if ( VarX->isBound() )  {

			if (VarX->value() == 0)  {

				VarY->removeRange(kMinusInf, C-1, this);

			}  else  {
				FEATHER_ASSERT( VarX->value() == 1 );
				VarY->removeRange(C, kPlusInf, this);
			}
		}

	}  else  {
		FEATHER_ASSERT( VarY == Qitem.getVarFired() );
		if ( VarY->max()  <  C )
			VarX->removeSingle( 0 , this);
		if ( VarY->min()  >= C)
			VarX->removeSingle( 1 , this);
	}
}



	void
Ns_ConstrMetaXeqYlessthanZ::ArcCons (void)
{
	makeBoolean(VarX, this);

	if ( VarY->max() < VarZ->min() )
		VarX->removeSingle( 0 , this);
	if ( VarY->min() >= VarZ->max() )
		VarX->removeSingle( 1 , this);

	if (VarX->isBound())  {

		if (VarX->value() == 0)  {

			VarZ->removeRange(VarY->max()+1, kPlusInf, this);

			VarY->removeRange(kMinusInf, VarZ->min()-1, this);

		}  else  {

			VarY->removeRange(VarZ->max(), kPlusInf, this);

			VarZ->removeRange(kMinusInf, VarY->min(), this);
		}
	}
}




	void
Ns_ConstrMetaXeqYlessthanZ::LocalArcCons (Ns_QueueItem& Qitem)
{
	if (VarX == Qitem.getVarFired())  {

		if ( VarX->isBound() )  {

			if (VarX->value() == 0)  {

				VarZ->removeRange(VarY->max()+1, kPlusInf, this);

				VarY->removeRange(kMinusInf, VarZ->min()-1, this);

			}  else  {
				FEATHER_ASSERT( VarX->value() == 1 );

				VarY->removeRange(VarZ->max(), kPlusInf, this);

				VarZ->removeRange(kMinusInf, VarY->min(), this);
			}
		}

	}  else  {

		if (VarY == Qitem.getVarFired())   {

			if ( VarX->isBound() )  {

				if (VarX->value() == 0)  {

					VarZ->removeRange(VarY->max()+1, kPlusInf, this);

				}  else  {
					FEATHER_ASSERT( VarX->value() == 1 );

					VarZ->removeRange(kMinusInf, VarY->min(), this);
				}
			}

		}  else  {
			FEATHER_ASSERT( VarZ == Qitem.getVarFired() );

			if ( VarX->isBound() )  {

				if (VarX->value() == 0)  {

					VarY->removeRange(kMinusInf, VarZ->min()-1, this);

				}  else  {
					FEATHER_ASSERT( VarX->value() == 1 );

					VarY->removeRange(VarZ->max(), kPlusInf, this);
				}
			}
		}

		if ( VarY->max() < VarZ->min())
			VarX->removeSingle( 0 , this);
		if ( VarY->min() >= VarZ->max())
			VarX->removeSingle( 1 , this);
	}
}








	void
Ns_ConstrMetaXeqYlesseqthanC::ArcCons (void)
{
	makeBoolean(VarX, this);

	if ( VarY->max() <= C )
		VarX->removeSingle( 0 , this);
	if ( VarY->min() > C )
		VarX->removeSingle( 1 , this);

	if (VarX->isBound())  {

		if (VarX->value() == 0)  {

			VarY->removeRange(kMinusInf, C, this);

		}  else  {
			FEATHER_ASSERT( VarX->value() == 1 );

			VarY->removeRange(C+1, kPlusInf, this);
		}
	}
}


	void
Ns_ConstrMetaXeqYlesseqthanC::LocalArcCons (Ns_QueueItem& Qitem)
{
	if (VarX == Qitem.getVarFired())  {

		if ( VarX->isBound() )  {

			if (VarX->value() == 0)  {

				VarY->removeRange(kMinusInf, C, this);

			}  else  {
				FEATHER_ASSERT( VarX->value() == 1);

				VarY->removeRange(C+1, kPlusInf, this);
			}
		}

	}  else  {
		FEATHER_ASSERT( VarY == Qitem.getVarFired() );

		if ( VarY->max() <= C)
			VarX->removeSingle( 0 , this);
		if ( VarY->min() > C)
			VarX->removeSingle( 1 , this);
	}
}


	void
Ns_ConstrMetaXeqYlesseqthanZ::ArcCons (void)
{
	makeBoolean(VarX, this);

	if ( VarY->max() <= VarZ->min())
		VarX->removeSingle( 0 , this);
	if ( VarY->min() > VarZ->max())
		VarX->removeSingle( 1 , this);

	if (VarX->isBound())  {

		if (VarX->value() == 0)  {

			VarZ->removeRange(VarY->max(), kPlusInf, this);

			VarY->removeRange(kMinusInf, VarZ->min(), this);

		}  else  {
			FEATHER_ASSERT( VarX->value() == 1);
			VarY->removeRange(VarZ->max()+1, kPlusInf, this);

			VarZ->removeRange(kMinusInf, VarY->min()-1, this);
		}
	}
}


	void
Ns_ConstrMetaXeqYlesseqthanZ::LocalArcCons (Ns_QueueItem& Qitem)
{
	if (VarX == Qitem.getVarFired())  {

		if ( VarX->isBound() )  {

			if (VarX->value() == 0)  {

				VarZ->removeRange(VarY->max(), kPlusInf, this);

				VarY->removeRange(kMinusInf, VarZ->min(), this);

			}  else  {
				FEATHER_ASSERT( VarX->value() == 1);

				VarY->removeRange(VarZ->max()+1, kPlusInf, this);

				VarZ->removeRange(kMinusInf, VarY->min()-1, this);
			}
		}

	}  else  {

		if (VarY == Qitem.getVarFired())  {

			if ( VarX->isBound() )  {

				if (VarX->value() == 0)  {

					VarZ->removeRange(VarY->max(), kPlusInf, this);

				}  else  {
					FEATHER_ASSERT( VarX->value() == 1 );
					VarZ->removeRange(kMinusInf, VarY->min()-1, this);
				}
			}

		}  else  {
			FEATHER_ASSERT( VarZ == Qitem.getVarFired() );
			if ( VarX->isBound() )  {

				if (VarX->value() == 0)  {

					VarY->removeRange(kMinusInf, VarZ->min(), this);

				}  else  {
					FEATHER_ASSERT( VarX->value() == 1 );
					VarY->removeRange(VarZ->max()+1, kPlusInf, this);
				}
			}
		}

		if ( VarY->max() <= VarZ->min() )
			VarX->removeSingle( 0 , this);
		if ( VarY->min() > VarZ->max() )
			VarX->removeSingle( 1 , this);
	}
}









	void
Ns_ConstrMetaXeqYgreaterthanC::ArcCons (void)
{
	makeBoolean(VarX, this);

	if ( VarY->min() > C)
		VarX->removeSingle( 0 , this);
	if ( VarY->max() <= C)
		VarX->removeSingle( 1 , this);

	if (VarX->isBound())  {

		if (VarX->value() == 0)  {

			VarY->removeRange(C+1, kPlusInf, this);

		}  else  {
			FEATHER_ASSERT( VarX->value() == 1);
			VarY->removeRange(kMinusInf, C, this);
		}
	}
}


	void
Ns_ConstrMetaXeqYgreaterthanC::LocalArcCons (Ns_QueueItem& Qitem)
{
	if (VarX == Qitem.getVarFired())  {

		if ( VarX->isBound() )  {

			if (VarX->value() == 0)  {

				VarY->removeRange(C+1, kPlusInf, this);

			}  else  {
				FEATHER_ASSERT( VarX->value() == 1 );

				VarY->removeRange(kMinusInf, C, this);
			}
		}

	}  else  {
		FEATHER_ASSERT( VarY == Qitem.getVarFired() );
		if ( VarY->min() > C)
			VarX->removeSingle( 0 , this);
		if ( VarY->max() <= C)
			VarX->removeSingle( 1 , this);
	}
}











	void
Ns_ConstrMetaXeqYgreatereqthanC::ArcCons (void)
{
	makeBoolean(VarX, this);

	if ( VarY->min() >= C)
		VarX->removeSingle( 0 , this);
	if ( VarY->max() < C)
		VarX->removeSingle( 1 , this);

	if (VarX->isBound())  {

		if (VarX->value() == 0)  {

			VarY->removeRange(C, kPlusInf, this);

		}  else  {
			FEATHER_ASSERT( VarX->value() == 1 );
			VarY->removeRange(kMinusInf, C-1, this);
		}
	}
}


	void
Ns_ConstrMetaXeqYgreatereqthanC::LocalArcCons (Ns_QueueItem& Qitem)
{
	if (VarX == Qitem.getVarFired())  {

		if ( VarX->isBound() )  {

			if (VarX->value() == 0)  {

				VarY->removeRange(C, kPlusInf, this);

			}  else  {
				FEATHER_ASSERT( VarX->value() == 1 );
				VarY->removeRange(kMinusInf, C-1, this);
			}
		}

	}  else  {
		FEATHER_ASSERT( VarY == Qitem.getVarFired() );
		if ( VarY->min() >= C)
			VarX->removeSingle( 0 , this);
		if ( VarY->max() < C)
			VarX->removeSingle( 1 , this);
	}
}




	void
Ns_ConstrMetaXeqYeqC::ArcCons (void)
{
	makeBoolean(VarX, this);

	if ( VarY->min() == C  &&  VarY->max() == C )
		VarX->removeSingle( 0 , this);
	if ( ! VarY->contains(C) )
		VarX->removeSingle( 1 , this);

	if ( VarX->isBound() )    {

		if ( VarX->value()  ==  0 )    {

			VarY->removeSingle( C , this);

		}  else  {
			FEATHER_ASSERT( VarX->value() == 1);

			VarY->removeRange(kMinusInf, C-1, this);

			VarY->removeRange(C+1, kPlusInf, this);
		}
	}
}


	void
Ns_ConstrMetaXeqYeqC::LocalArcCons (Ns_QueueItem& Qitem)
{
	if ( VarX  ==  Qitem.getVarFired() )    {

		if ( VarX->isBound() )    {

			if ( VarX->value()  ==  0 )    {

				VarY->removeSingle( C , this);

			}  else  {
				FEATHER_ASSERT( VarX->value() == 1 );
				VarY->removeRange(kMinusInf, C-1, this);

				VarY->removeRange(C+1, kPlusInf, this);
			}
		}

	}  else  {
		FEATHER_ASSERT( VarY == Qitem.getVarFired() );
		if ( VarY->min() == C  &&  VarY->max() == C )
			VarX->removeSingle( 0 , this);
		if ( ! VarY->contains(C) )
			VarX->removeSingle( 1 , this);
	}
}





	void
Ns_ConstrMetaXeqYneqC::ArcCons (void)
{
	makeBoolean(VarX, this);

	if ( ! VarY->contains(C))
		VarX->removeSingle( 0 , this);
	if ( VarY->min() == C  &&  VarY->max() == C )
		VarX->removeSingle( 1 , this);

	if (VarX->isBound())  {

		if (VarX->value() == 0)  {

			VarY->removeRange(kMinusInf, C-1, this);

			VarY->removeRange(C+1, kPlusInf, this);

		}  else  {
			FEATHER_ASSERT( VarX->value() == 1 );

			VarY->removeSingle( C , this);
		}
	}
}


	void
Ns_ConstrMetaXeqYneqC::LocalArcCons (Ns_QueueItem& Qitem)
{
	if (VarX == Qitem.getVarFired())  {

		if ( VarX->isBound() )  {

			if (VarX->value() == 0)  {

				VarY->removeRange(kMinusInf, C-1, this);

				VarY->removeRange(C+1, kPlusInf, this);

			}  else  {
				FEATHER_ASSERT( VarX->value() == 1 );

				VarY->removeSingle( C , this);

			}
		}

	}  else  {
		FEATHER_ASSERT( VarY == Qitem.getVarFired() );

		if ( ! VarY->contains(C) )
			VarX->removeSingle( 0 , this);
		if ( VarY->min() == C  &&  VarY->max() == C )
			VarX->removeSingle( 1 , this);
	}
}









	void
Ns_ConstrXeqYplusC::ArcCons (void)
{
	// debugNs("Calling ConstrXeqYplusC::ArcCons\n");

	bool  modification;

	do  {
		modification  =  false;


		//if ( VarX->min()  <  VarY->min()+C )
		VarX->removeRange(kMinusInf, VarY->min()+C-1, this, modification);

		//if ( VarX->max()  >  VarY->max()+C )

		VarX->removeRange(VarY->max()+C+1, kPlusInf, this, modification);

		//if ( VarY->min()  <  VarX->min()-C )

		VarY->removeRange(kMinusInf, VarX->min()-C-1, this, modification);

		//if ( VarY->max()  >  VarX->max()-C )
		VarY->removeRange(VarX->max()-C+1, kPlusInf, this, modification);


	}  while ( modification );


	//NsIntVar::const_iterator  v;
	//
	//for (v = VarX->begin();  v != VarX->end();  ++v)  {
	//
	//	if ( ! VarY->contains( *v - C ) )
	//		VarX->removeSingle( *v , this);
	//}
	//for (v = VarY->begin();  v != VarY->end();  ++v)  {
	//
	//	if ( ! VarX->contains( *v + C ) )
	//		VarY->removeSingle( *v , this);
	//}
}


	void
Ns_ConstrXeqYplusC::LocalArcCons (Ns_QueueItem& Qitem)
{
	// debugNs("Calling ConstrXeqYplusC::LocalArcCons\n");
	ArcCons();


	//if (VarX == Qitem.getVarFired())  {

	//	VarY->removeRange(kMinusInf, VarX->min()-C-1, this);
	//	VarY->removeRange(VarX->max()-C+1, kPlusInf, this);

	//}  else  {
	//	FEATHER_ASSERT( VarY == Qitem.getVarFired() ,
	//	"Ns_ConstrXeqYplusC::LocalArcCons: Wrong getVarFired");

	//	VarX->removeRange(kMinusInf, VarY->min()+C-1, this);
	//	VarX->removeRange(VarY->max()+C+1, kPlusInf, this);
	//}


	//Int SupportVal;
	//
	//if (VarX == Qitem.getVarFired())  {
	//	SupportVal = Qitem.getW() - C;
	//	if (VarY->contains( SupportVal ))
	//		VarY->removeSingle( SupportVal , this);
	//
	//}  else  {
	//	FEATHER_ASSERT( VarY == Qitem.getVarFired() ,
	//			"Ns_ConstrXeqYplusC::LocalArcCons: Wrong getVarFired");
	//
	//	SupportVal = Qitem.getW() + C;
	//	if (VarX->contains( SupportVal ))
	//		VarX->removeSingle( SupportVal , this);
	//}
}


	void
Ns_ConstrXeqCminusY::ArcCons (void)
{
	bool  modification;

	do  {
		modification  =  false;


		VarX->removeRange(kMinusInf, C-VarY->max()-1, this, modification);
		VarX->removeRange(C-VarY->min()+1, kPlusInf, this, modification);

		VarY->removeRange(kMinusInf, C-VarX->max()-1, this, modification);
		VarY->removeRange(C-VarX->min()+1, kPlusInf, this, modification);


	}  while ( modification );


	//NsIntVar::const_iterator  v;
	//for (v = VarX->begin();  v != VarX->end();  ++v)  {
	//	if ( ! VarY->contains( C - *v ) )
	//		VarX->removeSingle( *v , this);
	//}
	//for (v = VarY->begin();  v != VarY->end();  ++v)  {
	//	if ( ! VarX->contains( C - *v ) )
	//		VarY->removeSingle( *v , this);
	//}
}


	void
Ns_ConstrXeqCminusY::LocalArcCons (Ns_QueueItem& Qitem)
{
	ArcCons();


	//if (VarX == Qitem.getVarFired())  {

	//	VarY->removeRange(kMinusInf, C-VarX->max()-1, this);
	//	VarY->removeRange(C-VarX->min()+1, kPlusInf, this);

	//}  else  {
	//	FEATHER_ASSERT( VarY == Qitem.getVarFired() ,
	//	"Ns_ConstrXeqCminusY::LocalArcCons: Wrong getVarFired");

	//	VarX->removeRange(kMinusInf, C-VarY->max()-1, this);
	//	VarX->removeRange(C-VarY->min()+1, kPlusInf, this);
	//}


	//Int SupportVal;
	//
	//if (VarX == Qitem.getVarFired())  {
	//	SupportVal = C - Qitem.getW();
	//	if (VarY->contains( SupportVal ))
	//		VarY->removeSingle( SupportVal , this);
	//
	//}  else  {
	//	FEATHER_ASSERT( VarY == Qitem.getVarFired() ,  "Ns_ConstrXeqCminusY::LocalArcCons: Wrong getVarFired");
	//
	//	SupportVal = C - Qitem.getW();
	//	if (VarX->contains( SupportVal ))
	//		VarX->removeSingle( SupportVal , this);
	//}
}




	void
Ns_ConstrXeqYtimesC::ArcCons (void)
{
	bool  modification;

	do  {
		modification  =  false;


		if ( C  >=  0 )    {

			VarX->removeRange(kMinusInf, VarY->min()*C-1, this, modification);
			VarX->removeRange(VarY->max()*C+1, kPlusInf, this, modification);

			VarY->removeRange(kMinusInf, VarX->min()/C-1, this, modification);
			VarY->removeRange(VarX->max()/C+1, kPlusInf, this, modification);

		}  else  {

			VarX->removeRange(kMinusInf, VarY->max()*C-1, this, modification);
			VarX->removeRange(VarY->min()*C+1, kPlusInf, this, modification);

			VarY->removeRange(kMinusInf, VarX->max()/C-1, this, modification);
			VarY->removeRange(VarX->min()/C+1, kPlusInf, this, modification);
		}


	}  while ( modification );


	//if ( ( VarX->isBound()  &&  ! VarY->isBound() )  ||  ( ! VarX->isBound()  &&  VarY->isBound() ) )   {
	//	cout << "EEEEEEEEE?:\t" << *VarX << " = " << C << "*" << *VarY << ".\tVarX=" << VarX << " VarY=" << VarY << "\n";
	//	abort();
	//}

	//NsIntVar::const_iterator  v;
	//for (v = VarX->begin();  v != VarX->end();  ++v)  {
	//	if ( *v % C != 0  ||  ! VarY->contains( *v / C ) )
	//		VarX->removeSingle( *v , this);
	//}
	//for (v = VarY->begin();  v != VarY->end();  ++v)  {
	//	if ( ! VarX->contains( *v * C ) )
	//		VarY->removeSingle( *v , this);
	//}
}


	void
Ns_ConstrXeqYtimesC::LocalArcCons (Ns_QueueItem& Qitem)
{
	ArcCons();


	//if ( VarX  ==  Qitem.getVarFired() )  {

	//	if ( C  >=  0 )    {

	//		VarY->removeRange(kMinusInf, VarX->min()/C-1, this);
	//		VarY->removeRange(VarX->max()/C+1, kPlusInf, this);

	//	}  else  {

	//		VarY->removeRange(kMinusInf, VarX->max()/C-1, this);
	//		VarY->removeRange(VarX->min()/C+1, kPlusInf, this);
	//	}

	//}  else  {
	//	FEATHER_ASSERT( VarY == Qitem.getVarFired() ,  "Ns_ConstrXeqYtimesC::LocalArcCons: Wrong getVarFired");

	//	if ( C  >=  0 )    {

	//		VarX->removeRange(kMinusInf, VarY->min()*C-1, this);
	//		VarX->removeRange(VarY->max()*C+1, kPlusInf, this);

	//	}  else  {

	//		VarX->removeRange(kMinusInf, VarY->max()*C-1, this);
	//		VarX->removeRange(VarY->min()*C+1, kPlusInf, this);
	//	}
	//}

	//if ( Qitem.getVarFired()->isBound()  &&  ( ! VarX->isBound()  ||  ! VarY->isBound() ) )   {
	//if ( VarX->min() == 0   &&  VarX->max() == 19 )    {
	//	cout << "Bug?:\t" << *VarX << " = " << C << "*" << *VarY << ".\tVarX=" << VarX << " VarY=" << VarY << " fired=" << *Qitem.getVarFired() << "\n";
		//abort();
	//}

	//Int SupportVal;
	//
	//if (VarX == Qitem.getVarFired())  {
	//	SupportVal = Qitem.getW() / C;
	//	if (Qitem.getW() % C == 0  &&  VarY->contains( SupportVal ))
	//		VarY->removeSingle( SupportVal , this);
	//
	//}  else  {
	//	FEATHER_ASSERT( VarY == Qitem.getVarFired() ,  "Ns_ConstrXeqYtimesC::LocalArcCons: Wrong getVarFired");

	//	SupportVal = Qitem.getW() * C;
	//	if (VarX->contains( SupportVal ))
	//		VarX->removeSingle( SupportVal , this);
	//}
}









	void
Ns_ConstrXeqY::ArcCons (void)
{
	bool  modification;

	do  {
		modification  =  false;


		//if ( VarX->min()  <  VarY->min() )
		VarX->removeRange(kMinusInf, VarY->min()-1, this, modification);
		//if ( VarX->max()  >  VarY->max() )
		VarX->removeRange(VarY->max()+1, kPlusInf, this, modification);

		//if ( VarY->min()  <  VarX->min() )
		VarY->removeRange(kMinusInf, VarX->min()-1, this, modification);
		//if ( VarY->max()  >  VarX->max() )
		VarY->removeRange(VarX->max()+1, kPlusInf, this, modification);


	}  while ( modification );



	//NsIntVar::const_iterator  v;
	//for (v = VarX->begin();  v != VarX->end();  ++v)  {
	//	if ( ! VarY->contains( *v ) )
	//		VarX->removeSingle( *v , this);
	//}
	//for (v = VarY->begin();  v != VarY->end();  ++v)  {
	//	if ( ! VarX->contains( *v ) )
	//		VarY->removeSingle( *v , this);
	//}
}


	void
Ns_ConstrXeqY::LocalArcCons (Ns_QueueItem& Qitem)
{
	ArcCons();


	//if (VarX == Qitem.getVarFired())  {

	//	//if ( VarY->min()  <  VarX->min() )
	//		VarY->removeRange(kMinusInf, VarX->min()-1, this);
	//	//if ( VarY->max()  >  VarX->max() )
	//		VarY->removeRange(VarX->max()+1, kPlusInf, this);

	//}  else  {
	//	FEATHER_ASSERT( VarY == Qitem.getVarFired() ,
	//	"Ns_ConstrXeqY::LocalArcCons: Wrong getVarFired");

	//	//if ( VarX->min()  <  VarY->min() )
	//		VarX->removeRange(kMinusInf, VarY->min()-1, this);
	//	//if ( VarX->max()  >  VarY->max() )
	//		VarX->removeRange(VarY->max()+1, kPlusInf, this);
	//}


	//if (VarX == Qitem.getVarFired())  {
	//	if (VarY->contains( Qitem.getW() ))
	//		VarY->removeSingle( Qitem.getW() , this);

	//}  else  {
	//	FEATHER_ASSERT( VarY == Qitem.getVarFired() ,  "Ns_ConstrXeqY::LocalArcCons: Wrong getVarFired");

	//	if (VarX->contains( Qitem.getW() ))
	//		VarX->removeSingle( Qitem.getW() , this);
	//}
}






	void
Ns_ConstrXeqYplusCZspecial::ArcCons (void)
{
	NsIntVar::const_iterator  v, vz;
	for (v = VarX->begin();  v != VarX->end();  ++v)  {
		if ( ! VarY->contains( *v % C )   ||   ! VarZ->contains( (*v - *v % C) / C ) )
			VarX->removeSingle( *v , this);
	}

	for (v = VarY->begin();  v != VarY->end();  ++v)  {
		for (vz = VarZ->begin();  vz != VarZ->end();  ++vz)  {
			if ( VarX->contains( *v + C*(*vz) ) )
				break;
		}
		if ( vz == VarZ->end() )
			VarY->removeSingle( *v , this);
	}

	for (vz = VarZ->begin();  vz != VarZ->end();  ++vz)  {
		for (v = VarY->begin();  v != VarY->end();  ++v)  {
			if ( VarX->contains( *v + C*(*vz) ) )
				break;
		}
		if ( v == VarY->end() )
			VarZ->removeSingle( *vz , this);
	}
}


	void
Ns_ConstrXeqYplusCZspecial::LocalArcCons (Ns_QueueItem& Qitem)
{
	NsIntVar::const_iterator  v, vz;
	Int SupportVal;

	if (VarX == Qitem.getVarFired())  {

		SupportVal = Qitem.getW() / C;
		if (VarZ->contains( SupportVal ))  {
			if ( VarX->next(SupportVal * C -1)  >=  (SupportVal + 1) * C )
				VarZ->removeSingle( SupportVal , this);
		}

		SupportVal = Qitem.getW() % C;
		if (VarY->contains( SupportVal ))  {
			for (vz = VarZ->begin();  vz != VarZ->end();  ++vz)  {
				if ( VarX->contains( SupportVal  +  C*(*vz) ) )
					break;
			}
			if ( vz == VarZ->end() )
				VarY->removeSingle( SupportVal , this);
		}


	} else if (VarY == Qitem.getVarFired())  {

		for (SupportVal = C*(VarX->min() / C)  +  Qitem.getW();
			SupportVal <= VarX->max();   SupportVal += C)
		{
			//if (VarX->contains( SupportVal ))
			VarX->removeSingle( SupportVal , this);
		}

		for (vz = VarZ->begin();  vz != VarZ->end();  ++vz)  {
			for (v = VarY->begin();  v != VarY->end();  ++v)  {
				if ( VarX->contains( *v + C*(*vz) ) )
					break;
			}
			if ( v == VarY->end() )
				VarZ->removeSingle( *vz , this);
		}


	}  else  {
		FEATHER_ASSERT( VarZ == Qitem.getVarFired() );

		VarX->removeRange(Qitem.getW() * C, (Qitem.getW() + 1)*C - 1, this);

		for (v = VarY->begin();  v != VarY->end();  ++v)  {
			for (vz = VarZ->begin();  vz != VarZ->end();  ++vz)  {
				if ( VarX->contains( *v + C*(*vz) ) )
					break;
			}
			if ( vz == VarZ->end() )
				VarY->removeSingle( *v , this);
		}
	}
}








	void
Ns_ConstrXeqYdivC::ArcCons (void)
{
	bool  modification;

	do  {
		modification  =  false;


		if ( C  >=  0 )    {

			VarX->removeRange(kMinusInf, xDIVy(VarY->min(),C)-1, this, modification);
			VarX->removeRange(xDIVy(VarY->max(),C)+1, kPlusInf, this, modification);

			VarY->removeRange(kMinusInf, VarX->min()*C-1, this, modification);
			VarY->removeRange((VarX->max()+1)*C, kPlusInf, this, modification);

		}  else  {

			VarX->removeRange(kMinusInf, xDIVy(VarY->max(),C)-1, this, modification);
			VarX->removeRange(xDIVy(VarY->min(),C)+1, kPlusInf, this, modification);

			VarY->removeRange(kMinusInf, (VarX->max()+1)*C, this, modification);
			VarY->removeRange(VarX->min()*C+1, kPlusInf, this, modification);
		}


	}  while ( modification );


	//NsIntVar::const_iterator  v;
	//for (v = VarX->begin();  v != VarX->end();  ++v)   {
	//	if ( VarY->next(*v * C -1)  >=  (*v + 1) * C )
	//		VarX->removeSingle( *v , this);
	//}
	//for (v = VarY->begin();  v != VarY->end();  ++v)   {
	//	if ( ! VarX->contains( *v / C ) )
	//		VarY->removeSingle( *v , this);
	//}
}


	void
Ns_ConstrXeqYdivC::LocalArcCons (Ns_QueueItem& Qitem)
{
	ArcCons();


	////cout << *VarX << " == " << *VarY << " / " << C << "\n";

	//if ( VarX  ==  Qitem.getVarFired() )    {

	//	//cout << "VarX fired.\n";
	//	if ( C  >=  0 )    {

	//		VarY->removeRange(kMinusInf, VarX->min()*C-1, this);
	//		VarY->removeRange((VarX->max()+1)*C, kPlusInf, this);

	//	}  else  {

	//		VarY->removeRange(kMinusInf, (VarX->max()+1)*C, this);
	//		VarY->removeRange(VarX->min()*C+1, kPlusInf, this);
	//	}

	//}  else  {
	//	FEATHER_ASSERT( VarY == Qitem.getVarFired() ,
	//	"Ns_ConstrXeqYdivC::LocalArcCons: Wrong getVarFired");

	//	if ( C  >=  0 )    {

	//		VarX->removeRange(kMinusInf, xDIVy(VarY->min(),C)-1, this);
	//		VarX->removeRange(xDIVy(VarY->max(),C)+1, kPlusInf, this);

	//	}  else  {

	//		VarX->removeRange(kMinusInf, xDIVy(VarY->max(),C)-1, this);
	//		VarX->removeRange(xDIVy(VarY->min(),C)+1, kPlusInf, this);
	//	}
	//}

	//cout << *VarX << " == " << *VarY << " / " << C << "\n\n";


	//Int SupportVal;

	//if (VarX == Qitem.getVarFired())  {
	//	VarY->removeRange(Qitem.getW() * C, (Qitem.getW() + 1)*C - 1, this);

	//}  else  {
	//	FEATHER_ASSERT( VarY == Qitem.getVarFired() ,  "Ns_ConstrXeqYdivC::LocalArcCons: Wrong getVarFired");

	//	SupportVal = Qitem.getW() / C;
	//	if (VarX->contains( SupportVal ))  {
	//		if ( VarY->next(SupportVal * C -1)  >=  (SupportVal + 1) * C )
	//			VarX->removeSingle( SupportVal , this);
	//	}
	//}
}





	void
Ns_ConstrXeqCdivY::ArcCons (void)
{
	bool  modification;

	do  {
		modification  =  false;


		if ( VarY->min() < 0  &&  0 < VarY->max() )    {

			if ( C  >=  0 )    {

				VarX->removeRange(kMinusInf, xDIVy(C,VarY->previous(0))-1, this, modification);
				VarX->removeRange(xDIVy(C,VarY->next(0))+1, kPlusInf, this, modification);

			}  else  {

				VarX->removeRange(kMinusInf, xDIVy(C,VarY->next(0))-1, this, modification);
				VarX->removeRange(xDIVy(C,VarY->previous(0))+1, kPlusInf, this, modification);
			}

		}  else  {

			if ( C  >=  0 )    {

				VarX->removeRange(kMinusInf, xDIVy(C,VarY->max())-1, this, modification);
				VarX->removeRange(xDIVy(C,VarY->min())+1, kPlusInf, this, modification);

			}  else  {

				VarX->removeRange(kMinusInf, xDIVy(C,VarY->min())-1, this, modification);
				VarX->removeRange(xDIVy(C,VarY->max())+1, kPlusInf, this, modification);
			}
		}

		//	if (Qitem.getW() == 0)   {
		//		VarY->removeRange(C+1, kPlusInf, this);
		//	} else if (Qitem.getW() == -1)   {
		//		VarY->removeRange(kMinusInf, -C, this);
		//	}  else  {
		//		VarY->removeRange(C / (Qitem.getW()+1) +1,  C / Qitem.getW(), this);

		//cout << *VarX << " == " << C << " / " << *VarY << "\n";

		if ( ! ( VarX->min() < 0  &&  0 < VarX->max() ) )   {

			if ( C  >=  0 )    {

				//cout << "Y = [" << xDIVy(C,(VarX->max()+1))+1 << ".." << xDIVy(C,VarX->min()) << "]\n";

				if ( VarX->max()  ==  -1 )
					VarY->removeRange(kMinusInf, -C-1, this, modification);
					//VarY->removeRange(C+1, kPlusInf, this);
				else
					VarY->removeRange(kMinusInf, xDIVy(C,(VarX->max()+1)), this, modification);

				if ( VarX->min()  !=  0 )
					VarY->removeRange(xDIVy(C,VarX->min())+1, kPlusInf, this, modification);

			}  else  {

				if ( VarX->min()  ==  -1 )
					VarY->removeRange(C+1, kPlusInf, this, modification);
				else
					VarY->removeRange(kMinusInf, xDIVy(C,(VarX->min()+1)), this, modification);

				if ( VarX->max()  !=  0 )
					VarY->removeRange(xDIVy(C,VarX->max())+1, kPlusInf, this, modification);
			}
		}


	}  while ( modification );



	//cout << *VarX << " == " << C << " / " << *VarY << "\n\n";


	//NsIntVar::const_iterator  v;
	//for (v = VarX->begin();  v != VarX->end();  ++v)   {
	//	// there should be ( C/(v+1) , C/v ]  in VarY's domain
	//	if ((*v==0  &&  VarY->next(C) == kPlusInf)	// C = C/1
	//			||  (*v==-1  &&  kMinusInf == VarY->previous(-C+1))	// -C = C/-1
	//			||  (*v!=0   &&  *v!=-1  &&  VarY->next(C / (*v+1))  >  C / *v ) )
	//		VarX->removeSingle( *v , this);
	//}
	//for (v = VarY->begin();  v != VarY->end();  ++v)   {
	//	if ( ! VarX->contains( C / *v ) )
	//		VarY->removeSingle( *v , this);
	//}
}


	void
Ns_ConstrXeqCdivY::LocalArcCons (Ns_QueueItem& Qitem)
{
	ArcCons();


	////cout << *VarX << " == " << *VarY << " / " << C << "\n";

	//if ( VarX  ==  Qitem.getVarFired() )  {

	//	//cout << "VarX fired.\n";

	//	if ( ! ( VarX->min() < 0  &&  0 < VarX->max() ) )   {

	//		if ( C  >=  0 )    {

	//			//cout << "Y = [" << xDIVy(C,(VarX->max()+1))+1 << ".." << xDIVy(C,VarX->min()) << "]\n";

	//			if ( VarX->max()  ==  -1 )
	//				VarY->removeRange(kMinusInf, -C-1, this);
	//				//VarY->removeRange(C+1, kPlusInf, this);
	//			else
	//				VarY->removeRange(kMinusInf, xDIVy(C,(VarX->max()+1)), this);

	//			if ( VarX->min()  !=  0 )
	//				VarY->removeRange(xDIVy(C,VarX->min())+1, kPlusInf, this);

	//		}  else  {

	//			if ( VarX->min()  ==  -1 )
	//				VarY->removeRange(C+1, kPlusInf, this);
	//			else
	//				VarY->removeRange(kMinusInf, xDIVy(C,(VarX->min()+1)), this);

	//			if ( VarX->max()  !=  0 )
	//				VarY->removeRange(xDIVy(C,VarX->max())+1, kPlusInf, this);
	//		}
	//	}

	//}  else  {
	//	FEATHER_ASSERT( VarY == Qitem.getVarFired() ,
	//	"Ns_ConstrXeqCdivY::LocalArcCons: Wrong getVarFired");

	//	if ( VarY->min() < 0  &&  0 < VarY->max() )    {

	//		if ( C  >=  0 )    {

	//			VarX->removeRange(kMinusInf, xDIVy(C,VarY->previous(0))-1, this);
	//			VarX->removeRange(xDIVy(C,VarY->next(0))+1, kPlusInf, this);

	//		}  else  {

	//			VarX->removeRange(kMinusInf, xDIVy(C,VarY->next(0))-1, this);
	//			VarX->removeRange(xDIVy(C,VarY->previous(0))+1, kPlusInf, this);
	//		}

	//	}  else  {

	//		if ( C  >=  0 )    {

	//			VarX->removeRange(kMinusInf, xDIVy(C,VarY->max())-1, this);
	//			VarX->removeRange(xDIVy(C,VarY->min())+1, kPlusInf, this);

	//		}  else  {

	//			VarX->removeRange(kMinusInf, xDIVy(C,VarY->min())-1, this);
	//			VarX->removeRange(xDIVy(C,VarY->max())+1, kPlusInf, this);
	//		}
	//	}
	//}

	//cout << *VarX << " == " << *VarY << " / " << C << "\n\n";

	//Int SupportVal;

	//if (VarX == Qitem.getVarFired())   {
	//	if (Qitem.getW() == 0)   {
	//		VarY->removeRange(C+1, kPlusInf, this);
	//	} else if (Qitem.getW() == -1)   {
	//		VarY->removeRange(kMinusInf, -C, this);
	//	}  else  {
	//		VarY->removeRange(C / (Qitem.getW()+1) +1,  C / Qitem.getW(), this);
	//	}

	//}  else  {
	//	FEATHER_ASSERT( VarY == Qitem.getVarFired() ,  "Ns_ConstrXeqCdivY::LocalArcCons: Wrong getVarFired");
	//	if (Qitem.getW() == 0)
	//		return;

	//	SupportVal = C / Qitem.getW();
	//	if (VarX->contains( SupportVal ))   {
	//		if ((SupportVal==-1  &&  kMinusInf == VarY->previous(-C+1))	// -C = C/-1
	//				|| (SupportVal!=-1  &&  VarY->next(C / (SupportVal+1))  >  C / SupportVal ))
	//			VarX->removeSingle( SupportVal , this);
	//	}
	//}
}






	void
YmodC_min_max (const NsIntVar *VarY, const Int C, Int& min, Int& max)
{
	min = kPlusInf;
	max = kMinusInf;
	Int SupValY, v;
	FEATHER_ASSERT( C > 0 );
	for (SupValY = (VarY->min()/C)*C;  SupValY <= VarY->max();  SupValY += C)   {
		v = VarY->next(SupValY-1);
		if (v < SupValY+C  &&  min > v % C)
			min = v % C;
		v = VarY->previous(SupValY+C);
		if (v >= SupValY   &&  max < v % C)
			max = v % C;
		if (min == 0  &&  max==C-1)
			break;	// no alterations are possible
	}
}


	void
Ns_ConstrXeqYmodC::ArcCons (void)
{
	Int min, max;
	YmodC_min_max(VarY, C, min, max);
	VarX->removeRange(kMinusInf, min-1, this);
	VarX->removeRange(max+1, kPlusInf, this);

	NsIntVar::const_iterator  v;
	Int SupValY;
	for (v = VarX->begin();  v != VarX->end();  ++v)   {
		for (SupValY = (VarY->min()/C)*C + *v;  SupValY <= VarY->max();  SupValY += C)   {
			if ( VarY->contains( SupValY ) )
				break;
		}
		if (SupValY > VarY->max())
			VarX->removeSingle( *v , this);
	}
	for (v = VarY->begin();  v != VarY->end();  ++v)   {
		if ( ! VarX->contains( *v % C ) )
			VarY->removeSingle( *v , this);
	}
}


	void
Ns_ConstrXeqYmodC::LocalArcCons (Ns_QueueItem& Qitem)
{
	Int SupValX, SupValY;

	if (VarX == Qitem.getVarFired())  {
		SupValX = Qitem.getW();
		for (SupValY = (VarY->min()/C)*C + SupValX;  SupValY <= VarY->max();  SupValY += C)   {
			//if ( VarY->contains( SupValY ) )
			VarY->removeSingle( SupValY , this);
		}

	} else   {
		FEATHER_ASSERT( VarY == Qitem.getVarFired());

		SupValX = Qitem.getW() % C;
		if (VarX->contains( SupValX ))  {
			for (SupValY = (VarY->min()/C)*C + SupValX;  SupValY <= VarY->max();  SupValY += C)   {
				if ( VarY->contains( SupValY ) )
					break;
			}
			if (SupValY > VarY->max())
				VarX->removeSingle( SupValX , this);
		}
	}
}





	void
Ns_ConstrXneqY::ArcCons (void)
{
	if ( VarY->isBound() )
		VarX->removeSingle( VarY->value() , this);
	if ( VarX->isBound() )
		VarY->removeSingle( VarX->value() , this);
}


	void
Ns_ConstrXneqY::LocalArcCons (Ns_QueueItem& Qitem)
{
	if ( VarX  ==  Qitem.getVarFired() )   {

		if ( VarX->isBound() )
			VarY->removeSingle( VarX->value() , this);

	}  else  {
		FEATHER_ASSERT( VarY  ==  Qitem.getVarFired() );

		if ( VarY->isBound() )
			VarX->removeSingle( VarY->value() , this);
	}
}





	void
Ns_ConstrXeqAbsY::ArcCons (void)
{
	bool  modification;

	do  {
		modification  =  false;


		VarX->removeRange(kMinusInf, max(labs( max(VarY->min(),static_cast<Int>(0)) ), labs( min(static_cast<Int>(0),VarY->max()))) - 1, this, modification);
		VarX->removeRange(max(labs(VarY->min()),labs(VarY->max())) + 1, kPlusInf, this, modification);

		VarY->removeRange(kMinusInf, -(VarX->max() + 1), this, modification);
		VarY->removeRange(+(VarX->max() + 1), kPlusInf, this, modification);
		//if ( VarX->min()  !=  0 )
		//	VarY->removeRange(-(VarX->min() - 1), +(VarX->min() - 1), this);


	}  while ( modification );
}


	void
Ns_ConstrXeqAbsY::LocalArcCons (Ns_QueueItem& Qitem)
{
	ArcCons();


	//if (VarX == Qitem.getVarFired())  {

	//	VarY->removeRange(kMinusInf, -(VarX->max() + 1), this);
	//	VarY->removeRange(+(VarX->max() + 1), kPlusInf, this);
	//	//if ( VarX->min()  !=  0 )
	//	//	VarY->removeRange(-(VarX->min() - 1), +(VarX->min() - 1), this);

	//}  else  {
	//	FEATHER_ASSERT( VarY == Qitem.getVarFired() ,  "Ns_ConstrXeqAbsY::LocalArcCons: Wrong getVarFired");

	//	VarX->removeRange(kMinusInf, max(labs( max(VarY->min(),static_cast<Int>(0)) ), labs( min(static_cast<Int>(0),VarY->max()))) - 1, this);
	//	VarX->removeRange(max(labs(VarY->min()),labs(VarY->max())) + 1, kPlusInf, this);
	//}
}






	bool
intersectionEmpty (const NsIntVar *VarY, const NsIntVar *VarZ)
{
	return  ( VarY->min() > VarZ->max()  ||  VarY->max() < VarZ->min() );


	//Int valy=kMinusInf, valz;   // `valy' plays the role of the max{valy,valz} too

	//for ( ; ; )    {

	//	valz = VarZ->next(valy);
	//	valy = VarY->next(valy);
	//	if (valy == kPlusInf  ||  valz == kPlusInf)
	//		break;

	//	if (valy == valz)  {
	//		return  false;
	//	} else if (valy > valz)  {
	//		if ( VarZ->contains( valy ) )
	//			return  false;
	//	}  else  {
	//		if ( VarY->contains( valz ) )
	//			return  false;
	//		valy = valz;
	//	}
	//}

	//return  true;
}



//namespace  {
//    	void
//    MetaXeqYeqneqZArcCons (NsIntVar *VarX, NsIntVar *VarY, NsIntVar *VarZ,
//    		const Ns_Constraint *constraint, const bool neg)
//    {
//    }
//
//
//    	void
//    MetaXeqYeqneqZLocalArcCons (Ns_QueueItem& Qitem, NsIntVar *VarX, NsIntVar *VarY, NsIntVar *VarZ,
//    		const Ns_Constraint *constraint, const bool neg)
//    {
//    }
//}  // namespace





	void
Ns_ConstrMetaXeqYeqZ::ArcCons (void)
{
	//MetaXeqYeqneqZArcCons(VarX, VarY, VarZ, this, false);

	makeBoolean(VarX, this);

	if ( VarY->max() == VarZ->min()  &&  VarY->min() == VarZ->max() )
		VarX->removeSingle( neg , this);
	if ( intersectionEmpty(VarY,VarZ) )
		VarX->removeSingle( ! neg , this);

	if ( VarX->isBound() )    {

		if ( VarX->value()  ==  neg )    {

			if ( VarZ->isBound() )
				VarY->removeSingle( VarZ->value() , this);
			if ( VarY->isBound() )
				VarZ->removeSingle( VarY->value() , this);

		}  else  {
			FEATHER_ASSERT( VarX->value() == ! neg );

			VarY->removeRange(kMinusInf, VarZ->min()-1, this);
			VarY->removeRange(VarZ->max()+1, kPlusInf, this);

			VarZ->removeRange(kMinusInf, VarY->min()-1, this);
			VarZ->removeRange(VarY->max()+1, kPlusInf, this);

			//NsIntVar::const_iterator  v;
			//for (v = VarY->begin();  v != VarY->end();  ++v)  {
			//	if ( ! VarZ->contains( *v ) )
			//		VarY->removeSingle( *v , this);
			//}
			//for (v = VarZ->begin();  v != VarZ->end();  ++v)  {
			//	if ( ! VarY->contains( *v ) )
			//		VarZ->removeSingle( *v , this);
			//}
		}
	}
}


	void
Ns_ConstrMetaXeqYeqZ::LocalArcCons (Ns_QueueItem& Qitem)
{
	//MetaXeqYeqneqZLocalArcCons(Qitem, VarX, VarY, VarZ, this, false);

	if ( VarX  ==  Qitem.getVarFired() )    {

		if ( VarX->isBound() )    {

			if ( VarX->value()  ==  neg )    {

				if ( VarZ->isBound() )
					VarY->removeSingle( VarZ->value() , this);
				if ( VarY->isBound() )
					VarZ->removeSingle( VarY->value() , this);

			}  else  {
				FEATHER_ASSERT( VarX->value() == ! neg );

				VarY->removeRange(kMinusInf, VarZ->min()-1, this);
				VarY->removeRange(VarZ->max()+1, kPlusInf, this);

				VarZ->removeRange(kMinusInf, VarY->min()-1, this);
				VarZ->removeRange(VarY->max()+1, kPlusInf, this);

				//NsIntVar::const_iterator  v;
				//for (v = VarY->begin();  v != VarY->end();  ++v)  {
				//	if ( ! VarZ->contains( *v ) )
				//		VarY->removeSingle( *v , this);
				//}
				//for (v = VarZ->begin();  v != VarZ->end();  ++v)  {
				//	if ( ! VarY->contains( *v ) )
				//		VarZ->removeSingle( *v , this);
				//}
			}
		}

	}  else  {
		if (VarY == Qitem.getVarFired())    {

			if ( VarX->isBound() )    {

				if (VarX->value() == neg)    {

					if ( VarY->isBound() )
						VarZ->removeSingle( VarY->value() , this);

				}  else  {
					FEATHER_ASSERT( VarX->value() == ! neg );

					VarY->removeRange(kMinusInf, VarZ->min()-1, this);
					VarY->removeRange(VarZ->max()+1, kPlusInf, this);

					VarZ->removeRange(kMinusInf, VarY->min()-1, this);
					VarZ->removeRange(VarY->max()+1, kPlusInf, this);

					//if (VarZ->contains( Qitem.getW() ))
					//	VarZ->removeSingle( Qitem.getW() , this);
				}
			}

		}  else  {
			FEATHER_ASSERT( VarZ == Qitem.getVarFired() );

			if ( VarX->isBound() )    {

				if (VarX->value() == neg)    {

					if ( VarZ->isBound() )
						VarY->removeSingle( VarZ->value() , this);

				}  else  {
					FEATHER_ASSERT( VarX->value() == ! neg );

					VarY->removeRange(kMinusInf, VarZ->min()-1, this);
					VarY->removeRange(VarZ->max()+1, kPlusInf, this);

					VarZ->removeRange(kMinusInf, VarY->min()-1, this);
					VarZ->removeRange(VarY->max()+1, kPlusInf, this);

					//if (VarY->contains( Qitem.getW() ))
					//	VarY->removeSingle( Qitem.getW() , this);
				}
			}
		}

		if ( VarY->max() == VarZ->min()  &&  VarY->min() == VarZ->max() )
			VarX->removeSingle( neg , this);
		if ( intersectionEmpty(VarY,VarZ) )
			VarX->removeSingle( ! neg , this);
	}
}




//	void
//Ns_ConstrMetaXeqYneqZ::ArcCons (void)  const
//{
//	MetaXeqYeqneqZArcCons(VarX, VarY, VarZ, this, true);
//}
//
//
//	void
//Ns_ConstrMetaXeqYneqZ::LocalArcCons (Ns_QueueItem& Qitem)  const
//{
//	MetaXeqYeqneqZLocalArcCons(Qitem, VarX, VarY, VarZ, this, true);
//}












	void
Ns_ConstrXeqYandZ::ArcCons (void)
{
	makeBoolean(VarX, this);

	if ( (VarY->min() * VarZ->min()) )
		VarX->removeSingle( neg , this);
	if ( ! (VarY->max() * VarZ->max()) )
		VarX->removeSingle( ! neg , this);

	if ( VarX->isBound() )   {
		if (VarX->value() == neg)  {
			//VOID

		}  else  {
			FEATHER_ASSERT( VarX->value() == ! neg );
			//if ( VarY->contains(0) )
				VarY->removeSingle( 0 , this);
			//if ( VarZ->contains(0) )
				VarZ->removeSingle( 0 , this);
		}
	}
}


	void
Ns_ConstrXeqYandZ::LocalArcCons (Ns_QueueItem& Qitem)
{
	if (VarX == Qitem.getVarFired())  {
		if ( VarX->isBound() )   {
			if (VarX->value() == neg)  {
				//VOID

			}  else  {
				FEATHER_ASSERT( VarX->value() == ! neg );
				//if ( VarY->contains(0) )
					VarY->removeSingle( 0 , this);
				//if ( VarZ->contains(0) )
					VarZ->removeSingle( 0 , this);
			}
		}

	}  else  {
		//FEATHER_ASSERT( VarY == Qitem.getVarFired() ,  "Ns_ConstrXeqYandZ::LocalArcCons: Wrong getVarFired");

		if ( (VarY->min() * VarZ->min()) )
			VarX->removeSingle( neg , this);
		if ( ! (VarY->max() * VarZ->max()) )
			VarX->removeSingle( ! neg , this);
	}
}




	void
Ns_ConstrXeqYorZ::ArcCons (void)
{
	makeBoolean(VarX, this);

	if ( VarY->min() + VarZ->min()  !=  0 )
		VarX->removeSingle( neg , this);
	if ( VarY->max() + VarZ->max()  ==  0 )
		VarX->removeSingle( ! neg , this);

	if ( VarX->isBound() )   {
		if (VarX->value() == neg)  {
			//if ( VarY->contains(1) )
				VarY->removeSingle( 1 , this);
			//if ( VarZ->contains(1) )
				VarZ->removeSingle( 1 , this);
		}  else  {
			FEATHER_ASSERT( VarX->value() == ! neg );
			//VOID
		}
	}
}


	void
Ns_ConstrXeqYorZ::LocalArcCons (Ns_QueueItem& Qitem)
{
	if (VarX == Qitem.getVarFired())  {
		if ( VarX->isBound() )   {
			if (VarX->value() == neg)  {
				//if ( VarY->contains(1) )
					VarY->removeSingle( 1 , this);
				//if ( VarZ->contains(1) )
					VarZ->removeSingle( 1 , this);
			}  else  {
				FEATHER_ASSERT( VarX->value() == ! neg );
				//VOID
			}
		}

	}  else  {
		//FEATHER_ASSERT( VarY == Qitem.getVarFired() ,  "Ns_ConstrXeqYorZ::LocalArcCons: Wrong getVarFired");

		if ( VarY->min() + VarZ->min()  !=  0 )
			VarX->removeSingle( neg , this);
		if ( VarY->max() + VarZ->max()  ==  0 )
			VarX->removeSingle( ! neg , this);
	}
}




	void
Ns_ConstrXorY::ArcCons (void)
{
	if (VarY->isBound()  &&  VarY->value()==neg )
		VarX->removeSingle( neg , this);
	if (VarX->isBound()  &&  VarX->value()==neg )
		VarY->removeSingle( neg , this);
}


	void
Ns_ConstrXorY::LocalArcCons (Ns_QueueItem& Qitem)
{
	//Int SupportVal = Qitem.getW();

	if (VarX == Qitem.getVarFired())  {
		if (VarX->isBound()  &&  VarX->value()==neg )
			VarY->removeSingle( neg , this);

	}  else  {

		FEATHER_ASSERT( VarY == Qitem.getVarFired() );

		if ( VarY->isBound()  &&  VarY->value() == neg )
			VarX->removeSingle( neg , this);
	}
}





// bounds-consistency only
	void
Ns_ConstrXeqYplusZ::ArcCons (void)
{
	bool  changed_summinmax = true;

	for ( ; ; )     {
		do  {
			if ( ! VarX->removeRange(kMinusInf,  VarY->min() + VarZ->min() - 1, this) )
				return;

			if ( VarY->min() + VarZ->max() < VarX->min() )   {
				//if ( ! VarY->removeRange( VarY->min() , this) )
				//	return;	// ... to avoid an infinite loop
				if ( ! VarY->removeRange(kMinusInf,  - VarZ->max() + VarX->min() -1, this) )
					return;
				changed_summinmax = true;
			}
			if ( VarZ->min() + VarY->max() < VarX->min() )   {
				//if ( ! VarZ->removeRange( VarZ->min() , this) )
				//	return;	// ... to avoid an infinite loop
				if ( ! VarZ->removeRange(kMinusInf,  - VarY->max() + VarX->min() -1, this) )
					return;
				changed_summinmax = true;
			}
		} while ( VarX->min() < VarY->min() + VarZ->min() );

		//  Initially `changed_summinmax' was intentionally set true, in order the
		//   following `if' statement to be ignored, the first time it is executed.
		if ( ! changed_summinmax )
			break;
		changed_summinmax = false;

		do  {
			if ( ! VarX->removeRange(VarY->max() + VarZ->max() + 1,  kPlusInf, this) )
				return;

			if ( VarY->max() + VarZ->min() > VarX->max() )   {
				//if ( ! VarY->removeRange( VarY->max() , this) )
				//	return;	// ... to avoid an infinite loop
				if ( ! VarY->removeRange(-VarZ->min() + VarX->max() +1,  kPlusInf, this) )
					return;
				changed_summinmax = true;
			}
			if ( VarZ->max() + VarY->min() > VarX->max() )   {
				//if ( ! VarZ->removeRange( VarZ->max() , this) )
				//	return;	// ... to avoid an infinite loop
				if ( ! VarZ->removeRange(-VarY->min() + VarX->max() +1,  kPlusInf, this) )
					return;
				changed_summinmax = true;
			}
		} while ( VarX->max() > VarY->max() + VarZ->max() );

		if ( ! changed_summinmax )
			break;
		changed_summinmax = false;
	}
}


// bounds-consistency only
	void
Ns_ConstrXeqYplusZ::LocalArcCons (Ns_QueueItem& Qitem)
{
	//if (Qitem.getVarFired()->min() < Qitem.getW()  &&  Qitem.getW() < Qitem.getVarFired()->max())
	//	return;	// bounds-consistency does not care

	ArcCons();
}












	void
quotient_min_max (const NsIntVar *VarY, NsIntVar *VarZ,
		Int min, Int max, const Ns_Constraint* constraint)
{
	VarZ->removeSingle(0, constraint);
	FEATHER_ASSERT( ! ( VarZ->isBound()  &&  VarZ->value() == 0 ) );

	Int quotient;

	quotient  =  VarY->min() / VarZ->min();
	min  =  max  =  quotient;

	quotient  =  VarY->max() / VarZ->min();
	if ( quotient  <  min )
		min  =  quotient;
	if ( quotient  >  max )
		max  =  quotient;

	quotient  =  VarY->min() / VarZ->max();
	if ( quotient  <  min )
		min  =  quotient;
	if ( quotient  >  max )
		max  =  quotient;

	quotient  =  VarY->max() / VarZ->max();
	if ( quotient  <  min )
		min  =  quotient;
	if ( quotient  >  max )
		max  =  quotient;


	Int divisor;

	if ( (divisor = VarZ->previous(0))  !=  kMinusInf )    {

		quotient  =  VarY->min() / divisor;
		if ( quotient  <  min )
			min  =  quotient;
		if ( quotient  >  max )
			max  =  quotient;

		quotient  =  VarY->max() / divisor;
		if ( quotient  <  min )
			min  =  quotient;
		if ( quotient  >  max )
			max  =  quotient;
	}


	if ( (divisor = VarZ->next(0))  !=  kPlusInf )    {

		quotient  =  VarY->min() / divisor;
		if ( quotient  <  min )
			min  =  quotient;
		if ( quotient  >  max )
			max  =  quotient;

		quotient  =  VarY->max() / divisor;
		if ( quotient  <  min)
			min  =  quotient;
		if ( quotient  >  max )
			max  =  quotient;
	}
}



	void
product_min_max (const NsIntVar *VarY, const NsIntVar *VarZ, Int& min, Int& max)
{
	Int prod;

	prod = VarY->min() * VarZ->min();
	min = max = prod;

	prod = VarY->max() * VarZ->min();
	if (prod < min)
		min = prod;
	if (prod > max)
		max = prod;

	prod = VarY->min() * VarZ->max();
	if (prod < min)
		min = prod;
	if (prod > max)
		max = prod;

	prod = VarY->max() * VarZ->max();
	if (prod < min)
		min = prod;
	if (prod > max)
		max = prod;
}



namespace  {

		void
	product_prune_bound (NsIntVar *VarX, NsIntVar *VarY, NsIntVar *VarZ,
			bool& changed_minmax, const Ns_Constraint* constraint)
	{
		while ( ( VarY->min() * VarZ->min() < VarX->min()  ||  VarY->min() * VarZ->min() > VarX->max() )
		     && ( VarY->min() * VarZ->max() < VarX->min()  ||  VarY->min() * VarZ->max() > VarX->max() ) )
		{
			if ( ! VarY->removeSingle( VarY->min(), constraint ) )
				return;	// ... to avoid an infinite loop

			changed_minmax = true;
		}

		while ( ( VarY->max() * VarZ->min() < VarX->min()  ||  VarY->max() * VarZ->min() > VarX->max() )
		     && ( VarY->max() * VarZ->max() < VarX->min()  ||  VarY->max() * VarZ->max() > VarX->max() ) )
		{
			if ( ! VarY->removeSingle( VarY->max(), constraint ) )
				return;	// ... to avoid an infinite loop

			changed_minmax = true;
		}
	}

}  // namespace





// bounds-consistency only
	void
Ns_ConstrXeqYtimesZ::ArcCons (void)
{
	Int min, max;

	bool  changed_minmax;

	do  {
		changed_minmax = false;

		product_min_max(VarY, VarZ, min, max);
		VarX->removeRange(kMinusInf, min-1, this);
		VarX->removeRange(max+1, kPlusInf, this);

		product_prune_bound(VarX, VarY, VarZ, changed_minmax, this);
		product_prune_bound(VarX, VarZ, VarY, changed_minmax, this);

	} while ( changed_minmax );
}


// bounds-consistency only
	void
Ns_ConstrXeqYtimesZ::LocalArcCons (Ns_QueueItem& Qitem)
{
	//if (Qitem.getVarFired()->min() < Qitem.getW()  &&  Qitem.getW() < Qitem.getVarFired()->max())
	//	return;	// bounds-consistency does not care

	ArcCons();
}






}




