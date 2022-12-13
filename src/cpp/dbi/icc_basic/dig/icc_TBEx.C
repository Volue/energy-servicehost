#include "TBExKeytab9.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExKeytab9 :: TBExKeytab9 () :
TBKeytab9 (),
ExTable( this, 0, 0)
{

}

TBExKeytab9 :: TBExKeytab9 (const pTBExKeytab9 exKeytab9) : TBKeytab9((pTBKeytab9)exKeytab9),
		ExTable((ExTable *)exKeytab9){
}

TBExKeytab9 :: ~TBExKeytab9()
{
}
#include "TBExAccbaTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExAccbaTp :: TBExAccbaTp () :
TBAccbaTp (),
ExTable( this, 0, 1)
{

	AddMember(&contract, 1, CAN);
	AddMemberKey(0, &acbaKey, 22);
}

TBExAccbaTp :: TBExAccbaTp (const pTBExAccbaTp exAccbaTp) : TBAccbaTp((pTBAccbaTp)exAccbaTp),
		ExTable((ExTable *)exAccbaTp),
				contract( &(exAccbaTp->contract) )
{
}

TBExAccbaTp :: ~TBExAccbaTp()
{
}
#include "TBExAccntrec.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExAccntrec :: TBExAccntrec () :
TBAccntrec (),
ExTable( this, 1, 0)
{

	AddOwner(&oprtunit, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &opunKey, 0);
	AddOwnerAlternateKey(0, &oprtunitName, 1, icc_f.name);
	AddOwnerAlternateKey(0, &oprtunitShorname, 2, icc_f.shorname);
}

TBExAccntrec :: TBExAccntrec (const pTBExAccntrec exAccntrec) : TBAccntrec((pTBAccntrec)exAccntrec),
		ExTable((ExTable *)exAccntrec),
				oprtunitName( exAccntrec->oprtunitName ),
				oprtunitShorname( exAccntrec->oprtunitShorname ),
				oprtunit( &(exAccntrec->oprtunit) )
{
}

TBExAccntrec :: ~TBExAccntrec()
{
}
#include "TBExAccntrp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExAccntrp :: TBExAccntrp () :
TBAccntrp (),
ExTable( this, 1, 0)
{

	AddOwner(&oprtunit, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &opunKey, 0);
	AddOwnerAlternateKey(0, &oprtunitName, 1, icc_f.name);
	AddOwnerAlternateKey(0, &oprtunitShorname, 2, icc_f.shorname);
}

TBExAccntrp :: TBExAccntrp (const pTBExAccntrp exAccntrp) : TBAccntrp((pTBAccntrp)exAccntrp),
		ExTable((ExTable *)exAccntrp),
				oprtunitName( exAccntrp->oprtunitName ),
				oprtunitShorname( exAccntrp->oprtunitShorname ),
				oprtunit( &(exAccntrp->oprtunit) )
{
}

TBExAccntrp :: ~TBExAccntrp()
{
}
#include "TBExAccrpobj.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExAccrpobj :: TBExAccrpobj () :
TBAccrpobj (),
ExTable( this, 2, 0)
{

	AddOwner(&trdreport, 2, 1, MUST);
	AddOwnerPrimaryKey(0, &opunKey, 0);
	AddOwnerPrimaryKey(0, &rpcode, 1);
	AddOwnerAlternateKey(0, &trdreportRpcode, 1, icc_f.rpcode);
	AddOwner(&trobject, 2, 2, MUST);
	AddOwnerPrimaryKey(1, &trcode, 0);
	AddOwnerPrimaryKey(1, &opunKey, 2);
	AddOwnerAlternateKey(1, &trobjectTrcode, 0, icc_f.trcode);
	AddOwnerAlternateKey(1, &trobjectName, 1, icc_f.name);
}

TBExAccrpobj :: TBExAccrpobj (const pTBExAccrpobj exAccrpobj) : TBAccrpobj((pTBAccrpobj)exAccrpobj),
		ExTable((ExTable *)exAccrpobj),
				trdreportRpcode( exAccrpobj->trdreportRpcode ),
				trobjectTrcode( exAccrpobj->trobjectTrcode ),
				trobjectName( exAccrpobj->trobjectName ),
				trdreport( &(exAccrpobj->trdreport) ),
				trobject( &(exAccrpobj->trobject) )
{
}

TBExAccrpobj :: ~TBExAccrpobj()
{
}
#include "TBExAghcoprr.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExAghcoprr :: TBExAghcoprr () :
TBAghcoprr (),
ExTable( this, 2, 0)
{

	AddOwner(&aghydDs, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &hydKey, 0);
	AddOwnerAlternateKey(0, &aghydDsCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &aghydDsName, 2, icc_f.name);
	AddOwner(&prrunDs, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &prdKey, 0);
	AddOwnerAlternateKey(1, &prrunDsCode, 1, icc_f.code);
	AddOwnerAlternateKey(1, &prrunDsName, 2, icc_f.name);
}

TBExAghcoprr :: TBExAghcoprr (const pTBExAghcoprr exAghcoprr) : TBAghcoprr((pTBAghcoprr)exAghcoprr),
		ExTable((ExTable *)exAghcoprr),
				aghydDsCode( exAghcoprr->aghydDsCode ),
				aghydDsName( exAghcoprr->aghydDsName ),
				prrunDsCode( exAghcoprr->prrunDsCode ),
				prrunDsName( exAghcoprr->prrunDsName ),
				aghydDs( &(exAghcoprr->aghydDs) ),
				prrunDs( &(exAghcoprr->prrunDs) )
{
}

TBExAghcoprr :: ~TBExAghcoprr()
{
}
#include "TBExAgrmpart.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExAgrmpart :: TBExAgrmpart () :
TBAgrmpart (),
ExTable( this, 7, 1)
{

	AddOwner(&agrmtTp, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &powType, 0);
	AddOwnerAlternateKey(0, &agrmtTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &agrmtTpName, 2, icc_f.name);
	AddOwner(&exchPnt, 2, 2, MUST);
	AddOwnerPrimaryKey(1, &exchKey, 0);
	AddOwnerPrimaryKey(1, &seqno, 1);
	AddOwnerAlternateKey(1, &exchPntName, 2, icc_f.name);
	AddOwnerAlternateKey(1, &exchPntShorname, 3, icc_f.shorname);
	AddOwner(&groups, 1, 2, CAN);
	AddOwnerPrimaryKey(2, &grupKey, 0);
	AddOwnerAlternateKey(2, &groupsName, 1, icc_f.name);
	AddOwnerAlternateKey(2, &groupsDescript, 2, icc_f.descript);
	AddOwner(&oprtunit, 1, 2, MUST);
	AddOwnerPrimaryKey(3, &opunKey, 0);
	AddOwnerAlternateKey(3, &oprtunitName, 1, icc_f.name);
	AddOwnerAlternateKey(3, &oprtunitShorname, 2, icc_f.shorname);
	AddOwner(&tarifdef, 2, 2, MUST);
	AddOwnerPrimaryKey(4, &tarfKey, 0);
	AddOwnerPrimaryKey(4, &seqno, 1);
	AddOwnerAlternateKey(4, &tarifdefCode, 2, icc_f.code);
	AddOwnerAlternateKey(4, &tarifdefName, 3, icc_f.name);
	AddOwner(&debitTp, 1, 2, MUST);
	AddOwnerPrimaryKey(5, &debtKey, 0);
	AddOwnerAlternateKey(5, &debitTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(5, &debitTpName, 2, icc_f.name);
	AddOwner(&debssTp, 1, 2, MUST);
	AddOwnerPrimaryKey(6, &debsKey, 0);
	AddOwnerAlternateKey(6, &debssTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(6, &debssTpName, 2, icc_f.name);
	AddMember(&delivagm, 2, CAN);
	AddMemberKey(0, &opunKey, 5);
	AddMemberKey(0, &partNum, 6);
}

TBExAgrmpart :: TBExAgrmpart (const pTBExAgrmpart exAgrmpart) : TBAgrmpart((pTBAgrmpart)exAgrmpart),
		ExTable((ExTable *)exAgrmpart),
				agrmtTpCode( exAgrmpart->agrmtTpCode ),
				agrmtTpName( exAgrmpart->agrmtTpName ),
				exchPntName( exAgrmpart->exchPntName ),
				exchPntShorname( exAgrmpart->exchPntShorname ),
				groupsName( exAgrmpart->groupsName ),
				groupsDescript( exAgrmpart->groupsDescript ),
				oprtunitName( exAgrmpart->oprtunitName ),
				oprtunitShorname( exAgrmpart->oprtunitShorname ),
				tarifdefCode( exAgrmpart->tarifdefCode ),
				tarifdefName( exAgrmpart->tarifdefName ),
				debitTpCode( exAgrmpart->debitTpCode ),
				debitTpName( exAgrmpart->debitTpName ),
				debssTpCode( exAgrmpart->debssTpCode ),
				debssTpName( exAgrmpart->debssTpName ),
				agrmtTp( &(exAgrmpart->agrmtTp) ),
				exchPnt( &(exAgrmpart->exchPnt) ),
				groups( &(exAgrmpart->groups) ),
				oprtunit( &(exAgrmpart->oprtunit) ),
				tarifdef( &(exAgrmpart->tarifdef) ),
				debitTp( &(exAgrmpart->debitTp) ),
				debssTp( &(exAgrmpart->debssTp) ),
				delivagm( &(exAgrmpart->delivagm) )
{
}

TBExAgrmpart :: ~TBExAgrmpart()
{
}
#include "TBExBidblock.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExBidblock :: TBExBidblock () :
TBBidblock (),
ExTable( this, 4, 0)
{

	AddOwner(&elspotBlocksTp, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &blockno, 0);
	AddOwnerAlternateKey(0, &elspotBlocksTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &elspotBlocksTpName, 2, icc_f.name);
	AddOwner(&sign, 2, 2, CAN);
	AddOwnerPrimaryKey(1, &signcode, 0);
	AddOwnerPrimaryKey(1, &opunKey, 6);
	AddOwnerAlternateKey(1, &signSigncode, 0, icc_f.signcode);
	AddOwnerAlternateKey(1, &signName, 1, icc_f.name);
	AddOwner(&trareTp, 1, 2, MUST);
	AddOwnerPrimaryKey(2, &tratKey, 0);
	AddOwnerAlternateKey(2, &trareTpTratKey, 0, icc_f.tratKey);
	AddOwnerAlternateKey(2, &trareTpColour, 1, icc_f.colour);
	AddOwner(&trelsbid, 2, 1, MUST);
	AddOwnerPrimaryKey(3, &opunKey, 0);
	AddOwnerPrimaryKey(3, &trcode, 1);
	AddOwnerAlternateKey(3, &trelsbidTrcode, 1, icc_f.trcode);
}

TBExBidblock :: TBExBidblock (const pTBExBidblock exBidblock) : TBBidblock((pTBBidblock)exBidblock),
		ExTable((ExTable *)exBidblock),
				elspotBlocksTpCode( exBidblock->elspotBlocksTpCode ),
				elspotBlocksTpName( exBidblock->elspotBlocksTpName ),
				signSigncode( exBidblock->signSigncode ),
				signName( exBidblock->signName ),
				trareTpTratKey( exBidblock->trareTpTratKey ),
				trareTpColour( exBidblock->trareTpColour ),
				trelsbidTrcode( exBidblock->trelsbidTrcode ),
				elspotBlocksTp( &(exBidblock->elspotBlocksTp) ),
				sign( &(exBidblock->sign) ),
				trareTp( &(exBidblock->trareTp) ),
				trelsbid( &(exBidblock->trelsbid) )
{
}

TBExBidblock :: ~TBExBidblock()
{
}
#include "TBExBid24frm.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExBid24frm :: TBExBid24frm () :
TBBid24frm (),
ExTable( this, 3, 1)
{

	AddOwner(&sign, 2, 2, CAN);
	AddOwnerPrimaryKey(0, &signcode, 0);
	AddOwnerPrimaryKey(0, &opunKey, 6);
	AddOwnerAlternateKey(0, &signSigncode, 0, icc_f.signcode);
	AddOwnerAlternateKey(0, &signName, 1, icc_f.name);
	AddOwner(&trareTp, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &tratKey, 0);
	AddOwnerAlternateKey(1, &trareTpTratKey, 0, icc_f.tratKey);
	AddOwnerAlternateKey(1, &trareTpColour, 1, icc_f.colour);
	AddOwner(&trobject, 2, 2, MUST);
	AddOwnerPrimaryKey(2, &trcode, 0);
	AddOwnerPrimaryKey(2, &opunKey, 2);
	AddOwnerAlternateKey(2, &trobjectTrcode, 0, icc_f.trcode);
	AddOwnerAlternateKey(2, &trobjectName, 1, icc_f.name);
	AddMember(&bid24hou, 5, CAN);
	AddMemberKey(0, &opunKey, 0);
	AddMemberKey(0, &trcode, 1);
	AddMemberKey(0, &timstamp, 2);
	AddMemberKey(0, &tratKey, 3);
	AddMemberKey(0, &formno, 4);
}

TBExBid24frm :: TBExBid24frm (const pTBExBid24frm exBid24frm) : TBBid24frm((pTBBid24frm)exBid24frm),
		ExTable((ExTable *)exBid24frm),
				signSigncode( exBid24frm->signSigncode ),
				signName( exBid24frm->signName ),
				trareTpTratKey( exBid24frm->trareTpTratKey ),
				trareTpColour( exBid24frm->trareTpColour ),
				trobjectTrcode( exBid24frm->trobjectTrcode ),
				trobjectName( exBid24frm->trobjectName ),
				sign( &(exBid24frm->sign) ),
				trareTp( &(exBid24frm->trareTp) ),
				trobject( &(exBid24frm->trobject) ),
				bid24hou( &(exBid24frm->bid24hou) )
{
}

TBExBid24frm :: ~TBExBid24frm()
{
}
#include "TBExBid24hou.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExBid24hou :: TBExBid24hou () :
TBBid24hou (),
ExTable( this, 1, 0)
{

	AddOwner(&bid24frm, 5, 2, MUST);
	AddOwnerPrimaryKey(0, &opunKey, 0);
	AddOwnerPrimaryKey(0, &trcode, 1);
	AddOwnerPrimaryKey(0, &timstamp, 2);
	AddOwnerPrimaryKey(0, &tratKey, 3);
	AddOwnerPrimaryKey(0, &formno, 4);
	AddOwnerAlternateKey(0, &bid24frmTrcode, 1, icc_f.trcode);
	AddOwnerAlternateKey(0, &bid24frmTimstamp, 2, icc_f.timstamp);
}

TBExBid24hou :: TBExBid24hou (const pTBExBid24hou exBid24hou) : TBBid24hou((pTBBid24hou)exBid24hou),
		ExTable((ExTable *)exBid24hou),
				bid24frmTrcode( exBid24hou->bid24frmTrcode ),
				bid24frmTimstamp( exBid24hou->bid24frmTimstamp ),
				bid24frm( &(exBid24hou->bid24frm) )
{
}

TBExBid24hou :: ~TBExBid24hou()
{
}
#if 0
#include "TBExBidregpo.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExBidregpo :: TBExBidregpo () :
TBBidregpo (),
ExTable( this, 1, 0)
{

	AddOwner(&bidrpobj, 7, 2, MUST);
	AddOwnerPrimaryKey(0, &opunKea, 0);
	AddOwnerPrimaryKey(0, &trcode, 1);
	AddOwnerPrimaryKey(0, &opunKey, 2);
	AddOwnerPrimaryKey(0, &timstamp, 3);
	AddOwnerPrimaryKey(0, &formno, 4);
	AddOwnerPrimaryKey(0, &upDown, 5);
	AddOwnerPrimaryKey(0, &price, 6);
	AddOwnerAlternateKey(0, &bidrpobjTrcode, 1, icc_f.trcode);
	AddOwnerAlternateKey(0, &bidrpobjTimstamp, 3, icc_f.timstamp);
}

TBExBidregpo :: TBExBidregpo (const pTBExBidregpo exBidregpo) : TBBidregpo((pTBBidregpo)exBidregpo),
		ExTable((ExTable *)exBidregpo),
				bidrpobjTrcode( exBidregpo->bidrpobjTrcode ),
				bidrpobjTimstamp( exBidregpo->bidrpobjTimstamp ),
				bidrpobj( &(exBidregpo->bidrpobj) )
{
}

TBExBidregpo :: ~TBExBidregpo()
{
}
#endif
#include "TBExBidrpfrm.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExBidrpfrm :: TBExBidrpfrm () :
TBBidrpfrm (),
ExTable( this, 2, 0/*1*/)
{

	AddOwner(&oprtunit, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &opunKey, 0);
	AddOwnerAlternateKey(0, &oprtunitName, 1, icc_f.name);
	AddOwnerAlternateKey(0, &oprtunitShorname, 2, icc_f.shorname);
	AddOwner(&sign, 2, 2, CAN);
	AddOwnerPrimaryKey(1, &signcode, 0);
	AddOwnerPrimaryKey(1, &opunKey, 6);
	AddOwnerAlternateKey(1, &signSigncode, 0, icc_f.signcode);
	AddOwnerAlternateKey(1, &signName, 1, icc_f.name);
	//AddMember(&bidrpobj, 3, CAN);
	//AddMemberKey(0, &opunKey, 2);
	//AddMemberKey(0, &timstamp, 3);
	//AddMemberKey(0, &formno, 4);
}

TBExBidrpfrm :: TBExBidrpfrm (const pTBExBidrpfrm exBidrpfrm) : TBBidrpfrm((pTBBidrpfrm)exBidrpfrm),
		ExTable((ExTable *)exBidrpfrm),
				oprtunitName( exBidrpfrm->oprtunitName ),
				oprtunitShorname( exBidrpfrm->oprtunitShorname ),
				signSigncode( exBidrpfrm->signSigncode ),
				signName( exBidrpfrm->signName ),
				oprtunit( &(exBidrpfrm->oprtunit) ),
				sign( &(exBidrpfrm->sign) )//,
				//bidrpobj( &(exBidrpfrm->bidrpobj) )
{
}

TBExBidrpfrm :: ~TBExBidrpfrm()
{
}
#include "TBExBuyslTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExBuyslTp :: TBExBuyslTp () :
TBBuyslTp (),
ExTable( this, 0, 2)
{

	AddMember(&contract, 1, CAN);
	AddMemberKey(0, &buslKey, 27);
	AddMember(&troption, 1, CAN);
	AddMemberKey(1, &buslKey, 6);
}

TBExBuyslTp :: TBExBuyslTp (const pTBExBuyslTp exBuyslTp) : TBBuyslTp((pTBBuyslTp)exBuyslTp),
		ExTable((ExTable *)exBuyslTp),
				contract( &(exBuyslTp->contract) ),
				troption( &(exBuyslTp->troption) )
{
}

TBExBuyslTp :: ~TBExBuyslTp()
{
}
#include "TBExCatmarea.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExCatmarea :: TBExCatmarea () :
TBCatmarea (),
ExTable( this, 2, 1)
{

	AddOwner(&catmaTp, 1, 2, CAN);
	AddOwnerPrimaryKey(0, &cataKey, 0);
	AddOwnerAlternateKey(0, &catmaTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &catmaTpName, 2, icc_f.name);
	AddOwner(&hydcomp, 1, 1, MUST);
	AddOwnerPrimaryKey(1, &hydKey, 0);
	AddOwnerAlternateKey(1, &hydcompName, 3, icc_f.name);
	AddMember(&hypsoVl, 1, CAN);
	AddMemberKey(0, &hydKey, 4);
}

TBExCatmarea :: TBExCatmarea (const pTBExCatmarea exCatmarea) : TBCatmarea((pTBCatmarea)exCatmarea),
		ExTable((ExTable *)exCatmarea),
				catmaTpCode( exCatmarea->catmaTpCode ),
				catmaTpName( exCatmarea->catmaTpName ),
				hydcompName( exCatmarea->hydcompName ),
				catmaTp( &(exCatmarea->catmaTp) ),
				hydcomp( &(exCatmarea->hydcomp) ),
				hypsoVl( &(exCatmarea->hypsoVl) )
{
}

TBExCatmarea :: ~TBExCatmarea()
{
}
#include "TBExCcmetTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExCcmetTp :: TBExCcmetTp () :
TBCcmetTp (),
ExTable( this, 0, 1)
{

	AddMember(&tsapmCc, 1, CAN);
	AddMemberKey(0, &ccmeKey, 0);
}

TBExCcmetTp :: TBExCcmetTp (const pTBExCcmetTp exCcmetTp) : TBCcmetTp((pTBCcmetTp)exCcmetTp),
		ExTable((ExTable *)exCcmetTp),
				tsapmCc( &(exCcmetTp->tsapmCc) )
{
}

TBExCcmetTp :: ~TBExCcmetTp()
{
}
#include "TBExCntPers.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExCntPers :: TBExCntPers () :
TBCntPers (),
ExTable( this, 2, 0)
{

	AddOwner(&groups, 1, 2, CAN);
	AddOwnerPrimaryKey(0, &grupKey, 0);
	AddOwnerAlternateKey(0, &groupsName, 1, icc_f.name);
	AddOwnerAlternateKey(0, &groupsDescript, 2, icc_f.descript);
	AddOwner(&oprtunit, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &opunKey, 0);
	AddOwnerAlternateKey(1, &oprtunitName, 1, icc_f.name);
	AddOwnerAlternateKey(1, &oprtunitShorname, 2, icc_f.shorname);
}

TBExCntPers :: TBExCntPers (const pTBExCntPers exCntPers) : TBCntPers((pTBCntPers)exCntPers),
		ExTable((ExTable *)exCntPers),
				groupsName( exCntPers->groupsName ),
				groupsDescript( exCntPers->groupsDescript ),
				oprtunitName( exCntPers->oprtunitName ),
				oprtunitShorname( exCntPers->oprtunitShorname ),
				groups( &(exCntPers->groups) ),
				oprtunit( &(exCntPers->oprtunit) )
{
}

TBExCntPers :: ~TBExCntPers()
{
}
#include "TBExConstCo.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExConstCo :: TBExConstCo () :
TBConstCo (),
ExTable( this, 1, 0)
{

	AddOwner(&tsapmCc, 2, 0, MUST);
	AddOwnerPrimaryKey(0, &ccmeKey, 0);
	AddOwnerPrimaryKey(0, &timsKey, 1);
}

TBExConstCo :: TBExConstCo (const pTBExConstCo exConstCo) : TBConstCo((pTBConstCo)exConstCo),
		ExTable((ExTable *)exConstCo),
				tsapmCc( &(exConstCo->tsapmCc) )
{
}

TBExConstCo :: ~TBExConstCo()
{
}
#include "TBExContact.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExContact :: TBExContact () :
TBContact (),
ExTable( this, 1, 0)
{

	AddOwner(&partner, 2, 2, CAN);
	AddOwnerPrimaryKey(0, &opunKey, 0);
	AddOwnerPrimaryKey(0, &trcode, 1);
	AddOwnerAlternateKey(0, &partnerTrcode, 1, icc_f.trcode);
	AddOwnerAlternateKey(0, &partnerEntregno, 2, icc_f.entregno);
}

TBExContact :: TBExContact (const pTBExContact exContact) : TBContact((pTBContact)exContact),
		ExTable((ExTable *)exContact),
				partnerTrcode( exContact->partnerTrcode ),
				partnerEntregno( exContact->partnerEntregno ),
				partner( &(exContact->partner) )
{
}

TBExContact :: ~TBExContact()
{
}
#include "TBExContnode.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExContnode :: TBExContnode () :
TBContnode (),
ExTable( this, 0, 0)
{

}

TBExContnode :: TBExContnode (const pTBExContnode exContnode) : TBContnode((pTBContnode)exContnode),
		ExTable((ExTable *)exContnode){
}

TBExContnode :: ~TBExContnode()
{
}
#include "TBExContract.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExContract :: TBExContract () :
TBContract (),
ExTable( this, 6, 1)
{

	AddOwner(&accbaTp, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &acbaKey, 0);
	AddOwnerAlternateKey(0, &accbaTpAcbaKey, 0, icc_f.acbaKey);
	AddOwnerAlternateKey(0, &accbaTpName, 2, icc_f.name);
	AddOwner(&buyslTp, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &buslKey, 0);
	AddOwnerAlternateKey(1, &buyslTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(1, &buyslTpName, 2, icc_f.name);
	AddOwner(&cosetTp, 1, 2, MUST);
	AddOwnerPrimaryKey(2, &coseKey, 0);
	AddOwnerAlternateKey(2, &cosetTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(2, &cosetTpName, 2, icc_f.name);
	AddOwner(&costaTp, 1, 2, MUST);
	AddOwnerPrimaryKey(3, &cstpKey, 0);
	AddOwnerAlternateKey(3, &costaTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(3, &costaTpName, 2, icc_f.name);
	AddOwner(&couseTp, 1, 2, MUST);
	AddOwnerPrimaryKey(4, &cutpKey, 0);
	AddOwnerAlternateKey(4, &couseTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(4, &couseTpName, 2, icc_f.name);
	AddOwner(&trobject, 2, 2, MUST);
	AddOwnerPrimaryKey(5, &trcode, 0);
	AddOwnerPrimaryKey(5, &opunKey, 2);
	AddOwnerAlternateKey(5, &trobjectTrcode, 0, icc_f.trcode);
	AddOwnerAlternateKey(5, &trobjectName, 1, icc_f.name);
	AddMember(&invpfrm, 2, CAN);
	AddMemberKey(0, &opunKey, 12);
	AddMemberKey(0, &trcode, 13);
}

TBExContract :: TBExContract (const pTBExContract exContract) : TBContract((pTBContract)exContract),
		ExTable((ExTable *)exContract),
				accbaTpAcbaKey( exContract->accbaTpAcbaKey ),
				accbaTpName( exContract->accbaTpName ),
				buyslTpCode( exContract->buyslTpCode ),
				buyslTpName( exContract->buyslTpName ),
				cosetTpCode( exContract->cosetTpCode ),
				cosetTpName( exContract->cosetTpName ),
				costaTpCode( exContract->costaTpCode ),
				costaTpName( exContract->costaTpName ),
				couseTpCode( exContract->couseTpCode ),
				couseTpName( exContract->couseTpName ),
				trobjectTrcode( exContract->trobjectTrcode ),
				trobjectName( exContract->trobjectName ),
				accbaTp( &(exContract->accbaTp) ),
				buyslTp( &(exContract->buyslTp) ),
				cosetTp( &(exContract->cosetTp) ),
				costaTp( &(exContract->costaTp) ),
				couseTp( &(exContract->couseTp) ),
				trobject( &(exContract->trobject) ),
				invpfrm( &(exContract->invpfrm) )
{
}

TBExContract :: ~TBExContract()
{
}
#include "TBExCosetTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExCosetTp :: TBExCosetTp () :
TBCosetTp (),
ExTable( this, 0, 1)
{

	AddMember(&contract, 1, CAN);
	AddMemberKey(0, &coseKey, 23);
}

TBExCosetTp :: TBExCosetTp (const pTBExCosetTp exCosetTp) : TBCosetTp((pTBCosetTp)exCosetTp),
		ExTable((ExTable *)exCosetTp),
				contract( &(exCosetTp->contract) )
{
}

TBExCosetTp :: ~TBExCosetTp()
{
}
#include "TBExCostaTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExCostaTp :: TBExCostaTp () :
TBCostaTp (),
ExTable( this, 0, 1)
{

	AddMember(&contract, 1, CAN);
	AddMemberKey(0, &cstpKey, 24);
}

TBExCostaTp :: TBExCostaTp (const pTBExCostaTp exCostaTp) : TBCostaTp((pTBCostaTp)exCostaTp),
		ExTable((ExTable *)exCostaTp),
				contract( &(exCostaTp->contract) )
{
}

TBExCostaTp :: ~TBExCostaTp()
{
}
#include "TBExCounter.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExCounter :: TBExCounter () :
TBCounter (),
ExTable( this, 1, 1)
{

	AddOwner(&trobject, 2, 2, MUST);
	AddOwnerPrimaryKey(0, &trcode, 0);
	AddOwnerPrimaryKey(0, &opunKey, 2);
	AddOwnerAlternateKey(0, &trobjectTrcode, 0, icc_f.trcode);
	AddOwnerAlternateKey(0, &trobjectName, 1, icc_f.name);
	AddMember(&cntper, 2, CAN);
	AddMemberKey(0, &opunKey, 0);
	AddMemberKey(0, &trcode, 1);
}

TBExCounter :: TBExCounter (const pTBExCounter exCounter) : TBCounter((pTBCounter)exCounter),
		ExTable((ExTable *)exCounter),
				trobjectTrcode( exCounter->trobjectTrcode ),
				trobjectName( exCounter->trobjectName ),
				trobject( &(exCounter->trobject) ),
				cntper( &(exCounter->cntper) )
{
}

TBExCounter :: ~TBExCounter()
{
}
#include "TBExCouseTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExCouseTp :: TBExCouseTp () :
TBCouseTp (),
ExTable( this, 0, 1)
{

	AddMember(&contract, 1, CAN);
	AddMemberKey(0, &cutpKey, 26);
}

TBExCouseTp :: TBExCouseTp (const pTBExCouseTp exCouseTp) : TBCouseTp((pTBCouseTp)exCouseTp),
		ExTable((ExTable *)exCouseTp),
				contract( &(exCouseTp->contract) )
{
}

TBExCouseTp :: ~TBExCouseTp()
{
}
#include "TBExCurrency.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExCurrency :: TBExCurrency () :
TBCurrency (),
ExTable( this, 0, 0)
{

}

TBExCurrency :: TBExCurrency (const pTBExCurrency exCurrency) : TBCurrency((pTBCurrency)exCurrency),
		ExTable((ExTable *)exCurrency){
}

TBExCurrency :: ~TBExCurrency()
{
}
#include "TBExDelcobal.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExDelcobal :: TBExDelcobal () :
TBDelcobal (),
ExTable( this, 0, 0)
{

}

TBExDelcobal :: TBExDelcobal (const pTBExDelcobal exDelcobal) : TBDelcobal((pTBDelcobal)exDelcobal),
		ExTable((ExTable *)exDelcobal){
}

TBExDelcobal :: ~TBExDelcobal()
{
}
#include "TBExDirecTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExDirecTp :: TBExDirecTp () :
TBDirecTp (),
ExTable( this, 0, 1)
{

	AddMember(&trobject, 1, CAN);
	AddMemberKey(0, &dircKey, 3);
}

TBExDirecTp :: TBExDirecTp (const pTBExDirecTp exDirecTp) : TBDirecTp((pTBDirecTp)exDirecTp),
		ExTable((ExTable *)exDirecTp),
				trobject( &(exDirecTp->trobject) )
{
}

TBExDirecTp :: ~TBExDirecTp()
{
}
#include "TBExDoctext.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExDoctext :: TBExDoctext () :
TBDoctext (),
ExTable( this, 1, 0)
{

	AddOwner(&document, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &docindex, 0);
	AddOwnerAlternateKey(0, &documentCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &documentName, 2, icc_f.name);
}

TBExDoctext :: TBExDoctext (const pTBExDoctext exDoctext) : TBDoctext((pTBDoctext)exDoctext),
		ExTable((ExTable *)exDoctext),
				documentCode( exDoctext->documentCode ),
				documentName( exDoctext->documentName ),
				document( &(exDoctext->document) )
{
}

TBExDoctext :: ~TBExDoctext()
{
}
#include "TBExDocument.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExDocument :: TBExDocument () :
TBDocument (),
ExTable( this, 0, 1)
{

	AddMember(&doctext, 1, CAN);
	AddMemberKey(0, &docindex, 0);
}

TBExDocument :: TBExDocument (const pTBExDocument exDocument) : TBDocument((pTBDocument)exDocument),
		ExTable((ExTable *)exDocument),
				doctext( &(exDocument->doctext) )
{
}

TBExDocument :: ~TBExDocument()
{
}
#include "TBExEleccomp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExEleccomp :: TBExEleccomp () :
TBEleccomp (),
ExTable( this, 1, 6)
{

	AddOwner(&elcmpTp, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &elcmKey, 0);
	AddOwnerAlternateKey(0, &elcmpTpElcmKey, 0, icc_f.elcmKey);
	AddOwnerAlternateKey(0, &elcmpTpName, 2, icc_f.name);
	AddMember(&trobject, 1, CAN);
	AddMemberKey(0, &elecKey, 4);
	AddMember(&breaker, 1, CAN);
	AddMemberKey(1, &elecKey, 0);
	AddMember(&busbar, 1, CAN);
	AddMemberKey(2, &elecKey, 0);
	AddMember(&generato, 1, CAN);
	AddMemberKey(3, &elecKey, 0);
	AddMember(&trafo2w, 1, CAN);
	AddMemberKey(4, &elecKey, 0);
	AddMember(&translin, 1, CAN);
	AddMemberKey(5, &elecKey, 0);
}

TBExEleccomp :: TBExEleccomp (const pTBExEleccomp exEleccomp) : TBEleccomp((pTBEleccomp)exEleccomp),
		ExTable((ExTable *)exEleccomp),
				elcmpTpElcmKey( exEleccomp->elcmpTpElcmKey ),
				elcmpTpName( exEleccomp->elcmpTpName ),
				elcmpTp( &(exEleccomp->elcmpTp) ),
				trobject( &(exEleccomp->trobject) ),
				breaker( &(exEleccomp->breaker) ),
				busbar( &(exEleccomp->busbar) ),
				generato( &(exEleccomp->generato) ),
				trafo2w( &(exEleccomp->trafo2w) ),
				translin( &(exEleccomp->translin) )
{
}

TBExEleccomp :: ~TBExEleccomp()
{
}
#include "TBExElspotBlocksTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExElspotBlocksTp :: TBExElspotBlocksTp () :
TBElspotBlocksTp (),
ExTable( this, 0, 1)
{

	AddMember(&bidblock, 1, CAN);
	AddMemberKey(0, &blockno, 6);
}

TBExElspotBlocksTp :: TBExElspotBlocksTp (const pTBExElspotBlocksTp exElspotBlocksTp) : TBElspotBlocksTp((pTBElspotBlocksTp)exElspotBlocksTp),
		ExTable((ExTable *)exElspotBlocksTp),
				bidblock( &(exElspotBlocksTp->bidblock) )
{
}

TBExElspotBlocksTp :: ~TBExElspotBlocksTp()
{
}
#include "TBExExchPnt.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExExchPnt :: TBExExchPnt () :
TBExchPnt (),
ExTable( this, 2, 2)
{

	AddOwner(&exchpTp, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &exchTyp, 0);
	AddOwnerAlternateKey(0, &exchpTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &exchpTpName, 2, icc_f.name);
	AddOwner(&groups, 1, 2, CAN);
	AddOwnerPrimaryKey(1, &grupKey, 0);
	AddOwnerAlternateKey(1, &groupsName, 1, icc_f.name);
	AddOwnerAlternateKey(1, &groupsDescript, 2, icc_f.descript);
	AddMember(&agrmpart, 2, CAN);
	AddMemberKey(0, &exchKey, 7);
	AddMemberKey(0, &seqno, 8);
	AddMember(&meacoexc, 2, CAN);
	AddMemberKey(1, &exchKey, 0);
	AddMemberKey(1, &seqno, 1);
}

TBExExchPnt :: TBExExchPnt (const pTBExExchPnt exExchPnt) : TBExchPnt((pTBExchPnt)exExchPnt),
		ExTable((ExTable *)exExchPnt),
				exchpTpCode( exExchPnt->exchpTpCode ),
				exchpTpName( exExchPnt->exchpTpName ),
				groupsName( exExchPnt->groupsName ),
				groupsDescript( exExchPnt->groupsDescript ),
				exchpTp( &(exExchPnt->exchpTp) ),
				groups( &(exExchPnt->groups) ),
				agrmpart( &(exExchPnt->agrmpart) ),
				meacoexc( &(exExchPnt->meacoexc) )
{
}

TBExExchPnt :: ~TBExExchPnt()
{
}
#include "TBExExchtask.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExExchtask :: TBExExchtask () :
TBExchtask (),
ExTable( this, 1, 0)
{

	AddOwner(&exchmeth, 1, 1, MUST);
	AddOwnerPrimaryKey(0, &exmtKey, 0);
	AddOwnerAlternateKey(0, &exchmethName, 1, icc_f.name);
}

TBExExchtask :: TBExExchtask (const pTBExExchtask exExchtask) : TBExchtask((pTBExchtask)exExchtask),
		ExTable((ExTable *)exExchtask),
				exchmethName( exExchtask->exchmethName ),
				exchmeth( &(exExchtask->exchmeth) )
{
}

TBExExchtask :: ~TBExExchtask()
{
}
#include "TBExFngroup.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExFngroup :: TBExFngroup () :
TBFngroup (),
ExTable( this, 1, 0)
{

	AddOwner(&program, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &progKey, 0);
	AddOwnerAlternateKey(0, &programCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &programName, 2, icc_f.name);
}

TBExFngroup :: TBExFngroup (const pTBExFngroup exFngroup) : TBFngroup((pTBFngroup)exFngroup),
		ExTable((ExTable *)exFngroup),
				programCode( exFngroup->programCode ),
				programName( exFngroup->programName ),
				program( &(exFngroup->program) )
{
}

TBExFngroup :: ~TBExFngroup()
{
}
#include "TBExGenerato.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExGenerato :: TBExGenerato () :
TBGenerato (),
ExTable( this, 2, 0)
{

	AddOwner(&eleccomp, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &elecKey, 0);
	AddOwnerAlternateKey(0, &eleccompName, 1, icc_f.name);
	AddOwnerAlternateKey(0, &eleccompElcmKey, 6, icc_f.elcmKey);
	AddOwner(&hydgenun, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &hydKey, 0);
	AddOwnerAlternateKey(1, &hydgenunCode, 2, icc_f.code);
	AddOwnerAlternateKey(1, &hydgenunName, 3, icc_f.name);
}

TBExGenerato :: TBExGenerato (const pTBExGenerato exGenerato) : TBGenerato((pTBGenerato)exGenerato),
		ExTable((ExTable *)exGenerato),
				eleccompName( exGenerato->eleccompName ),
				eleccompElcmKey( exGenerato->eleccompElcmKey ),
				hydgenunCode( exGenerato->hydgenunCode ),
				hydgenunName( exGenerato->hydgenunName ),
				eleccomp( &(exGenerato->eleccomp) ),
				hydgenun( &(exGenerato->hydgenun) )
{
}

TBExGenerato :: ~TBExGenerato()
{
}
#include "TBExHfncofun.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExHfncofun :: TBExHfncofun () :
TBHfncofun (),
ExTable( this, 2, 0)
{

	AddOwner(&funcDs, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &funKey, 0);
	AddOwnerAlternateKey(0, &funcDsCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &funcDsName, 2, icc_f.name);
	AddOwner(&hydcoFn, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &fnKey, 0);
	AddOwnerAlternateKey(1, &hydcoFnFnexpr, 1, icc_f.fnexpr);
	AddOwnerAlternateKey(1, &hydcoFnDescript, 6, icc_f.descript);
}

TBExHfncofun :: TBExHfncofun (const pTBExHfncofun exHfncofun) : TBHfncofun((pTBHfncofun)exHfncofun),
		ExTable((ExTable *)exHfncofun),
				funcDsCode( exHfncofun->funcDsCode ),
				funcDsName( exHfncofun->funcDsName ),
				hydcoFnFnexpr( exHfncofun->hydcoFnFnexpr ),
				hydcoFnDescript( exHfncofun->hydcoFnDescript ),
				funcDs( &(exHfncofun->funcDs) ),
				hydcoFn( &(exHfncofun->hydcoFn) )
{
}

TBExHfncofun :: ~TBExHfncofun()
{
}
#include "TBExHydcomp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExHydcomp :: TBExHydcomp () :
TBHydcomp (),
ExTable( this, 4, 20)
{

	AddOwner(&groups, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &grupKey, 0);
	AddOwnerAlternateKey(0, &groupsName, 1, icc_f.name);
	AddOwnerAlternateKey(0, &groupsDescript, 2, icc_f.descript);
	AddOwner(&hydcoTp, 1, 2, CAN);
	AddOwnerPrimaryKey(1, &hydtKey, 0);
	AddOwnerAlternateKey(1, &hydcoTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(1, &hydcoTpName, 2, icc_f.name);
	AddOwner(&oprtunit, 1, 2, CAN);
	AddOwnerPrimaryKey(2, &opunKey, 0);
	AddOwnerAlternateKey(2, &oprtunitName, 1, icc_f.name);
	AddOwnerAlternateKey(2, &oprtunitShorname, 2, icc_f.shorname);
	AddOwner(&watercou, 1, 2, MUST);
	AddOwnerPrimaryKey(3, &watcKey, 0);
	AddOwnerAlternateKey(3, &watercouCode, 1, icc_f.code);
	AddOwnerAlternateKey(3, &watercouName, 2, icc_f.name);
	AddMember(&fieldsta, 1, CAN);
	AddMemberKey(0, &hydKey, 13);
	AddMember(&hydcoFn, 1, CAN);
	AddMemberKey(1, &hydKey, 10);
	AddMember(&oprrst, 1, CAN);
	AddMemberKey(2, &hydKey, 7);
	AddMember(&prparDs, 1, CAN);
	AddMemberKey(3, &hydKey, 8);
	AddMember(&stateDs, 1, CAN);
	AddMemberKey(4, &hydKey, 6);
	AddMember(&timcohyd, 1, CAN);
	AddMemberKey(5, &hydKey, 0);
	AddMember(&trobject, 1, CAN);
	AddMemberKey(6, &hydKey, 6);
	AddMember(&aghydDs, 1, CAN);
	AddMemberKey(7, &hydKey, 0);
	AddMember(&catmarea, 1, CAN);
	AddMemberKey(8, &hydKey, 0);
	AddMember(&hydgenun, 1, CAN);
	AddMemberKey(9, &hydKey, 0);
	AddMember(&hydplant, 1, CAN);
	AddMemberKey(10, &hydKey, 0);
	AddMember(&hyrelAc, 1, CAN);
	AddMemberKey(11, &hydKey, 4);
	AddMember(&prdeprpl, 1, CAN);
	AddMemberKey(12, &hydKey, 0);
	AddMember(&prlirepl, 1, CAN);
	AddMemberKey(13, &hydKey, 0);
	AddMember(&profile, 1, CAN);
	AddMemberKey(14, &hydKey, 0);
	AddMember(&reservoi, 1, CAN);
	AddMemberKey(15, &hydKey, 0);
	AddMember(&secpoint, 1, CAN);
	AddMemberKey(16, &hydKey, 0);
	AddMember(&wtgate, 1, CAN);
	AddMemberKey(17, &hydKey, 0);
	AddMember(&wtroujun, 1, CAN);
	AddMemberKey(18, &hydKey, 0);
	AddMember(&wtroute, 1, CAN);
	AddMemberKey(19, &hydKey, 0);
}

TBExHydcomp :: TBExHydcomp (const pTBExHydcomp exHydcomp) : TBHydcomp((pTBHydcomp)exHydcomp),
		ExTable((ExTable *)exHydcomp),
				groupsName( exHydcomp->groupsName ),
				groupsDescript( exHydcomp->groupsDescript ),
				hydcoTpCode( exHydcomp->hydcoTpCode ),
				hydcoTpName( exHydcomp->hydcoTpName ),
				oprtunitName( exHydcomp->oprtunitName ),
				oprtunitShorname( exHydcomp->oprtunitShorname ),
				watercouCode( exHydcomp->watercouCode ),
				watercouName( exHydcomp->watercouName ),
				groups( &(exHydcomp->groups) ),
				hydcoTp( &(exHydcomp->hydcoTp) ),
				oprtunit( &(exHydcomp->oprtunit) ),
				watercou( &(exHydcomp->watercou) ),
				fieldsta( &(exHydcomp->fieldsta) ),
				hydcoFn( &(exHydcomp->hydcoFn) ),
				oprrst( &(exHydcomp->oprrst) ),
				prparDs( &(exHydcomp->prparDs) ),
				stateDs( &(exHydcomp->stateDs) ),
				timcohyd( &(exHydcomp->timcohyd) ),
				trobject( &(exHydcomp->trobject) ),
				aghydDs( &(exHydcomp->aghydDs) ),
				catmarea( &(exHydcomp->catmarea) ),
				hydgenun( &(exHydcomp->hydgenun) ),
				hydplant( &(exHydcomp->hydplant) ),
				hyrelAc( &(exHydcomp->hyrelAc) ),
				prdeprpl( &(exHydcomp->prdeprpl) ),
				prlirepl( &(exHydcomp->prlirepl) ),
				profile( &(exHydcomp->profile) ),
				reservoi( &(exHydcomp->reservoi) ),
				secpoint( &(exHydcomp->secpoint) ),
				wtgate( &(exHydcomp->wtgate) ),
				wtroujun( &(exHydcomp->wtroujun) ),
				wtroute( &(exHydcomp->wtroute) )
{
}

TBExHydcomp :: ~TBExHydcomp()
{
}
#include "TBExHydcoFn.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExHydcoFn :: TBExHydcoFn () :
TBHydcoFn (),
ExTable( this, 3, 2)
{

	AddOwner(&groups, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &grupKey, 0);
	AddOwnerAlternateKey(0, &groupsName, 1, icc_f.name);
	AddOwnerAlternateKey(0, &groupsDescript, 2, icc_f.descript);
	AddOwner(&hydcomp, 1, 1, CAN);
	AddOwnerPrimaryKey(1, &hydKey, 0);
	AddOwnerAlternateKey(1, &hydcompName, 3, icc_f.name);
	AddOwner(&hydfnTp, 1, 2, MUST);
	AddOwnerPrimaryKey(2, &hydfKey, 0);
	AddOwnerAlternateKey(2, &hydfnTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(2, &hydfnTpName, 2, icc_f.name);
	AddMember(&hfncofun, 1, CAN);
	AddMemberKey(0, &fnKey, 0);
	AddMember(&hydfnVl, 1, CAN);
	AddMemberKey(1, &fnKey, 0);
}

TBExHydcoFn :: TBExHydcoFn (const pTBExHydcoFn exHydcoFn) : TBHydcoFn((pTBHydcoFn)exHydcoFn),
		ExTable((ExTable *)exHydcoFn),
				groupsName( exHydcoFn->groupsName ),
				groupsDescript( exHydcoFn->groupsDescript ),
				hydcompName( exHydcoFn->hydcompName ),
				hydfnTpCode( exHydcoFn->hydfnTpCode ),
				hydfnTpName( exHydcoFn->hydfnTpName ),
				groups( &(exHydcoFn->groups) ),
				hydcomp( &(exHydcoFn->hydcomp) ),
				hydfnTp( &(exHydcoFn->hydfnTp) ),
				hfncofun( &(exHydcoFn->hfncofun) ),
				hydfnVl( &(exHydcoFn->hydfnVl) )
{
}

TBExHydcoFn :: ~TBExHydcoFn()
{
}
#include "TBExHydfnVl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExHydfnVl :: TBExHydfnVl () :
TBHydfnVl (),
ExTable( this, 1, 0)
{

	AddOwner(&hydcoFn, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &fnKey, 0);
	AddOwnerAlternateKey(0, &hydcoFnFnexpr, 1, icc_f.fnexpr);
	AddOwnerAlternateKey(0, &hydcoFnDescript, 6, icc_f.descript);
}

TBExHydfnVl :: TBExHydfnVl (const pTBExHydfnVl exHydfnVl) : TBHydfnVl((pTBHydfnVl)exHydfnVl),
		ExTable((ExTable *)exHydfnVl),
				hydcoFnFnexpr( exHydfnVl->hydcoFnFnexpr ),
				hydcoFnDescript( exHydfnVl->hydcoFnDescript ),
				hydcoFn( &(exHydfnVl->hydcoFn) )
{
}

TBExHydfnVl :: ~TBExHydfnVl()
{
}
#include "TBExHyrelAc.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExHyrelAc :: TBExHyrelAc () :
TBHyrelAc (),
ExTable( this, 2, 2)
{

	AddOwner(&groups, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &grupKey, 0);
	AddOwnerAlternateKey(0, &groupsName, 1, icc_f.name);
	AddOwnerAlternateKey(0, &groupsDescript, 2, icc_f.descript);
	AddOwner(&hydcomp, 1, 1, MUST);
	AddOwnerPrimaryKey(1, &hydKey, 0);
	AddOwnerAlternateKey(1, &hydcompName, 3, icc_f.name);
	AddMember(&aghcohyr, 1, CAN);
	AddMemberKey(0, &relKey, 2);
	AddMember(&wtfcohyr, 1, CAN);
	AddMemberKey(1, &relKey, 1);
}

TBExHyrelAc :: TBExHyrelAc (const pTBExHyrelAc exHyrelAc) : TBHyrelAc((pTBHyrelAc)exHyrelAc),
		ExTable((ExTable *)exHyrelAc),
				groupsName( exHyrelAc->groupsName ),
				groupsDescript( exHyrelAc->groupsDescript ),
				hydcompName( exHyrelAc->hydcompName ),
				groups( &(exHyrelAc->groups) ),
				hydcomp( &(exHyrelAc->hydcomp) ),
				aghcohyr( &(exHyrelAc->aghcohyr) ),
				wtfcohyr( &(exHyrelAc->wtfcohyr) )
{
}

TBExHyrelAc :: ~TBExHyrelAc()
{
}
#include "TBExExchmeth.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExExchmeth :: TBExExchmeth () :
TBExchmeth (),
ExTable( this, 0, 2)
{

	AddMember(&exchtask, 1, CAN);
	AddMemberKey(0, &exmtKey, 0);
	AddMember(&impexpts, 1, CAN);
	AddMemberKey(1, &exmtKey, 9);
}

TBExExchmeth :: TBExExchmeth (const pTBExExchmeth exExchmeth) : TBExchmeth((pTBExchmeth)exExchmeth),
		ExTable((ExTable *)exExchmeth),
				exchtask( &(exExchmeth->exchtask) ),
				impexpts( &(exExchmeth->impexpts) )
{
}

TBExExchmeth :: ~TBExExchmeth()
{
}
#include "TBExImpexpts.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExImpexpts :: TBExImpexpts () :
TBImpexpts (),
ExTable( this, 2, 0)
{

	AddOwner(&exchmeth, 1, 1, CAN);
	AddOwnerPrimaryKey(0, &exmtKey, 0);
	AddOwnerAlternateKey(0, &exchmethName, 1, icc_f.name);
	AddOwner(&timeser, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &timsKey, 0);
	AddOwnerAlternateKey(1, &timeserTscode, 1, icc_f.tscode);
	AddOwnerAlternateKey(1, &timeserName, 2, icc_f.name);
}

TBExImpexpts :: TBExImpexpts (const pTBExImpexpts exImpexpts) : TBImpexpts((pTBImpexpts)exImpexpts),
		ExTable((ExTable *)exImpexpts),
				exchmethName( exImpexpts->exchmethName ),
				timeserTscode( exImpexpts->timeserTscode ),
				timeserName( exImpexpts->timeserName ),
				exchmeth( &(exImpexpts->exchmeth) ),
				timeser( &(exImpexpts->timeser) )
{
}

TBExImpexpts :: ~TBExImpexpts()
{
}
#include "TBExInvper.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExInvper :: TBExInvper () :
TBInvper (),
ExTable( this, 1, 0)
{

	AddOwner(&trobject, 2, 2, MUST);
	AddOwnerPrimaryKey(0, &trcode, 0);
	AddOwnerPrimaryKey(0, &opunKey, 2);
	AddOwnerAlternateKey(0, &trobjectTrcode, 0, icc_f.trcode);
	AddOwnerAlternateKey(0, &trobjectName, 1, icc_f.name);
}

TBExInvper :: TBExInvper (const pTBExInvper exInvper) : TBInvper((pTBInvper)exInvper),
		ExTable((ExTable *)exInvper),
				trobjectTrcode( exInvper->trobjectTrcode ),
				trobjectName( exInvper->trobjectName ),
				trobject( &(exInvper->trobject) )
{
}

TBExInvper :: ~TBExInvper()
{
}
#include "TBExInvpfrm.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExInvpfrm :: TBExInvpfrm () :
TBInvpfrm (),
ExTable( this, 1, 1)
{

	AddOwner(&contract, 2, 2, MUST);
	AddOwnerPrimaryKey(0, &opunKey, 0);
	AddOwnerPrimaryKey(0, &trcode, 1);
	AddOwnerAlternateKey(0, &contractTrcode, 1, icc_f.trcode);
	AddOwnerAlternateKey(0, &contractOffdate, 2, icc_f.offdate);
	AddMember(&invplin, 1, CAN);
	AddMemberKey(0, &invpKey, 0);
}

TBExInvpfrm :: TBExInvpfrm (const pTBExInvpfrm exInvpfrm) : TBInvpfrm((pTBInvpfrm)exInvpfrm),
		ExTable((ExTable *)exInvpfrm),
				contractTrcode( exInvpfrm->contractTrcode ),
				contractOffdate( exInvpfrm->contractOffdate ),
				contract( &(exInvpfrm->contract) ),
				invplin( &(exInvpfrm->invplin) )
{
}

TBExInvpfrm :: ~TBExInvpfrm()
{
}
#include "TBExInvplin.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExInvplin :: TBExInvplin () :
TBInvplin (),
ExTable( this, 2, 0)
{

	AddOwner(&invarTp, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &artKey, 0);
	AddOwnerAlternateKey(0, &invarTpCode, 2, icc_f.code);
	AddOwnerAlternateKey(0, &invarTpName, 3, icc_f.name);
	AddOwner(&invpfrm, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &invpKey, 0);
	AddOwnerAlternateKey(1, &invpfrmIdate, 1, icc_f.idate);
	AddOwnerAlternateKey(1, &invpfrmDueDate, 2, icc_f.dueDate);
}

TBExInvplin :: TBExInvplin (const pTBExInvplin exInvplin) : TBInvplin((pTBInvplin)exInvplin),
		ExTable((ExTable *)exInvplin),
				invarTpCode( exInvplin->invarTpCode ),
				invarTpName( exInvplin->invarTpName ),
				invpfrmIdate( exInvplin->invpfrmIdate ),
				invpfrmDueDate( exInvplin->invpfrmDueDate ),
				invarTp( &(exInvplin->invarTp) ),
				invpfrm( &(exInvplin->invpfrm) )
{
}

TBExInvplin :: ~TBExInvplin()
{
}
#include "TBExMadcomaa.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExMadcomaa :: TBExMadcomaa () :
TBMadcomaa (),
ExTable( this, 2, 0)
{

	AddOwner(&maradiv, 3, 1, MUST);
	AddOwnerPrimaryKey(0, &yearweek, 0);
	AddOwnerPrimaryKey(0, &mardKey, 1);
	AddOwnerPrimaryKey(0, &mardiv, 2);
	AddOwnerAlternateKey(0, &maradivMardKey, 1, icc_f.mardKey);
	AddOwner(&trareTp, 1, 2, CAN);
	AddOwnerPrimaryKey(1, &tratKey, 0);
	AddOwnerAlternateKey(1, &trareTpTratKey, 0, icc_f.tratKey);
	AddOwnerAlternateKey(1, &trareTpColour, 1, icc_f.colour);
}

TBExMadcomaa :: TBExMadcomaa (const pTBExMadcomaa exMadcomaa) : TBMadcomaa((pTBMadcomaa)exMadcomaa),
		ExTable((ExTable *)exMadcomaa),
				maradivMardKey( exMadcomaa->maradivMardKey ),
				trareTpTratKey( exMadcomaa->trareTpTratKey ),
				trareTpColour( exMadcomaa->trareTpColour ),
				maradiv( &(exMadcomaa->maradiv) ),
				trareTp( &(exMadcomaa->trareTp) )
{
}

TBExMadcomaa :: ~TBExMadcomaa()
{
}
#include "TBExMailnTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExMailnTp :: TBExMailnTp () :
TBMailnTp (),
ExTable( this, 0, 0)
{

}

TBExMailnTp :: TBExMailnTp (const pTBExMailnTp exMailnTp) : TBMailnTp((pTBMailnTp)exMailnTp),
		ExTable((ExTable *)exMailnTp){
}

TBExMailnTp :: ~TBExMailnTp()
{
}
#include "TBExMaradiv.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExMaradiv :: TBExMaradiv () :
TBMaradiv (),
ExTable( this, 2, 1)
{

	AddOwner(&mardicol, 1, 1, MUST);
	AddOwnerPrimaryKey(0, &mardiv, 0);
	AddOwnerAlternateKey(0, &mardicolColour, 1, icc_f.colour);
	AddOwner(&mardiTp, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &mardKey, 0);
	AddOwnerAlternateKey(1, &mardiTpMardKey, 0, icc_f.mardKey);
	AddOwnerAlternateKey(1, &mardiTpName, 2, icc_f.name);
	AddMember(&madcomaa, 3, CAN);
	AddMemberKey(0, &yearweek, 0);
	AddMemberKey(0, &mardKey, 1);
	AddMemberKey(0, &mardiv, 2);
}

TBExMaradiv :: TBExMaradiv (const pTBExMaradiv exMaradiv) : TBMaradiv((pTBMaradiv)exMaradiv),
		ExTable((ExTable *)exMaradiv),
				mardicolColour( exMaradiv->mardicolColour ),
				mardiTpMardKey( exMaradiv->mardiTpMardKey ),
				mardiTpName( exMaradiv->mardiTpName ),
				mardicol( &(exMaradiv->mardicol) ),
				mardiTp( &(exMaradiv->mardiTp) ),
				madcomaa( &(exMaradiv->madcomaa) )
{
}

TBExMaradiv :: ~TBExMaradiv()
{
}
#include "TBExMareaTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExMareaTp :: TBExMareaTp () :
TBMareaTp (),
ExTable( this, 0, 2)
{

	AddMember(&tmarkar, 1, CAN);
	AddMemberKey(0, &martKey, 2);
	AddMember(&trareTp, 1, CAN);
	AddMemberKey(1, &martKey, 2);
}

TBExMareaTp :: TBExMareaTp (const pTBExMareaTp exMareaTp) : TBMareaTp((pTBMareaTp)exMareaTp),
		ExTable((ExTable *)exMareaTp),
				tmarkar( &(exMareaTp->tmarkar) ),
				trareTp( &(exMareaTp->trareTp) )
{
}

TBExMareaTp :: ~TBExMareaTp()
{
}
#include "TBExMeacoexc.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExMeacoexc :: TBExMeacoexc () :
TBMeacoexc (),
ExTable( this, 3, 0)
{

	AddOwner(&exchPnt, 2, 2, MUST);
	AddOwnerPrimaryKey(0, &exchKey, 0);
	AddOwnerPrimaryKey(0, &seqno, 1);
	AddOwnerAlternateKey(0, &exchPntName, 2, icc_f.name);
	AddOwnerAlternateKey(0, &exchPntShorname, 3, icc_f.shorname);
	AddOwner(&excpfact, 1, 1, MUST);
	AddOwnerPrimaryKey(1, &efctKey, 0);
	AddOwnerAlternateKey(1, &excpfactName, 1, icc_f.name);
	AddOwner(&measPnt, 2, 2, MUST);
	AddOwnerPrimaryKey(2, &measKey, 0);
	AddOwnerPrimaryKey(2, &seqno, 1);
	AddOwnerAlternateKey(2, &measPntName, 2, icc_f.name);
	AddOwnerAlternateKey(2, &measPntDescript, 3, icc_f.descript);
}

TBExMeacoexc :: TBExMeacoexc (const pTBExMeacoexc exMeacoexc) : TBMeacoexc((pTBMeacoexc)exMeacoexc),
		ExTable((ExTable *)exMeacoexc),
				exchPntName( exMeacoexc->exchPntName ),
				exchPntShorname( exMeacoexc->exchPntShorname ),
				excpfactName( exMeacoexc->excpfactName ),
				measPntName( exMeacoexc->measPntName ),
				measPntDescript( exMeacoexc->measPntDescript ),
				exchPnt( &(exMeacoexc->exchPnt) ),
				excpfact( &(exMeacoexc->excpfact) ),
				measPnt( &(exMeacoexc->measPnt) )
{
}

TBExMeacoexc :: ~TBExMeacoexc()
{
}
#include "TBExMeasPnt.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExMeasPnt :: TBExMeasPnt () :
TBMeasPnt (),
ExTable( this, 1, 2)
{

	AddOwner(&groups, 1, 2, CAN);
	AddOwnerPrimaryKey(0, &grupKey, 0);
	AddOwnerAlternateKey(0, &groupsName, 1, icc_f.name);
	AddOwnerAlternateKey(0, &groupsDescript, 2, icc_f.descript);
	AddMember(&meacoexc, 2, CAN);
	AddMemberKey(0, &measKey, 2);
	AddMemberKey(0, &seqno, 1);
	AddMember(&measTs, 2, CAN);
	AddMemberKey(1, &measKey, 0);
	AddMemberKey(1, &seqno, 1);
}

TBExMeasPnt :: TBExMeasPnt (const pTBExMeasPnt exMeasPnt) : TBMeasPnt((pTBMeasPnt)exMeasPnt),
		ExTable((ExTable *)exMeasPnt),
				groupsName( exMeasPnt->groupsName ),
				groupsDescript( exMeasPnt->groupsDescript ),
				groups( &(exMeasPnt->groups) ),
				meacoexc( &(exMeasPnt->meacoexc) ),
				measTs( &(exMeasPnt->measTs) )
{
}

TBExMeasPnt :: ~TBExMeasPnt()
{
}
#include "TBExMeasTs.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExMeasTs :: TBExMeasTs () :
TBMeasTs (),
ExTable( this, 5, 0)
{

	AddOwner(&groups, 1, 2, CAN);
	AddOwnerPrimaryKey(0, &grupKey, 0);
	AddOwnerAlternateKey(0, &groupsName, 1, icc_f.name);
	AddOwnerAlternateKey(0, &groupsDescript, 2, icc_f.descript);
	AddOwner(&measPnt, 2, 2, MUST);
	AddOwnerPrimaryKey(1, &measKey, 0);
	AddOwnerPrimaryKey(1, &seqno, 1);
	AddOwnerAlternateKey(1, &measPntName, 2, icc_f.name);
	AddOwnerAlternateKey(1, &measPntDescript, 3, icc_f.descript);
	AddOwner(&meatsTp, 1, 1, MUST);
	AddOwnerPrimaryKey(2, &metsKey, 0);
	AddOwnerAlternateKey(2, &meatsTpName, 1, icc_f.name);
	AddOwner(&timeser, 1, 2, MUST);
	AddOwnerPrimaryKey(3, &timsKey, 0);
	AddOwnerAlternateKey(3, &timeserTscode, 1, icc_f.tscode);
	AddOwnerAlternateKey(3, &timeserName, 2, icc_f.name);
	AddOwner(&excpfact, 1, 1, MUST);
	AddOwnerPrimaryKey(4, &efctKey, 0);
	AddOwnerAlternateKey(4, &excpfactName, 1, icc_f.name);
}

TBExMeasTs :: TBExMeasTs (const pTBExMeasTs exMeasTs) : TBMeasTs((pTBMeasTs)exMeasTs),
		ExTable((ExTable *)exMeasTs),
				groupsName( exMeasTs->groupsName ),
				groupsDescript( exMeasTs->groupsDescript ),
				measPntName( exMeasTs->measPntName ),
				measPntDescript( exMeasTs->measPntDescript ),
				meatsTpName( exMeasTs->meatsTpName ),
				timeserTscode( exMeasTs->timeserTscode ),
				timeserName( exMeasTs->timeserName ),
				excpfactName( exMeasTs->excpfactName ),
				groups( &(exMeasTs->groups) ),
				measPnt( &(exMeasTs->measPnt) ),
				meatsTp( &(exMeasTs->meatsTp) ),
				timeser( &(exMeasTs->timeser) ),
				excpfact( &(exMeasTs->excpfact) )
{
}

TBExMeasTs :: ~TBExMeasTs()
{
}
#include "TBExMeavlCo.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExMeavlCo :: TBExMeavlCo () :
TBMeavlCo (),
ExTable( this, 1, 1)
{

	AddOwner(&tsapmCc, 2, 0, MUST);
	AddOwnerPrimaryKey(0, &ccmeKey, 0);
	AddOwnerPrimaryKey(0, &timsKey, 1);
	AddMember(&tsrefwei, 2, CAN);
	AddMemberKey(0, &ccmeKey, 0);
	AddMemberKey(0, &timsKey, 1);
}

TBExMeavlCo :: TBExMeavlCo (const pTBExMeavlCo exMeavlCo) : TBMeavlCo((pTBMeavlCo)exMeavlCo),
		ExTable((ExTable *)exMeavlCo),
				tsapmCc( &(exMeavlCo->tsapmCc) ),
				tsrefwei( &(exMeavlCo->tsrefwei) )
{
}

TBExMeavlCo :: ~TBExMeavlCo()
{
}
#include "TBExMimaxCt.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExMimaxCt :: TBExMimaxCt () :
TBMimaxCt (),
ExTable( this, 1, 0)
{

	AddOwner(&tsapmCc, 2, 0, MUST);
	AddOwnerPrimaryKey(0, &ccmeKey, 0);
	AddOwnerPrimaryKey(0, &timsKey, 1);
}

TBExMimaxCt :: TBExMimaxCt (const pTBExMimaxCt exMimaxCt) : TBMimaxCt((pTBMimaxCt)exMimaxCt),
		ExTable((ExTable *)exMimaxCt),
				tsapmCc( &(exMimaxCt->tsapmCc) )
{
}

TBExMimaxCt :: ~TBExMimaxCt()
{
}
#include "TBExNetarea.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExNetarea :: TBExNetarea () :
TBNetarea (),
ExTable( this, 2, 0)
{

	AddOwner(&groups, 1, 2, CAN);
	AddOwnerPrimaryKey(0, &grupKey, 0);
	AddOwnerAlternateKey(0, &groupsName, 1, icc_f.name);
	AddOwnerAlternateKey(0, &groupsDescript, 2, icc_f.descript);
	AddOwner(&netarTp, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &nettKey, 0);
	AddOwnerAlternateKey(1, &netarTpShorname, 1, icc_f.shorname);
	AddOwnerAlternateKey(1, &netarTpName, 2, icc_f.name);
}

TBExNetarea :: TBExNetarea (const pTBExNetarea exNetarea) : TBNetarea((pTBNetarea)exNetarea),
		ExTable((ExTable *)exNetarea),
				groupsName( exNetarea->groupsName ),
				groupsDescript( exNetarea->groupsDescript ),
				netarTpShorname( exNetarea->netarTpShorname ),
				netarTpName( exNetarea->netarTpName ),
				groups( &(exNetarea->groups) ),
				netarTp( &(exNetarea->netarTp) )
{
}

TBExNetarea :: ~TBExNetarea()
{
}
#include "TBExOpbidare.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExOpbidare :: TBExOpbidare () :
TBOpbidare (),
ExTable( this, 2, 0)
{

	AddOwner(&oprtunit, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &opunKey, 0);
	AddOwnerAlternateKey(0, &oprtunitName, 1, icc_f.name);
	AddOwnerAlternateKey(0, &oprtunitShorname, 2, icc_f.shorname);
	AddOwner(&sign, 2, 2, CAN);
	AddOwnerPrimaryKey(1, &signcode, 0);
	AddOwnerPrimaryKey(1, &opunKey, 6);
	AddOwnerAlternateKey(1, &signSigncode, 0, icc_f.signcode);
	AddOwnerAlternateKey(1, &signName, 1, icc_f.name);
}

TBExOpbidare :: TBExOpbidare (const pTBExOpbidare exOpbidare) : TBOpbidare((pTBOpbidare)exOpbidare),
		ExTable((ExTable *)exOpbidare),
				oprtunitName( exOpbidare->oprtunitName ),
				oprtunitShorname( exOpbidare->oprtunitShorname ),
				signSigncode( exOpbidare->signSigncode ),
				signName( exOpbidare->signName ),
				oprtunit( &(exOpbidare->oprtunit) ),
				sign( &(exOpbidare->sign) )
{
}

TBExOpbidare :: ~TBExOpbidare()
{
}
#include "TBExOprrsTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExOprrsTp :: TBExOprrsTp () :
TBOprrsTp (),
ExTable( this, 0, 1)
{

	AddMember(&oprrst, 1, CAN);
	AddMemberKey(0, &oprtKey, 8);
}

TBExOprrsTp :: TBExOprrsTp (const pTBExOprrsTp exOprrsTp) : TBOprrsTp((pTBOprrsTp)exOprrsTp),
		ExTable((ExTable *)exOprrsTp),
				oprrst( &(exOprrsTp->oprrst) )
{
}

TBExOprrsTp :: ~TBExOprrsTp()
{
}
#include "TBExOprrst.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExOprrst::TBExOprrst() :
	TBOprrst(),
	ExTable(this, 4, 2)
{

	AddOwner(&groups, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &grupKey, 0);
	AddOwnerAlternateKey(0, &groupsName, 1, icc_f.name);
	AddOwnerAlternateKey(0, &groupsDescript, 2, icc_f.descript);
	AddOwner(&hydcomp, 1, 1, CAN);
	AddOwnerPrimaryKey(1, &hydKey, 0);
	AddOwnerAlternateKey(1, &hydcompName, 3, icc_f.name);
	AddOwner(&oprrsTp, 1, 2, MUST);
	AddOwnerPrimaryKey(2, &oprtKey, 0);
	AddOwnerAlternateKey(2, &oprrsTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(2, &oprrsTpName, 2, icc_f.name);
	AddOwner(&unmeaTp, 1, 2, MUST);
	AddOwnerPrimaryKey(3, &unmeKey, 0);
	AddOwnerAlternateKey(3, &unmeaTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(3, &unmeaTpName, 2, icc_f.name);
	AddMember(&oprcofun, 1, CAN);
	AddMemberKey(0, &oprsKey, 1);
	AddMember(&oprrsVl, 1, CAN);
	AddMemberKey(1, &oprsKey, 0);
}

TBExOprrst::TBExOprrst(const pTBExOprrst exOprrst) : TBOprrst((pTBOprrst)exOprrst),
ExTable((ExTable *)exOprrst),
groupsName(exOprrst->groupsName),
groupsDescript(exOprrst->groupsDescript),
hydcompName(exOprrst->hydcompName),
oprrsTpCode(exOprrst->oprrsTpCode),
oprrsTpName(exOprrst->oprrsTpName),
unmeaTpCode(exOprrst->unmeaTpCode),
unmeaTpName(exOprrst->unmeaTpName),
groups(&(exOprrst->groups)),
hydcomp(&(exOprrst->hydcomp)),
oprrsTp(&(exOprrst->oprrsTp)),
unmeaTp(&(exOprrst->unmeaTp)),
oprcofun(&(exOprrst->oprcofun)),
oprrsVl(&(exOprrst->oprrsVl))
{
}

TBExOprrst :: ~TBExOprrst()
{
}
#include "TBExOprrsVl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExOprrsVl :: TBExOprrsVl () :
TBOprrsVl (),
ExTable( this, 1, 0)
{

	AddOwner(&oprrst, 1, 1, MUST);
	AddOwnerPrimaryKey(0, &oprsKey, 0);
	AddOwnerAlternateKey(0, &oprrstDescript, 4, icc_f.descript);
}

TBExOprrsVl :: TBExOprrsVl (const pTBExOprrsVl exOprrsVl) : TBOprrsVl((pTBOprrsVl)exOprrsVl),
		ExTable((ExTable *)exOprrsVl),
				oprrstDescript( exOprrsVl->oprrstDescript ),
				oprrst( &(exOprrsVl->oprrst) )
{
}

TBExOprrsVl :: ~TBExOprrsVl()
{
}
#include "TBExOprtunit.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExOprtunit :: TBExOprtunit () :
TBOprtunit (),
ExTable( this, 2, 12)
{

	AddOwner(&country, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &countryKey, 0);
	AddOwnerAlternateKey(0, &countryCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &countryName, 2, icc_f.name);
	AddOwner(&groups, 1, 2, CAN);
	AddOwnerPrimaryKey(1, &grupKey, 0);
	AddOwnerAlternateKey(1, &groupsName, 1, icc_f.name);
	AddOwnerAlternateKey(1, &groupsDescript, 2, icc_f.descript);
	AddMember(&fistcent, 1, CAN);
	AddMemberKey(0, &opunKey, 17);
	AddMember(&hydcomp, 1, CAN);
	AddMemberKey(1, &opunKey, 9);
	AddMember(&agrmpart, 1, CAN);
	AddMemberKey(2, &opunKey, 0);
	AddMember(&accntrec, 1, CAN);
	AddMemberKey(3, &opunKey, 0);
	AddMember(&accntrp, 1, CAN);
	AddMemberKey(4, &opunKey, 0);
	AddMember(&bidrpfrm, 1, CAN);
	AddMemberKey(5, &opunKey, 0);
	AddMember(&cntPers, 1, CAN);
	AddMemberKey(6, &opunKey, 0);
	AddMember(&recadr, 1, CAN);
	AddMemberKey(7, &opunKey, 0);
	AddMember(&sign, 1, CAN);
	AddMemberKey(8, &opunKey, 6);
	AddMember(&trobject, 1, CAN);
	AddMemberKey(9, &opunKey, 2);
	AddMember(&opbidare, 1, CAN);
	AddMemberKey(10, &opunKey, 0);
	AddMember(&debitsys, 1, CAN);
	AddMemberKey(11, &opunKey, 0);
}

TBExOprtunit :: TBExOprtunit (const pTBExOprtunit exOprtunit) : TBOprtunit((pTBOprtunit)exOprtunit),
		ExTable((ExTable *)exOprtunit),
				countryCode( exOprtunit->countryCode ),
				countryName( exOprtunit->countryName ),
				groupsName( exOprtunit->groupsName ),
				groupsDescript( exOprtunit->groupsDescript ),
				country( &(exOprtunit->country) ),
				groups( &(exOprtunit->groups) ),
				fistcent( &(exOprtunit->fistcent) ),
				hydcomp( &(exOprtunit->hydcomp) ),
				agrmpart( &(exOprtunit->agrmpart) ),
				accntrec( &(exOprtunit->accntrec) ),
				accntrp( &(exOprtunit->accntrp) ),
				bidrpfrm( &(exOprtunit->bidrpfrm) ),
				cntPers( &(exOprtunit->cntPers) ),
				recadr( &(exOprtunit->recadr) ),
				sign( &(exOprtunit->sign) ),
				trobject( &(exOprtunit->trobject) ),
				opbidare( &(exOprtunit->opbidare) ),
				debitsys( &(exOprtunit->debitsys) )
{
}

TBExOprtunit :: ~TBExOprtunit()
{
}
#include "TBExPartempl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExPartempl :: TBExPartempl () :
TBPartempl (),
ExTable( this, 1, 2)
{

	AddOwner(&program, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &progKey, 0);
	AddOwnerAlternateKey(0, &programCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &programName, 2, icc_f.name);
	AddMember(&parVl, 1, CAN);
	AddMemberKey(0, &partKey, 9);
	AddMember(&prparMx, 1, CAN);
	AddMemberKey(1, &partKey, 5);
}

TBExPartempl :: TBExPartempl (const pTBExPartempl exPartempl) : TBPartempl((pTBPartempl)exPartempl),
		ExTable((ExTable *)exPartempl),
				programCode( exPartempl->programCode ),
				programName( exPartempl->programName ),
				program( &(exPartempl->program) ),
				parVl( &(exPartempl->parVl) ),
				prparMx( &(exPartempl->prparMx) )
{
}

TBExPartempl :: ~TBExPartempl()
{
}
#include "TBExPartner.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExPartner :: TBExPartner () :
TBPartner (),
ExTable( this, 0, 1)
{

	AddMember(&contact, 2, CAN);
	AddMemberKey(0, &opunKey, 0);
	AddMemberKey(0, &trcode, 1);
}

TBExPartner :: TBExPartner (const pTBExPartner exPartner) : TBPartner((pTBPartner)exPartner),
		ExTable((ExTable *)exPartner),
				contact( &(exPartner->contact) )
{
}

TBExPartner :: ~TBExPartner()
{
}
#include "TBExParVl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExParVl :: TBExParVl () :
TBParVl (),
ExTable( this, 2, 0)
{

	AddOwner(&partempl, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &partKey, 0);
	AddOwnerAlternateKey(0, &partemplCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &partemplName, 2, icc_f.name);
	AddOwner(&prparDs, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &pardKey, 0);
	AddOwnerAlternateKey(1, &prparDsCode, 1, icc_f.code);
	AddOwnerAlternateKey(1, &prparDsName, 2, icc_f.name);
}

TBExParVl :: TBExParVl (const pTBExParVl exParVl) : TBParVl((pTBParVl)exParVl),
		ExTable((ExTable *)exParVl),
				partemplCode( exParVl->partemplCode ),
				partemplName( exParVl->partemplName ),
				prparDsCode( exParVl->prparDsCode ),
				prparDsName( exParVl->prparDsName ),
				partempl( &(exParVl->partempl) ),
				prparDs( &(exParVl->prparDs) )
{
}

TBExParVl :: ~TBExParVl()
{
}
#include "TBExPriceint.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExPriceint :: TBExPriceint () :
TBPriceint (),
ExTable( this, 1, 0)
{

	AddOwner(&unmeaTp, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &unmeKey, 0);
	AddOwnerAlternateKey(0, &unmeaTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &unmeaTpName, 2, icc_f.name);
}

TBExPriceint :: TBExPriceint (const pTBExPriceint exPriceint) : TBPriceint((pTBPriceint)exPriceint),
		ExTable((ExTable *)exPriceint),
				unmeaTpCode( exPriceint->unmeaTpCode ),
				unmeaTpName( exPriceint->unmeaTpName ),
				unmeaTp( &(exPriceint->unmeaTp) )
{
}

TBExPriceint :: ~TBExPriceint()
{
}
#include "TBExProcotro.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExProcotro :: TBExProcotro () :
TBProcotro (),
ExTable( this, 2, 0)
{

	AddOwner(&protoTp, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &protKey, 0);
	AddOwnerAlternateKey(0, &protoTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &protoTpName, 2, icc_f.name);
	AddOwner(&trobject, 2, 2, MUST);
	AddOwnerPrimaryKey(1, &trcode, 0);
	AddOwnerPrimaryKey(1, &opunKey, 2);
	AddOwnerAlternateKey(1, &trobjectTrcode, 0, icc_f.trcode);
	AddOwnerAlternateKey(1, &trobjectName, 1, icc_f.name);
}

TBExProcotro :: TBExProcotro (const pTBExProcotro exProcotro) : TBProcotro((pTBProcotro)exProcotro),
		ExTable((ExTable *)exProcotro),
				protoTpCode( exProcotro->protoTpCode ),
				protoTpName( exProcotro->protoTpName ),
				trobjectTrcode( exProcotro->trobjectTrcode ),
				trobjectName( exProcotro->trobjectName ),
				protoTp( &(exProcotro->protoTp) ),
				trobject( &(exProcotro->trobject) )
{
}

TBExProcotro :: ~TBExProcotro()
{
}
#include "TBExProgram.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExProgram :: TBExProgram () :
TBProgram (),
ExTable( this, 0, 12)
{

	AddMember(&aghydDs, 1, CAN);
	AddMemberKey(0, &progKey, 7);
	AddMember(&fngroup, 1, CAN);
	AddMemberKey(1, &progKey, 1);
	AddMember(&funcDs, 1, CAN);
	AddMemberKey(2, &progKey, 7);
	AddMember(&macroDs, 1, CAN);
	AddMemberKey(3, &progKey, 4);
	AddMember(&partempl, 1, CAN);
	AddMemberKey(4, &progKey, 4);
	AddMember(&prparDs, 1, CAN);
	AddMemberKey(5, &progKey, 9);
	AddMember(&prrunDs, 1, CAN);
	AddMemberKey(6, &progKey, 13);
	AddMember(&spparDs, 1, CAN);
	AddMemberKey(7, &progKey, 7);
	AddMember(&statempl, 1, CAN);
	AddMemberKey(8, &progKey, 6);
	AddMember(&stateDs, 1, CAN);
	AddMemberKey(9, &progKey, 7);
	AddMember(&timseDs, 1, CAN);
	AddMemberKey(10, &progKey, 7);
	AddMember(&userprog, 1, CAN);
	AddMemberKey(11, &progKey, 1);
}

TBExProgram :: TBExProgram (const pTBExProgram exProgram) : TBProgram((pTBProgram)exProgram),
		ExTable((ExTable *)exProgram),
				aghydDs( &(exProgram->aghydDs) ),
				fngroup( &(exProgram->fngroup) ),
				funcDs( &(exProgram->funcDs) ),
				macroDs( &(exProgram->macroDs) ),
				partempl( &(exProgram->partempl) ),
				prparDs( &(exProgram->prparDs) ),
				prrunDs( &(exProgram->prrunDs) ),
				spparDs( &(exProgram->spparDs) ),
				statempl( &(exProgram->statempl) ),
				stateDs( &(exProgram->stateDs) ),
				timseDs( &(exProgram->timseDs) ),
				userprog( &(exProgram->userprog) )
{
}

TBExProgram :: ~TBExProgram()
{
}
#include "TBExPrparDs.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExPrparDs :: TBExPrparDs () :
TBPrparDs (),
ExTable( this, 2, 3)
{

	AddOwner(&hydcomp, 1, 1, CAN);
	AddOwnerPrimaryKey(0, &hydKey, 0);
	AddOwnerAlternateKey(0, &hydcompName, 3, icc_f.name);
	AddOwner(&program, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &progKey, 0);
	AddOwnerAlternateKey(1, &programCode, 1, icc_f.code);
	AddOwnerAlternateKey(1, &programName, 2, icc_f.name);
	AddMember(&prrunDs, 1, CAN);
	AddMemberKey(0, &pardKey, 12);
	AddMember(&parVl, 1, CAN);
	AddMemberKey(1, &pardKey, 8);
	AddMember(&prparMx, 1, CAN);
	AddMemberKey(2, &pardKey, 4);
}

TBExPrparDs :: TBExPrparDs (const pTBExPrparDs exPrparDs) : TBPrparDs((pTBPrparDs)exPrparDs),
		ExTable((ExTable *)exPrparDs),
				hydcompName( exPrparDs->hydcompName ),
				programCode( exPrparDs->programCode ),
				programName( exPrparDs->programName ),
				hydcomp( &(exPrparDs->hydcomp) ),
				program( &(exPrparDs->program) ),
				prrunDs( &(exPrparDs->prrunDs) ),
				parVl( &(exPrparDs->parVl) ),
				prparMx( &(exPrparDs->prparMx) )
{
}

TBExPrparDs :: ~TBExPrparDs()
{
}
#include "TBExPrrunDs.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExPrrunDs :: TBExPrrunDs () :
TBPrrunDs (),
ExTable( this, 9, 3)
{

	AddOwner(&funcDs, 1, 2, CAN);
	AddOwnerPrimaryKey(0, &funKey, 0);
	AddOwnerAlternateKey(0, &funcDsCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &funcDsName, 2, icc_f.name);
	AddOwner(&groups, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &grupKey, 0);
	AddOwnerAlternateKey(1, &groupsName, 1, icc_f.name);
	AddOwnerAlternateKey(1, &groupsDescript, 2, icc_f.descript);
	AddOwner(&ldfarea, 1, 2, CAN);
	AddOwnerPrimaryKey(2, &ldfKey, 0);
	AddOwnerAlternateKey(2, &ldfareaCode, 1, icc_f.code);
	AddOwnerAlternateKey(2, &ldfareaName, 2, icc_f.name);
	AddOwner(&program, 1, 2, MUST);
	AddOwnerPrimaryKey(3, &progKey, 0);
	AddOwnerAlternateKey(3, &programCode, 1, icc_f.code);
	AddOwnerAlternateKey(3, &programName, 2, icc_f.name);
	AddOwner(&prparDs, 1, 2, CAN);
	AddOwnerPrimaryKey(4, &pardKey, 0);
	AddOwnerAlternateKey(4, &prparDsCode, 1, icc_f.code);
	AddOwnerAlternateKey(4, &prparDsName, 2, icc_f.name);
	AddOwner(&simuler, 1, 2, CAN);
	AddOwnerPrimaryKey(5, &simKey, 0);
	AddOwnerAlternateKey(5, &simulerCode, 1, icc_f.code);
	AddOwnerAlternateKey(5, &simulerDescript, 2, icc_f.descript);
	AddOwner(&spparDs, 1, 2, CAN);
	AddOwnerPrimaryKey(6, &painKey, 0);
	AddOwnerAlternateKey(6, &spparDsCode, 1, icc_f.code);
	AddOwnerAlternateKey(6, &spparDsName, 2, icc_f.name);
	AddOwner(&timseDs, 1, 2, CAN);
	AddOwnerPrimaryKey(7, &tdsKey, 0);
	AddOwnerAlternateKey(7, &timseDsCode, 1, icc_f.code);
	AddOwnerAlternateKey(7, &timseDsName, 2, icc_f.name);
	AddOwner(&watercou, 1, 2, CAN);
	AddOwnerPrimaryKey(8, &watcKey, 0);
	AddOwnerAlternateKey(8, &watercouCode, 1, icc_f.code);
	AddOwnerAlternateKey(8, &watercouName, 2, icc_f.name);
	AddMember(&stacoprr, 1, CAN);
	AddMemberKey(0, &prdKey, 0);
	AddMember(&timeser, 1, CAN);
	AddMemberKey(1, &prdKey, 23);
	AddMember(&aghcoprr, 1, CAN);
	AddMemberKey(2, &prdKey, 0);
}

TBExPrrunDs :: TBExPrrunDs (const pTBExPrrunDs exPrrunDs) : TBPrrunDs((pTBPrrunDs)exPrrunDs),
		ExTable((ExTable *)exPrrunDs),
				funcDsCode( exPrrunDs->funcDsCode ),
				funcDsName( exPrrunDs->funcDsName ),
				groupsName( exPrrunDs->groupsName ),
				groupsDescript( exPrrunDs->groupsDescript ),
				ldfareaCode( exPrrunDs->ldfareaCode ),
				ldfareaName( exPrrunDs->ldfareaName ),
				programCode( exPrrunDs->programCode ),
				programName( exPrrunDs->programName ),
				prparDsCode( exPrrunDs->prparDsCode ),
				prparDsName( exPrrunDs->prparDsName ),
				simulerCode( exPrrunDs->simulerCode ),
				simulerDescript( exPrrunDs->simulerDescript ),
				spparDsCode( exPrrunDs->spparDsCode ),
				spparDsName( exPrrunDs->spparDsName ),
				timseDsCode( exPrrunDs->timseDsCode ),
				timseDsName( exPrrunDs->timseDsName ),
				watercouCode( exPrrunDs->watercouCode ),
				watercouName( exPrrunDs->watercouName ),
				funcDs( &(exPrrunDs->funcDs) ),
				groups( &(exPrrunDs->groups) ),
				ldfarea( &(exPrrunDs->ldfarea) ),
				program( &(exPrrunDs->program) ),
				prparDs( &(exPrrunDs->prparDs) ),
				simuler( &(exPrrunDs->simuler) ),
				spparDs( &(exPrrunDs->spparDs) ),
				timseDs( &(exPrrunDs->timseDs) ),
				watercou( &(exPrrunDs->watercou) ),
				stacoprr( &(exPrrunDs->stacoprr) ),
				timeser( &(exPrrunDs->timeser) ),
				aghcoprr( &(exPrrunDs->aghcoprr) )
{
}

TBExPrrunDs :: ~TBExPrrunDs()
{
}
#include "TBExRecadr.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExRecadr :: TBExRecadr () :
TBRecadr (),
ExTable( this, 2, 0)
{

	AddOwner(&oprtunit, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &opunKey, 0);
	AddOwnerAlternateKey(0, &oprtunitName, 1, icc_f.name);
	AddOwnerAlternateKey(0, &oprtunitShorname, 2, icc_f.shorname);
	AddOwner(&protoTp, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &protKey, 0);
	AddOwnerAlternateKey(1, &protoTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(1, &protoTpName, 2, icc_f.name);
}

TBExRecadr :: TBExRecadr (const pTBExRecadr exRecadr) : TBRecadr((pTBRecadr)exRecadr),
		ExTable((ExTable *)exRecadr),
				oprtunitName( exRecadr->oprtunitName ),
				oprtunitShorname( exRecadr->oprtunitShorname ),
				protoTpCode( exRecadr->protoTpCode ),
				protoTpName( exRecadr->protoTpName ),
				oprtunit( &(exRecadr->oprtunit) ),
				protoTp( &(exRecadr->protoTp) )
{
}

TBExRecadr :: ~TBExRecadr()
{
}
#include "TBExRefTrobject.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExRefTrobject :: TBExRefTrobject () :
TBRefTrobject (),
ExTable( this, 1, 0)
{

	AddOwner(&trobject, 2, 2, MUST);
	AddOwnerPrimaryKey(0, &trcode, 0);
	AddOwnerPrimaryKey(0, &opunKey, 2);
	AddOwnerAlternateKey(0, &trobjectTrcode, 0, icc_f.trcode);
	AddOwnerAlternateKey(0, &trobjectName, 1, icc_f.name);
}

TBExRefTrobject :: TBExRefTrobject (const pTBExRefTrobject exRefTrobject) : TBRefTrobject((pTBRefTrobject)exRefTrobject),
		ExTable((ExTable *)exRefTrobject),
				trobjectTrcode( exRefTrobject->trobjectTrcode ),
				trobjectName( exRefTrobject->trobjectName ),
				trobject( &(exRefTrobject->trobject) )
{
}

TBExRefTrobject :: ~TBExRefTrobject()
{
}
#include "TBExReservoi.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExReservoi :: TBExReservoi () :
TBReservoi (),
ExTable( this, 2, 0)
{

	AddOwner(&hydcomp, 1, 1, MUST);
	AddOwnerPrimaryKey(0, &hydKey, 0);
	AddOwnerAlternateKey(0, &hydcompName, 3, icc_f.name);
	AddOwner(&resvoTp, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &restKey, 0);
	AddOwnerAlternateKey(1, &resvoTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(1, &resvoTpName, 2, icc_f.name);
}

TBExReservoi :: TBExReservoi (const pTBExReservoi exReservoi) : TBReservoi((pTBReservoi)exReservoi),
		ExTable((ExTable *)exReservoi),
				hydcompName( exReservoi->hydcompName ),
				resvoTpCode( exReservoi->resvoTpCode ),
				resvoTpName( exReservoi->resvoTpName ),
				hydcomp( &(exReservoi->hydcomp) ),
				resvoTp( &(exReservoi->resvoTp) )
{
}

TBExReservoi :: ~TBExReservoi()
{
}
#include "TBExSign.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExSign :: TBExSign () :
TBSign (),
ExTable( this, 1, 4)
{

	AddOwner(&oprtunit, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &opunKey, 0);
	AddOwnerAlternateKey(0, &oprtunitName, 1, icc_f.name);
	AddOwnerAlternateKey(0, &oprtunitShorname, 2, icc_f.shorname);
	AddMember(&bid24frm, 2, CAN);
	AddMemberKey(0, &signcode, 9);
	AddMemberKey(0, &opunKey, 0);
	AddMember(&bidrpfrm, 2, CAN);
	AddMemberKey(1, &signcode, 7);
	AddMemberKey(1, &opunKey, 0);
	AddMember(&opbidare, 2, CAN);
	AddMemberKey(2, &signcode, 3);
	AddMemberKey(2, &opunKey, 0);
	AddMember(&bidblock, 2, CAN);
	AddMemberKey(3, &signcode, 9);
	AddMemberKey(3, &opunKey, 0);
}

TBExSign :: TBExSign (const pTBExSign exSign) : TBSign((pTBSign)exSign),
		ExTable((ExTable *)exSign),
				oprtunitName( exSign->oprtunitName ),
				oprtunitShorname( exSign->oprtunitShorname ),
				oprtunit( &(exSign->oprtunit) ),
				bid24frm( &(exSign->bid24frm) ),
				bidrpfrm( &(exSign->bidrpfrm) ),
				opbidare( &(exSign->opbidare) ),
				bidblock( &(exSign->bidblock) )
{
}

TBExSign :: ~TBExSign()
{
}
#include "TBExTarifdef.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTarifdef :: TBExTarifdef () :
TBTarifdef (),
ExTable( this, 0, 2)
{

	AddMember(&agrmpart, 2, CAN);
	AddMemberKey(0, &tarfKey, 10);
	AddMemberKey(0, &seqno, 8);
	AddMember(&delivagm, 2, CAN);
	AddMemberKey(1, &tarfKey, 9);
	AddMemberKey(1, &seqno, 10);
}

TBExTarifdef :: TBExTarifdef (const pTBExTarifdef exTarifdef) : TBTarifdef((pTBTarifdef)exTarifdef),
		ExTable((ExTable *)exTarifdef),
				agrmpart( &(exTarifdef->agrmpart) ),
				delivagm( &(exTarifdef->delivagm) )
{
}

TBExTarifdef :: ~TBExTarifdef()
{
}
#include "TBExTimcohyd.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTimcohyd :: TBExTimcohyd () :
TBTimcohyd (),
ExTable( this, 3, 0)
{

	AddOwner(&hydcomp, 1, 1, CAN);
	AddOwnerPrimaryKey(0, &hydKey, 0);
	AddOwnerAlternateKey(0, &hydcompName, 3, icc_f.name);
	AddOwner(&timcoTp, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &ticoKey, 0);
	AddOwnerAlternateKey(1, &timcoTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(1, &timcoTpName, 2, icc_f.name);
	AddOwner(&timeser, 1, 2, MUST);
	AddOwnerPrimaryKey(2, &timsKey, 0);
	AddOwnerAlternateKey(2, &timeserTscode, 1, icc_f.tscode);
	AddOwnerAlternateKey(2, &timeserName, 2, icc_f.name);
}

TBExTimcohyd :: TBExTimcohyd (const pTBExTimcohyd exTimcohyd) : TBTimcohyd((pTBTimcohyd)exTimcohyd),
		ExTable((ExTable *)exTimcohyd),
				hydcompName( exTimcohyd->hydcompName ),
				timcoTpCode( exTimcohyd->timcoTpCode ),
				timcoTpName( exTimcohyd->timcoTpName ),
				timeserTscode( exTimcohyd->timeserTscode ),
				timeserName( exTimcohyd->timeserName ),
				hydcomp( &(exTimcohyd->hydcomp) ),
				timcoTp( &(exTimcohyd->timcoTp) ),
				timeser( &(exTimcohyd->timeser) )
{
}

TBExTimcohyd :: ~TBExTimcohyd()
{
}
#include "TBExTimcovcg.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTimcovcg :: TBExTimcovcg () :
TBTimcovcg (),
ExTable( this, 1, 0)
{

	AddOwner(&timeser, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &timsKey, 0);
	AddOwnerAlternateKey(0, &timeserTscode, 1, icc_f.tscode);
	AddOwnerAlternateKey(0, &timeserName, 2, icc_f.name);
}

TBExTimcovcg :: TBExTimcovcg (const pTBExTimcovcg exTimcovcg) : TBTimcovcg((pTBTimcovcg)exTimcovcg),
		ExTable((ExTable *)exTimcovcg),
				timeserTscode( exTimcovcg->timeserTscode ),
				timeserName( exTimcovcg->timeserName ),
				timeser( &(exTimcovcg->timeser) )
{
}

TBExTimcovcg :: ~TBExTimcovcg()
{
}
#include "TBExTimeser.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTimeser :: TBExTimeser () :
TBTimeser (),
ExTable( this, 9, 14)
{

	AddOwner(&groups, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &grupKey, 0);
	AddOwnerAlternateKey(0, &groupsName, 1, icc_f.name);
	AddOwnerAlternateKey(0, &groupsDescript, 2, icc_f.descript);
	AddOwner(&prrunDs, 1, 2, CAN);
	AddOwnerPrimaryKey(1, &prdKey, 0);
	AddOwnerAlternateKey(1, &prrunDsCode, 1, icc_f.code);
	AddOwnerAlternateKey(1, &prrunDsName, 2, icc_f.name);
	AddOwner(&tscaTp, 1, 2, CAN);
	AddOwnerPrimaryKey(2, &tscaKey, 0);
	AddOwnerAlternateKey(2, &tscaTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(2, &tscaTpName, 2, icc_f.name);
	AddOwner(&tseriTp, 1, 2, MUST);
	AddOwnerPrimaryKey(3, &tserKey, 0);
	AddOwnerAlternateKey(3, &tseriTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(3, &tseriTpName, 2, icc_f.name);
	AddOwner(&tsintTp, 1, 2, MUST);
	AddOwnerPrimaryKey(4, &tsinKey, 0);
	AddOwnerAlternateKey(4, &tsintTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(4, &tsintTpName, 2, icc_f.name);
	AddOwner(&tsorgTp, 1, 2, MUST);
	AddOwnerPrimaryKey(5, &tsorKey, 0);
	AddOwnerAlternateKey(5, &tsorgTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(5, &tsorgTpName, 2, icc_f.name);
	AddOwner(&tsproduc, 1, 2, CAN);
	AddOwnerPrimaryKey(6, &tsprKey, 0);
	AddOwnerAlternateKey(6, &tsproducCode, 1, icc_f.code);
	AddOwnerAlternateKey(6, &tsproducName, 2, icc_f.name);
	AddOwner(&tsquTp, 1, 2, CAN);
	AddOwnerPrimaryKey(7, &tsquKey, 0);
	AddOwnerAlternateKey(7, &tsquTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(7, &tsquTpName, 2, icc_f.name);
	AddOwner(&unmeaTp, 1, 2, MUST);
	AddOwnerPrimaryKey(8, &unmeKey, 0);
	AddOwnerAlternateKey(8, &unmeaTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(8, &unmeaTpName, 2, icc_f.name);
	AddMember(&trobject, 1, CAN);
	AddMemberKey(0, &timsKey, 7);
	AddMember(&tsstat, 1, CAN);
	AddMemberKey(1, &timsKey, 0);
	AddMember(&impexpts, 1, CAN);
	AddMemberKey(2, &timsKey, 1);
	AddMember(&measTs, 1, CAN);
	AddMemberKey(3, &timsKey, 2);
	AddMember(&mtrcotim, 1, CAN);
	AddMemberKey(4, &timsKey, 0);
	AddMember(&resulVl, 1, CAN);
	AddMemberKey(5, &timsKey, 4);
	AddMember(&sensor, 1, CAN);
	AddMemberKey(6, &timsKey, 14);
	AddMember(&tdscotim, 1, CAN);
	AddMemberKey(7, &timsKey, 1);
	AddMember(&timcohyd, 1, CAN);
	AddMemberKey(8, &timsKey, 1);
	AddMember(&timcovcg, 1, CAN);
	AddMemberKey(9, &timsKey, 0);
	AddMember(&meterVl, 1, CAN);
	AddMemberKey(10, &timsKey, 0);
	AddMember(&tscoupl, 1, CAN);
	AddMemberKey(11, &timsKey, 3);
	AddMember(&tsfunVl, 1, CAN);
	AddMemberKey(12, &timsKey, 5);
	AddMember(&tsorgVl, 1, CAN);
	AddMemberKey(13, &timsKey, 0);
}

TBExTimeser :: TBExTimeser (const pTBExTimeser exTimeser) : TBTimeser((pTBTimeser)exTimeser),
		ExTable((ExTable *)exTimeser),
				groupsName( exTimeser->groupsName ),
				groupsDescript( exTimeser->groupsDescript ),
				prrunDsCode( exTimeser->prrunDsCode ),
				prrunDsName( exTimeser->prrunDsName ),
				tscaTpCode( exTimeser->tscaTpCode ),
				tscaTpName( exTimeser->tscaTpName ),
				tseriTpCode( exTimeser->tseriTpCode ),
				tseriTpName( exTimeser->tseriTpName ),
				tsintTpCode( exTimeser->tsintTpCode ),
				tsintTpName( exTimeser->tsintTpName ),
				tsorgTpCode( exTimeser->tsorgTpCode ),
				tsorgTpName( exTimeser->tsorgTpName ),
				tsproducCode( exTimeser->tsproducCode ),
				tsproducName( exTimeser->tsproducName ),
				tsquTpCode( exTimeser->tsquTpCode ),
				tsquTpName( exTimeser->tsquTpName ),
				unmeaTpCode( exTimeser->unmeaTpCode ),
				unmeaTpName( exTimeser->unmeaTpName ),
				groups( &(exTimeser->groups) ),
				prrunDs( &(exTimeser->prrunDs) ),
				tscaTp( &(exTimeser->tscaTp) ),
				tseriTp( &(exTimeser->tseriTp) ),
				tsintTp( &(exTimeser->tsintTp) ),
				tsorgTp( &(exTimeser->tsorgTp) ),
				tsproduc( &(exTimeser->tsproduc) ),
				tsquTp( &(exTimeser->tsquTp) ),
				unmeaTp( &(exTimeser->unmeaTp) ),
				trobject( &(exTimeser->trobject) ),
				tsstat( &(exTimeser->tsstat) ),
				impexpts( &(exTimeser->impexpts) ),
				measTs( &(exTimeser->measTs) ),
				mtrcotim( &(exTimeser->mtrcotim) ),
				resulVl( &(exTimeser->resulVl) ),
				sensor( &(exTimeser->sensor) ),
				tdscotim( &(exTimeser->tdscotim) ),
				timcohyd( &(exTimeser->timcohyd) ),
				timcovcg( &(exTimeser->timcovcg) ),
				meterVl( &(exTimeser->meterVl) ),
				tscoupl( &(exTimeser->tscoupl) ),
				tsfunVl( &(exTimeser->tsfunVl) ),
				tsorgVl( &(exTimeser->tsorgVl) )
{
}

TBExTimeser :: ~TBExTimeser()
{
}
#include "TBExTmarkar.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTmarkar :: TBExTmarkar () :
TBTmarkar (),
ExTable( this, 2, 0)
{

	AddOwner(&mareaTp, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &martKey, 0);
	AddOwnerAlternateKey(0, &mareaTpMartKey, 0, icc_f.martKey);
	AddOwnerAlternateKey(0, &mareaTpName, 2, icc_f.name);
	AddOwner(&trareTp, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &tratKey, 0);
	AddOwnerAlternateKey(1, &trareTpTratKey, 0, icc_f.tratKey);
	AddOwnerAlternateKey(1, &trareTpColour, 1, icc_f.colour);
}

TBExTmarkar :: TBExTmarkar (const pTBExTmarkar exTmarkar) : TBTmarkar((pTBTmarkar)exTmarkar),
		ExTable((ExTable *)exTmarkar),
				mareaTpMartKey( exTmarkar->mareaTpMartKey ),
				mareaTpName( exTmarkar->mareaTpName ),
				trareTpTratKey( exTmarkar->trareTpTratKey ),
				trareTpColour( exTmarkar->trareTpColour ),
				mareaTp( &(exTmarkar->mareaTp) ),
				trareTp( &(exTmarkar->trareTp) )
{
}

TBExTmarkar :: ~TBExTmarkar()
{
}
#include "TBExTrareTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTrareTp :: TBExTrareTp () :
TBTrareTp (),
ExTable( this, 1, 4)
{

	AddOwner(&mareaTp, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &martKey, 0);
	AddOwnerAlternateKey(0, &mareaTpMartKey, 0, icc_f.martKey);
	AddOwnerAlternateKey(0, &mareaTpName, 2, icc_f.name);
	AddMember(&madcomaa, 1, CAN);
	AddMemberKey(0, &tratKey, 3);
	AddMember(&bid24frm, 1, CAN);
	AddMemberKey(1, &tratKey, 3);
	AddMember(&bidblock, 1, CAN);
	AddMemberKey(2, &tratKey, 3);
	AddMember(&tmarkar, 1, CAN);
	AddMemberKey(3, &tratKey, 3);
}

TBExTrareTp :: TBExTrareTp (const pTBExTrareTp exTrareTp) : TBTrareTp((pTBTrareTp)exTrareTp),
		ExTable((ExTable *)exTrareTp),
				mareaTpMartKey( exTrareTp->mareaTpMartKey ),
				mareaTpName( exTrareTp->mareaTpName ),
				mareaTp( &(exTrareTp->mareaTp) ),
				madcomaa( &(exTrareTp->madcomaa) ),
				bid24frm( &(exTrareTp->bid24frm) ),
				bidblock( &(exTrareTp->bidblock) ),
				tmarkar( &(exTrareTp->tmarkar) )
{
}

TBExTrareTp :: ~TBExTrareTp()
{
}
#include "TBExTrbusbar.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTrbusbar :: TBExTrbusbar () :
TBTrbusbar (),
ExTable( this, 0, 0)
{

}

TBExTrbusbar :: TBExTrbusbar (const pTBExTrbusbar exTrbusbar) : TBTrbusbar((pTBTrbusbar)exTrbusbar),
		ExTable((ExTable *)exTrbusbar){
}

TBExTrbusbar :: ~TBExTrbusbar()
{
}
#include "TBExTrdreport.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTrdreport :: TBExTrdreport () :
TBTrdreport (),
ExTable( this, 1, 1)
{

	AddOwner(&trdrpTp, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &trrpKey, 0);
	AddOwnerAlternateKey(0, &trdrpTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &trdrpTpName, 2, icc_f.name);
	AddMember(&accrpobj, 2, CAN);
	AddMemberKey(0, &opunKey, 0);
	AddMemberKey(0, &rpcode, 1);
}

TBExTrdreport :: TBExTrdreport (const pTBExTrdreport exTrdreport) : TBTrdreport((pTBTrdreport)exTrdreport),
		ExTable((ExTable *)exTrdreport),
				trdrpTpCode( exTrdreport->trdrpTpCode ),
				trdrpTpName( exTrdreport->trdrpTpName ),
				trdrpTp( &(exTrdreport->trdrpTp) ),
				accrpobj( &(exTrdreport->accrpobj) )
{
}

TBExTrdreport :: ~TBExTrdreport()
{
}
#include "TBExTrelsbid.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTrelsbid :: TBExTrelsbid () :
TBTrelsbid (),
ExTable( this, 2, 2)
{

	AddOwner(&trobject, 2, 2, MUST);
	AddOwnerPrimaryKey(0, &trcode, 0);
	AddOwnerPrimaryKey(0, &opunKey, 2);
	AddOwnerAlternateKey(0, &trobjectTrcode, 0, icc_f.trcode);
	AddOwnerAlternateKey(0, &trobjectName, 1, icc_f.name);
	AddOwner(&unmeaTp, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &unmeKey, 0);
	AddOwnerAlternateKey(1, &unmeaTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(1, &unmeaTpName, 2, icc_f.name);
	AddMember(&bidblock, 2, CAN);
	AddMemberKey(0, &opunKey, 0);
	AddMemberKey(0, &trcode, 1);
	AddMember(&bidcots, 2, CAN);
	AddMemberKey(1, &opunKey, 0);
	AddMemberKey(1, &trcode, 1);
}

TBExTrelsbid :: TBExTrelsbid (const pTBExTrelsbid exTrelsbid) : TBTrelsbid((pTBTrelsbid)exTrelsbid),
		ExTable((ExTable *)exTrelsbid),
				trobjectTrcode( exTrelsbid->trobjectTrcode ),
				trobjectName( exTrelsbid->trobjectName ),
				unmeaTpCode( exTrelsbid->unmeaTpCode ),
				unmeaTpName( exTrelsbid->unmeaTpName ),
				trobject( &(exTrelsbid->trobject) ),
				unmeaTp( &(exTrelsbid->unmeaTp) ),
				bidblock( &(exTrelsbid->bidblock) ),
				bidcots( &(exTrelsbid->bidcots) )
{
}

TBExTrelsbid :: ~TBExTrelsbid()
{
}
#include "TBExTrobcomm.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTrobcomm :: TBExTrobcomm () :
TBTrobcomm (),
ExTable( this, 1, 0)
{

	AddOwner(&trobject, 2, 2, MUST);
	AddOwnerPrimaryKey(0, &trcode, 0);
	AddOwnerPrimaryKey(0, &opunKey, 2);
	AddOwnerAlternateKey(0, &trobjectTrcode, 0, icc_f.trcode);
	AddOwnerAlternateKey(0, &trobjectName, 1, icc_f.name);
}

TBExTrobcomm :: TBExTrobcomm (const pTBExTrobcomm exTrobcomm) : TBTrobcomm((pTBTrobcomm)exTrobcomm),
		ExTable((ExTable *)exTrobcomm),
				trobjectTrcode( exTrobcomm->trobjectTrcode ),
				trobjectName( exTrobcomm->trobjectName ),
				trobject( &(exTrobcomm->trobject) )
{
}

TBExTrobcomm :: ~TBExTrobcomm()
{
}
#include "TBExTrobject.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTrobject :: TBExTrobject () :
TBTrobject (),
ExTable( this, 7, 12)
{

	AddOwner(&direcTp, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &dircKey, 0);
	AddOwnerAlternateKey(0, &direcTpDircKey, 0, icc_f.dircKey);
	AddOwnerAlternateKey(0, &direcTpName, 2, icc_f.name);
	AddOwner(&eleccomp, 1, 2, CAN);
	AddOwnerPrimaryKey(1, &elecKey, 0);
	AddOwnerAlternateKey(1, &eleccompName, 1, icc_f.name);
	AddOwnerAlternateKey(1, &eleccompElcmKey, 6, icc_f.elcmKey);
	AddOwner(&elecnode, 1, 2, CAN);
	AddOwnerPrimaryKey(2, &elnoKey, 0);
	AddOwnerAlternateKey(2, &elecnodeName, 2, icc_f.name);
	AddOwnerAlternateKey(2, &elecnodeDescript, 3, icc_f.descript);
	AddOwner(&hydcomp, 1, 1, CAN);
	AddOwnerPrimaryKey(3, &hydKey, 0);
	AddOwnerAlternateKey(3, &hydcompName, 3, icc_f.name);
	AddOwner(&oprtunit, 1, 2, MUST);
	AddOwnerPrimaryKey(4, &opunKey, 0);
	AddOwnerAlternateKey(4, &oprtunitName, 1, icc_f.name);
	AddOwnerAlternateKey(4, &oprtunitShorname, 2, icc_f.shorname);
	AddOwner(&timeser, 1, 2, CAN);
	AddOwnerPrimaryKey(5, &timsKey, 0);
	AddOwnerAlternateKey(5, &timeserTscode, 1, icc_f.tscode);
	AddOwnerAlternateKey(5, &timeserName, 2, icc_f.name);
	AddOwner(&trobjTp, 1, 2, MUST);
	AddOwnerPrimaryKey(6, &trotKey, 0);
	AddOwnerAlternateKey(6, &trobjTpTrotKey, 0, icc_f.trotKey);
	AddOwnerAlternateKey(6, &trobjTpName, 2, icc_f.name);
	AddMember(&accrpobj, 2, CAN);
	AddMemberKey(0, &trcode, 3);
	AddMemberKey(0, &opunKey, 0);
	AddMember(&bid24frm, 2, CAN);
	AddMemberKey(1, &trcode, 1);
	AddMemberKey(1, &opunKey, 0);
	AddMember(&contract, 2, CAN);
	AddMemberKey(2, &trcode, 1);
	AddMemberKey(2, &opunKey, 0);
	AddMember(&counter, 2, CAN);
	AddMemberKey(3, &trcode, 1);
	AddMemberKey(3, &opunKey, 0);
	AddMember(&trelsbid, 2, CAN);
	AddMemberKey(4, &trcode, 1);
	AddMemberKey(4, &opunKey, 0);
	AddMember(&trprod, 2, CAN);
	AddMemberKey(5, &trcode, 1);
	AddMemberKey(5, &opunKey, 0);
	AddMember(&invper, 2, CAN);
	AddMemberKey(6, &trcode, 1);
	AddMemberKey(6, &opunKey, 0);
	AddMember(&procotro, 2, CAN);
	AddMemberKey(7, &trcode, 1);
	AddMemberKey(7, &opunKey, 0);
	AddMember(&refTrobject, 2, CAN);
	AddMemberKey(8, &trcode, 2);
	AddMemberKey(8, &opunKey, 1);
	AddMember(&tariff, 2, CAN);
	AddMemberKey(9, &trcode, 1);
	AddMemberKey(9, &opunKey, 0);
	AddMember(&trobcomm, 2, CAN);
	AddMemberKey(10, &trcode, 1);
	AddMemberKey(10, &opunKey, 0);
	AddMember(&troption, 2, CAN);
	AddMemberKey(11, &trcode, 1);
	AddMemberKey(11, &opunKey, 0);
}

TBExTrobject :: TBExTrobject (const pTBExTrobject exTrobject) : TBTrobject((pTBTrobject)exTrobject),
		ExTable((ExTable *)exTrobject),
				direcTpDircKey( exTrobject->direcTpDircKey ),
				direcTpName( exTrobject->direcTpName ),
				eleccompName( exTrobject->eleccompName ),
				eleccompElcmKey( exTrobject->eleccompElcmKey ),
				elecnodeName( exTrobject->elecnodeName ),
				elecnodeDescript( exTrobject->elecnodeDescript ),
				hydcompName( exTrobject->hydcompName ),
				oprtunitName( exTrobject->oprtunitName ),
				oprtunitShorname( exTrobject->oprtunitShorname ),
				timeserTscode( exTrobject->timeserTscode ),
				timeserName( exTrobject->timeserName ),
				trobjTpTrotKey( exTrobject->trobjTpTrotKey ),
				trobjTpName( exTrobject->trobjTpName ),
				direcTp( &(exTrobject->direcTp) ),
				eleccomp( &(exTrobject->eleccomp) ),
				elecnode( &(exTrobject->elecnode) ),
				hydcomp( &(exTrobject->hydcomp) ),
				oprtunit( &(exTrobject->oprtunit) ),
				timeser( &(exTrobject->timeser) ),
				trobjTp( &(exTrobject->trobjTp) ),
				accrpobj( &(exTrobject->accrpobj) ),
				bid24frm( &(exTrobject->bid24frm) ),
				contract( &(exTrobject->contract) ),
				counter( &(exTrobject->counter) ),
				trelsbid( &(exTrobject->trelsbid) ),
				trprod( &(exTrobject->trprod) ),
				invper( &(exTrobject->invper) ),
				procotro( &(exTrobject->procotro) ),
				refTrobject( &(exTrobject->refTrobject) ),
				tariff( &(exTrobject->tariff) ),
				trobcomm( &(exTrobject->trobcomm) ),
				troption( &(exTrobject->troption) )
{
}

TBExTrobject :: ~TBExTrobject()
{
}
#include "TBExTrobjTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTrobjTp :: TBExTrobjTp () :
TBTrobjTp (),
ExTable( this, 0, 1)
{

	AddMember(&trobject, 1, CAN);
	AddMemberKey(0, &trotKey, 8);
}

TBExTrobjTp :: TBExTrobjTp (const pTBExTrobjTp exTrobjTp) : TBTrobjTp((pTBTrobjTp)exTrobjTp),
		ExTable((ExTable *)exTrobjTp),
				trobject( &(exTrobjTp->trobject) )
{
}

TBExTrobjTp :: ~TBExTrobjTp()
{
}
#include "TBExTrocotro.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTrocotro :: TBExTrocotro () :
TBTrocotro (),
ExTable( this, 0, 0)
{

}

TBExTrocotro :: TBExTrocotro (const pTBExTrocotro exTrocotro) : TBTrocotro((pTBTrocotro)exTrocotro),
		ExTable((ExTable *)exTrocotro){
}

TBExTrocotro :: ~TBExTrocotro()
{
}
#include "TBExTroption.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTroption :: TBExTroption () :
TBTroption (),
ExTable( this, 2, 0)
{

	AddOwner(&buyslTp, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &buslKey, 0);
	AddOwnerAlternateKey(0, &buyslTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &buyslTpName, 2, icc_f.name);
	AddOwner(&trobject, 2, 2, MUST);
	AddOwnerPrimaryKey(1, &trcode, 0);
	AddOwnerPrimaryKey(1, &opunKey, 2);
	AddOwnerAlternateKey(1, &trobjectTrcode, 0, icc_f.trcode);
	AddOwnerAlternateKey(1, &trobjectName, 1, icc_f.name);
}

TBExTroption :: TBExTroption (const pTBExTroption exTroption) : TBTroption((pTBTroption)exTroption),
		ExTable((ExTable *)exTroption),
				buyslTpCode( exTroption->buyslTpCode ),
				buyslTpName( exTroption->buyslTpName ),
				trobjectTrcode( exTroption->trobjectTrcode ),
				trobjectName( exTroption->trobjectName ),
				buyslTp( &(exTroption->buyslTp) ),
				trobject( &(exTroption->trobject) )
{
}

TBExTroption :: ~TBExTroption()
{
}
#include "TBExTrprod.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTrprod :: TBExTrprod () :
TBTrprod (),
ExTable( this, 2, 0)
{

	AddOwner(&trobject, 2, 2, MUST);
	AddOwnerPrimaryKey(0, &trcode, 0);
	AddOwnerPrimaryKey(0, &opunKey, 2);
	AddOwnerAlternateKey(0, &trobjectTrcode, 0, icc_f.trcode);
	AddOwnerAlternateKey(0, &trobjectName, 1, icc_f.name);
	AddOwner(&unmeaTp, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &unmeKey, 0);
	AddOwnerAlternateKey(1, &unmeaTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(1, &unmeaTpName, 2, icc_f.name);
}

TBExTrprod :: TBExTrprod (const pTBExTrprod exTrprod) : TBTrprod((pTBTrprod)exTrprod),
		ExTable((ExTable *)exTrprod),
				trobjectTrcode( exTrprod->trobjectTrcode ),
				trobjectName( exTrprod->trobjectName ),
				unmeaTpCode( exTrprod->unmeaTpCode ),
				unmeaTpName( exTrprod->unmeaTpName ),
				trobject( &(exTrprod->trobject) ),
				unmeaTp( &(exTrprod->unmeaTp) )
{
}

TBExTrprod :: ~TBExTrprod()
{
}
#include "TBExTsapmCc.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTsapmCc :: TBExTsapmCc () :
TBTsapmCc (),
ExTable( this, 1, 5)
{

	AddOwner(&ccmetTp, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &ccmeKey, 0);
	AddOwnerAlternateKey(0, &ccmetTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &ccmetTpName, 2, icc_f.name);
	AddMember(&auregCo, 2, CAN);
	AddMemberKey(0, &ccmeKey, 0);
	AddMemberKey(0, &timsKey, 1);
	AddMember(&constCo, 2, CAN);
	AddMemberKey(1, &ccmeKey, 0);
	AddMemberKey(1, &timsKey, 1);
	AddMember(&meavlCo, 2, CAN);
	AddMemberKey(2, &ccmeKey, 0);
	AddMemberKey(2, &timsKey, 1);
	AddMember(&mimaxCt, 2, CAN);
	AddMemberKey(3, &ccmeKey, 0);
	AddMemberKey(3, &timsKey, 1);
	AddMember(&repvlCt, 2, CAN);
	AddMemberKey(4, &ccmeKey, 0);
	AddMemberKey(4, &timsKey, 1);
}

TBExTsapmCc :: TBExTsapmCc (const pTBExTsapmCc exTsapmCc) : TBTsapmCc((pTBTsapmCc)exTsapmCc),
		ExTable((ExTable *)exTsapmCc),
				ccmetTpCode( exTsapmCc->ccmetTpCode ),
				ccmetTpName( exTsapmCc->ccmetTpName ),
				ccmetTp( &(exTsapmCc->ccmetTp) ),
				auregCo( &(exTsapmCc->auregCo) ),
				constCo( &(exTsapmCc->constCo) ),
				meavlCo( &(exTsapmCc->meavlCo) ),
				mimaxCt( &(exTsapmCc->mimaxCt) ),
				repvlCt( &(exTsapmCc->repvlCt) )
{
}

TBExTsapmCc :: ~TBExTsapmCc()
{
}
#include "TBExTscoupl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTscoupl :: TBExTscoupl () :
TBTscoupl (),
ExTable( this, 1, 0)
{

	AddOwner(&timeser, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &timsKey, 0);
	AddOwnerAlternateKey(0, &timeserTscode, 1, icc_f.tscode);
	AddOwnerAlternateKey(0, &timeserName, 2, icc_f.name);
}

TBExTscoupl :: TBExTscoupl (const pTBExTscoupl exTscoupl) : TBTscoupl((pTBTscoupl)exTscoupl),
		ExTable((ExTable *)exTscoupl),
				timeserTscode( exTscoupl->timeserTscode ),
				timeserName( exTscoupl->timeserName ),
				timeser( &(exTscoupl->timeser) )
{
}

TBExTscoupl :: ~TBExTscoupl()
{
}
#include "TBExTseriTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTseriTp :: TBExTseriTp () :
TBTseriTp (),
ExTable( this, 0, 3)
{

	AddMember(&timeser, 1, CAN);
	AddMemberKey(0, &tserKey, 29);
	AddMember(&unmeaVd, 1, CAN);
	AddMemberKey(1, &tserKey, 1);
	AddMember(&accuVl, 1, CAN);
	AddMemberKey(2, &tserKey, 1);
}

TBExTseriTp :: TBExTseriTp (const pTBExTseriTp exTseriTp) : TBTseriTp((pTBTseriTp)exTseriTp),
		ExTable((ExTable *)exTseriTp),
				timeser( &(exTseriTp->timeser) ),
				unmeaVd( &(exTseriTp->unmeaVd) ),
				accuVl( &(exTseriTp->accuVl) )
{
}

TBExTseriTp :: ~TBExTseriTp()
{
}
#include "TBExTsintTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTsintTp :: TBExTsintTp () :
TBTsintTp (),
ExTable( this, 0, 2)
{

	AddMember(&timeser, 1, CAN);
	AddMemberKey(0, &tsinKey, 27);
	AddMember(&accuVl, 1, CAN);
	AddMemberKey(1, &tsinKey, 3);
}

TBExTsintTp :: TBExTsintTp (const pTBExTsintTp exTsintTp) : TBTsintTp((pTBTsintTp)exTsintTp),
		ExTable((ExTable *)exTsintTp),
				timeser( &(exTsintTp->timeser) ),
				accuVl( &(exTsintTp->accuVl) )
{
}

TBExTsintTp :: ~TBExTsintTp()
{
}
#include "TBExTsproduc.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTsproduc :: TBExTsproduc () :
TBTsproduc (),
ExTable( this, 0, 1)
{

	AddMember(&timeser, 1, CAN);
	AddMemberKey(0, &tsprKey, 24);
}

TBExTsproduc :: TBExTsproduc (const pTBExTsproduc exTsproduc) : TBTsproduc((pTBTsproduc)exTsproduc),
		ExTable((ExTable *)exTsproduc),
				timeser( &(exTsproduc->timeser) )
{
}

TBExTsproduc :: ~TBExTsproduc()
{
}
#include "TBExTsrefwei.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTsrefwei :: TBExTsrefwei () :
TBTsrefwei (),
ExTable( this, 1, 0)
{

	AddOwner(&meavlCo, 2, 0, MUST);
	AddOwnerPrimaryKey(0, &ccmeKey, 0);
	AddOwnerPrimaryKey(0, &timsKey, 1);
}

TBExTsrefwei :: TBExTsrefwei (const pTBExTsrefwei exTsrefwei) : TBTsrefwei((pTBTsrefwei)exTsrefwei),
		ExTable((ExTable *)exTsrefwei),
				meavlCo( &(exTsrefwei->meavlCo) )
{
}

TBExTsrefwei :: ~TBExTsrefwei()
{
}
#include "TBExTsstatus.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTsstatus :: TBExTsstatus () :
TBTsstatus (),
ExTable( this, 1, 0)
{

	AddOwner(&tstradef, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &tstrKey, 0);
	AddOwnerAlternateKey(0, &tstradefName, 1, icc_f.name);
	AddOwnerAlternateKey(0, &tstradefDescript, 3, icc_f.descript);
}

TBExTsstatus :: TBExTsstatus (const pTBExTsstatus exTsstatus) : TBTsstatus((pTBTsstatus)exTsstatus),
		ExTable((ExTable *)exTsstatus),
				tstradefName( exTsstatus->tstradefName ),
				tstradefDescript( exTsstatus->tstradefDescript ),
				tstradef( &(exTsstatus->tstradef) )
{
}

TBExTsstatus :: ~TBExTsstatus()
{
}
#include "TBExTstradef.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExTstradef :: TBExTstradef () :
TBTstradef (),
ExTable( this, 1, 1)
{

	AddOwner(&protoTp, 1, 2, MUST);
	AddOwnerPrimaryKey(0, &protKey, 0);
	AddOwnerAlternateKey(0, &protoTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(0, &protoTpName, 2, icc_f.name);
	AddMember(&tsstatus, 1, CAN);
	AddMemberKey(0, &tstrKey, 0);
}

TBExTstradef :: TBExTstradef (const pTBExTstradef exTstradef) : TBTstradef((pTBTstradef)exTstradef),
		ExTable((ExTable *)exTstradef),
				protoTpCode( exTstradef->protoTpCode ),
				protoTpName( exTstradef->protoTpName ),
				protoTp( &(exTstradef->protoTp) ),
				tsstatus( &(exTstradef->tsstatus) )
{
}

TBExTstradef :: ~TBExTstradef()
{
}
#include "TBExUnmeaTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExUnmeaTp :: TBExUnmeaTp () :
TBUnmeaTp (),
ExTable( this, 0, 8)
{

	AddMember(&oprrst, 1, CAN);
	AddMemberKey(0, &unmeKey, 9);
	AddMember(&timeser, 1, CAN);
	AddMemberKey(1, &unmeKey, 30);
	AddMember(&unmeaVd, 1, CAN);
	AddMemberKey(2, &unmeKey, 2);
	AddMember(&accuVl, 1, CAN);
	AddMemberKey(3, &unmeKey, 0);
	AddMember(&unfacVl, 1, CAN);
	AddMemberKey(4, &unmeKey, 0);
	AddMember(&priceint, 1, CAN);
	AddMemberKey(5, &unmeKey, 1);
	AddMember(&trelsbid, 1, CAN);
	AddMemberKey(6, &unmeKey, 2);
	AddMember(&trprod, 1, CAN);
	AddMemberKey(7, &unmeKey, 3);
}

TBExUnmeaTp :: TBExUnmeaTp (const pTBExUnmeaTp exUnmeaTp) : TBUnmeaTp((pTBUnmeaTp)exUnmeaTp),
		ExTable((ExTable *)exUnmeaTp),
				oprrst( &(exUnmeaTp->oprrst) ),
				timeser( &(exUnmeaTp->timeser) ),
				unmeaVd( &(exUnmeaTp->unmeaVd) ),
				accuVl( &(exUnmeaTp->accuVl) ),
				unfacVl( &(exUnmeaTp->unfacVl) ),
				priceint( &(exUnmeaTp->priceint) ),
				trelsbid( &(exUnmeaTp->trelsbid) ),
				trprod( &(exUnmeaTp->trprod) )
{
}

TBExUnmeaTp :: ~TBExUnmeaTp()
{
}
#include "TBExWatercou.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExWatercou :: TBExWatercou () :
TBWatercou (),
ExTable( this, 0, 4)
{

	AddMember(&fieldsta, 1, CAN);
	AddMemberKey(0, &watcKey, 15);
	AddMember(&prrunDs, 1, CAN);
	AddMemberKey(1, &watcKey, 17);
	AddMember(&hydcomp, 1, CAN);
	AddMemberKey(2, &watcKey, 1);
	AddMember(&project, 1, CAN);
	AddMemberKey(3, &watcKey, 6);
}

TBExWatercou :: TBExWatercou (const pTBExWatercou exWatercou) : TBWatercou((pTBWatercou)exWatercou),
		ExTable((ExTable *)exWatercou),
				fieldsta( &(exWatercou->fieldsta) ),
				prrunDs( &(exWatercou->prrunDs) ),
				hydcomp( &(exWatercou->hydcomp) ),
				project( &(exWatercou->project) )
{
}

TBExWatercou :: ~TBExWatercou()
{
}
#include "TBExWtgate.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExWtgate :: TBExWtgate () :
TBWtgate (),
ExTable( this, 2, 0)
{

	AddOwner(&hydcomp, 1, 1, MUST);
	AddOwnerPrimaryKey(0, &hydKey, 0);
	AddOwnerAlternateKey(0, &hydcompName, 3, icc_f.name);
	AddOwner(&wtgtTp, 1, 2, MUST);
	AddOwnerPrimaryKey(1, &wtgtKey, 0);
	AddOwnerAlternateKey(1, &wtgtTpCode, 1, icc_f.code);
	AddOwnerAlternateKey(1, &wtgtTpName, 2, icc_f.name);
}

TBExWtgate :: TBExWtgate (const pTBExWtgate exWtgate) : TBWtgate((pTBWtgate)exWtgate),
		ExTable((ExTable *)exWtgate),
				hydcompName( exWtgate->hydcompName ),
				wtgtTpCode( exWtgate->wtgtTpCode ),
				wtgtTpName( exWtgate->wtgtTpName ),
				hydcomp( &(exWtgate->hydcomp) ),
				wtgtTp( &(exWtgate->wtgtTp) )
{
}

TBExWtgate :: ~TBExWtgate()
{
}
