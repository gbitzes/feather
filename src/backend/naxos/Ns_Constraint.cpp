#include <backend/naxos/Ns_Constraint.h>
#include <backend/naxos/naxos.h>

namespace feather {

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


	// NsIntVar::const_iterator  v;
	
	// for (v = VarX->begin();  v != VarX->end();  ++v)  {
	
	// 	if ( ! VarY->contains( *v - C ) )
	// 		VarX->removeSingle( *v , this);
	// }
	// for (v = VarY->begin();  v != VarY->end();  ++v)  {
	
	// 	if ( ! VarX->contains( *v + C ) )
	// 		VarY->removeSingle( *v , this);
	// }

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
	//	assert_Ns( VarY == Qitem.getVarFired() ,
	//	"Ns_ConstrXeqYplusC::LocalArcCons: Wrong getVarFired");

	//	VarX->removeRange(kMinusInf, VarY->min()+C-1, this);
	//	VarX->removeRange(VarY->max()+C+1, kPlusInf, this);
	//}


	//NsInt  SupportVal;
	//
	//if (VarX == Qitem.getVarFired())  {
	//	SupportVal = Qitem.getW() - C;
	//	if (VarY->contains( SupportVal ))
	//		VarY->removeSingle( SupportVal , this);
	//
	//}  else  {
	//	assert_Ns( VarY == Qitem.getVarFired() ,
	//			"Ns_ConstrXeqYplusC::LocalArcCons: Wrong getVarFired");
	//
	//	SupportVal = Qitem.getW() + C;
	//	if (VarX->contains( SupportVal ))
	//		VarX->removeSingle( SupportVal , this);
	//}
}





}




