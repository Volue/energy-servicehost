#include "iccTables.h"
#include "TBKeytab9.h"


// C o n s t r u c t o r s

TBKeytab9 :: TBKeytab9 () : Table(icc_t.Keytab9, 5, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBKeytab9 :: TBKeytab9 (const pTBKeytab9 keytab9) : Table ((Table *) keytab9)
{
	AddAllFields();

	if (keytab9)
	{
		if ( ! keytab9->GetRcount().IsNull() )
			SetRcount(keytab9->GetRcount());
		if ( ! keytab9->GetSeqno().IsNull() )
			SetSeqno(keytab9->GetSeqno());
		if ( ! keytab9->GetIvalue1().IsNull() )
			SetIvalue1(keytab9->GetIvalue1());
		if ( ! keytab9->GetIvalue2().IsNull() )
			SetIvalue2(keytab9->GetIvalue2());
		if ( ! keytab9->GetCvalue()->IsNull() )
			SetCvalue(keytab9->GetCvalue());
	}
}


TBKeytab9 :: ~TBKeytab9 ()
{
	// Delete all pointer members.

}

void TBKeytab9 :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&rcount);
	AddKeyField((OSDataType *)&seqno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&rcount, icc_f.rcount);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&ivalue1, icc_f.ivalue1);
	AddField((OSDataType *)&ivalue2, icc_f.ivalue2);
	AddField((OSDataType *)&cvalue, icc_f.cvalue);
}

#include "TBAccbaTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBAccbaTp :: TBAccbaTp () : Table(icc_t.AccbaTp, 3, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBAccbaTp :: TBAccbaTp (const pTBAccbaTp accbaTp) : Table ((Table *) accbaTp)
{
	AddAllFields();

	if (accbaTp)
	{
		if ( ! accbaTp->GetAcbaKey()->IsNull() )
			SetAcbaKey(accbaTp->GetAcbaKey());
		if ( ! accbaTp->GetEnumval().IsNull() )
			SetEnumval(accbaTp->GetEnumval());
		if ( ! accbaTp->GetName()->IsNull() )
			SetName(accbaTp->GetName());
	}
}


TBAccbaTp :: ~TBAccbaTp ()
{
	// Delete all pointer members.

}

void TBAccbaTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&acbaKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&acbaKey, icc_f.acbaKey);
	AddField((OSDataType *)&enumval, icc_f.enumval);
	AddField((OSDataType *)&name, icc_f.name);
}

#include "TBAccntrec.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBAccntrec :: TBAccntrec () : Table(icc_t.Accntrec, 12, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBAccntrec :: TBAccntrec (const pTBAccntrec accntrec) : Table ((Table *) accntrec)
{
	AddAllFields();

	if (accntrec)
	{
		if ( ! accntrec->GetOpunKey().IsNull() )
			SetOpunKey(accntrec->GetOpunKey());
		if ( ! accntrec->GetSeqno().IsNull() )
			SetSeqno(accntrec->GetSeqno());
		if ( ! accntrec->GetBalance()->IsNull() )
			SetBalance(accntrec->GetBalance());
		if ( ! accntrec->GetStardate()->IsNull() )
			SetStardate(accntrec->GetStardate());
		if ( ! accntrec->GetEnddate()->IsNull() )
			SetEnddate(accntrec->GetEnddate());
		if ( ! accntrec->GetTrcode()->IsNull() )
			SetTrcode(accntrec->GetTrcode());
		if ( ! accntrec->GetEdiCode()->IsNull() )
			SetEdiCode(accntrec->GetEdiCode());
		if ( ! accntrec->GetDescript()->IsNull() )
			SetDescript(accntrec->GetDescript());
		if ( ! accntrec->GetTrobjco1()->IsNull() )
			SetTrobjco1(accntrec->GetTrobjco1());
		if ( ! accntrec->GetTrobjco2()->IsNull() )
			SetTrobjco2(accntrec->GetTrobjco2());
		if ( ! accntrec->GetPrintco1().IsNull() )
			SetPrintco1(accntrec->GetPrintco1());
		if ( ! accntrec->GetTrcode1()->IsNull() )
			SetTrcode1(accntrec->GetTrcode1());
	}
}


TBAccntrec :: ~TBAccntrec ()
{
	// Delete all pointer members.

}

void TBAccntrec :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&seqno);
	AddKeyField((OSDataType *)&balance);
	AddKeyField((OSDataType *)&stardate);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&balance, icc_f.balance);
	AddField((OSDataType *)&stardate, icc_f.stardate);
	AddField((OSDataType *)&enddate, icc_f.enddate);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&ediCode, icc_f.ediCode);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&trobjco1, icc_f.trobjco1);
	AddField((OSDataType *)&trobjco2, icc_f.trobjco2);
	AddField((OSDataType *)&printco1, icc_f.printco1);
	AddField((OSDataType *)&trcode1, icc_f.trcode1);
}

#include "TBAccntrp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBAccntrp :: TBAccntrp () : Table(icc_t.Accntrp, 5, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBAccntrp :: TBAccntrp (const pTBAccntrp accntrp) : Table ((Table *) accntrp)
{
	AddAllFields();

	if (accntrp)
	{
		if ( ! accntrp->GetOpunKey().IsNull() )
			SetOpunKey(accntrp->GetOpunKey());
		if ( ! accntrp->GetYearweek().IsNull() )
			SetYearweek(accntrp->GetYearweek());
		if ( ! accntrp->GetSentdate()->IsNull() )
			SetSentdate(accntrp->GetSentdate());
		if ( ! accntrp->GetMessRef()->IsNull() )
			SetMessRef(accntrp->GetMessRef());
		if ( ! accntrp->GetSigncode()->IsNull() )
			SetSigncode(accntrp->GetSigncode());
	}
}


TBAccntrp :: ~TBAccntrp ()
{
	// Delete all pointer members.

}

void TBAccntrp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&yearweek);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&yearweek, icc_f.yearweek);
	AddField((OSDataType *)&sentdate, icc_f.sentdate);
	AddField((OSDataType *)&messRef, icc_f.messRef);
	AddField((OSDataType *)&signcode, icc_f.signcode);
}

#include "TBAccrpobj.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBAccrpobj :: TBAccrpobj () : Table(icc_t.Accrpobj, 9, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBAccrpobj :: TBAccrpobj (const pTBAccrpobj accrpobj) : Table ((Table *) accrpobj)
{
	AddAllFields();

	if (accrpobj)
	{
		if ( ! accrpobj->GetOpunKey().IsNull() )
			SetOpunKey(accrpobj->GetOpunKey());
		if ( ! accrpobj->GetRpcode()->IsNull() )
			SetRpcode(accrpobj->GetRpcode());
		if ( ! accrpobj->GetSeqno().IsNull() )
			SetSeqno(accrpobj->GetSeqno());
		if ( ! accrpobj->GetTrcode()->IsNull() )
			SetTrcode(accrpobj->GetTrcode());
		if ( ! accrpobj->GetHeader1()->IsNull() )
			SetHeader1(accrpobj->GetHeader1());
		if ( ! accrpobj->GetHeader2()->IsNull() )
			SetHeader2(accrpobj->GetHeader2());
		if ( ! accrpobj->GetHeader3()->IsNull() )
			SetHeader3(accrpobj->GetHeader3());
		if ( ! accrpobj->GetHeader4()->IsNull() )
			SetHeader4(accrpobj->GetHeader4());
		if ( ! accrpobj->GetDecimals().IsNull() )
			SetDecimals(accrpobj->GetDecimals());
	}
}


TBAccrpobj :: ~TBAccrpobj ()
{
	// Delete all pointer members.

}

void TBAccrpobj :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&rpcode);
	AddKeyField((OSDataType *)&seqno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&rpcode, icc_f.rpcode);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&header1, icc_f.header1);
	AddField((OSDataType *)&header2, icc_f.header2);
	AddField((OSDataType *)&header3, icc_f.header3);
	AddField((OSDataType *)&header4, icc_f.header4);
	AddField((OSDataType *)&decimals, icc_f.decimals);
}

#include "TBAccuVl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBAccuVl :: TBAccuVl () : Table(icc_t.AccuVl, 6, 4, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBAccuVl :: TBAccuVl (const pTBAccuVl accuVl) : Table ((Table *) accuVl)
{
	AddAllFields();

	if (accuVl)
	{
		if ( ! accuVl->GetUnmeKey().IsNull() )
			SetUnmeKey(accuVl->GetUnmeKey());
		if ( ! accuVl->GetTserKey().IsNull() )
			SetTserKey(accuVl->GetTserKey());
		if ( ! accuVl->GetUnmeKea().IsNull() )
			SetUnmeKea(accuVl->GetUnmeKea());
		if ( ! accuVl->GetTsinKey().IsNull() )
			SetTsinKey(accuVl->GetTsinKey());
		if ( ! accuVl->GetSeqno().IsNull() )
			SetSeqno(accuVl->GetSeqno());
		if ( ! accuVl->GetAccKey().IsNull() )
			SetAccKey(accuVl->GetAccKey());
	}
}


TBAccuVl :: ~TBAccuVl ()
{
	// Delete all pointer members.

}

void TBAccuVl :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&unmeKey);
	AddKeyField((OSDataType *)&tserKey);
	AddKeyField((OSDataType *)&unmeKea);
	AddKeyField((OSDataType *)&tsinKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&unmeKey, icc_f.unmeKey);
	AddField((OSDataType *)&tserKey, icc_f.tserKey);
	AddField((OSDataType *)&unmeKea, icc_f.unmeKea);
	AddField((OSDataType *)&tsinKey, icc_f.tsinKey);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&accKey, icc_f.accKey);
}

#include "TBAghcohyr.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBAghcohyr :: TBAghcohyr () : Table(icc_t.Aghcohyr, 3, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBAghcohyr :: TBAghcohyr (const pTBAghcohyr aghcohyr) : Table ((Table *) aghcohyr)
{
	AddAllFields();

	if (aghcohyr)
	{
		if ( ! aghcohyr->GetAchKey().IsNull() )
			SetAchKey(aghcohyr->GetAchKey());
		if ( ! aghcohyr->GetHydKey().IsNull() )
			SetHydKey(aghcohyr->GetHydKey());
		if ( ! aghcohyr->GetRelKey().IsNull() )
			SetRelKey(aghcohyr->GetRelKey());
	}
}


TBAghcohyr :: ~TBAghcohyr ()
{
	// Delete all pointer members.

}

void TBAghcohyr :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&achKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&achKey, icc_f.achKey);
	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&relKey, icc_f.relKey);
}

#include "TBAghcoprr.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBAghcoprr :: TBAghcoprr () : Table(icc_t.Aghcoprr, 2, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBAghcoprr :: TBAghcoprr (const pTBAghcoprr aghcoprr) : Table ((Table *) aghcoprr)
{
	AddAllFields();

	if (aghcoprr)
	{
		if ( ! aghcoprr->GetPrdKey().IsNull() )
			SetPrdKey(aghcoprr->GetPrdKey());
		if ( ! aghcoprr->GetHydKey().IsNull() )
			SetHydKey(aghcoprr->GetHydKey());
	}
}


TBAghcoprr :: ~TBAghcoprr ()
{
	// Delete all pointer members.

}

void TBAghcoprr :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&prdKey);
	AddKeyField((OSDataType *)&hydKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&prdKey, icc_f.prdKey);
	AddField((OSDataType *)&hydKey, icc_f.hydKey);
}

#include "TBAghydDs.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBAghydDs :: TBAghydDs () : Table(icc_t.AghydDs, 8, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBAghydDs :: TBAghydDs (const pTBAghydDs aghydDs) : Table ((Table *) aghydDs)
{
	AddAllFields();

	if (aghydDs)
	{
		if ( ! aghydDs->GetHydKey().IsNull() )
			SetHydKey(aghydDs->GetHydKey());
		if ( ! aghydDs->GetCode()->IsNull() )
			SetCode(aghydDs->GetCode());
		if ( ! aghydDs->GetName()->IsNull() )
			SetName(aghydDs->GetName());
		if ( ! aghydDs->GetEsttime()->IsNull() )
			SetEsttime(aghydDs->GetEsttime());
		if ( ! aghydDs->GetOpname()->IsNull() )
			SetOpname(aghydDs->GetOpname());
		if ( ! aghydDs->GetDescript()->IsNull() )
			SetDescript(aghydDs->GetDescript());
		if ( ! aghydDs->GetVersion().IsNull() )
			SetVersion(aghydDs->GetVersion());
		if ( ! aghydDs->GetProgKey().IsNull() )
			SetProgKey(aghydDs->GetProgKey());
	}
}


TBAghydDs :: ~TBAghydDs ()
{
	// Delete all pointer members.

}

void TBAghydDs :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&hydKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&esttime, icc_f.esttime);
	AddField((OSDataType *)&opname, icc_f.opname);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&version, icc_f.version);
	AddField((OSDataType *)&progKey, icc_f.progKey);
}

#include "TBAgrmpart.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBAgrmpart :: TBAgrmpart () : Table(icc_t.Agrmpart, 13, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBAgrmpart :: TBAgrmpart (const pTBAgrmpart agrmpart) : Table ((Table *) agrmpart)
{
	AddAllFields();

	if (agrmpart)
	{
		if ( ! agrmpart->GetOpunKey().IsNull() )
			SetOpunKey(agrmpart->GetOpunKey());
		if ( ! agrmpart->GetPartNum().IsNull() )
			SetPartNum(agrmpart->GetPartNum());
		if ( ! agrmpart->GetName()->IsNull() )
			SetName(agrmpart->GetName());
		if ( ! agrmpart->GetValftime()->IsNull() )
			SetValftime(agrmpart->GetValftime());
		if ( ! agrmpart->GetValttime()->IsNull() )
			SetValttime(agrmpart->GetValttime());
		if ( ! agrmpart->GetInfo()->IsNull() )
			SetInfo(agrmpart->GetInfo());
		if ( ! agrmpart->GetPowType().IsNull() )
			SetPowType(agrmpart->GetPowType());
		if ( ! agrmpart->GetExchKey().IsNull() )
			SetExchKey(agrmpart->GetExchKey());
		if ( ! agrmpart->GetSeqno().IsNull() )
			SetSeqno(agrmpart->GetSeqno());
		if ( ! agrmpart->GetGrupKey().IsNull() )
			SetGrupKey(agrmpart->GetGrupKey());
		if ( ! agrmpart->GetTarfKey().IsNull() )
			SetTarfKey(agrmpart->GetTarfKey());
		if ( ! agrmpart->GetDebtKey().IsNull() )
			SetDebtKey(agrmpart->GetDebtKey());
		if ( ! agrmpart->GetDebsKey().IsNull() )
			SetDebsKey(agrmpart->GetDebsKey());
	}
}


TBAgrmpart :: ~TBAgrmpart ()
{
	// Delete all pointer members.

}

void TBAgrmpart :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&partNum);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&partNum, icc_f.partNum);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&valftime, icc_f.valftime);
	AddField((OSDataType *)&valttime, icc_f.valttime);
	AddField((OSDataType *)&info, icc_f.info);
	AddField((OSDataType *)&powType, icc_f.powType);
	AddField((OSDataType *)&exchKey, icc_f.exchKey);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&grupKey, icc_f.grupKey);
	AddField((OSDataType *)&tarfKey, icc_f.tarfKey);
	AddField((OSDataType *)&debtKey, icc_f.debtKey);
	AddField((OSDataType *)&debsKey, icc_f.debsKey);
}

#include "TBAgrmtTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBAgrmtTp :: TBAgrmtTp () : Table(icc_t.AgrmtTp, 3, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBAgrmtTp :: TBAgrmtTp (const pTBAgrmtTp agrmtTp) : Table ((Table *) agrmtTp)
{
	AddAllFields();

	if (agrmtTp)
	{
		if ( ! agrmtTp->GetPowType().IsNull() )
			SetPowType(agrmtTp->GetPowType());
		if ( ! agrmtTp->GetCode()->IsNull() )
			SetCode(agrmtTp->GetCode());
		if ( ! agrmtTp->GetName()->IsNull() )
			SetName(agrmtTp->GetName());
	}
}


TBAgrmtTp :: ~TBAgrmtTp ()
{
	// Delete all pointer members.

}

void TBAgrmtTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&powType);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&powType, icc_f.powType);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
}

#include "TBAuregCo.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBAuregCo :: TBAuregCo () : Table(icc_t.AuregCo, 6, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBAuregCo :: TBAuregCo (const pTBAuregCo auregCo) : Table ((Table *) auregCo)
{
	AddAllFields();

	if (auregCo)
	{
		if ( ! auregCo->GetCcmeKey().IsNull() )
			SetCcmeKey(auregCo->GetCcmeKey());
		if ( ! auregCo->GetTimsKey().IsNull() )
			SetTimsKey(auregCo->GetTimsKey());
		if ( ! auregCo->GetModnumb().IsNull() )
			SetModnumb(auregCo->GetModnumb());
		if ( ! auregCo->GetName()->IsNull() )
			SetName(auregCo->GetName());
		if ( ! auregCo->GetDescript()->IsNull() )
			SetDescript(auregCo->GetDescript());
		if ( ! auregCo->GetUpdint().IsNull() )
			SetUpdint(auregCo->GetUpdint());
	}
}


TBAuregCo :: ~TBAuregCo ()
{
	// Delete all pointer members.

}

void TBAuregCo :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&ccmeKey);
	AddKeyField((OSDataType *)&timsKey);
	AddKeyField((OSDataType *)&modnumb);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&ccmeKey, icc_f.ccmeKey);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&modnumb, icc_f.modnumb);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&updint, icc_f.updint);
}

#include "TBBidblock.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBBidblock :: TBBidblock () : Table(icc_t.Bidblock, 18, 5, 0, TABLE)
{
	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBBidblock :: TBBidblock (const pTBBidblock bidblock) : Table ((Table *) bidblock)
{
	AddAllFields();

	if (bidblock)
	{
		if ( ! bidblock->GetOpunKey().IsNull() )
			SetOpunKey(bidblock->GetOpunKey());
		if ( ! bidblock->GetTrcode()->IsNull() )
			SetTrcode(bidblock->GetTrcode());
		if ( ! bidblock->GetTimstamp()->IsNull() )
			SetTimstamp(bidblock->GetTimstamp());
		if ( ! bidblock->GetTratKey()->IsNull() )
			SetTratKey(bidblock->GetTratKey());
		if ( ! bidblock->GetBidRef()->IsNull() )
			SetBidRef(bidblock->GetBidRef());
		if ( ! bidblock->GetMessRef()->IsNull() )
			SetMessRef(bidblock->GetMessRef());
		if ( ! bidblock->GetBlockno().IsNull() )
			SetBlockno(bidblock->GetBlockno());
		if ( ! bidblock->GetPrice().IsNull() )
			SetPrice(bidblock->GetPrice());
		if ( ! bidblock->GetVolume().IsNull() )
			SetVolume(bidblock->GetVolume());
		if ( ! bidblock->GetSigncode()->IsNull() )
			SetSigncode(bidblock->GetSigncode());
		if ( ! bidblock->GetSentdate()->IsNull() )
			SetSentdate(bidblock->GetSentdate());
		if ( ! bidblock->GetKnockingDown().IsNull() )
			SetKnockingDown(bidblock->GetKnockingDown());
		if ( ! bidblock->GetTradeKey().IsNull() )
			SetTradeKey(bidblock->GetTradeKey());
		if ( ! bidblock->GetBidcobid()->IsNull() )
			SetBidcobid(bidblock->GetBidcobid());
		if ( ! bidblock->GetState().IsNull() )
			SetState(bidblock->GetState());
		if ( ! bidblock->GetAcceptedStart().IsNull() )
			SetAcceptedStart(bidblock->GetAcceptedStart());
		if ( ! bidblock->GetAcceptedEnd().IsNull() )
			SetAcceptedEnd(bidblock->GetAcceptedEnd());
		if (!bidblock->GetOrderId()->IsNull())
			SetOrderId(bidblock->GetOrderId());
	}
}


TBBidblock :: ~TBBidblock ()
{
	// Delete all pointer members.

}

void TBBidblock :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);
	AddKeyField((OSDataType *)&timstamp);
	AddKeyField((OSDataType *)&tratKey);
	AddKeyField((OSDataType *)&bidRef);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&timstamp, icc_f.timstamp);
	AddField((OSDataType *)&tratKey, icc_f.tratKey);
	AddField((OSDataType *)&bidRef, icc_f.bidRef);
	AddField((OSDataType *)&messRef, icc_f.messRef);
	AddField((OSDataType *)&blockno, icc_f.blockno);
	AddField((OSDataType *)&price, icc_f.price);
	AddField((OSDataType *)&volume, icc_f.volume);
	AddField((OSDataType *)&signcode, icc_f.signcode);
	AddField((OSDataType *)&sentdate, icc_f.sentdate);
	AddField((OSDataType *)&knockingDown, icc_f.knockingDown);
	AddField((OSDataType *)&tradeKey, icc_f.tradeKey);
	AddField((OSDataType *)&bidcobid, icc_f.bidcobid);
	AddField((OSDataType *)&state, icc_f.state);
	AddField((OSDataType *)&acceptedStart, icc_f.acceptedStart);
	AddField((OSDataType *)&acceptedEnd, icc_f.acceptedEnd);
	AddField((OSDataType *)&orderId, icc_f.orderId);
}

#include "TBBidcots.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBBidcots :: TBBidcots () : Table(icc_t.Bidcots, 6, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}

TBBidcots :: TBBidcots (const pTBBidcots bidcots) : Table ((Table *) bidcots)
{
	AddAllFields();

	if (bidcots)
	{
		if ( ! bidcots->GetOpunKey().IsNull() )
			SetOpunKey(bidcots->GetOpunKey());
		if ( ! bidcots->GetTrcode()->IsNull() )
			SetTrcode(bidcots->GetTrcode());
		if ( ! bidcots->GetSeqno().IsNull() )
			SetSeqno(bidcots->GetSeqno());
		if ( ! bidcots->GetTimsKey().IsNull() )
			SetTimsKey(bidcots->GetTimsKey());
		if ( ! bidcots->GetPriceTimsKey().IsNull() )
			SetPriceTimsKey(bidcots->GetPriceTimsKey());
		if ( ! bidcots->GetVlcode().IsNull() )
			SetVlcode(bidcots->GetVlcode());
	}
}

TBBidcots :: ~TBBidcots ()
{
	// Delete all pointer members.

}

void TBBidcots :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);
	AddKeyField((OSDataType *)&seqno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&priceKey, icc_f.priceTimsKey);
	AddField((OSDataType *)&vlcode, icc_f.vlcode);
}

#include "TBBid24frm.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBBid24frm :: TBBid24frm () : Table(icc_t.Bid24frm, 11, 5, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBBid24frm :: TBBid24frm (const pTBBid24frm bid24frm) : Table ((Table *) bid24frm)
{
	AddAllFields();

	if (bid24frm)
	{
		if ( ! bid24frm->GetOpunKey().IsNull() )
			SetOpunKey(bid24frm->GetOpunKey());
		if ( ! bid24frm->GetTrcode()->IsNull() )
			SetTrcode(bid24frm->GetTrcode());
		if ( ! bid24frm->GetTimstamp()->IsNull() )
			SetTimstamp(bid24frm->GetTimstamp());
		if ( ! bid24frm->GetTratKey()->IsNull() )
			SetTratKey(bid24frm->GetTratKey());
		if ( ! bid24frm->GetFormno().IsNull() )
			SetFormno(bid24frm->GetFormno());
		if ( ! bid24frm->GetSentdate()->IsNull() )
			SetSentdate(bid24frm->GetSentdate());
		if ( ! bid24frm->GetFormstat().IsNull() )
			SetFormstat(bid24frm->GetFormstat());
		if ( ! bid24frm->GetFormdays()->IsNull() )
			SetFormdays(bid24frm->GetFormdays());
		if ( ! bid24frm->GetMessRef()->IsNull() )
			SetMessRef(bid24frm->GetMessRef());
		if ( ! bid24frm->GetSigncode()->IsNull() )
			SetSigncode(bid24frm->GetSigncode());
		if (!bid24frm->GetOrderId()->IsNull())
			SetOrderId(bid24frm->GetOrderId());
	}
}


TBBid24frm :: ~TBBid24frm ()
{
	// Delete all pointer members.

}

void TBBid24frm :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);
	AddKeyField((OSDataType *)&timstamp);
	AddKeyField((OSDataType *)&tratKey);
	AddKeyField((OSDataType *)&formno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&timstamp, icc_f.timstamp);
	AddField((OSDataType *)&tratKey, icc_f.tratKey);
	AddField((OSDataType *)&formno, icc_f.formno);
	AddField((OSDataType *)&sentdate, icc_f.sentdate);
	AddField((OSDataType *)&formstat, icc_f.formstat);
	AddField((OSDataType *)&formdays, icc_f.formdays);
	AddField((OSDataType *)&messRef, icc_f.messRef);
	AddField((OSDataType *)&signcode, icc_f.signcode);
	AddField((OSDataType *)&orderId, icc_f.orderId);
}

#include "TBBid24hou.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBBid24hou :: TBBid24hou () : Table(icc_t.Bid24hou, 9, 7, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBBid24hou :: TBBid24hou (const pTBBid24hou bid24hou) : Table ((Table *) bid24hou)
{
	AddAllFields();

	if (bid24hou)
	{
		if ( ! bid24hou->GetOpunKey().IsNull() )
			SetOpunKey(bid24hou->GetOpunKey());
		if ( ! bid24hou->GetTrcode()->IsNull() )
			SetTrcode(bid24hou->GetTrcode());
		if ( ! bid24hou->GetTimstamp()->IsNull() )
			SetTimstamp(bid24hou->GetTimstamp());
		if ( ! bid24hou->GetTratKey()->IsNull() )
			SetTratKey(bid24hou->GetTratKey());
		if ( ! bid24hou->GetFormno().IsNull() )
			SetFormno(bid24hou->GetFormno());
		if ( ! bid24hou->GetStartime()->IsNull() )
			SetStartime(bid24hou->GetStartime());
		if ( ! bid24hou->GetPrice().IsNull() )
			SetPrice(bid24hou->GetPrice());
		if ( ! bid24hou->GetBidquant().IsNull() )
			SetBidquant(bid24hou->GetBidquant());
		if ( ! bid24hou->GetEndtime()->IsNull() )
			SetEndtime(bid24hou->GetEndtime());
	}
}


TBBid24hou :: ~TBBid24hou ()
{
	// Delete all pointer members.

}

void TBBid24hou :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);
	AddKeyField((OSDataType *)&timstamp);
	AddKeyField((OSDataType *)&tratKey);
	AddKeyField((OSDataType *)&formno);
	AddKeyField((OSDataType *)&startime);
	AddKeyField((OSDataType *)&price);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&timstamp, icc_f.timstamp);
	AddField((OSDataType *)&tratKey, icc_f.tratKey);
	AddField((OSDataType *)&formno, icc_f.formno);
	AddField((OSDataType *)&startime, icc_f.startime);
	AddField((OSDataType *)&price, icc_f.price);
	AddField((OSDataType *)&bidquant, icc_f.bidquant);
	AddField((OSDataType *)&endtime, icc_f.endtime);
}

#include "TBBidrpfrm.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBBidrpfrm :: TBBidrpfrm () : Table(icc_t.Bidrpfrm, 10, 5, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBBidrpfrm :: TBBidrpfrm (const pTBBidrpfrm bidrpfrm) : Table ((Table *) bidrpfrm)
{
	AddAllFields();

	if (bidrpfrm)
	{
		if ( ! bidrpfrm->GetOpunKey().IsNull() )
			SetOpunKey(bidrpfrm->GetOpunKey());
		if ( ! bidrpfrm->GetBidtpKey().IsNull() )
			SetBidtpKey(bidrpfrm->GetBidtpKey());
		if ( ! bidrpfrm->GetPcodeKey().IsNull() )
			SetPcodeKey(bidrpfrm->GetPcodeKey());
		if ( ! bidrpfrm->GetTimstamp()->IsNull() )
			SetTimstamp(bidrpfrm->GetTimstamp());
		if ( ! bidrpfrm->GetFormno().IsNull() )
			SetFormno(bidrpfrm->GetFormno());
		if ( ! bidrpfrm->GetSentdate()->IsNull() )
			SetSentdate(bidrpfrm->GetSentdate());
		if ( ! bidrpfrm->GetFormstat().IsNull() )
			SetFormstat(bidrpfrm->GetFormstat());
		if ( ! bidrpfrm->GetMessRef()->IsNull() )
			SetMessRef(bidrpfrm->GetMessRef());
		if ( ! bidrpfrm->GetDescript()->IsNull() )
			SetDescript(bidrpfrm->GetDescript());
		if ( ! bidrpfrm->GetSigncode()->IsNull() )
			SetSigncode(bidrpfrm->GetSigncode());
	}
}


TBBidrpfrm :: ~TBBidrpfrm ()
{
	// Delete all pointer members.

}

void TBBidrpfrm :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&bidtpKey);
	AddKeyField((OSDataType *)&pcodeKey);
	AddKeyField((OSDataType *)&timstamp);
	AddKeyField((OSDataType *)&formno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&bidtpKey, icc_f.bidtpKey);
	AddField((OSDataType *)&pcodeKey, "PCODE_KEY");
	AddField((OSDataType *)&timstamp, icc_f.timstamp);
	AddField((OSDataType *)&formno, icc_f.formno);
	AddField((OSDataType *)&sentdate, icc_f.sentdate);
	AddField((OSDataType *)&formstat, icc_f.formstat);
	AddField((OSDataType *)&messRef, icc_f.messRef);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&signcode, icc_f.signcode);
}
#include "TBBreaker.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBBreaker :: TBBreaker () : Table(icc_t.Breaker, 1, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBBreaker :: TBBreaker (const pTBBreaker breaker) : Table ((Table *) breaker)
{
	AddAllFields();

	if (breaker)
	{
		if ( ! breaker->GetElecKey().IsNull() )
			SetElecKey(breaker->GetElecKey());
	}
}


TBBreaker :: ~TBBreaker ()
{
	// Delete all pointer members.

}

void TBBreaker :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&elecKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&elecKey, icc_f.elecKey);
}

#include "TBBusbar.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBBusbar :: TBBusbar () : Table(icc_t.Busbar, 4, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBBusbar :: TBBusbar (const pTBBusbar busbar) : Table ((Table *) busbar)
{
	AddAllFields();

	if (busbar)
	{
		if ( ! busbar->GetElecKey().IsNull() )
			SetElecKey(busbar->GetElecKey());
		if ( ! busbar->GetBusbarno().IsNull() )
			SetBusbarno(busbar->GetBusbarno());
		if ( ! busbar->GetBusbacod()->IsNull() )
			SetBusbacod(busbar->GetBusbacod());
		if ( ! busbar->GetElnoKey().IsNull() )
			SetElnoKey(busbar->GetElnoKey());
	}
}


TBBusbar :: ~TBBusbar ()
{
	// Delete all pointer members.

}

void TBBusbar :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&elecKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&elecKey, icc_f.elecKey);
	AddField((OSDataType *)&busbarno, icc_f.busbarno);
	AddField((OSDataType *)&busbacod, icc_f.busbacod);
	AddField((OSDataType *)&elnoKey, icc_f.elnoKey);
}

#include "TBBuyslTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBBuyslTp :: TBBuyslTp () : Table(icc_t.BuyslTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBBuyslTp :: TBBuyslTp (const pTBBuyslTp buyslTp) : Table ((Table *) buyslTp)
{
	AddAllFields();

	if (buyslTp)
	{
		if ( ! buyslTp->GetBuslKey().IsNull() )
			SetBuslKey(buyslTp->GetBuslKey());
		if ( ! buyslTp->GetCode()->IsNull() )
			SetCode(buyslTp->GetCode());
		if ( ! buyslTp->GetName()->IsNull() )
			SetName(buyslTp->GetName());
		if ( ! buyslTp->GetDescript()->IsNull() )
			SetDescript(buyslTp->GetDescript());
	}
}


TBBuyslTp :: ~TBBuyslTp ()
{
	// Delete all pointer members.

}

void TBBuyslTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&buslKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&buslKey, icc_f.buslKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBCatmarea.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBCatmarea :: TBCatmarea () : Table(icc_t.Catmarea, 14, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBCatmarea :: TBCatmarea (const pTBCatmarea catmarea) : Table ((Table *) catmarea)
{
	AddAllFields();

	if (catmarea)
	{
		if ( ! catmarea->GetHydKey().IsNull() )
			SetHydKey(catmarea->GetHydKey());
		if ( ! catmarea->GetCode()->IsNull() )
			SetCode(catmarea->GetCode());
		if ( ! catmarea->GetDescript()->IsNull() )
			SetDescript(catmarea->GetDescript());
		if ( ! catmarea->GetLowalttu().IsNull() )
			SetLowalttu(catmarea->GetLowalttu());
		if ( ! catmarea->GetHigalttu().IsNull() )
			SetHigalttu(catmarea->GetHigalttu());
		if ( ! catmarea->GetCatchare().IsNull() )
			SetCatchare(catmarea->GetCatchare());
		if ( ! catmarea->GetSeaarea().IsNull() )
			SetSeaarea(catmarea->GetSeaarea());
		if ( ! catmarea->GetRresarea().IsNull() )
			SetRresarea(catmarea->GetRresarea());
		if ( ! catmarea->GetMeaoutfl().IsNull() )
			SetMeaoutfl(catmarea->GetMeaoutfl());
		if ( ! catmarea->GetFirsyear().IsNull() )
			SetFirsyear(catmarea->GetFirsyear());
		if ( ! catmarea->GetLastyear().IsNull() )
			SetLastyear(catmarea->GetLastyear());
		if ( ! catmarea->GetCataKey().IsNull() )
			SetCataKey(catmarea->GetCataKey());
		if ( ! catmarea->GetAltitude().IsNull() )
			SetAltitude(catmarea->GetAltitude());
		if ( ! catmarea->GetOwnerPct().IsNull() )
			SetOwnerPct(catmarea->GetOwnerPct());
	}
}


TBCatmarea :: ~TBCatmarea ()
{
	// Delete all pointer members.

}

void TBCatmarea :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&hydKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&lowalttu, icc_f.lowalttu);
	AddField((OSDataType *)&higalttu, icc_f.higalttu);
	AddField((OSDataType *)&catchare, icc_f.catchare);
	AddField((OSDataType *)&seaarea, icc_f.seaarea);
	AddField((OSDataType *)&rresarea, icc_f.rresarea);
	AddField((OSDataType *)&meaoutfl, icc_f.meaoutfl);
	AddField((OSDataType *)&firsyear, icc_f.firsyear);
	AddField((OSDataType *)&lastyear, icc_f.lastyear);
	AddField((OSDataType *)&cataKey, icc_f.cataKey);
	AddField((OSDataType *)&altitude, icc_f.altitude);
	AddField((OSDataType *)&ownerPct, icc_f.ownerPct);
}

#include "TBCatmaTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBCatmaTp :: TBCatmaTp () : Table(icc_t.CatmaTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBCatmaTp :: TBCatmaTp (const pTBCatmaTp catmaTp) : Table ((Table *) catmaTp)
{
	AddAllFields();

	if (catmaTp)
	{
		if ( ! catmaTp->GetCataKey().IsNull() )
			SetCataKey(catmaTp->GetCataKey());
		if ( ! catmaTp->GetCode()->IsNull() )
			SetCode(catmaTp->GetCode());
		if ( ! catmaTp->GetName()->IsNull() )
			SetName(catmaTp->GetName());
		if ( ! catmaTp->GetDescript()->IsNull() )
			SetDescript(catmaTp->GetDescript());
	}
}


TBCatmaTp :: ~TBCatmaTp ()
{
	// Delete all pointer members.

}

void TBCatmaTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&cataKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&cataKey, icc_f.cataKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBCcmetTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBCcmetTp :: TBCcmetTp () : Table(icc_t.CcmetTp, 5, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBCcmetTp :: TBCcmetTp (const pTBCcmetTp ccmetTp) : Table ((Table *) ccmetTp)
{
	AddAllFields();

	if (ccmetTp)
	{
		if ( ! ccmetTp->GetCcmeKey().IsNull() )
			SetCcmeKey(ccmetTp->GetCcmeKey());
		if ( ! ccmetTp->GetCode()->IsNull() )
			SetCode(ccmetTp->GetCode());
		if ( ! ccmetTp->GetName()->IsNull() )
			SetName(ccmetTp->GetName());
		if ( ! ccmetTp->GetDescript()->IsNull() )
			SetDescript(ccmetTp->GetDescript());
		if ( ! ccmetTp->GetMethod().IsNull() )
			SetMethod(ccmetTp->GetMethod());
	}
}


TBCcmetTp :: ~TBCcmetTp ()
{
	// Delete all pointer members.

}

void TBCcmetTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&ccmeKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&ccmeKey, icc_f.ccmeKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&method, icc_f.method);
}

#include "TBCntper.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBCntper :: TBCntper () : Table(icc_t.Cntper, 4, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBCntper :: TBCntper (const pTBCntper cntper) : Table ((Table *) cntper)
{
	AddAllFields();

	if (cntper)
	{
		if ( ! cntper->GetOpunKey().IsNull() )
			SetOpunKey(cntper->GetOpunKey());
		if ( ! cntper->GetTrcode()->IsNull() )
			SetTrcode(cntper->GetTrcode());
		if ( ! cntper->GetStardate()->IsNull() )
			SetStardate(cntper->GetStardate());
		if ( ! cntper->GetEnddate()->IsNull() )
			SetEnddate(cntper->GetEnddate());
	}
}


TBCntper :: ~TBCntper ()
{
	// Delete all pointer members.

}

void TBCntper :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);
	AddKeyField((OSDataType *)&stardate);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&stardate, icc_f.stardate);
	AddField((OSDataType *)&enddate, icc_f.enddate);
}

#include "TBCntPers.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBCntPers :: TBCntPers () : Table(icc_t.CntPers, 11, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBCntPers :: TBCntPers (const pTBCntPers cntPers) : Table ((Table *) cntPers)
{
	AddAllFields();

	if (cntPers)
	{
		if ( ! cntPers->GetOpunKey().IsNull() )
			SetOpunKey(cntPers->GetOpunKey());
		if ( ! cntPers->GetPersNum().IsNull() )
			SetPersNum(cntPers->GetPersNum());
		if ( ! cntPers->GetName()->IsNull() )
			SetName(cntPers->GetName());
		if ( ! cntPers->GetPhonenr1()->IsNull() )
			SetPhonenr1(cntPers->GetPhonenr1());
		if ( ! cntPers->GetPhonenr2()->IsNull() )
			SetPhonenr2(cntPers->GetPhonenr2());
		if ( ! cntPers->GetFaxnumb()->IsNull() )
			SetFaxnumb(cntPers->GetFaxnumb());
		if ( ! cntPers->GetEmailAd()->IsNull() )
			SetEmailAd(cntPers->GetEmailAd());
		if ( ! cntPers->GetWorktask()->IsNull() )
			SetWorktask(cntPers->GetWorktask());
		if ( ! cntPers->GetInfo()->IsNull() )
			SetInfo(cntPers->GetInfo());
		if ( ! cntPers->GetGrupKey().IsNull() )
			SetGrupKey(cntPers->GetGrupKey());
		if ( ! cntPers->GetAddress()->IsNull() )
			SetAddress(cntPers->GetAddress());
	}
}


TBCntPers :: ~TBCntPers ()
{
	// Delete all pointer members.

}

void TBCntPers :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&persNum);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&persNum, icc_f.persNum);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&phonenr1, icc_f.phonenr1);
	AddField((OSDataType *)&phonenr2, icc_f.phonenr2);
	AddField((OSDataType *)&faxnumb, icc_f.faxnumb);
	AddField((OSDataType *)&emailAd, icc_f.emailAd);
	AddField((OSDataType *)&worktask, icc_f.worktask);
	AddField((OSDataType *)&info, icc_f.info);
	AddField((OSDataType *)&grupKey, icc_f.grupKey);
	AddField((OSDataType *)&address, icc_f.address);
}

#include "TBConstCo.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBConstCo :: TBConstCo () : Table(icc_t.ConstCo, 3, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBConstCo :: TBConstCo (const pTBConstCo constCo) : Table ((Table *) constCo)
{
	AddAllFields();

	if (constCo)
	{
		if ( ! constCo->GetCcmeKey().IsNull() )
			SetCcmeKey(constCo->GetCcmeKey());
		if ( ! constCo->GetTimsKey().IsNull() )
			SetTimsKey(constCo->GetTimsKey());
		if ( ! constCo->GetConstval().IsNull() )
			SetConstval(constCo->GetConstval());
	}
}


TBConstCo :: ~TBConstCo ()
{
	// Delete all pointer members.

}

void TBConstCo :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&ccmeKey);
	AddKeyField((OSDataType *)&timsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&ccmeKey, icc_f.ccmeKey);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&constval, icc_f.constval);
}

#include "TBContact.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBContact :: TBContact () : Table(icc_t.Contact, 7, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBContact :: TBContact (const pTBContact contact) : Table ((Table *) contact)
{
	AddAllFields();

	if (contact)
	{
		if ( ! contact->GetOpunKey().IsNull() )
			SetOpunKey(contact->GetOpunKey());
		if ( ! contact->GetTrcode()->IsNull() )
			SetTrcode(contact->GetTrcode());
		if ( ! contact->GetName()->IsNull() )
			SetName(contact->GetName());
		if ( ! contact->GetPhonenum()->IsNull() )
			SetPhonenum(contact->GetPhonenum());
		if ( ! contact->GetFaxnumb()->IsNull() )
			SetFaxnumb(contact->GetFaxnumb());
		if ( ! contact->GetField()->IsNull() )
			SetField(contact->GetField());
		if ( ! contact->GetEmail()->IsNull() )
			SetEmail(contact->GetEmail());
	}
}


TBContact :: ~TBContact ()
{
	// Delete all pointer members.

}

void TBContact :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);
	AddKeyField((OSDataType *)&name);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&phonenum, icc_f.phonenum);
	AddField((OSDataType *)&faxnumb, icc_f.faxnumb);
	AddField((OSDataType *)&field, icc_f.field);
	AddField((OSDataType *)&email, icc_f.email);
}

#include "TBContnode.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBContnode :: TBContnode () : Table(icc_t.Contnode, 4, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBContnode :: TBContnode (const pTBContnode contnode) : Table ((Table *) contnode)
{
	AddAllFields();

	if (contnode)
	{
		if ( ! contnode->GetOpunKey().IsNull() )
			SetOpunKey(contnode->GetOpunKey());
		if ( ! contnode->GetTrcode()->IsNull() )
			SetTrcode(contnode->GetTrcode());
		if ( ! contnode->GetCnPort().IsNull() )
			SetCnPort(contnode->GetCnPort());
		if ( ! contnode->GetSpot().IsNull() )
			SetSpot(contnode->GetSpot());
	}
}


TBContnode :: ~TBContnode ()
{
	// Delete all pointer members.

}

void TBContnode :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&cnPort, icc_f.cnPort);
	AddField((OSDataType *)&spot, icc_f.spot);
}

#include "TBContract.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBContract :: TBContract () : Table(icc_t.Contract, 28, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBContract :: TBContract (const pTBContract contract) : Table ((Table *) contract)
{
	AddAllFields();

	if (contract)
	{
		if ( ! contract->GetOpunKey().IsNull() )
			SetOpunKey(contract->GetOpunKey());
		if ( ! contract->GetTrcode()->IsNull() )
			SetTrcode(contract->GetTrcode());
		if ( ! contract->GetOffdate()->IsNull() )
			SetOffdate(contract->GetOffdate());
		if ( ! contract->GetOffdur()->IsNull() )
			SetOffdur(contract->GetOffdur());
		if ( ! contract->GetAgredate()->IsNull() )
			SetAgredate(contract->GetAgredate());
		if ( ! contract->GetEnddate()->IsNull() )
			SetEnddate(contract->GetEnddate());
		if ( ! contract->GetVioldate()->IsNull() )
			SetVioldate(contract->GetVioldate());
		if ( ! contract->GetPayPer().IsNull() )
			SetPayPer(contract->GetPayPer());
		if ( ! contract->GetInvAble().IsNull() )
			SetInvAble(contract->GetInvAble());
		if ( ! contract->GetExtraA()->IsNull() )
			SetExtraA(contract->GetExtraA());
		if ( ! contract->GetExtraB()->IsNull() )
			SetExtraB(contract->GetExtraB());
		if ( ! contract->GetExtraC()->IsNull() )
			SetExtraC(contract->GetExtraC());
		if ( ! contract->GetExtraD()->IsNull() )
			SetExtraD(contract->GetExtraD());
		if ( ! contract->GetContrNo().IsNull() )
			SetContrNo(contract->GetContrNo());
		if ( ! contract->GetPrioInv().IsNull() )
			SetPrioInv(contract->GetPrioInv());
		if ( ! contract->GetStardate()->IsNull() )
			SetStardate(contract->GetStardate());
		if ( ! contract->GetStopdate()->IsNull() )
			SetStopdate(contract->GetStopdate());
		if ( ! contract->GetIdxDate()->IsNull() )
			SetIdxDate(contract->GetIdxDate());
		if ( ! contract->GetAdjDate()->IsNull() )
			SetAdjDate(contract->GetAdjDate());
		if ( ! contract->GetAdjRef()->IsNull() )
			SetAdjRef(contract->GetAdjRef());
		if ( ! contract->GetNxtAdj()->IsNull() )
			SetNxtAdj(contract->GetNxtAdj());
		if ( ! contract->GetResolut()->IsNull() )
			SetResolut(contract->GetResolut());
		if ( ! contract->GetAcbaKey()->IsNull() )
			SetAcbaKey(contract->GetAcbaKey());
		if ( ! contract->GetCoseKey().IsNull() )
			SetCoseKey(contract->GetCoseKey());
		if ( ! contract->GetCstpKey().IsNull() )
			SetCstpKey(contract->GetCstpKey());
		if ( ! contract->GetCustKey().IsNull() )
			SetCustKey(contract->GetCustKey());
		if ( ! contract->GetCutpKey().IsNull() )
			SetCutpKey(contract->GetCutpKey());
		if ( ! contract->GetBuslKey().IsNull() )
			SetBuslKey(contract->GetBuslKey());
	}
}


TBContract :: ~TBContract ()
{
	// Delete all pointer members.

}

void TBContract :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&offdate, icc_f.offdate);
	AddField((OSDataType *)&offdur, icc_f.offdur);
	AddField((OSDataType *)&agredate, icc_f.agredate);
	AddField((OSDataType *)&enddate, icc_f.enddate);
	AddField((OSDataType *)&violdate, icc_f.violdate);
	AddField((OSDataType *)&payPer, icc_f.payPer);
	AddField((OSDataType *)&invAble, icc_f.invAble);
	AddField((OSDataType *)&extraA, icc_f.extraA);
	AddField((OSDataType *)&extraB, icc_f.extraB);
	AddField((OSDataType *)&extraC, icc_f.extraC);
	AddField((OSDataType *)&extraD, icc_f.extraD);
	AddField((OSDataType *)&contrNo, icc_f.contrNo);
	AddField((OSDataType *)&prioInv, icc_f.prioInv);
	AddField((OSDataType *)&stardate, icc_f.stardate);
	AddField((OSDataType *)&stopdate, icc_f.stopdate);
	AddField((OSDataType *)&idxDate, icc_f.idxDate);
	AddField((OSDataType *)&adjDate, icc_f.adjDate);
	AddField((OSDataType *)&adjRef, icc_f.adjRef);
	AddField((OSDataType *)&nxtAdj, icc_f.nxtAdj);
	AddField((OSDataType *)&resolut, icc_f.resolut);
	AddField((OSDataType *)&acbaKey, icc_f.acbaKey);
	AddField((OSDataType *)&coseKey, icc_f.coseKey);
	AddField((OSDataType *)&cstpKey, icc_f.cstpKey);
	AddField((OSDataType *)&custKey, icc_f.custKey);
	AddField((OSDataType *)&cutpKey, icc_f.cutpKey);
	AddField((OSDataType *)&buslKey, icc_f.buslKey);
}

#include "TBCosetTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBCosetTp :: TBCosetTp () : Table(icc_t.CosetTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBCosetTp :: TBCosetTp (const pTBCosetTp cosetTp) : Table ((Table *) cosetTp)
{
	AddAllFields();

	if (cosetTp)
	{
		if ( ! cosetTp->GetCoseKey().IsNull() )
			SetCoseKey(cosetTp->GetCoseKey());
		if ( ! cosetTp->GetCode()->IsNull() )
			SetCode(cosetTp->GetCode());
		if ( ! cosetTp->GetName()->IsNull() )
			SetName(cosetTp->GetName());
		if ( ! cosetTp->GetDescript()->IsNull() )
			SetDescript(cosetTp->GetDescript());
	}
}


TBCosetTp :: ~TBCosetTp ()
{
	// Delete all pointer members.

}

void TBCosetTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&coseKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&coseKey, icc_f.coseKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBCostaTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBCostaTp :: TBCostaTp () : Table(icc_t.CostaTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBCostaTp :: TBCostaTp (const pTBCostaTp costaTp) : Table ((Table *) costaTp)
{
	AddAllFields();

	if (costaTp)
	{
		if ( ! costaTp->GetCstpKey().IsNull() )
			SetCstpKey(costaTp->GetCstpKey());
		if ( ! costaTp->GetCode()->IsNull() )
			SetCode(costaTp->GetCode());
		if ( ! costaTp->GetName()->IsNull() )
			SetName(costaTp->GetName());
		if ( ! costaTp->GetDescript()->IsNull() )
			SetDescript(costaTp->GetDescript());
	}
}


TBCostaTp :: ~TBCostaTp ()
{
	// Delete all pointer members.

}

void TBCostaTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&cstpKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&cstpKey, icc_f.cstpKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBCounter.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBCounter :: TBCounter () : Table(icc_t.Counter, 7, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBCounter :: TBCounter (const pTBCounter counter) : Table ((Table *) counter)
{
	AddAllFields();

	if (counter)
	{
		if ( ! counter->GetOpunKey().IsNull() )
			SetOpunKey(counter->GetOpunKey());
		if ( ! counter->GetTrcode()->IsNull() )
			SetTrcode(counter->GetTrcode());
		if ( ! counter->GetPpmconst().IsNull() )
			SetPpmconst(counter->GetPpmconst());
		if ( ! counter->GetDeviaFl().IsNull() )
			SetDeviaFl(counter->GetDeviaFl());
		if ( ! counter->GetSmothFl().IsNull() )
			SetSmothFl(counter->GetSmothFl());
		if ( ! counter->GetUpperlim().IsNull() )
			SetUpperlim(counter->GetUpperlim());
		if ( ! counter->GetSmthPrm().IsNull() )
			SetSmthPrm(counter->GetSmthPrm());
	}
}


TBCounter :: ~TBCounter ()
{
	// Delete all pointer members.

}

void TBCounter :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&ppmconst, icc_f.ppmconst);
	AddField((OSDataType *)&deviaFl, icc_f.deviaFl);
	AddField((OSDataType *)&smothFl, icc_f.smothFl);
	AddField((OSDataType *)&upperlim, icc_f.upperlim);
	AddField((OSDataType *)&smthPrm, icc_f.smthPrm);
}

#include "TBCountry.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBCountry :: TBCountry () : Table(icc_t.Country, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBCountry :: TBCountry (const pTBCountry country) : Table ((Table *) country)
{
	AddAllFields();

	if (country)
	{
		if ( ! country->GetCountryKey().IsNull() )
			SetCountryKey(country->GetCountryKey());
		if ( ! country->GetCode()->IsNull() )
			SetCode(country->GetCode());
		if ( ! country->GetName()->IsNull() )
			SetName(country->GetName());
		if ( ! country->GetEdiCode()->IsNull() )
			SetEdiCode(country->GetEdiCode());
	}
}


TBCountry :: ~TBCountry ()
{
	// Delete all pointer members.

}

void TBCountry :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&countryKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&countryKey, icc_f.countryKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&ediCode, icc_f.ediCode);
}

#include "TBCouseTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBCouseTp :: TBCouseTp () : Table(icc_t.CouseTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBCouseTp :: TBCouseTp (const pTBCouseTp couseTp) : Table ((Table *) couseTp)
{
	AddAllFields();

	if (couseTp)
	{
		if ( ! couseTp->GetCutpKey().IsNull() )
			SetCutpKey(couseTp->GetCutpKey());
		if ( ! couseTp->GetCode()->IsNull() )
			SetCode(couseTp->GetCode());
		if ( ! couseTp->GetName()->IsNull() )
			SetName(couseTp->GetName());
		if ( ! couseTp->GetDescript()->IsNull() )
			SetDescript(couseTp->GetDescript());
	}
}


TBCouseTp :: ~TBCouseTp ()
{
	// Delete all pointer members.

}

void TBCouseTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&cutpKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&cutpKey, icc_f.cutpKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBCurrency.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBCurrency :: TBCurrency () : Table(icc_t.Currency, 4, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBCurrency :: TBCurrency (const pTBCurrency currency) : Table ((Table *) currency)
{
	AddAllFields();

	if (currency)
	{
		if ( ! currency->GetOpunKey().IsNull() )
			SetOpunKey(currency->GetOpunKey());
		if ( ! currency->GetTrcode()->IsNull() )
			SetTrcode(currency->GetTrcode());
		if ( ! currency->GetCurrGr()->IsNull() )
			SetCurrGr(currency->GetCurrGr());
		if ( ! currency->GetCurrFac().IsNull() )
			SetCurrFac(currency->GetCurrFac());
	}
}


TBCurrency :: ~TBCurrency ()
{
	// Delete all pointer members.

}

void TBCurrency :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&currGr, icc_f.currGr);
	AddField((OSDataType *)&currFac, icc_f.currFac);
}

#include "TBDebitTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBDebitTp :: TBDebitTp () : Table(icc_t.DebitTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBDebitTp :: TBDebitTp (const pTBDebitTp debitTp) : Table ((Table *) debitTp)
{
	AddAllFields();

	if (debitTp)
	{
		if ( ! debitTp->GetDebtKey().IsNull() )
			SetDebtKey(debitTp->GetDebtKey());
		if ( ! debitTp->GetCode()->IsNull() )
			SetCode(debitTp->GetCode());
		if ( ! debitTp->GetName()->IsNull() )
			SetName(debitTp->GetName());
		if ( ! debitTp->GetDescript()->IsNull() )
			SetDescript(debitTp->GetDescript());
	}
}


TBDebitTp :: ~TBDebitTp ()
{
	// Delete all pointer members.

}

void TBDebitTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&debtKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&debtKey, icc_f.debtKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBDebitsys.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBDebitsys :: TBDebitsys () : Table(icc_t.Debitsys, 4, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBDebitsys :: TBDebitsys (const pTBDebitsys debitsys) : Table ((Table *) debitsys)
{
	AddAllFields();

	if (debitsys)
	{
		if ( ! debitsys->GetOpunKey().IsNull() )
			SetOpunKey(debitsys->GetOpunKey());
		if ( ! debitsys->GetDebsKey().IsNull() )
			SetDebsKey(debitsys->GetDebsKey());
		if ( ! debitsys->GetName()->IsNull() )
			SetName(debitsys->GetName());
		if ( ! debitsys->GetExpPath()->IsNull() )
			SetExpPath(debitsys->GetExpPath());
	}
}


TBDebitsys :: ~TBDebitsys ()
{
	// Delete all pointer members.

}

void TBDebitsys :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&debsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&debsKey, icc_f.debsKey);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&expPath, icc_f.expPath);
}

#include "TBDebssTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBDebssTp :: TBDebssTp () : Table(icc_t.DebssTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBDebssTp :: TBDebssTp (const pTBDebssTp debssTp) : Table ((Table *) debssTp)
{
	AddAllFields();

	if (debssTp)
	{
		if ( ! debssTp->GetDebsKey().IsNull() )
			SetDebsKey(debssTp->GetDebsKey());
		if ( ! debssTp->GetCode()->IsNull() )
			SetCode(debssTp->GetCode());
		if ( ! debssTp->GetName()->IsNull() )
			SetName(debssTp->GetName());
		if ( ! debssTp->GetDescript()->IsNull() )
			SetDescript(debssTp->GetDescript());
	}
}


TBDebssTp :: ~TBDebssTp ()
{
	// Delete all pointer members.

}

void TBDebssTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&debsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&debsKey, icc_f.debsKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBDelcobal.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBDelcobal :: TBDelcobal () : Table(icc_t.Delcobal, 5, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBDelcobal :: TBDelcobal (const pTBDelcobal delcobal) : Table ((Table *) delcobal)
{
	AddAllFields();

	if (delcobal)
	{
		if ( ! delcobal->GetDelResp().IsNull() )
			SetDelResp(delcobal->GetDelResp());
		if ( ! delcobal->GetBalResp().IsNull() )
			SetBalResp(delcobal->GetBalResp());
		if ( ! delcobal->GetSeqno().IsNull() )
			SetSeqno(delcobal->GetSeqno());
		if ( ! delcobal->GetValftime()->IsNull() )
			SetValftime(delcobal->GetValftime());
		if ( ! delcobal->GetValttime()->IsNull() )
			SetValttime(delcobal->GetValttime());
	}
}


TBDelcobal :: ~TBDelcobal ()
{
	// Delete all pointer members.

}

void TBDelcobal :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&delResp);
	AddKeyField((OSDataType *)&balResp);
	AddKeyField((OSDataType *)&seqno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&delResp, icc_f.delResp);
	AddField((OSDataType *)&balResp, icc_f.balResp);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&valftime, icc_f.valftime);
	AddField((OSDataType *)&valttime, icc_f.valttime);
}

#include "TBDelivagm.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBDelivagm :: TBDelivagm () : Table(icc_t.Delivagm, 11, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBDelivagm :: TBDelivagm (const pTBDelivagm delivagm) : Table ((Table *) delivagm)
{
	AddAllFields();

	if (delivagm)
	{
		if ( ! delivagm->GetDagmKey().IsNull() )
			SetDagmKey(delivagm->GetDagmKey());
		if ( ! delivagm->GetDagmpart().IsNull() )
			SetDagmpart(delivagm->GetDagmpart());
		if ( ! delivagm->GetName()->IsNull() )
			SetName(delivagm->GetName());
		if ( ! delivagm->GetPPower().IsNull() )
			SetPPower(delivagm->GetPPower());
		if ( ! delivagm->GetQPower().IsNull() )
			SetQPower(delivagm->GetQPower());
		if ( ! delivagm->GetOpunKey().IsNull() )
			SetOpunKey(delivagm->GetOpunKey());
		if ( ! delivagm->GetPartNum().IsNull() )
			SetPartNum(delivagm->GetPartNum());
		if ( ! delivagm->GetGrupKey().IsNull() )
			SetGrupKey(delivagm->GetGrupKey());
		if ( ! delivagm->GetDelResp().IsNull() )
			SetDelResp(delivagm->GetDelResp());
		if ( ! delivagm->GetTarfKey().IsNull() )
			SetTarfKey(delivagm->GetTarfKey());
		if ( ! delivagm->GetSeqno().IsNull() )
			SetSeqno(delivagm->GetSeqno());
	}
}


TBDelivagm :: ~TBDelivagm ()
{
	// Delete all pointer members.

}

void TBDelivagm :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&dagmKey);
	AddKeyField((OSDataType *)&dagmpart);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&dagmKey, icc_f.dagmKey);
	AddField((OSDataType *)&dagmpart, icc_f.dagmpart);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&pPower, icc_f.pPower);
	AddField((OSDataType *)&qPower, icc_f.qPower);
	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&partNum, icc_f.partNum);
	AddField((OSDataType *)&grupKey, icc_f.grupKey);
	AddField((OSDataType *)&delResp, icc_f.delResp);
	AddField((OSDataType *)&tarfKey, icc_f.tarfKey);
	AddField((OSDataType *)&seqno, icc_f.seqno);
}

#include "TBDirecTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBDirecTp :: TBDirecTp () : Table(icc_t.DirecTp, 3, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBDirecTp :: TBDirecTp (const pTBDirecTp direcTp) : Table ((Table *) direcTp)
{
	AddAllFields();

	if (direcTp)
	{
		if ( ! direcTp->GetDircKey()->IsNull() )
			SetDircKey(direcTp->GetDircKey());
		if ( ! direcTp->GetEnumval().IsNull() )
			SetEnumval(direcTp->GetEnumval());
		if ( ! direcTp->GetName()->IsNull() )
			SetName(direcTp->GetName());
	}
}


TBDirecTp :: ~TBDirecTp ()
{
	// Delete all pointer members.

}

void TBDirecTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&dircKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&dircKey, icc_f.dircKey);
	AddField((OSDataType *)&enumval, icc_f.enumval);
	AddField((OSDataType *)&name, icc_f.name);
}

#include "TBDoctext.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBDoctext :: TBDoctext () : Table(icc_t.Doctext, 3, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBDoctext :: TBDoctext (const pTBDoctext doctext) : Table ((Table *) doctext)
{
	AddAllFields();

	if (doctext)
	{
		if ( ! doctext->GetDocindex().IsNull() )
			SetDocindex(doctext->GetDocindex());
		if ( ! doctext->GetLinenumb().IsNull() )
			SetLinenumb(doctext->GetLinenumb());
		if ( ! doctext->GetDescript()->IsNull() )
			SetDescript(doctext->GetDescript());
	}
}


TBDoctext :: ~TBDoctext ()
{
	// Delete all pointer members.

}

void TBDoctext :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&docindex);
	AddKeyField((OSDataType *)&linenumb);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&docindex, icc_f.docindex);
	AddField((OSDataType *)&linenumb, icc_f.linenumb);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBDocument.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBDocument :: TBDocument () : Table(icc_t.Document, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBDocument :: TBDocument (const pTBDocument document) : Table ((Table *) document)
{
	AddAllFields();

	if (document)
	{
		if ( ! document->GetDocindex().IsNull() )
			SetDocindex(document->GetDocindex());
		if ( ! document->GetCode()->IsNull() )
			SetCode(document->GetCode());
		if ( ! document->GetName()->IsNull() )
			SetName(document->GetName());
		if ( ! document->GetDescript()->IsNull() )
			SetDescript(document->GetDescript());
	}
}


TBDocument :: ~TBDocument ()
{
	// Delete all pointer members.

}

void TBDocument :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&docindex);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&docindex, icc_f.docindex);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBElccoelc.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBElccoelc :: TBElccoelc () : Table(icc_t.Elccoelc, 2, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBElccoelc :: TBElccoelc (const pTBElccoelc elccoelc) : Table ((Table *) elccoelc)
{
	AddAllFields();

	if (elccoelc)
	{
		if ( ! elccoelc->GetElKey1().IsNull() )
			SetElKey1(elccoelc->GetElKey1());
		if ( ! elccoelc->GetElKey2().IsNull() )
			SetElKey2(elccoelc->GetElKey2());
	}
}


TBElccoelc :: ~TBElccoelc ()
{
	// Delete all pointer members.

}

void TBElccoelc :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&elKey1);
	AddKeyField((OSDataType *)&elKey2);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&elKey1, icc_f.elKey1);
	AddField((OSDataType *)&elKey2, icc_f.elKey2);
}

#include "TBElcmpTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBElcmpTp :: TBElcmpTp () : Table(icc_t.ElcmpTp, 4, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBElcmpTp :: TBElcmpTp (const pTBElcmpTp elcmpTp) : Table ((Table *) elcmpTp)
{
	AddAllFields();

	if (elcmpTp)
	{
		if ( ! elcmpTp->GetElcmKey()->IsNull() )
			SetElcmKey(elcmpTp->GetElcmKey());
		if ( ! elcmpTp->GetEnumval().IsNull() )
			SetEnumval(elcmpTp->GetEnumval());
		if ( ! elcmpTp->GetName()->IsNull() )
			SetName(elcmpTp->GetName());
		if ( ! elcmpTp->GetDescript()->IsNull() )
			SetDescript(elcmpTp->GetDescript());
	}
}


TBElcmpTp :: ~TBElcmpTp ()
{
	// Delete all pointer members.

}

void TBElcmpTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&elcmKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&elcmKey, icc_f.elcmKey);
	AddField((OSDataType *)&enumval, icc_f.enumval);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBEleccomp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBEleccomp :: TBEleccomp () : Table(icc_t.Eleccomp, 7, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBEleccomp :: TBEleccomp (const pTBEleccomp eleccomp) : Table ((Table *) eleccomp)
{
	AddAllFields();

	if (eleccomp)
	{
		if ( ! eleccomp->GetElecKey().IsNull() )
			SetElecKey(eleccomp->GetElecKey());
		if ( ! eleccomp->GetName()->IsNull() )
			SetName(eleccomp->GetName());
		if ( ! eleccomp->GetInstdate().IsNull() )
			SetInstdate(eleccomp->GetInstdate());
		if ( ! eleccomp->GetRemvdate().IsNull() )
			SetRemvdate(eleccomp->GetRemvdate());
		if ( ! eleccomp->GetBasevolt().IsNull() )
			SetBasevolt(eleccomp->GetBasevolt());
		if ( ! eleccomp->GetNetlevel().IsNull() )
			SetNetlevel(eleccomp->GetNetlevel());
		if ( ! eleccomp->GetElcmKey()->IsNull() )
			SetElcmKey(eleccomp->GetElcmKey());
	}
}


TBEleccomp :: ~TBEleccomp ()
{
	// Delete all pointer members.

}

void TBEleccomp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&elecKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&elecKey, icc_f.elecKey);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&instdate, icc_f.instdate);
	AddField((OSDataType *)&remvdate, icc_f.remvdate);
	AddField((OSDataType *)&basevolt, icc_f.basevolt);
	AddField((OSDataType *)&netlevel, icc_f.netlevel);
	AddField((OSDataType *)&elcmKey, icc_f.elcmKey);
}

#include "TBElecnode.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBElecnode :: TBElecnode () : Table(icc_t.Elecnode, 7, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBElecnode :: TBElecnode (const pTBElecnode elecnode) : Table ((Table *) elecnode)
{
	AddAllFields();

	if (elecnode)
	{
		if ( ! elecnode->GetElnoKey().IsNull() )
			SetElnoKey(elecnode->GetElnoKey());
		if ( ! elecnode->GetElnodnum().IsNull() )
			SetElnodnum(elecnode->GetElnodnum());
		if ( ! elecnode->GetName()->IsNull() )
			SetName(elecnode->GetName());
		if ( ! elecnode->GetDescript()->IsNull() )
			SetDescript(elecnode->GetDescript());
		if ( ! elecnode->GetBasevolt().IsNull() )
			SetBasevolt(elecnode->GetBasevolt());
		if ( ! elecnode->GetMinvltlm().IsNull() )
			SetMinvltlm(elecnode->GetMinvltlm());
		if ( ! elecnode->GetMaxvltlm().IsNull() )
			SetMaxvltlm(elecnode->GetMaxvltlm());
	}
}


TBElecnode :: ~TBElecnode ()
{
	// Delete all pointer members.

}

void TBElecnode :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&elnoKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&elnoKey, icc_f.elnoKey);
	AddField((OSDataType *)&elnodnum, icc_f.elnodnum);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&basevolt, icc_f.basevolt);
	AddField((OSDataType *)&minvltlm, icc_f.minvltlm);
	AddField((OSDataType *)&maxvltlm, icc_f.maxvltlm);
}

#include "TBElspotBlocksTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBElspotBlocksTp :: TBElspotBlocksTp () : Table(icc_t.ElspotBlocksTp, 5, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBElspotBlocksTp :: TBElspotBlocksTp (const pTBElspotBlocksTp elspotBlocksTp) : Table ((Table *) elspotBlocksTp)
{
	AddAllFields();

	if (elspotBlocksTp)
	{
		if ( ! elspotBlocksTp->GetBlockno().IsNull() )
			SetBlockno(elspotBlocksTp->GetBlockno());
		if ( ! elspotBlocksTp->GetCode()->IsNull() )
			SetCode(elspotBlocksTp->GetCode());
		if ( ! elspotBlocksTp->GetName()->IsNull() )
			SetName(elspotBlocksTp->GetName());
		if ( ! elspotBlocksTp->GetFromHour().IsNull() )
			SetFromHour(elspotBlocksTp->GetFromHour());
		if ( ! elspotBlocksTp->GetToHour().IsNull() )
			SetToHour(elspotBlocksTp->GetToHour());
	}
}


TBElspotBlocksTp :: ~TBElspotBlocksTp ()
{
	// Delete all pointer members.

}

void TBElspotBlocksTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&blockno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&blockno, icc_f.blockno);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&fromHour, icc_f.fromHour);
	AddField((OSDataType *)&toHour, icc_f.toHour);
}

#include "TBExchpTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExchpTp :: TBExchpTp () : Table(icc_t.ExchpTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBExchpTp :: TBExchpTp (const pTBExchpTp exchpTp) : Table ((Table *) exchpTp)
{
	AddAllFields();

	if (exchpTp)
	{
		if ( ! exchpTp->GetExchTyp().IsNull() )
			SetExchTyp(exchpTp->GetExchTyp());
		if ( ! exchpTp->GetCode()->IsNull() )
			SetCode(exchpTp->GetCode());
		if ( ! exchpTp->GetName()->IsNull() )
			SetName(exchpTp->GetName());
		if ( ! exchpTp->GetDescript()->IsNull() )
			SetDescript(exchpTp->GetDescript());
	}
}


TBExchpTp :: ~TBExchpTp ()
{
	// Delete all pointer members.

}

void TBExchpTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&exchTyp);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&exchTyp, icc_f.exchTyp);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBExchPnt.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExchPnt :: TBExchPnt () : Table(icc_t.ExchPnt, 16, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBExchPnt :: TBExchPnt (const pTBExchPnt exchPnt) : Table ((Table *) exchPnt)
{
	AddAllFields();

	if (exchPnt)
	{
		if ( ! exchPnt->GetExchKey().IsNull() )
			SetExchKey(exchPnt->GetExchKey());
		if ( ! exchPnt->GetSeqno().IsNull() )
			SetSeqno(exchPnt->GetSeqno());
		if ( ! exchPnt->GetName()->IsNull() )
			SetName(exchPnt->GetName());
		if ( ! exchPnt->GetShorname()->IsNull() )
			SetShorname(exchPnt->GetShorname());
		if ( ! exchPnt->GetValftime()->IsNull() )
			SetValftime(exchPnt->GetValftime());
		if ( ! exchPnt->GetValttime()->IsNull() )
			SetValttime(exchPnt->GetValttime());
		if ( ! exchPnt->GetInfo()->IsNull() )
			SetInfo(exchPnt->GetInfo());
		if ( ! exchPnt->GetCre1008().IsNull() )
			SetCre1008(exchPnt->GetCre1008());
		if ( ! exchPnt->GetExchTyp().IsNull() )
			SetExchTyp(exchPnt->GetExchTyp());
		if ( ! exchPnt->GetGrupKey().IsNull() )
			SetGrupKey(exchPnt->GetGrupKey());
		if ( ! exchPnt->GetNetaKey().IsNull() )
			SetNetaKey(exchPnt->GetNetaKey());
		if ( ! exchPnt->GetSeqnoeee().IsNull() )
			SetSeqnoeee(exchPnt->GetSeqnoeee());
		if ( ! exchPnt->GetNetbKey().IsNull() )
			SetNetbKey(exchPnt->GetNetbKey());
		if ( ! exchPnt->GetSeqnoeea().IsNull() )
			SetSeqnoeea(exchPnt->GetSeqnoeea());
		if ( ! exchPnt->GetDelResp().IsNull() )
			SetDelResp(exchPnt->GetDelResp());
		if ( ! exchPnt->GetDescript()->IsNull() )
			SetDescript(exchPnt->GetDescript());
	}
}


TBExchPnt :: ~TBExchPnt ()
{
	// Delete all pointer members.

}

void TBExchPnt :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&exchKey);
	AddKeyField((OSDataType *)&seqno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&exchKey, icc_f.exchKey);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&shorname, icc_f.shorname);
	AddField((OSDataType *)&valftime, icc_f.valftime);
	AddField((OSDataType *)&valttime, icc_f.valttime);
	AddField((OSDataType *)&info, icc_f.info);
	AddField((OSDataType *)&cre1008, icc_f.cre1008);
	AddField((OSDataType *)&exchTyp, icc_f.exchTyp);
	AddField((OSDataType *)&grupKey, icc_f.grupKey);
	AddField((OSDataType *)&netaKey, icc_f.netaKey);
	AddField((OSDataType *)&seqnoeee, icc_f.seqnoeee);
	AddField((OSDataType *)&netbKey, icc_f.netbKey);
	AddField((OSDataType *)&seqnoeea, icc_f.seqnoeea);
	AddField((OSDataType *)&delResp, icc_f.delResp);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBExcpfact.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExcpfact :: TBExcpfact () : Table(icc_t.Excpfact, 2, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBExcpfact :: TBExcpfact (const pTBExcpfact excpfact) : Table ((Table *) excpfact)
{
	AddAllFields();

	if (excpfact)
	{
		if ( ! excpfact->GetEfctKey().IsNull() )
			SetEfctKey(excpfact->GetEfctKey());
		if ( ! excpfact->GetName()->IsNull() )
			SetName(excpfact->GetName());
	}
}


TBExcpfact :: ~TBExcpfact ()
{
	// Delete all pointer members.

}

void TBExcpfact :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&efctKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&efctKey, icc_f.efctKey);
	AddField((OSDataType *)&name, icc_f.name);
}

#include "TBExchtask.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExchtask :: TBExchtask () : Table(icc_t.Exchtask, 12, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBExchtask :: TBExchtask (const pTBExchtask exchtask) : Table ((Table *) exchtask)
{
	AddAllFields();

	if (exchtask)
	{
		if ( ! exchtask->GetExmtKey().IsNull() )
			SetExmtKey(exchtask->GetExmtKey());
		if ( ! exchtask->GetSeqno().IsNull() )
			SetSeqno(exchtask->GetSeqno());
		if ( ! exchtask->GetUnit().IsNull() )
			SetUnit(exchtask->GetUnit());
		if ( ! exchtask->GetValtran().IsNull() )
			SetValtran(exchtask->GetValtran());
		if ( ! exchtask->GetStartime()->IsNull() )
			SetStartime(exchtask->GetStartime());
		if ( ! exchtask->GetFromtime()->IsNull() )
			SetFromtime(exchtask->GetFromtime());
		if ( ! exchtask->GetTotime()->IsNull() )
			SetTotime(exchtask->GetTotime());
		if ( ! exchtask->GetValftime()->IsNull() )
			SetValftime(exchtask->GetValftime());
		if ( ! exchtask->GetValttime()->IsNull() )
			SetValttime(exchtask->GetValttime());
		if ( ! exchtask->GetIccSender().IsNull() )
			SetIccSender(exchtask->GetIccSender());
		if ( ! exchtask->GetRunInNormalTime().IsNull() )
			SetRunInNormalTime(exchtask->GetRunInNormalTime());
		if ( ! exchtask->GetExportDatesInNormalTime().IsNull() )
			SetExportDatesInNormalTime(exchtask->GetExportDatesInNormalTime());
	}
}


TBExchtask :: ~TBExchtask ()
{
	// Delete all pointer members.

}

void TBExchtask :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&exmtKey);
	AddKeyField((OSDataType *)&seqno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&exmtKey, icc_f.exmtKey);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&unit, icc_f.unit);
	AddField((OSDataType *)&valtran, icc_f.valtran);
	AddField((OSDataType *)&startime, icc_f.startime);
	AddField((OSDataType *)&fromtime, icc_f.fromtime);
	AddField((OSDataType *)&totime, icc_f.totime);
	AddField((OSDataType *)&valftime, icc_f.valftime);
	AddField((OSDataType *)&valttime, icc_f.valttime);
	AddField((OSDataType *)&iccSender, icc_f.iccSender);
	AddField((OSDataType *)&runInNormalTime, icc_f.runInNormalTime);
	AddField((OSDataType *)&exportDatesInNormalTime, icc_f.exportDatesInNormalTime);
}

#include "TBFieldsta.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBFieldsta :: TBFieldsta () : Table(icc_t.Fieldsta, 16, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBFieldsta :: TBFieldsta (const pTBFieldsta fieldsta) : Table ((Table *) fieldsta)
{
	AddAllFields();

	if (fieldsta)
	{
		if ( ! fieldsta->GetFcno().IsNull() )
			SetFcno(fieldsta->GetFcno());
		if ( ! fieldsta->GetFsno().IsNull() )
			SetFsno(fieldsta->GetFsno());
		if ( ! fieldsta->GetCode()->IsNull() )
			SetCode(fieldsta->GetCode());
		if ( ! fieldsta->GetName()->IsNull() )
			SetName(fieldsta->GetName());
		if ( ! fieldsta->GetDescript()->IsNull() )
			SetDescript(fieldsta->GetDescript());
		if ( ! fieldsta->GetDateesta()->IsNull() )
			SetDateesta(fieldsta->GetDateesta());
		if ( ! fieldsta->GetDateclos()->IsNull() )
			SetDateclos(fieldsta->GetDateclos());
		if ( ! fieldsta->GetBloccode().IsNull() )
			SetBloccode(fieldsta->GetBloccode());
		if ( ! fieldsta->GetTestmode().IsNull() )
			SetTestmode(fieldsta->GetTestmode());
		if ( ! fieldsta->GetUtmEast().IsNull() )
			SetUtmEast(fieldsta->GetUtmEast());
		if ( ! fieldsta->GetUtmNort().IsNull() )
			SetUtmNort(fieldsta->GetUtmNort());
		if ( ! fieldsta->GetAltitude().IsNull() )
			SetAltitude(fieldsta->GetAltitude());
		if ( ! fieldsta->GetFiesKey().IsNull() )
			SetFiesKey(fieldsta->GetFiesKey());
		if ( ! fieldsta->GetHydKey().IsNull() )
			SetHydKey(fieldsta->GetHydKey());
		if ( ! fieldsta->GetMireKey().IsNull() )
			SetMireKey(fieldsta->GetMireKey());
		if ( ! fieldsta->GetWatcKey().IsNull() )
			SetWatcKey(fieldsta->GetWatcKey());
	}
}


TBFieldsta :: ~TBFieldsta ()
{
	// Delete all pointer members.

}

void TBFieldsta :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&fcno);
	AddKeyField((OSDataType *)&fsno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&fcno, icc_f.fcno);
	AddField((OSDataType *)&fsno, icc_f.fsno);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&dateesta, icc_f.dateesta);
	AddField((OSDataType *)&dateclos, icc_f.dateclos);
	AddField((OSDataType *)&bloccode, icc_f.bloccode);
	AddField((OSDataType *)&testmode, icc_f.testmode);
	AddField((OSDataType *)&utmEast, icc_f.utmEast);
	AddField((OSDataType *)&utmNort, icc_f.utmNort);
	AddField((OSDataType *)&altitude, icc_f.altitude);
	AddField((OSDataType *)&fiesKey, icc_f.fiesKey);
	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&mireKey, icc_f.mireKey);
	AddField((OSDataType *)&watcKey, icc_f.watcKey);
}

#include "TBFistcent.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBFistcent :: TBFistcent () : Table(icc_t.Fistcent, 18, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBFistcent :: TBFistcent (const pTBFistcent fistcent) : Table ((Table *) fistcent)
{
	AddAllFields();

	if (fistcent)
	{
		if ( ! fistcent->GetFcno().IsNull() )
			SetFcno(fistcent->GetFcno());
		if ( ! fistcent->GetCode()->IsNull() )
			SetCode(fistcent->GetCode());
		if ( ! fistcent->GetName()->IsNull() )
			SetName(fistcent->GetName());
		if ( ! fistcent->GetDescript()->IsNull() )
			SetDescript(fistcent->GetDescript());
		if ( ! fistcent->GetDateesta()->IsNull() )
			SetDateesta(fistcent->GetDateesta());
		if ( ! fistcent->GetPhonenum()->IsNull() )
			SetPhonenum(fistcent->GetPhonenum());
		if ( ! fistcent->GetBloccode().IsNull() )
			SetBloccode(fistcent->GetBloccode());
		if ( ! fistcent->GetTestmode().IsNull() )
			SetTestmode(fistcent->GetTestmode());
		if ( ! fistcent->GetMxconrep().IsNull() )
			SetMxconrep(fistcent->GetMxconrep());
		if ( ! fistcent->GetNumbmeas().IsNull() )
			SetNumbmeas(fistcent->GetNumbmeas());
		if ( ! fistcent->GetTimelag().IsNull() )
			SetTimelag(fistcent->GetTimelag());
		if ( ! fistcent->GetDaylight()->IsNull() )
			SetDaylight(fistcent->GetDaylight());
		if ( ! fistcent->GetDalastme()->IsNull() )
			SetDalastme(fistcent->GetDalastme());
		if ( ! fistcent->GetEdcuseid().IsNull() )
			SetEdcuseid(fistcent->GetEdcuseid());
		if ( ! fistcent->GetEdcpassw().IsNull() )
			SetEdcpassw(fistcent->GetEdcpassw());
		if ( ! fistcent->GetEdcowner().IsNull() )
			SetEdcowner(fistcent->GetEdcowner());
		if ( ! fistcent->GetEdcvers().IsNull() )
			SetEdcvers(fistcent->GetEdcvers());
		if ( ! fistcent->GetOpunKey().IsNull() )
			SetOpunKey(fistcent->GetOpunKey());
	}
}


TBFistcent :: ~TBFistcent ()
{
	// Delete all pointer members.

}

void TBFistcent :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&fcno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&fcno, icc_f.fcno);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&dateesta, icc_f.dateesta);
	AddField((OSDataType *)&phonenum, icc_f.phonenum);
	AddField((OSDataType *)&bloccode, icc_f.bloccode);
	AddField((OSDataType *)&testmode, icc_f.testmode);
	AddField((OSDataType *)&mxconrep, icc_f.mxconrep);
	AddField((OSDataType *)&numbmeas, icc_f.numbmeas);
	AddField((OSDataType *)&timelag, icc_f.timelag);
	AddField((OSDataType *)&daylight, icc_f.daylight);
	AddField((OSDataType *)&dalastme, icc_f.dalastme);
	AddField((OSDataType *)&edcuseid, icc_f.edcuseid);
	AddField((OSDataType *)&edcpassw, icc_f.edcpassw);
	AddField((OSDataType *)&edcowner, icc_f.edcowner);
	AddField((OSDataType *)&edcvers, icc_f.edcvers);
	AddField((OSDataType *)&opunKey, icc_f.opunKey);
}

#include "TBFngroup.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBFngroup :: TBFngroup () : Table(icc_t.Fngroup, 4, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBFngroup :: TBFngroup (const pTBFngroup fngroup) : Table ((Table *) fngroup)
{
	AddAllFields();

	if (fngroup)
	{
		if ( ! fngroup->GetFngrNam()->IsNull() )
			SetFngrNam(fngroup->GetFngrNam());
		if ( ! fngroup->GetProgKey().IsNull() )
			SetProgKey(fngroup->GetProgKey());
		if ( ! fngroup->GetFngrNum().IsNull() )
			SetFngrNum(fngroup->GetFngrNum());
		if ( ! fngroup->GetDescript()->IsNull() )
			SetDescript(fngroup->GetDescript());
	}
}


TBFngroup :: ~TBFngroup ()
{
	// Delete all pointer members.

}

void TBFngroup :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&fngrNam);
	AddKeyField((OSDataType *)&progKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&fngrNam, icc_f.fngrNam);
	AddField((OSDataType *)&progKey, icc_f.progKey);
	AddField((OSDataType *)&fngrNum, icc_f.fngrNum);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBFuncDs.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBFuncDs :: TBFuncDs () : Table(icc_t.FuncDs, 8, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBFuncDs :: TBFuncDs (const pTBFuncDs funcDs) : Table ((Table *) funcDs)
{
	AddAllFields();

	if (funcDs)
	{
		if ( ! funcDs->GetFunKey().IsNull() )
			SetFunKey(funcDs->GetFunKey());
		if ( ! funcDs->GetCode()->IsNull() )
			SetCode(funcDs->GetCode());
		if ( ! funcDs->GetName()->IsNull() )
			SetName(funcDs->GetName());
		if ( ! funcDs->GetEsttime()->IsNull() )
			SetEsttime(funcDs->GetEsttime());
		if ( ! funcDs->GetOpname()->IsNull() )
			SetOpname(funcDs->GetOpname());
		if ( ! funcDs->GetDescript()->IsNull() )
			SetDescript(funcDs->GetDescript());
		if ( ! funcDs->GetVersion().IsNull() )
			SetVersion(funcDs->GetVersion());
		if ( ! funcDs->GetProgKey().IsNull() )
			SetProgKey(funcDs->GetProgKey());
	}
}


TBFuncDs :: ~TBFuncDs ()
{
	// Delete all pointer members.

}

void TBFuncDs :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&funKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&funKey, icc_f.funKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&esttime, icc_f.esttime);
	AddField((OSDataType *)&opname, icc_f.opname);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&version, icc_f.version);
	AddField((OSDataType *)&progKey, icc_f.progKey);
}

#include "TBGenerato.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBGenerato :: TBGenerato () : Table(icc_t.Generato, 10, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBGenerato :: TBGenerato (const pTBGenerato generato) : Table ((Table *) generato)
{
	AddAllFields();

	if (generato)
	{
		if ( ! generato->GetElecKey().IsNull() )
			SetElecKey(generato->GetElecKey());
		if ( ! generato->GetCode()->IsNull() )
			SetCode(generato->GetCode());
		if ( ! generato->GetName()->IsNull() )
			SetName(generato->GetName());
		if ( ! generato->GetQmin().IsNull() )
			SetQmin(generato->GetQmin());
		if ( ! generato->GetQmaks().IsNull() )
			SetQmaks(generato->GetQmaks());
		if ( ! generato->GetPmax().IsNull() )
			SetPmax(generato->GetPmax());
		if ( ! generato->GetPconsmax().IsNull() )
			SetPconsmax(generato->GetPconsmax());
		if ( ! generato->GetDescript()->IsNull() )
			SetDescript(generato->GetDescript());
		if ( ! generato->GetHydKey().IsNull() )
			SetHydKey(generato->GetHydKey());
		if ( ! generato->GetStartdelay().IsNull() )
			SetStartdelay(generato->GetStartdelay());
	}
}


TBGenerato :: ~TBGenerato ()
{
	// Delete all pointer members.

}

void TBGenerato :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&elecKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&elecKey, icc_f.elecKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&qmin, icc_f.qmin);
	AddField((OSDataType *)&qmaks, icc_f.qmaks);
	AddField((OSDataType *)&pmax, icc_f.pmax);
	AddField((OSDataType *)&pconsmax, icc_f.pconsmax);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&startdelay, icc_f.startdelay);
}

#include "TBGroups.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBGroups :: TBGroups () : Table(icc_t.Groups, 3, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBGroups :: TBGroups (const pTBGroups groups) : Table ((Table *) groups)
{
	AddAllFields();

	if (groups)
	{
		if ( ! groups->GetGrupKey().IsNull() )
			SetGrupKey(groups->GetGrupKey());
		if ( ! groups->GetName()->IsNull() )
			SetName(groups->GetName());
		if ( ! groups->GetDescript()->IsNull() )
			SetDescript(groups->GetDescript());
	}
}


TBGroups :: ~TBGroups ()
{
	// Delete all pointer members.

}

void TBGroups :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&grupKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&grupKey, icc_f.grupKey);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBHfncofun.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBHfncofun :: TBHfncofun () : Table(icc_t.Hfncofun, 2, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBHfncofun :: TBHfncofun (const pTBHfncofun hfncofun) : Table ((Table *) hfncofun)
{
	AddAllFields();

	if (hfncofun)
	{
		if ( ! hfncofun->GetFnKey().IsNull() )
			SetFnKey(hfncofun->GetFnKey());
		if ( ! hfncofun->GetFunKey().IsNull() )
			SetFunKey(hfncofun->GetFunKey());
	}
}


TBHfncofun :: ~TBHfncofun ()
{
	// Delete all pointer members.

}

void TBHfncofun :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&fnKey);
	AddKeyField((OSDataType *)&funKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&fnKey, icc_f.fnKey);
	AddField((OSDataType *)&funKey, icc_f.funKey);
}

#include "TBHydcomp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBHydcomp :: TBHydcomp () : Table(icc_t.Hydcomp, 10, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBHydcomp :: TBHydcomp (const pTBHydcomp hydcomp) : Table ((Table *) hydcomp)
{
	AddAllFields();

	if (hydcomp)
	{
		if ( ! hydcomp->GetHydKey().IsNull() )
			SetHydKey(hydcomp->GetHydKey());
		if ( ! hydcomp->GetWatcKey().IsNull() )
			SetWatcKey(hydcomp->GetWatcKey());
		if ( ! hydcomp->GetHydtKey().IsNull() )
			SetHydtKey(hydcomp->GetHydtKey());
		if ( ! hydcomp->GetName()->IsNull() )
			SetName(hydcomp->GetName());
		if ( ! hydcomp->GetVersion().IsNull() )
			SetVersion(hydcomp->GetVersion());
		if ( ! hydcomp->GetUtmEast().IsNull() )
			SetUtmEast(hydcomp->GetUtmEast());
		if ( ! hydcomp->GetUtmNort().IsNull() )
			SetUtmNort(hydcomp->GetUtmNort());
		if ( ! hydcomp->GetGrupKey().IsNull() )
			SetGrupKey(hydcomp->GetGrupKey());
		if ( ! hydcomp->GetHydKeyh().IsNull() )
			SetHydKeyh(hydcomp->GetHydKeyh());
		if ( ! hydcomp->GetOpunKey().IsNull() )
			SetOpunKey(hydcomp->GetOpunKey());
	}
}


TBHydcomp :: ~TBHydcomp ()
{
	// Delete all pointer members.

}

void TBHydcomp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&hydKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&watcKey, icc_f.watcKey);
	AddField((OSDataType *)&hydtKey, icc_f.hydtKey);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&version, icc_f.version);
	AddField((OSDataType *)&utmEast, icc_f.utmEast);
	AddField((OSDataType *)&utmNort, icc_f.utmNort);
	AddField((OSDataType *)&grupKey, icc_f.grupKey);
	AddField((OSDataType *)&hydKeyh, icc_f.hydKeyh);
	AddField((OSDataType *)&opunKey, icc_f.opunKey);
}

#include "TBHydcoFn.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBHydcoFn :: TBHydcoFn () : Table(icc_t.HydcoFn, 12, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBHydcoFn :: TBHydcoFn (const pTBHydcoFn hydcoFn) : Table ((Table *) hydcoFn)
{
	AddAllFields();

	if (hydcoFn)
	{
		if ( ! hydcoFn->GetFnKey().IsNull() )
			SetFnKey(hydcoFn->GetFnKey());
		if ( ! hydcoFn->GetFnexpr()->IsNull() )
			SetFnexpr(hydcoFn->GetFnexpr());
		if ( ! hydcoFn->GetXunmeas().IsNull() )
			SetXunmeas(hydcoFn->GetXunmeas());
		if ( ! hydcoFn->GetYunmeas().IsNull() )
			SetYunmeas(hydcoFn->GetYunmeas());
		if ( ! hydcoFn->GetZunmeas().IsNull() )
			SetZunmeas(hydcoFn->GetZunmeas());
		if ( ! hydcoFn->GetRefvalue().IsNull() )
			SetRefvalue(hydcoFn->GetRefvalue());
		if ( ! hydcoFn->GetDescript()->IsNull() )
			SetDescript(hydcoFn->GetDescript());
		if ( ! hydcoFn->GetVersion().IsNull() )
			SetVersion(hydcoFn->GetVersion());
		if ( ! hydcoFn->GetCurvetyp().IsNull() )
			SetCurvetyp(hydcoFn->GetCurvetyp());
		if ( ! hydcoFn->GetGrupKey().IsNull() )
			SetGrupKey(hydcoFn->GetGrupKey());
		if ( ! hydcoFn->GetHydKey().IsNull() )
			SetHydKey(hydcoFn->GetHydKey());
		if ( ! hydcoFn->GetHydfKey().IsNull() )
			SetHydfKey(hydcoFn->GetHydfKey());
	}
}


TBHydcoFn :: ~TBHydcoFn ()
{
	// Delete all pointer members.

}

void TBHydcoFn :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&fnKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&fnKey, icc_f.fnKey);
	AddField((OSDataType *)&fnexpr, icc_f.fnexpr);
	AddField((OSDataType *)&xunmeas, icc_f.xunmeas);
	AddField((OSDataType *)&yunmeas, icc_f.yunmeas);
	AddField((OSDataType *)&zunmeas, icc_f.zunmeas);
	AddField((OSDataType *)&refvalue, icc_f.refvalue);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&version, icc_f.version);
	AddField((OSDataType *)&curvetyp, icc_f.curvetyp);
	AddField((OSDataType *)&grupKey, icc_f.grupKey);
	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&hydfKey, icc_f.hydfKey);
}

#include "TBHydcoTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBHydcoTp :: TBHydcoTp () : Table(icc_t.HydcoTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBHydcoTp :: TBHydcoTp (const pTBHydcoTp hydcoTp) : Table ((Table *) hydcoTp)
{
	AddAllFields();

	if (hydcoTp)
	{
		if ( ! hydcoTp->GetHydtKey().IsNull() )
			SetHydtKey(hydcoTp->GetHydtKey());
		if ( ! hydcoTp->GetCode()->IsNull() )
			SetCode(hydcoTp->GetCode());
		if ( ! hydcoTp->GetName()->IsNull() )
			SetName(hydcoTp->GetName());
		if ( ! hydcoTp->GetDescript()->IsNull() )
			SetDescript(hydcoTp->GetDescript());
	}
}


TBHydcoTp :: ~TBHydcoTp ()
{
	// Delete all pointer members.

}

void TBHydcoTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&hydtKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&hydtKey, icc_f.hydtKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBHydfnTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBHydfnTp :: TBHydfnTp () : Table(icc_t.HydfnTp, 5, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBHydfnTp :: TBHydfnTp (const pTBHydfnTp hydfnTp) : Table ((Table *) hydfnTp)
{
	AddAllFields();

	if (hydfnTp)
	{
		if ( ! hydfnTp->GetHydfKey().IsNull() )
			SetHydfKey(hydfnTp->GetHydfKey());
		if ( ! hydfnTp->GetCode()->IsNull() )
			SetCode(hydfnTp->GetCode());
		if ( ! hydfnTp->GetName()->IsNull() )
			SetName(hydfnTp->GetName());
		if ( ! hydfnTp->GetDescript()->IsNull() )
			SetDescript(hydfnTp->GetDescript());
		if ( ! hydfnTp->GetNumofarg().IsNull() )
			SetNumofarg(hydfnTp->GetNumofarg());
	}
}


TBHydfnTp :: ~TBHydfnTp ()
{
	// Delete all pointer members.

}

void TBHydfnTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&hydfKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&hydfKey, icc_f.hydfKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&numofarg, icc_f.numofarg);
}

#include "TBHydfnVl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBHydfnVl :: TBHydfnVl () : Table(icc_t.HydfnVl, 5, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBHydfnVl :: TBHydfnVl (const pTBHydfnVl hydfnVl) : Table ((Table *) hydfnVl)
{
	AddAllFields();

	if (hydfnVl)
	{
		if ( ! hydfnVl->GetFnKey().IsNull() )
			SetFnKey(hydfnVl->GetFnKey());
		if ( ! hydfnVl->GetFnvlKey().IsNull() )
			SetFnvlKey(hydfnVl->GetFnvlKey());
		if ( ! hydfnVl->GetArg1().IsNull() )
			SetArg1(hydfnVl->GetArg1());
		if ( ! hydfnVl->GetArg2().IsNull() )
			SetArg2(hydfnVl->GetArg2());
		if ( ! hydfnVl->GetFnVal().IsNull() )
			SetFnVal(hydfnVl->GetFnVal());
	}
}


TBHydfnVl :: ~TBHydfnVl ()
{
	// Delete all pointer members.

}

void TBHydfnVl :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&fnKey);
	AddKeyField((OSDataType *)&fnvlKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&fnKey, icc_f.fnKey);
	AddField((OSDataType *)&fnvlKey, icc_f.fnvlKey);
	AddField((OSDataType *)&arg1, icc_f.arg1);
	AddField((OSDataType *)&arg2, icc_f.arg2);
	AddField((OSDataType *)&fnVal, icc_f.fnVal);
}

#include "TBHydgenun.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBHydgenun :: TBHydgenun () : Table(icc_t.Hydgenun, 9, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBHydgenun :: TBHydgenun (const pTBHydgenun hydgenun) : Table ((Table *) hydgenun)
{
	AddAllFields();

	if (hydgenun)
	{
		if ( ! hydgenun->GetHydKey().IsNull() )
			SetHydKey(hydgenun->GetHydKey());
		if ( ! hydgenun->GetGenunno().IsNull() )
			SetGenunno(hydgenun->GetGenunno());
		if ( ! hydgenun->GetCode()->IsNull() )
			SetCode(hydgenun->GetCode());
		if ( ! hydgenun->GetName()->IsNull() )
			SetName(hydgenun->GetName());
		if ( ! hydgenun->GetPstocNo().IsNull() )
			SetPstocNo(hydgenun->GetPstocNo());
		if ( ! hydgenun->GetDtubeNo().IsNull() )
			SetDtubeNo(hydgenun->GetDtubeNo());
		if ( ! hydgenun->GetGenunTp().IsNull() )
			SetGenunTp(hydgenun->GetGenunTp());
		if ( ! hydgenun->GetDescript()->IsNull() )
			SetDescript(hydgenun->GetDescript());
		if ( ! hydgenun->GetInlgtNo().IsNull() )
			SetInlgtNo(hydgenun->GetInlgtNo());
	}
}


TBHydgenun :: ~TBHydgenun ()
{
	// Delete all pointer members.

}

void TBHydgenun :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&hydKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&genunno, icc_f.genunno);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&pstocNo, icc_f.pstocNo);
	AddField((OSDataType *)&dtubeNo, icc_f.dtubeNo);
	AddField((OSDataType *)&genunTp, icc_f.genunTp);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&inlgtNo, icc_f.inlgtNo);
}

#include "TBHydplant.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBHydplant :: TBHydplant () : Table(icc_t.Hydplant, 13, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBHydplant :: TBHydplant (const pTBHydplant hydplant) : Table ((Table *) hydplant)
{
	AddAllFields();

	if (hydplant)
	{
		if ( ! hydplant->GetHydKey().IsNull() )
			SetHydKey(hydplant->GetHydKey());
		if ( ! hydplant->GetContline().IsNull() )
			SetContline(hydplant->GetContline());
		if ( ! hydplant->GetVuNo()->IsNull() )
			SetVuNo(hydplant->GetVuNo());
		if ( ! hydplant->GetEnerfact().IsNull() )
			SetEnerfact(hydplant->GetEnerfact());
		if ( ! hydplant->GetNomhead().IsNull() )
			SetNomhead(hydplant->GetNomhead());
		if ( ! hydplant->GetNoPstoc().IsNull() )
			SetNoPstoc(hydplant->GetNoPstoc());
		if ( ! hydplant->GetNoDtube().IsNull() )
			SetNoDtube(hydplant->GetNoDtube());
		if ( ! hydplant->GetDescript()->IsNull() )
			SetDescript(hydplant->GetDescript());
		if ( ! hydplant->GetHyplKey().IsNull() )
			SetHyplKey(hydplant->GetHyplKey());
		if ( ! hydplant->GetStartdelay().IsNull() )
			SetStartdelay(hydplant->GetStartdelay());
		if ( ! hydplant->GetSerialstart().IsNull() )
			SetSerialstart(hydplant->GetSerialstart());
		if ( ! hydplant->GetMindelay().IsNull() )
			SetMindelay(hydplant->GetMindelay());
		if ( ! hydplant->GetOwnerPct().IsNull() )
			SetOwnerPct(hydplant->GetOwnerPct());
	}
}


TBHydplant :: ~TBHydplant ()
{
	// Delete all pointer members.

}

void TBHydplant :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&hydKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&contline, icc_f.contline);
	AddField((OSDataType *)&vuNo, icc_f.vuNo);
	AddField((OSDataType *)&enerfact, icc_f.enerfact);
	AddField((OSDataType *)&nomhead, icc_f.nomhead);
	AddField((OSDataType *)&noPstoc, icc_f.noPstoc);
	AddField((OSDataType *)&noDtube, icc_f.noDtube);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&hyplKey, icc_f.hyplKey);
	AddField((OSDataType *)&startdelay, icc_f.startdelay);
	AddField((OSDataType *)&serialstart, icc_f.serialstart);
	AddField((OSDataType *)&mindelay, icc_f.mindelay);
	AddField((OSDataType *)&ownerPct, icc_f.ownerPct);
}

#include "TBHypsoVl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBHypsoVl :: TBHypsoVl () : Table(icc_t.HypsoVl, 5, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBHypsoVl :: TBHypsoVl (const pTBHypsoVl hypsoVl) : Table ((Table *) hypsoVl)
{
	AddAllFields();

	if (hypsoVl)
	{
		if ( ! hypsoVl->GetHypsKey().IsNull() )
			SetHypsKey(hypsoVl->GetHypsKey());
		if ( ! hypsoVl->GetAltitude().IsNull() )
			SetAltitude(hypsoVl->GetAltitude());
		if ( ! hypsoVl->GetCatchare().IsNull() )
			SetCatchare(hypsoVl->GetCatchare());
		if ( ! hypsoVl->GetGlacarea().IsNull() )
			SetGlacarea(hypsoVl->GetGlacarea());
		if ( ! hypsoVl->GetHydKey().IsNull() )
			SetHydKey(hypsoVl->GetHydKey());
	}
}


TBHypsoVl :: ~TBHypsoVl ()
{
	// Delete all pointer members.

}

void TBHypsoVl :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&hypsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&hypsKey, icc_f.hypsKey);
	AddField((OSDataType *)&altitude, icc_f.altitude);
	AddField((OSDataType *)&catchare, icc_f.catchare);
	AddField((OSDataType *)&glacarea, icc_f.glacarea);
	AddField((OSDataType *)&hydKey, icc_f.hydKey);
}

#include "TBHyrelAc.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBHyrelAc :: TBHyrelAc () : Table(icc_t.HyrelAc, 5, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBHyrelAc :: TBHyrelAc (const pTBHyrelAc hyrelAc) : Table ((Table *) hyrelAc)
{
	AddAllFields();

	if (hyrelAc)
	{
		if ( ! hyrelAc->GetRelKey().IsNull() )
			SetRelKey(hyrelAc->GetRelKey());
		if ( ! hyrelAc->GetHyretype().IsNull() )
			SetHyretype(hyrelAc->GetHyretype());
		if ( ! hyrelAc->GetGrupKey().IsNull() )
			SetGrupKey(hyrelAc->GetGrupKey());
		if ( ! hyrelAc->GetHydKeyh().IsNull() )
			SetHydKeyh(hyrelAc->GetHydKeyh());
		if ( ! hyrelAc->GetHydKey().IsNull() )
			SetHydKey(hyrelAc->GetHydKey());
	}
}


TBHyrelAc :: ~TBHyrelAc ()
{
	// Delete all pointer members.

}

void TBHyrelAc :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&relKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&relKey, icc_f.relKey);
	AddField((OSDataType *)&hyretype, icc_f.hyretype);
	AddField((OSDataType *)&grupKey, icc_f.grupKey);
	AddField((OSDataType *)&hydKeyh, icc_f.hydKeyh);
	AddField((OSDataType *)&hydKey, icc_f.hydKey);
}

#include "TBIccApp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBIccApp :: TBIccApp () : Table(icc_t.IccApp, 8, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBIccApp :: TBIccApp (const pTBIccApp iccApp) : Table ((Table *) iccApp)
{
	AddAllFields();

	if (iccApp)
	{
		if ( ! iccApp->GetAppKey().IsNull() )
			SetAppKey(iccApp->GetAppKey());
		if ( ! iccApp->GetName()->IsNull() )
			SetName(iccApp->GetName());
		if ( ! iccApp->GetStartime()->IsNull() )
			SetStartime(iccApp->GetStartime());
		if ( ! iccApp->GetFintime()->IsNull() )
			SetFintime(iccApp->GetFintime());
		if ( ! iccApp->GetOsuser()->IsNull() )
			SetOsuser(iccApp->GetOsuser());
		if ( ! iccApp->GetDbuser()->IsNull() )
			SetDbuser(iccApp->GetDbuser());
		if ( ! iccApp->GetAppstat().IsNull() )
			SetAppstat(iccApp->GetAppstat());
		if ( ! iccApp->GetNomessages().IsNull() )
			SetNomessages(iccApp->GetNomessages());
	}
}


TBIccApp :: ~TBIccApp ()
{
	// Delete all pointer members.

}

void TBIccApp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&appKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&appKey, icc_f.appKey);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&startime, icc_f.startime);
	AddField((OSDataType *)&fintime, icc_f.fintime);
	AddField((OSDataType *)&osuser, icc_f.osuser);
	AddField((OSDataType *)&dbuser, icc_f.dbuser);
	AddField((OSDataType *)&appstat, icc_f.appstat);
	AddField((OSDataType *)&nomessages, icc_f.nomessages);
}

#include "TBIccAppl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBIccAppl :: TBIccAppl () : Table(icc_t.IccAppl, 10, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBIccAppl :: TBIccAppl (const pTBIccAppl iccAppl) : Table ((Table *) iccAppl)
{
	AddAllFields();

	if (iccAppl)
	{
		if ( ! iccAppl->GetAppKey().IsNull() )
			SetAppKey(iccAppl->GetAppKey());
		if ( ! iccAppl->GetRcount().IsNull() )
			SetRcount(iccAppl->GetRcount());
		if ( ! iccAppl->GetDatetim()->IsNull() )
			SetDatetim(iccAppl->GetDatetim());
		if ( ! iccAppl->GetInfotype().IsNull() )
			SetInfotype(iccAppl->GetInfotype());
		if ( ! iccAppl->GetIlevel().IsNull() )
			SetIlevel(iccAppl->GetIlevel());
		if ( ! iccAppl->GetDescript()->IsNull() )
			SetDescript(iccAppl->GetDescript());
		if ( ! iccAppl->GetProfileKey().IsNull() )
			SetProfileKey(iccAppl->GetProfileKey());
		if ( ! iccAppl->GetSignThisEvent().IsNull() )
			SetSignThisEvent(iccAppl->GetSignThisEvent());
		if ( ! iccAppl->GetSignOsuser()->IsNull() )
			SetSignOsuser(iccAppl->GetSignOsuser());
		if ( ! iccAppl->GetSignDatetim()->IsNull() )
			SetSignDatetim(iccAppl->GetSignDatetim());
	}
}


TBIccAppl :: ~TBIccAppl ()
{
	// Delete all pointer members.

}

void TBIccAppl :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&appKey);
	AddKeyField((OSDataType *)&rcount);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&appKey, icc_f.appKey);
	AddField((OSDataType *)&rcount, icc_f.rcount);
	AddField((OSDataType *)&datetim, icc_f.datetim);
	AddField((OSDataType *)&infotype, icc_f.infotype);
	AddField((OSDataType *)&ilevel, icc_f.ilevel);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&profileKey, icc_f.profileKey);
	AddField((OSDataType *)&signThisEvent, icc_f.signThisEvent);
	AddField((OSDataType *)&signOsuser, icc_f.signOsuser);
	AddField((OSDataType *)&signDatetim, icc_f.signDatetim);
}

#include "TBExchmeth.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBExchmeth :: TBExchmeth () : Table(icc_t.Exchmeth, 2, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBExchmeth :: TBExchmeth (const pTBExchmeth exchmeth) : Table ((Table *) exchmeth)
{
	AddAllFields();

	if (exchmeth)
	{
		if ( ! exchmeth->GetExmtKey().IsNull() )
			SetExmtKey(exchmeth->GetExmtKey());
		if ( ! exchmeth->GetName()->IsNull() )
			SetName(exchmeth->GetName());
	}
}


TBExchmeth :: ~TBExchmeth ()
{
	// Delete all pointer members.

}

void TBExchmeth :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&exmtKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&exmtKey, icc_f.exmtKey);
	AddField((OSDataType *)&name, icc_f.name);
}

#include "TBImpexpts.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBImpexpts :: TBImpexpts () : Table(icc_t.Impexpts, 18, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBImpexpts :: TBImpexpts (const pTBImpexpts impexpts) : Table ((Table *) impexpts)
{
	AddAllFields();

	if (impexpts)
	{
		if ( ! impexpts->GetOpunKey().IsNull() )
			SetOpunKey(impexpts->GetOpunKey());
		if ( ! impexpts->GetTimsKey().IsNull() )
			SetTimsKey(impexpts->GetTimsKey());
		if ( ! impexpts->GetSeqno().IsNull() )
			SetSeqno(impexpts->GetSeqno());
		if ( ! impexpts->GetGloberef()->IsNull() )
			SetGloberef(impexpts->GetGloberef());
		if ( ! impexpts->GetOffset().IsNull() )
			SetOffset(impexpts->GetOffset());
		if ( ! impexpts->GetFactor().IsNull() )
			SetFactor(impexpts->GetFactor());
		if ( ! impexpts->GetVstaint().IsNull() )
			SetVstaint(impexpts->GetVstaint());
		if ( ! impexpts->GetValftime()->IsNull() )
			SetValftime(impexpts->GetValftime());
		if ( ! impexpts->GetValttime()->IsNull() )
			SetValttime(impexpts->GetValttime());
		if ( ! impexpts->GetExmtKey().IsNull() )
			SetExmtKey(impexpts->GetExmtKey());
		if ( ! impexpts->GetTstrKey().IsNull() )
			SetTstrKey(impexpts->GetTstrKey());
		if ( ! impexpts->GetDirect().IsNull() )
			SetDirect(impexpts->GetDirect());
		if ( ! impexpts->GetCopySvk().IsNull() )
			SetCopySvk(impexpts->GetCopySvk());
		if ( ! impexpts->GetSender().IsNull() )
			SetSender(impexpts->GetSender());
		if ( ! impexpts->GetRval1().IsNull() )
			SetRval1(impexpts->GetRval1());
		if ( ! impexpts->GetRval2().IsNull() )
			SetRval2(impexpts->GetRval2());
		if ( ! impexpts->GetProductCode()->IsNull() )
			SetProductCode(impexpts->GetProductCode());
		if ( ! impexpts->GetTsinKeyExport().IsNull() )
			SetTsinKeyExport(impexpts->GetTsinKeyExport());
	}
}


TBImpexpts :: ~TBImpexpts ()
{
	// Delete all pointer members.

}

void TBImpexpts :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&timsKey);
	AddKeyField((OSDataType *)&seqno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&globeref, icc_f.globeref);
	AddField((OSDataType *)&offset, icc_f.offset);
	AddField((OSDataType *)&factor, icc_f.factor);
	AddField((OSDataType *)&vstaint, icc_f.vstaint);
	AddField((OSDataType *)&valftime, icc_f.valftime);
	AddField((OSDataType *)&valttime, icc_f.valttime);
	AddField((OSDataType *)&exmtKey, icc_f.exmtKey);
	AddField((OSDataType *)&tstrKey, icc_f.tstrKey);
	AddField((OSDataType *)&direct, icc_f.direct);
	AddField((OSDataType *)&copySvk, icc_f.copySvk);
	AddField((OSDataType *)&sender, icc_f.sender);
	AddField((OSDataType *)&rval1, icc_f.rval1);
	AddField((OSDataType *)&rval2, icc_f.rval2);
	AddField((OSDataType *)&productCode, icc_f.productCode);
	AddField((OSDataType *)&tsinKeyExport, icc_f.tsinKeyExport);
}

#include "TBInvarTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBInvarTp :: TBInvarTp () : Table(icc_t.InvarTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBInvarTp :: TBInvarTp (const pTBInvarTp invarTp) : Table ((Table *) invarTp)
{
	AddAllFields();

	if (invarTp)
	{
		if ( ! invarTp->GetArtKey().IsNull() )
			SetArtKey(invarTp->GetArtKey());
		if ( ! invarTp->GetArtNo().IsNull() )
			SetArtNo(invarTp->GetArtNo());
		if ( ! invarTp->GetCode()->IsNull() )
			SetCode(invarTp->GetCode());
		if ( ! invarTp->GetName()->IsNull() )
			SetName(invarTp->GetName());
	}
}


TBInvarTp :: ~TBInvarTp ()
{
	// Delete all pointer members.

}

void TBInvarTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&artKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&artKey, icc_f.artKey);
	AddField((OSDataType *)&artNo, icc_f.artNo);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
}

#include "TBInvper.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBInvper :: TBInvper () : Table(icc_t.Invper, 3, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBInvper :: TBInvper (const pTBInvper invper) : Table ((Table *) invper)
{
	AddAllFields();

	if (invper)
	{
		if ( ! invper->GetOpunKey().IsNull() )
			SetOpunKey(invper->GetOpunKey());
		if ( ! invper->GetTrcode()->IsNull() )
			SetTrcode(invper->GetTrcode());
		if ( ! invper->GetInvinter().IsNull() )
			SetInvinter(invper->GetInvinter());
	}
}


TBInvper :: ~TBInvper ()
{
	// Delete all pointer members.

}

void TBInvper :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&invinter, icc_f.invinter);
}

#include "TBInvpfrm.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBInvpfrm :: TBInvpfrm () : Table(icc_t.Invpfrm, 17, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBInvpfrm :: TBInvpfrm (const pTBInvpfrm invpfrm) : Table ((Table *) invpfrm)
{
	AddAllFields();

	if (invpfrm)
	{
		if ( ! invpfrm->GetInvpKey().IsNull() )
			SetInvpKey(invpfrm->GetInvpKey());
		if ( ! invpfrm->GetIdate()->IsNull() )
			SetIdate(invpfrm->GetIdate());
		if ( ! invpfrm->GetDueDate()->IsNull() )
			SetDueDate(invpfrm->GetDueDate());
		if ( ! invpfrm->GetOrderno()->IsNull() )
			SetOrderno(invpfrm->GetOrderno());
		if ( ! invpfrm->GetInvStat().IsNull() )
			SetInvStat(invpfrm->GetInvStat());
		if ( ! invpfrm->GetTraStat().IsNull() )
			SetTraStat(invpfrm->GetTraStat());
		if ( ! invpfrm->GetInvTp().IsNull() )
			SetInvTp(invpfrm->GetInvTp());
		if ( ! invpfrm->GetFormTp().IsNull() )
			SetFormTp(invpfrm->GetFormTp());
		if ( ! invpfrm->GetPayTerm()->IsNull() )
			SetPayTerm(invpfrm->GetPayTerm());
		if ( ! invpfrm->GetExtP()->IsNull() )
			SetExtP(invpfrm->GetExtP());
		if ( ! invpfrm->GetExtC()->IsNull() )
			SetExtC(invpfrm->GetExtC());
		if ( ! invpfrm->GetOurRef()->IsNull() )
			SetOurRef(invpfrm->GetOurRef());
		if ( ! invpfrm->GetOpunKey().IsNull() )
			SetOpunKey(invpfrm->GetOpunKey());
		if ( ! invpfrm->GetTrcode()->IsNull() )
			SetTrcode(invpfrm->GetTrcode());
		if ( ! invpfrm->GetInvRef().IsNull() )
			SetInvRef(invpfrm->GetInvRef());
		if ( ! invpfrm->GetOpunKea().IsNull() )
			SetOpunKea(invpfrm->GetOpunKea());
		if ( ! invpfrm->GetTrcodeii()->IsNull() )
			SetTrcodeii(invpfrm->GetTrcodeii());
	}
}


TBInvpfrm :: ~TBInvpfrm ()
{
	// Delete all pointer members.

}

void TBInvpfrm :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&invpKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&invpKey, icc_f.invpKey);
	AddField((OSDataType *)&idate, icc_f.idate);
	AddField((OSDataType *)&dueDate, icc_f.dueDate);
	AddField((OSDataType *)&orderno, icc_f.orderno);
	AddField((OSDataType *)&invStat, icc_f.invStat);
	AddField((OSDataType *)&traStat, icc_f.traStat);
	AddField((OSDataType *)&invTp, icc_f.invTp);
	AddField((OSDataType *)&formTp, icc_f.formTp);
	AddField((OSDataType *)&payTerm, icc_f.payTerm);
	AddField((OSDataType *)&extP, icc_f.extP);
	AddField((OSDataType *)&extC, icc_f.extC);
	AddField((OSDataType *)&ourRef, icc_f.ourRef);
	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&invRef, icc_f.invRef);
	AddField((OSDataType *)&opunKea, icc_f.opunKea);
	AddField((OSDataType *)&trcodeii, icc_f.trcodeii);
}

#include "TBInvplin.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBInvplin :: TBInvplin () : Table(icc_t.Invplin, 13, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBInvplin :: TBInvplin (const pTBInvplin invplin) : Table ((Table *) invplin)
{
	AddAllFields();

	if (invplin)
	{
		if ( ! invplin->GetInvpKey().IsNull() )
			SetInvpKey(invplin->GetInvpKey());
		if ( ! invplin->GetArtKey().IsNull() )
			SetArtKey(invplin->GetArtKey());
		if ( ! invplin->GetSeqno().IsNull() )
			SetSeqno(invplin->GetSeqno());
		if ( ! invplin->GetFromdate()->IsNull() )
			SetFromdate(invplin->GetFromdate());
		if ( ! invplin->GetTodate()->IsNull() )
			SetTodate(invplin->GetTodate());
		if ( ! invplin->GetQuantity().IsNull() )
			SetQuantity(invplin->GetQuantity());
		if ( ! invplin->GetPrice().IsNull() )
			SetPrice(invplin->GetPrice());
		if ( ! invplin->GetTotal().IsNull() )
			SetTotal(invplin->GetTotal());
		if ( ! invplin->GetArtNo().IsNull() )
			SetArtNo(invplin->GetArtNo());
		if ( ! invplin->GetArtText()->IsNull() )
			SetArtText(invplin->GetArtText());
		if ( ! invplin->GetArtDesc()->IsNull() )
			SetArtDesc(invplin->GetArtDesc());
		if ( ! invplin->GetOpunKey().IsNull() )
			SetOpunKey(invplin->GetOpunKey());
		if ( ! invplin->GetTrcode()->IsNull() )
			SetTrcode(invplin->GetTrcode());
	}
}


TBInvplin :: ~TBInvplin ()
{
	// Delete all pointer members.

}

void TBInvplin :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&invpKey);
	AddKeyField((OSDataType *)&artKey);
	AddKeyField((OSDataType *)&seqno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&invpKey, icc_f.invpKey);
	AddField((OSDataType *)&artKey, icc_f.artKey);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&fromdate, icc_f.fromdate);
	AddField((OSDataType *)&todate, icc_f.todate);
	AddField((OSDataType *)&quantity, icc_f.quantity);
	AddField((OSDataType *)&price, icc_f.price);
	AddField((OSDataType *)&total, icc_f.total);
	AddField((OSDataType *)&artNo, icc_f.artNo);
	AddField((OSDataType *)&artText, icc_f.artText);
	AddField((OSDataType *)&artDesc, icc_f.artDesc);
	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
}

#include "TBLastOkExported.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBLastOkExported :: TBLastOkExported () : Table(icc_t.LastOkExported, 3, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBLastOkExported :: TBLastOkExported (const pTBLastOkExported lastOkExported) : Table ((Table *) lastOkExported)
{
	AddAllFields();

	if (lastOkExported)
	{
		if ( ! lastOkExported->GetOpunKey().IsNull() )
			SetOpunKey(lastOkExported->GetOpunKey());
		if ( ! lastOkExported->GetTimsKey().IsNull() )
			SetTimsKey(lastOkExported->GetTimsKey());
		if ( ! lastOkExported->GetLastOkExported()->IsNull() )
			SetLastOkExported(lastOkExported->GetLastOkExported());
	}
}


TBLastOkExported :: ~TBLastOkExported ()
{
	// Delete all pointer members.

}

void TBLastOkExported :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&timsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&lastOkExported, icc_f.lastOkExported);
}

#include "TBLdfarea.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBLdfarea :: TBLdfarea () : Table(icc_t.Ldfarea, 5, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBLdfarea :: TBLdfarea (const pTBLdfarea ldfarea) : Table ((Table *) ldfarea)
{
	AddAllFields();

	if (ldfarea)
	{
		if ( ! ldfarea->GetLdfKey().IsNull() )
			SetLdfKey(ldfarea->GetLdfKey());
		if ( ! ldfarea->GetCode()->IsNull() )
			SetCode(ldfarea->GetCode());
		if ( ! ldfarea->GetName()->IsNull() )
			SetName(ldfarea->GetName());
		if ( ! ldfarea->GetDescript()->IsNull() )
			SetDescript(ldfarea->GetDescript());
		if ( ! ldfarea->GetArea().IsNull() )
			SetArea(ldfarea->GetArea());
	}
}


TBLdfarea :: ~TBLdfarea ()
{
	// Delete all pointer members.

}

void TBLdfarea :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&ldfKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&ldfKey, icc_f.ldfKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&area, icc_f.area);
}

#include "TBMacroDs.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBMacroDs :: TBMacroDs () : Table(icc_t.MacroDs, 5, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBMacroDs :: TBMacroDs (const pTBMacroDs macroDs) : Table ((Table *) macroDs)
{
	AddAllFields();

	if (macroDs)
	{
		if ( ! macroDs->GetMadsKey().IsNull() )
			SetMadsKey(macroDs->GetMadsKey());
		if ( ! macroDs->GetName()->IsNull() )
			SetName(macroDs->GetName());
		if ( ! macroDs->GetDescript()->IsNull() )
			SetDescript(macroDs->GetDescript());
		if ( ! macroDs->GetMadsKea().IsNull() )
			SetMadsKea(macroDs->GetMadsKea());
		if ( ! macroDs->GetProgKey().IsNull() )
			SetProgKey(macroDs->GetProgKey());
	}
}


TBMacroDs :: ~TBMacroDs ()
{
	// Delete all pointer members.

}

void TBMacroDs :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&madsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&madsKey, icc_f.madsKey);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&madsKea, icc_f.madsKea);
	AddField((OSDataType *)&progKey, icc_f.progKey);
}

#include "TBMadcomaa.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBMadcomaa :: TBMadcomaa () : Table(icc_t.Madcomaa, 5, 5, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBMadcomaa :: TBMadcomaa (const pTBMadcomaa madcomaa) : Table ((Table *) madcomaa)
{
	AddAllFields();

	if (madcomaa)
	{
		if ( ! madcomaa->GetYearweek().IsNull() )
			SetYearweek(madcomaa->GetYearweek());
		if ( ! madcomaa->GetMardKey()->IsNull() )
			SetMardKey(madcomaa->GetMardKey());
		if ( ! madcomaa->GetMardiv().IsNull() )
			SetMardiv(madcomaa->GetMardiv());
		if ( ! madcomaa->GetTratKey()->IsNull() )
			SetTratKey(madcomaa->GetTratKey());
		if ( ! madcomaa->GetTratKea()->IsNull() )
			SetTratKea(madcomaa->GetTratKea());
	}
}


TBMadcomaa :: ~TBMadcomaa ()
{
	// Delete all pointer members.

}

void TBMadcomaa :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&yearweek);
	AddKeyField((OSDataType *)&mardKey);
	AddKeyField((OSDataType *)&mardiv);
	AddKeyField((OSDataType *)&tratKey);
	AddKeyField((OSDataType *)&tratKea);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&yearweek, icc_f.yearweek);
	AddField((OSDataType *)&mardKey, icc_f.mardKey);
	AddField((OSDataType *)&mardiv, icc_f.mardiv);
	AddField((OSDataType *)&tratKey, icc_f.tratKey);
	AddField((OSDataType *)&tratKea, icc_f.tratKea);
}

#include "TBMailnTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBMailnTp :: TBMailnTp () : Table(icc_t.MailnTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBMailnTp :: TBMailnTp (const pTBMailnTp mailnTp) : Table ((Table *) mailnTp)
{
	AddAllFields();

	if (mailnTp)
	{
		if ( ! mailnTp->GetMailKey().IsNull() )
			SetMailKey(mailnTp->GetMailKey());
		if ( ! mailnTp->GetZipCode()->IsNull() )
			SetZipCode(mailnTp->GetZipCode());
		if ( ! mailnTp->GetPOffice()->IsNull() )
			SetPOffice(mailnTp->GetPOffice());
		if ( ! mailnTp->GetCountry()->IsNull() )
			SetCountry(mailnTp->GetCountry());
	}
}


TBMailnTp :: ~TBMailnTp ()
{
	// Delete all pointer members.

}

void TBMailnTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&mailKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&mailKey, icc_f.mailKey);
	AddField((OSDataType *)&zipCode, icc_f.zipCode);
	AddField((OSDataType *)&pOffice, icc_f.pOffice);
	AddField((OSDataType *)&country, icc_f.country);
}

#include "TBMaradiv.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBMaradiv :: TBMaradiv () : Table(icc_t.Maradiv, 3, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBMaradiv :: TBMaradiv (const pTBMaradiv maradiv) : Table ((Table *) maradiv)
{
	AddAllFields();

	if (maradiv)
	{
		if ( ! maradiv->GetYearweek().IsNull() )
			SetYearweek(maradiv->GetYearweek());
		if ( ! maradiv->GetMardKey()->IsNull() )
			SetMardKey(maradiv->GetMardKey());
		if ( ! maradiv->GetMardiv().IsNull() )
			SetMardiv(maradiv->GetMardiv());
	}
}


TBMaradiv :: ~TBMaradiv ()
{
	// Delete all pointer members.

}

void TBMaradiv :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&yearweek);
	AddKeyField((OSDataType *)&mardKey);
	AddKeyField((OSDataType *)&mardiv);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&yearweek, icc_f.yearweek);
	AddField((OSDataType *)&mardKey, icc_f.mardKey);
	AddField((OSDataType *)&mardiv, icc_f.mardiv);
}

#include "TBMardicol.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBMardicol :: TBMardicol () : Table(icc_t.Mardicol, 2, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBMardicol :: TBMardicol (const pTBMardicol mardicol) : Table ((Table *) mardicol)
{
	AddAllFields();

	if (mardicol)
	{
		if ( ! mardicol->GetMardiv().IsNull() )
			SetMardiv(mardicol->GetMardiv());
		if ( ! mardicol->GetColour()->IsNull() )
			SetColour(mardicol->GetColour());
	}
}


TBMardicol :: ~TBMardicol ()
{
	// Delete all pointer members.

}

void TBMardicol :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&mardiv);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&mardiv, icc_f.mardiv);
	AddField((OSDataType *)&colour, icc_f.colour);
}

#include "TBMardiTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBMardiTp :: TBMardiTp () : Table(icc_t.MardiTp, 3, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBMardiTp :: TBMardiTp (const pTBMardiTp mardiTp) : Table ((Table *) mardiTp)
{
	AddAllFields();

	if (mardiTp)
	{
		if ( ! mardiTp->GetMardKey()->IsNull() )
			SetMardKey(mardiTp->GetMardKey());
		if ( ! mardiTp->GetEnumval().IsNull() )
			SetEnumval(mardiTp->GetEnumval());
		if ( ! mardiTp->GetName()->IsNull() )
			SetName(mardiTp->GetName());
	}
}


TBMardiTp :: ~TBMardiTp ()
{
	// Delete all pointer members.

}

void TBMardiTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&mardKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&mardKey, icc_f.mardKey);
	AddField((OSDataType *)&enumval, icc_f.enumval);
	AddField((OSDataType *)&name, icc_f.name);
}

#include "TBMareaTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBMareaTp :: TBMareaTp () : Table(icc_t.MareaTp, 3, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBMareaTp :: TBMareaTp (const pTBMareaTp mareaTp) : Table ((Table *) mareaTp)
{
	AddAllFields();

	if (mareaTp)
	{
		if ( ! mareaTp->GetMartKey()->IsNull() )
			SetMartKey(mareaTp->GetMartKey());
		if ( ! mareaTp->GetEnumval().IsNull() )
			SetEnumval(mareaTp->GetEnumval());
		if ( ! mareaTp->GetName()->IsNull() )
			SetName(mareaTp->GetName());
	}
}


TBMareaTp :: ~TBMareaTp ()
{
	// Delete all pointer members.

}

void TBMareaTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&martKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&martKey, icc_f.martKey);
	AddField((OSDataType *)&enumval, icc_f.enumval);
	AddField((OSDataType *)&name, icc_f.name);
}

#include "TBMeacoexc.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBMeacoexc :: TBMeacoexc () : Table(icc_t.Meacoexc, 5, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBMeacoexc :: TBMeacoexc (const pTBMeacoexc meacoexc) : Table ((Table *) meacoexc)
{
	AddAllFields();

	if (meacoexc)
	{
		if ( ! meacoexc->GetExchKey().IsNull() )
			SetExchKey(meacoexc->GetExchKey());
		if ( ! meacoexc->GetSeqno().IsNull() )
			SetSeqno(meacoexc->GetSeqno());
		if ( ! meacoexc->GetMeasKey().IsNull() )
			SetMeasKey(meacoexc->GetMeasKey());
		if ( ! meacoexc->GetSeqnommm().IsNull() )
			SetSeqnommm(meacoexc->GetSeqnommm());
		if ( ! meacoexc->GetEfctKey().IsNull() )
			SetEfctKey(meacoexc->GetEfctKey());
	}
}


TBMeacoexc :: ~TBMeacoexc ()
{
	// Delete all pointer members.

}

void TBMeacoexc :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&exchKey);
	AddKeyField((OSDataType *)&seqno);
	AddKeyField((OSDataType *)&measKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&exchKey, icc_f.exchKey);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&measKey, icc_f.measKey);
	AddField((OSDataType *)&seqnommm, icc_f.seqnommm);
	AddField((OSDataType *)&efctKey, icc_f.efctKey);
}

#include "TBMeasPnt.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBMeasPnt :: TBMeasPnt () : Table(icc_t.MeasPnt, 21, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBMeasPnt :: TBMeasPnt (const pTBMeasPnt measPnt) : Table ((Table *) measPnt)
{
	AddAllFields();

	if (measPnt)
	{
		if ( ! measPnt->GetMeasKey().IsNull() )
			SetMeasKey(measPnt->GetMeasKey());
		if ( ! measPnt->GetSeqno().IsNull() )
			SetSeqno(measPnt->GetSeqno());
		if ( ! measPnt->GetName()->IsNull() )
			SetName(measPnt->GetName());
		if ( ! measPnt->GetDescript()->IsNull() )
			SetDescript(measPnt->GetDescript());
		if ( ! measPnt->GetValftime()->IsNull() )
			SetValftime(measPnt->GetValftime());
		if ( ! measPnt->GetValttime()->IsNull() )
			SetValttime(measPnt->GetValttime());
		if ( ! measPnt->GetInfo()->IsNull() )
			SetInfo(measPnt->GetInfo());
		if ( ! measPnt->GetGrupKey().IsNull() )
			SetGrupKey(measPnt->GetGrupKey());
		if ( ! measPnt->GetFusesz()->IsNull() )
			SetFusesz(measPnt->GetFusesz());
		if ( ! measPnt->GetVolts()->IsNull() )
			SetVolts(measPnt->GetVolts());
		if ( ! measPnt->GetEanNumber()->IsNull() )
			SetEanNumber(measPnt->GetEanNumber());
		if ( ! measPnt->GetInslKey().IsNull() )
			SetInslKey(measPnt->GetInslKey());
		if ( ! measPnt->GetLocation()->IsNull() )
			SetLocation(measPnt->GetLocation());
		if ( ! measPnt->GetReadKey().IsNull() )
			SetReadKey(measPnt->GetReadKey());
		if ( ! measPnt->GetListno()->IsNull() )
			SetListno(measPnt->GetListno());
		if ( ! measPnt->GetReadno().IsNull() )
			SetReadno(measPnt->GetReadno());
		if ( ! measPnt->GetReadable().IsNull() )
			SetReadable(measPnt->GetReadable());
		if ( ! measPnt->GetNPhases().IsNull() )
			SetNPhases(measPnt->GetNPhases());
		if ( ! measPnt->GetDebtTyp().IsNull() )
			SetDebtTyp(measPnt->GetDebtTyp());
		if ( ! measPnt->GetXCoordinate().IsNull() )
			SetXCoordinate(measPnt->GetXCoordinate());
		if ( ! measPnt->GetYCoordinate().IsNull() )
			SetYCoordinate(measPnt->GetYCoordinate());
	}
}


TBMeasPnt :: ~TBMeasPnt ()
{
	// Delete all pointer members.

}

void TBMeasPnt :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&measKey);
	AddKeyField((OSDataType *)&seqno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&measKey, icc_f.measKey);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&valftime, icc_f.valftime);
	AddField((OSDataType *)&valttime, icc_f.valttime);
	AddField((OSDataType *)&info, icc_f.info);
	AddField((OSDataType *)&grupKey, icc_f.grupKey);
	AddField((OSDataType *)&fusesz, icc_f.fusesz);
	AddField((OSDataType *)&volts, icc_f.volts);
	AddField((OSDataType *)&eanNumber, icc_f.eanNumber);
	AddField((OSDataType *)&inslKey, icc_f.inslKey);
	AddField((OSDataType *)&location, icc_f.location);
	AddField((OSDataType *)&readKey, icc_f.readKey);
	AddField((OSDataType *)&listno, icc_f.listno);
	AddField((OSDataType *)&readno, icc_f.readno);
	AddField((OSDataType *)&readable, icc_f.readable);
	AddField((OSDataType *)&nPhases, icc_f.nPhases);
	AddField((OSDataType *)&debtTyp, icc_f.debtTyp);
	AddField((OSDataType *)&xCoordinate, icc_f.xCoordinate);
	AddField((OSDataType *)&yCoordinate, icc_f.yCoordinate);
}

#include "TBMeasTs.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBMeasTs :: TBMeasTs () : Table(icc_t.MeasTs, 6, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBMeasTs :: TBMeasTs (const pTBMeasTs measTs) : Table ((Table *) measTs)
{
	AddAllFields();

	if (measTs)
	{
		if ( ! measTs->GetMeasKey().IsNull() )
			SetMeasKey(measTs->GetMeasKey());
		if ( ! measTs->GetSeqno().IsNull() )
			SetSeqno(measTs->GetSeqno());
		if ( ! measTs->GetTimsKey().IsNull() )
			SetTimsKey(measTs->GetTimsKey());
		if ( ! measTs->GetGrupKey().IsNull() )
			SetGrupKey(measTs->GetGrupKey());
		if ( ! measTs->GetMetsKey().IsNull() )
			SetMetsKey(measTs->GetMetsKey());
		if ( ! measTs->GetEfctKey().IsNull() )
			SetEfctKey(measTs->GetEfctKey());
	}
}


TBMeasTs :: ~TBMeasTs ()
{
	// Delete all pointer members.

}

void TBMeasTs :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&measKey);
	AddKeyField((OSDataType *)&seqno);
	AddKeyField((OSDataType *)&timsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&measKey, icc_f.measKey);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&grupKey, icc_f.grupKey);
	AddField((OSDataType *)&metsKey, icc_f.metsKey);
	AddField((OSDataType *)&efctKey, icc_f.efctKey);
}

#include "TBMeatsTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBMeatsTp :: TBMeatsTp () : Table(icc_t.MeatsTp, 2, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBMeatsTp :: TBMeatsTp (const pTBMeatsTp meatsTp) : Table ((Table *) meatsTp)
{
	AddAllFields();

	if (meatsTp)
	{
		if ( ! meatsTp->GetMetsKey().IsNull() )
			SetMetsKey(meatsTp->GetMetsKey());
		if ( ! meatsTp->GetName()->IsNull() )
			SetName(meatsTp->GetName());
	}
}


TBMeatsTp :: ~TBMeatsTp ()
{
	// Delete all pointer members.

}

void TBMeatsTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&metsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&metsKey, icc_f.metsKey);
	AddField((OSDataType *)&name, icc_f.name);
}

#include "TBMeavlCo.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBMeavlCo :: TBMeavlCo () : Table(icc_t.MeavlCo, 6, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBMeavlCo :: TBMeavlCo (const pTBMeavlCo meavlCo) : Table ((Table *) meavlCo)
{
	AddAllFields();

	if (meavlCo)
	{
		if ( ! meavlCo->GetCcmeKey().IsNull() )
			SetCcmeKey(meavlCo->GetCcmeKey());
		if ( ! meavlCo->GetTimsKey().IsNull() )
			SetTimsKey(meavlCo->GetTimsKey());
		if ( ! meavlCo->GetErrorpct().IsNull() )
			SetErrorpct(meavlCo->GetErrorpct());
		if ( ! meavlCo->GetMeadjust().IsNull() )
			SetMeadjust(meavlCo->GetMeadjust());
		if ( ! meavlCo->GetIsAccumulatedSeries().IsNull() )
			SetIsAccumulatedSeries(meavlCo->GetIsAccumulatedSeries());
		if ( ! meavlCo->GetLimitR().IsNull() )
			SetLimitR(meavlCo->GetLimitR());
	}
}


TBMeavlCo :: ~TBMeavlCo ()
{
	// Delete all pointer members.

}

void TBMeavlCo :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&ccmeKey);
	AddKeyField((OSDataType *)&timsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&ccmeKey, icc_f.ccmeKey);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&errorpct, icc_f.errorpct);
	AddField((OSDataType *)&meadjust, icc_f.meadjust);
	AddField((OSDataType *)&isAccumulatedSeries, icc_f.isAccSeries);
	AddField((OSDataType *)&limitr, icc_f.limitr);
}

#include "TBMimaxCt.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBMimaxCt :: TBMimaxCt () : Table(icc_t.MimaxCt, 7, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBMimaxCt :: TBMimaxCt (const pTBMimaxCt mimaxCt) : Table ((Table *) mimaxCt)
{
	AddAllFields();

	if (mimaxCt)
	{
		if ( ! mimaxCt->GetCcmeKey().IsNull() )
			SetCcmeKey(mimaxCt->GetCcmeKey());
		if ( ! mimaxCt->GetTimsKey().IsNull() )
			SetTimsKey(mimaxCt->GetTimsKey());
		if ( ! mimaxCt->GetStamonth().IsNull() )
			SetStamonth(mimaxCt->GetStamonth());
		if ( ! mimaxCt->GetMaxvalue().IsNull() )
			SetMaxvalue(mimaxCt->GetMaxvalue());
		if ( ! mimaxCt->GetMinvalue().IsNull() )
			SetMinvalue(mimaxCt->GetMinvalue());
		if ( ! mimaxCt->GetMinextre().IsNull() )
			SetMinextre(mimaxCt->GetMinextre());
		if ( ! mimaxCt->GetConstval().IsNull() )
			SetConstval(mimaxCt->GetConstval());
	}
}


TBMimaxCt :: ~TBMimaxCt ()
{
	// Delete all pointer members.

}

void TBMimaxCt :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&ccmeKey);
	AddKeyField((OSDataType *)&timsKey);
	AddKeyField((OSDataType *)&stamonth);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&ccmeKey, icc_f.ccmeKey);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&stamonth, icc_f.stamonth);
	AddField((OSDataType *)&maxvalue, icc_f.maxvalue);
	AddField((OSDataType *)&minvalue, icc_f.minvalue);
	AddField((OSDataType *)&minextre, icc_f.minextre);
	AddField((OSDataType *)&constval, icc_f.constval);
}

#include "TBMtrcotim.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBMtrcotim :: TBMtrcotim () : Table(icc_t.Mtrcotim, 4, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBMtrcotim :: TBMtrcotim (const pTBMtrcotim mtrcotim) : Table ((Table *) mtrcotim)
{
	AddAllFields();

	if (mtrcotim)
	{
		if ( ! mtrcotim->GetTimsKey().IsNull() )
			SetTimsKey(mtrcotim->GetTimsKey());
		if ( ! mtrcotim->GetMcmpKey().IsNull() )
			SetMcmpKey(mtrcotim->GetMcmpKey());
		if ( ! mtrcotim->GetValftime()->IsNull() )
			SetValftime(mtrcotim->GetValftime());
		if ( ! mtrcotim->GetValttime()->IsNull() )
			SetValttime(mtrcotim->GetValttime());
	}
}


TBMtrcotim :: ~TBMtrcotim ()
{
	// Delete all pointer members.

}

void TBMtrcotim :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&timsKey);
	AddKeyField((OSDataType *)&mcmpKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&mcmpKey, icc_f.mcmpKey);
	AddField((OSDataType *)&valftime, icc_f.valftime);
	AddField((OSDataType *)&valttime, icc_f.valttime);
}

#include "TBNetarea.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBNetarea :: TBNetarea () : Table(icc_t.Netarea, 17, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBNetarea :: TBNetarea (const pTBNetarea netarea) : Table ((Table *) netarea)
{
	AddAllFields();

	if (netarea)
	{
		if ( ! netarea->GetNetaKey().IsNull() )
			SetNetaKey(netarea->GetNetaKey());
		if ( ! netarea->GetSeqno().IsNull() )
			SetSeqno(netarea->GetSeqno());
		if ( ! netarea->GetSvkId()->IsNull() )
			SetSvkId(netarea->GetSvkId());
		if ( ! netarea->GetName()->IsNull() )
			SetName(netarea->GetName());
		if ( ! netarea->GetShorname()->IsNull() )
			SetShorname(netarea->GetShorname());
		if ( ! netarea->GetValftime()->IsNull() )
			SetValftime(netarea->GetValftime());
		if ( ! netarea->GetValttime()->IsNull() )
			SetValttime(netarea->GetValttime());
		if ( ! netarea->GetInfo()->IsNull() )
			SetInfo(netarea->GetInfo());
		if ( ! netarea->GetRestIs().IsNull() )
			SetRestIs(netarea->GetRestIs());
		if ( ! netarea->GetDelNorm().IsNull() )
			SetDelNorm(netarea->GetDelNorm());
		if ( ! netarea->GetCalcLos().IsNull() )
			SetCalcLos(netarea->GetCalcLos());
		if ( ! netarea->GetGrupKey().IsNull() )
			SetGrupKey(netarea->GetGrupKey());
		if ( ! netarea->GetNettKey().IsNull() )
			SetNettKey(netarea->GetNettKey());
		if ( ! netarea->GetBalResp().IsNull() )
			SetBalResp(netarea->GetBalResp());
		if ( ! netarea->GetBalSale().IsNull() )
			SetBalSale(netarea->GetBalSale());
		if ( ! netarea->GetNetowner().IsNull() )
			SetNetowner(netarea->GetNetowner());
		if ( ! netarea->GetReporter().IsNull() )
			SetReporter(netarea->GetReporter());
	}
}


TBNetarea :: ~TBNetarea ()
{
	// Delete all pointer members.

}

void TBNetarea :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&netaKey);
	AddKeyField((OSDataType *)&seqno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&netaKey, icc_f.netaKey);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&svkId, icc_f.svkId);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&shorname, icc_f.shorname);
	AddField((OSDataType *)&valftime, icc_f.valftime);
	AddField((OSDataType *)&valttime, icc_f.valttime);
	AddField((OSDataType *)&info, icc_f.info);
	AddField((OSDataType *)&restIs, icc_f.restIs);
	AddField((OSDataType *)&delNorm, icc_f.delNorm);
	AddField((OSDataType *)&calcLos, icc_f.calcLos);
	AddField((OSDataType *)&grupKey, icc_f.grupKey);
	AddField((OSDataType *)&nettKey, icc_f.nettKey);
	AddField((OSDataType *)&balResp, icc_f.balResp);
	AddField((OSDataType *)&balSale, icc_f.balSale);
	AddField((OSDataType *)&netowner, icc_f.netowner);
	AddField((OSDataType *)&reporter, icc_f.reporter);
}

#include "TBNetarTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBNetarTp :: TBNetarTp () : Table(icc_t.NetarTp, 3, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBNetarTp :: TBNetarTp (const pTBNetarTp netarTp) : Table ((Table *) netarTp)
{
	AddAllFields();

	if (netarTp)
	{
		if ( ! netarTp->GetNettKey().IsNull() )
			SetNettKey(netarTp->GetNettKey());
		if ( ! netarTp->GetShorname()->IsNull() )
			SetShorname(netarTp->GetShorname());
		if ( ! netarTp->GetName()->IsNull() )
			SetName(netarTp->GetName());
	}
}


TBNetarTp :: ~TBNetarTp ()
{
	// Delete all pointer members.

}

void TBNetarTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&nettKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&nettKey, icc_f.nettKey);
	AddField((OSDataType *)&shorname, icc_f.shorname);
	AddField((OSDataType *)&name, icc_f.name);
}

#include "TBOpbidare.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBOpbidare :: TBOpbidare () : Table(icc_t.Opbidare, 6, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBOpbidare :: TBOpbidare (const pTBOpbidare opbidare) : Table ((Table *) opbidare)
{
	AddAllFields();

	if (opbidare)
	{
		if ( ! opbidare->GetOpunKey().IsNull() )
			SetOpunKey(opbidare->GetOpunKey());
		if ( ! opbidare->GetMondate()->IsNull() )
			SetMondate(opbidare->GetMondate());
		if ( ! opbidare->GetTratKey()->IsNull() )
			SetTratKey(opbidare->GetTratKey());
		if ( ! opbidare->GetSigncode()->IsNull() )
			SetSigncode(opbidare->GetSigncode());
		if ( ! opbidare->GetMessRef()->IsNull() )
			SetMessRef(opbidare->GetMessRef());
		if ( ! opbidare->GetSentdate()->IsNull() )
			SetSentdate(opbidare->GetSentdate());
	}
}


TBOpbidare :: ~TBOpbidare ()
{
	// Delete all pointer members.

}

void TBOpbidare :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&mondate);
	AddKeyField((OSDataType *)&tratKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&mondate, icc_f.mondate);
	AddField((OSDataType *)&tratKey, icc_f.tratKey);
	AddField((OSDataType *)&signcode, icc_f.signcode);
	AddField((OSDataType *)&messRef, icc_f.messRef);
	AddField((OSDataType *)&sentdate, icc_f.sentdate);
}

#include "TBOprcofun.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBOprcofun::TBOprcofun() : Table(icc_t.Oprcofun, 2, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBOprcofun::TBOprcofun(const pTBOprcofun oprcofun) : Table((Table *)oprcofun)
{
	AddAllFields();

	if (oprcofun)
	{
		if (!oprcofun->GetFunKey().IsNull())
			SetFunKey(oprcofun->GetFunKey());
		if (!oprcofun->GetOprsKey().IsNull())
			SetOprsKey(oprcofun->GetOprsKey());
	}
}


TBOprcofun :: ~TBOprcofun()
{
	// Delete all pointer members.

}

void TBOprcofun::AddAllFields(void)
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&funKey);
	AddKeyField((OSDataType *)&oprsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&funKey, icc_f.funKey);
	AddField((OSDataType *)&oprsKey, icc_f.oprsKey);
}

#include "TBOprrst.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBOprrst :: TBOprrst () : Table(icc_t.Oprrst, 10, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBOprrst :: TBOprrst (const pTBOprrst oprrst) : Table ((Table *) oprrst)
{
	AddAllFields();

	if (oprrst)
	{
		if ( ! oprrst->GetOprsKey().IsNull() )
			SetOprsKey(oprrst->GetOprsKey());
		if ( ! oprrst->GetVersion().IsNull() )
			SetVersion(oprrst->GetVersion());
		if ( ! oprrst->GetPeriod().IsNull() )
			SetPeriod(oprrst->GetPeriod());
		if ( ! oprrst->GetCurvetyp().IsNull() )
			SetCurvetyp(oprrst->GetCurvetyp());
		if ( ! oprrst->GetDescript()->IsNull() )
			SetDescript(oprrst->GetDescript());
		if ( ! oprrst->GetGrupKey().IsNull() )
			SetGrupKey(oprrst->GetGrupKey());
		if ( ! oprrst->GetHydKeyo().IsNull() )
			SetHydKeyo(oprrst->GetHydKeyo());
		if ( ! oprrst->GetHydKey().IsNull() )
			SetHydKey(oprrst->GetHydKey());
		if ( ! oprrst->GetOprtKey().IsNull() )
			SetOprtKey(oprrst->GetOprtKey());
		if ( ! oprrst->GetUnmeKey().IsNull() )
			SetUnmeKey(oprrst->GetUnmeKey());
	}
}


TBOprrst :: ~TBOprrst ()
{
	// Delete all pointer members.

}

void TBOprrst :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&oprsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&oprsKey, icc_f.oprsKey);
	AddField((OSDataType *)&version, icc_f.version);
	AddField((OSDataType *)&period, icc_f.period);
	AddField((OSDataType *)&curvetyp, icc_f.curvetyp);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&grupKey, icc_f.grupKey);
	AddField((OSDataType *)&hydKeyo, icc_f.hydKeyo);
	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&oprtKey, icc_f.oprtKey);
	AddField((OSDataType *)&unmeKey, icc_f.unmeKey);
}

#include "TBOprrsTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBOprrsTp :: TBOprrsTp () : Table(icc_t.OprrsTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBOprrsTp :: TBOprrsTp (const pTBOprrsTp oprrsTp) : Table ((Table *) oprrsTp)
{
	AddAllFields();

	if (oprrsTp)
	{
		if ( ! oprrsTp->GetOprtKey().IsNull() )
			SetOprtKey(oprrsTp->GetOprtKey());
		if ( ! oprrsTp->GetCode()->IsNull() )
			SetCode(oprrsTp->GetCode());
		if ( ! oprrsTp->GetName()->IsNull() )
			SetName(oprrsTp->GetName());
		if ( ! oprrsTp->GetDescript()->IsNull() )
			SetDescript(oprrsTp->GetDescript());
	}
}


TBOprrsTp :: ~TBOprrsTp ()
{
	// Delete all pointer members.

}

void TBOprrsTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&oprtKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&oprtKey, icc_f.oprtKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBOprrsVl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBOprrsVl :: TBOprrsVl () : Table(icc_t.OprrsVl, 8, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBOprrsVl :: TBOprrsVl (const pTBOprrsVl oprrsVl) : Table ((Table *) oprrsVl)
{
	AddAllFields();

	if (oprrsVl)
	{
		if ( ! oprrsVl->GetOprsKey().IsNull() )
			SetOprsKey(oprrsVl->GetOprsKey());
		if ( ! oprrsVl->GetStartime()->IsNull() )
			SetStartime(oprrsVl->GetStartime());
		if ( ! oprrsVl->GetSeqno().IsNull() )
			SetSeqno(oprrsVl->GetSeqno());
		if ( ! oprrsVl->GetFintime()->IsNull() )
			SetFintime(oprrsVl->GetFintime());
		if ( ! oprrsVl->GetDescript()->IsNull() )
			SetDescript(oprrsVl->GetDescript());
		if ( ! oprrsVl->GetStartend().IsNull() )
			SetStartend(oprrsVl->GetStartend());
		if ( ! oprrsVl->GetRestVl1().IsNull() )
			SetRestVl1(oprrsVl->GetRestVl1());
		if ( ! oprrsVl->GetRestVl2().IsNull() )
			SetRestVl2(oprrsVl->GetRestVl2());
	}
}


TBOprrsVl :: ~TBOprrsVl ()
{
	// Delete all pointer members.

}

void TBOprrsVl :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&oprsKey);
	AddKeyField((OSDataType *)&startime);
	AddKeyField((OSDataType *)&seqno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&oprsKey, icc_f.oprsKey);
	AddField((OSDataType *)&startime, icc_f.startime);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&fintime, icc_f.fintime);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&startend, icc_f.startend);
	AddField((OSDataType *)&restVl1, icc_f.restVl1);
	AddField((OSDataType *)&restVl2, icc_f.restVl2);
}

#include "TBOprtunit.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBOprtunit :: TBOprtunit () : Table(icc_t.Oprtunit, 35, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBOprtunit :: TBOprtunit (const pTBOprtunit oprtunit) : Table ((Table *) oprtunit)
{
	AddAllFields();

	if (oprtunit)
	{
		if ( ! oprtunit->GetOpunKey().IsNull() )
			SetOpunKey(oprtunit->GetOpunKey());
		if ( ! oprtunit->GetName()->IsNull() )
			SetName(oprtunit->GetName());
		if ( ! oprtunit->GetShorname()->IsNull() )
			SetShorname(oprtunit->GetShorname());
		if ( ! oprtunit->GetDescript()->IsNull() )
			SetDescript(oprtunit->GetDescript());
		if ( ! oprtunit->GetAddress()->IsNull() )
			SetAddress(oprtunit->GetAddress());
		if ( ! oprtunit->GetPostcode()->IsNull() )
			SetPostcode(oprtunit->GetPostcode());
		if ( ! oprtunit->GetPosttown()->IsNull() )
			SetPosttown(oprtunit->GetPosttown());
		if ( ! oprtunit->GetCountry()->IsNull() )
			SetCountry(oprtunit->GetCountry());
		if ( ! oprtunit->GetPhonenum()->IsNull() )
			SetPhonenum(oprtunit->GetPhonenum());
		if ( ! oprtunit->GetReferee1()->IsNull() )
			SetReferee1(oprtunit->GetReferee1());
		if ( ! oprtunit->GetReferee2()->IsNull() )
			SetReferee2(oprtunit->GetReferee2());
		if ( ! oprtunit->GetReferee3()->IsNull() )
			SetReferee3(oprtunit->GetReferee3());
		if ( ! oprtunit->GetRefsubj()->IsNull() )
			SetRefsubj(oprtunit->GetRefsubj());
		if ( ! oprtunit->GetFaxnumb()->IsNull() )
			SetFaxnumb(oprtunit->GetFaxnumb());
		if ( ! oprtunit->GetEdcowner().IsNull() )
			SetEdcowner(oprtunit->GetEdcowner());
		if ( ! oprtunit->GetCode()->IsNull() )
			SetCode(oprtunit->GetCode());
		if ( ! oprtunit->GetOprstatu().IsNull() )
			SetOprstatu(oprtunit->GetOprstatu());
		if ( ! oprtunit->GetLogo1()->IsNull() )
			SetLogo1(oprtunit->GetLogo1());
		if ( ! oprtunit->GetLogo2()->IsNull() )
			SetLogo2(oprtunit->GetLogo2());
		if ( ! oprtunit->GetWeekoff().IsNull() )
			SetWeekoff(oprtunit->GetWeekoff());
		if ( ! oprtunit->GetBankacc()->IsNull() )
			SetBankacc(oprtunit->GetBankacc());
		if ( ! oprtunit->GetPostacc()->IsNull() )
			SetPostacc(oprtunit->GetPostacc());
		if ( ! oprtunit->GetGrupKey().IsNull() )
			SetGrupKey(oprtunit->GetGrupKey());
		if ( ! oprtunit->GetOpunKea().IsNull() )
			SetOpunKea(oprtunit->GetOpunKea());
		if ( ! oprtunit->GetBasCurr()->IsNull() )
			SetBasCurr(oprtunit->GetBasCurr());
		if ( ! oprtunit->GetRepCurr()->IsNull() )
			SetRepCurr(oprtunit->GetRepCurr());
		if ( ! oprtunit->GetContrNo().IsNull() )
			SetContrNo(oprtunit->GetContrNo());
		if ( ! oprtunit->GetCurrFac().IsNull() )
			SetCurrFac(oprtunit->GetCurrFac());
		if ( ! oprtunit->GetTrade().IsNull() )
			SetTrade(oprtunit->GetTrade());
		if ( ! oprtunit->GetCountryKey().IsNull() )
			SetCountryKey(oprtunit->GetCountryKey());
		if ( ! oprtunit->GetTsoOpunKey().IsNull() )
			SetTsoOpunKey(oprtunit->GetTsoOpunKey());
		if ( ! oprtunit->GetEdiCodelistResp()->IsNull() )
			SetEdiCodelistResp(oprtunit->GetEdiCodelistResp());
		if ( ! oprtunit->GetCopyExport().IsNull() )
			SetCopyExport(oprtunit->GetCopyExport());
		if ( ! oprtunit->GetCopySettlement().IsNull() )
			SetCopySettlement(oprtunit->GetCopySettlement());
		if (! oprtunit->GetTsoClrRef().IsNull() )
			SetTsoClrRef(oprtunit->GetTsoClrRef());
	}
}


TBOprtunit :: ~TBOprtunit ()
{
	// Delete all pointer members.

}

void TBOprtunit :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&shorname, icc_f.shorname);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&address, icc_f.address);
	AddField((OSDataType *)&postcode, icc_f.postcode);
	AddField((OSDataType *)&posttown, icc_f.posttown);
	AddField((OSDataType *)&country, icc_f.country);
	AddField((OSDataType *)&phonenum, icc_f.phonenum);
	AddField((OSDataType *)&referee1, icc_f.referee1);
	AddField((OSDataType *)&referee2, icc_f.referee2);
	AddField((OSDataType *)&referee3, icc_f.referee3);
	AddField((OSDataType *)&refsubj, icc_f.refsubj);
	AddField((OSDataType *)&faxnumb, icc_f.faxnumb);
	AddField((OSDataType *)&edcowner, icc_f.edcowner);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&oprstatu, icc_f.oprstatu);
	AddField((OSDataType *)&logo1, icc_f.logo1);
	AddField((OSDataType *)&logo2, icc_f.logo2);
	AddField((OSDataType *)&weekoff, icc_f.weekoff);
	AddField((OSDataType *)&bankacc, icc_f.bankacc);
	AddField((OSDataType *)&postacc, icc_f.postacc);
	AddField((OSDataType *)&grupKey, icc_f.grupKey);
	AddField((OSDataType *)&opunKea, icc_f.opunKea);
	AddField((OSDataType *)&basCurr, icc_f.basCurr);
	AddField((OSDataType *)&repCurr, icc_f.repCurr);
	AddField((OSDataType *)&contrNo, icc_f.contrNo);
	AddField((OSDataType *)&currFac, icc_f.currFac);
	AddField((OSDataType *)&trade, icc_f.trade);
	AddField((OSDataType *)&countryKey, icc_f.countryKey);
	AddField((OSDataType *)&tsoOpunKey, icc_f.tsoOpunKey);
	AddField((OSDataType *)&ediCodelistResp, icc_f.ediCodelistResp);
	AddField((OSDataType *)&copyExport, icc_f.copyExport);
	AddField((OSDataType *)&copySettlement, icc_f.copySettlement);
	AddField((OSDataType *)&tsoClrRef, "TSO_CLR_REF");
}

#include "TBOwnerTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBOwnerTp :: TBOwnerTp () : Table(icc_t.OwnerTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBOwnerTp :: TBOwnerTp (const pTBOwnerTp ownerTp) : Table ((Table *) ownerTp)
{
	AddAllFields();

	if (ownerTp)
	{
		if ( ! ownerTp->GetOwnrKey().IsNull() )
			SetOwnrKey(ownerTp->GetOwnrKey());
		if ( ! ownerTp->GetCode()->IsNull() )
			SetCode(ownerTp->GetCode());
		if ( ! ownerTp->GetName()->IsNull() )
			SetName(ownerTp->GetName());
		if ( ! ownerTp->GetEdiCode()->IsNull() )
			SetEdiCode(ownerTp->GetEdiCode());
	}
}


TBOwnerTp :: ~TBOwnerTp ()
{
	// Delete all pointer members.

}

void TBOwnerTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&ownrKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&ownrKey, icc_f.ownrKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&ediCode, icc_f.ediCode);
}

#include "TBPartempl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBPartempl :: TBPartempl () : Table(icc_t.Partempl, 6, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBPartempl :: TBPartempl (const pTBPartempl partempl) : Table ((Table *) partempl)
{
	AddAllFields();

	if (partempl)
	{
		if ( ! partempl->GetPartKey().IsNull() )
			SetPartKey(partempl->GetPartKey());
		if ( ! partempl->GetCode()->IsNull() )
			SetCode(partempl->GetCode());
		if ( ! partempl->GetName()->IsNull() )
			SetName(partempl->GetName());
		if ( ! partempl->GetDescript()->IsNull() )
			SetDescript(partempl->GetDescript());
		if ( ! partempl->GetProgKey().IsNull() )
			SetProgKey(partempl->GetProgKey());
		if ( ! partempl->GetPartKea().IsNull() )
			SetPartKea(partempl->GetPartKea());
	}
}


TBPartempl :: ~TBPartempl ()
{
	// Delete all pointer members.

}

void TBPartempl :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&partKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&partKey, icc_f.partKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&progKey, icc_f.progKey);
	AddField((OSDataType *)&partKea, icc_f.partKea);
}

#include "TBPartner.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBPartner :: TBPartner () : Table(icc_t.Partner, 15, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBPartner :: TBPartner (const pTBPartner partner) : Table ((Table *) partner)
{
	AddAllFields();

	if (partner)
	{
		if ( ! partner->GetOpunKey().IsNull() )
			SetOpunKey(partner->GetOpunKey());
		if ( ! partner->GetTrcode()->IsNull() )
			SetTrcode(partner->GetTrcode());
		if ( ! partner->GetEntregno()->IsNull() )
			SetEntregno(partner->GetEntregno());
		if ( ! partner->GetUnitregn()->IsNull() )
			SetUnitregn(partner->GetUnitregn());
		if ( ! partner->GetVisitadr()->IsNull() )
			SetVisitadr(partner->GetVisitadr());
		if ( ! partner->GetSmailadr()->IsNull() )
			SetSmailadr(partner->GetSmailadr());
		if ( ! partner->GetInvAdr()->IsNull() )
			SetInvAdr(partner->GetInvAdr());
		if ( ! partner->GetPhonenum()->IsNull() )
			SetPhonenum(partner->GetPhonenum());
		if ( ! partner->GetFaxnumb()->IsNull() )
			SetFaxnumb(partner->GetFaxnumb());
		if ( ! partner->GetEmaildom()->IsNull() )
			SetEmaildom(partner->GetEmaildom());
		if ( ! partner->GetPayPer().IsNull() )
			SetPayPer(partner->GetPayPer());
		if ( ! partner->GetInvTele()->IsNull() )
			SetInvTele(partner->GetInvTele());
		if ( ! partner->GetInvFax()->IsNull() )
			SetInvFax(partner->GetInvFax());
		if ( ! partner->GetMailKea().IsNull() )
			SetMailKea(partner->GetMailKea());
		if ( ! partner->GetMailKey().IsNull() )
			SetMailKey(partner->GetMailKey());
	}
}


TBPartner :: ~TBPartner ()
{
	// Delete all pointer members.

}

void TBPartner :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&entregno, icc_f.entregno);
	AddField((OSDataType *)&unitregn, icc_f.unitregn);
	AddField((OSDataType *)&visitadr, icc_f.visitadr);
	AddField((OSDataType *)&smailadr, icc_f.smailadr);
	AddField((OSDataType *)&invAdr, icc_f.invAdr);
	AddField((OSDataType *)&phonenum, icc_f.phonenum);
	AddField((OSDataType *)&faxnumb, icc_f.faxnumb);
	AddField((OSDataType *)&emaildom, icc_f.emaildom);
	AddField((OSDataType *)&payPer, icc_f.payPer);
	AddField((OSDataType *)&invTele, icc_f.invTele);
	AddField((OSDataType *)&invFax, icc_f.invFax);
	AddField((OSDataType *)&mailKea, icc_f.mailKea);
	AddField((OSDataType *)&mailKey, icc_f.mailKey);
}

#include "TBParVl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBParVl :: TBParVl () : Table(icc_t.ParVl, 11, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBParVl :: TBParVl (const pTBParVl parVl) : Table ((Table *) parVl)
{
	AddAllFields();

	if (parVl)
	{
		if ( ! parVl->GetParvKey().IsNull() )
			SetParvKey(parVl->GetParvKey());
		if ( ! parVl->GetSeqno().IsNull() )
			SetSeqno(parVl->GetSeqno());
		if ( ! parVl->GetCode()->IsNull() )
			SetCode(parVl->GetCode());
		if ( ! parVl->GetMaxvalue().IsNull() )
			SetMaxvalue(parVl->GetMaxvalue());
		if ( ! parVl->GetMinvalue().IsNull() )
			SetMinvalue(parVl->GetMinvalue());
		if ( ! parVl->GetRvalue().IsNull() )
			SetRvalue(parVl->GetRvalue());
		if ( ! parVl->GetCharvl()->IsNull() )
			SetCharvl(parVl->GetCharvl());
		if ( ! parVl->GetIvalue().IsNull() )
			SetIvalue(parVl->GetIvalue());
		if ( ! parVl->GetPardKey().IsNull() )
			SetPardKey(parVl->GetPardKey());
		if ( ! parVl->GetPartKey().IsNull() )
			SetPartKey(parVl->GetPartKey());
		if ( ! parVl->GetParvKea().IsNull() )
			SetParvKea(parVl->GetParvKea());
	}
}


TBParVl :: ~TBParVl ()
{
	// Delete all pointer members.

}

void TBParVl :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&parvKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&parvKey, icc_f.parvKey);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&maxvalue, icc_f.maxvalue);
	AddField((OSDataType *)&minvalue, icc_f.minvalue);
	AddField((OSDataType *)&rvalue, icc_f.rvalue);
	AddField((OSDataType *)&charvl, icc_f.charvl);
	AddField((OSDataType *)&ivalue, icc_f.ivalue);
	AddField((OSDataType *)&pardKey, icc_f.pardKey);
	AddField((OSDataType *)&partKey, icc_f.partKey);
	AddField((OSDataType *)&parvKea, icc_f.parvKea);
}

#include "TBPrdeprpl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBPrdeprpl :: TBPrdeprpl () : Table(icc_t.Prdeprpl, 4, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBPrdeprpl :: TBPrdeprpl (const pTBPrdeprpl prdeprpl) : Table ((Table *) prdeprpl)
{
	AddAllFields();

	if (prdeprpl)
	{
		if ( ! prdeprpl->GetHydKey().IsNull() )
			SetHydKey(prdeprpl->GetHydKey());
		if ( ! prdeprpl->GetPricelev().IsNull() )
			SetPricelev(prdeprpl->GetPricelev());
		if ( ! prdeprpl->GetDatetim()->IsNull() )
			SetDatetim(prdeprpl->GetDatetim());
		if ( ! prdeprpl->GetGenAmo().IsNull() )
			SetGenAmo(prdeprpl->GetGenAmo());
	}
}


TBPrdeprpl :: ~TBPrdeprpl ()
{
	// Delete all pointer members.

}

void TBPrdeprpl :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&hydKey);
	AddKeyField((OSDataType *)&pricelev);
	AddKeyField((OSDataType *)&datetim);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&pricelev, icc_f.pricelev);
	AddField((OSDataType *)&datetim, icc_f.datetim);
	AddField((OSDataType *)&genAmo, icc_f.genAmo);
}

#include "TBPriceint.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBPriceint :: TBPriceint () : Table(icc_t.Priceint, 4, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBPriceint :: TBPriceint (const pTBPriceint priceint) : Table ((Table *) priceint)
{
	AddAllFields();

	if (priceint)
	{
		if ( ! priceint->GetMondate()->IsNull() )
			SetMondate(priceint->GetMondate());
		if ( ! priceint->GetUnmeKey().IsNull() )
			SetUnmeKey(priceint->GetUnmeKey());
		if ( ! priceint->GetPricemin().IsNull() )
			SetPricemin(priceint->GetPricemin());
		if ( ! priceint->GetPricemax().IsNull() )
			SetPricemax(priceint->GetPricemax());
	}
}


TBPriceint :: ~TBPriceint ()
{
	// Delete all pointer members.

}

void TBPriceint :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&mondate);
	AddKeyField((OSDataType *)&unmeKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&mondate, icc_f.mondate);
	AddField((OSDataType *)&unmeKey, icc_f.unmeKey);
	AddField((OSDataType *)&pricemin, icc_f.pricemin);
	AddField((OSDataType *)&pricemax, icc_f.pricemax);
}

#include "TBPrlirepl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBPrlirepl :: TBPrlirepl () : Table(icc_t.Prlirepl, 4, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBPrlirepl :: TBPrlirepl (const pTBPrlirepl prlirepl) : Table ((Table *) prlirepl)
{
	AddAllFields();

	if (prlirepl)
	{
		if ( ! prlirepl->GetHydKey().IsNull() )
			SetHydKey(prlirepl->GetHydKey());
		if ( ! prlirepl->GetDatetim()->IsNull() )
			SetDatetim(prlirepl->GetDatetim());
		if ( ! prlirepl->GetGenAmo().IsNull() )
			SetGenAmo(prlirepl->GetGenAmo());
		if ( ! prlirepl->GetPricePt().IsNull() )
			SetPricePt(prlirepl->GetPricePt());
	}
}


TBPrlirepl :: ~TBPrlirepl ()
{
	// Delete all pointer members.

}

void TBPrlirepl :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&hydKey);
	AddKeyField((OSDataType *)&datetim);
	AddKeyField((OSDataType *)&genAmo);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&datetim, icc_f.datetim);
	AddField((OSDataType *)&genAmo, icc_f.genAmo);
	AddField((OSDataType *)&pricePt, icc_f.pricePt);
}

#include "TBProcotro.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBProcotro :: TBProcotro () : Table(icc_t.Procotro, 4, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBProcotro :: TBProcotro (const pTBProcotro procotro) : Table ((Table *) procotro)
{
	AddAllFields();

	if (procotro)
	{
		if ( ! procotro->GetOpunKey().IsNull() )
			SetOpunKey(procotro->GetOpunKey());
		if ( ! procotro->GetTrcode()->IsNull() )
			SetTrcode(procotro->GetTrcode());
		if ( ! procotro->GetProtKey().IsNull() )
			SetProtKey(procotro->GetProtKey());
		if ( ! procotro->GetGloberef()->IsNull() )
			SetGloberef(procotro->GetGloberef());
	}
}


TBProcotro :: ~TBProcotro ()
{
	// Delete all pointer members.

}

void TBProcotro :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);
	AddKeyField((OSDataType *)&protKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&protKey, icc_f.protKey);
	AddField((OSDataType *)&globeref, icc_f.globeref);
}

#include "TBProfile.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBProfile :: TBProfile () : Table(icc_t.Profile, 11, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBProfile :: TBProfile (const pTBProfile profile) : Table ((Table *) profile)
{
	AddAllFields();

	if (profile)
	{
		if ( ! profile->GetHydKey().IsNull() )
			SetHydKey(profile->GetHydKey());
		if ( ! profile->GetVuNo()->IsNull() )
			SetVuNo(profile->GetVuNo());
		if ( ! profile->GetContline().IsNull() )
			SetContline(profile->GetContline());
		if ( ! profile->GetRelX().IsNull() )
			SetRelX(profile->GetRelX());
		if ( ! profile->GetRelY().IsNull() )
			SetRelY(profile->GetRelY());
		if ( ! profile->GetMaLeft().IsNull() )
			SetMaLeft(profile->GetMaLeft());
		if ( ! profile->GetMaCent().IsNull() )
			SetMaCent(profile->GetMaCent());
		if ( ! profile->GetMaRight().IsNull() )
			SetMaRight(profile->GetMaRight());
		if ( ! profile->GetKCoef().IsNull() )
			SetKCoef(profile->GetKCoef());
		if ( ! profile->GetDistance().IsNull() )
			SetDistance(profile->GetDistance());
		if ( ! profile->GetSectKey().IsNull() )
			SetSectKey(profile->GetSectKey());
	}
}


TBProfile :: ~TBProfile ()
{
	// Delete all pointer members.

}

void TBProfile :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&hydKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&vuNo, icc_f.vuNo);
	AddField((OSDataType *)&contline, icc_f.contline);
	AddField((OSDataType *)&relX, icc_f.relX);
	AddField((OSDataType *)&relY, icc_f.relY);
	AddField((OSDataType *)&maLeft, icc_f.maLeft);
	AddField((OSDataType *)&maCent, icc_f.maCent);
	AddField((OSDataType *)&maRight, icc_f.maRight);
	AddField((OSDataType *)&kCoef, icc_f.kCoef);
	AddField((OSDataType *)&distance, icc_f.distance);
	AddField((OSDataType *)&sectKey, icc_f.sectKey);
}

#include "TBProgram.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBProgram :: TBProgram () : Table(icc_t.Program, 8, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBProgram :: TBProgram (const pTBProgram program) : Table ((Table *) program)
{
	AddAllFields();

	if (program)
	{
		if ( ! program->GetProgKey().IsNull() )
			SetProgKey(program->GetProgKey());
		if ( ! program->GetCode()->IsNull() )
			SetCode(program->GetCode());
		if ( ! program->GetName()->IsNull() )
			SetName(program->GetName());
		if ( ! program->GetDescript()->IsNull() )
			SetDescript(program->GetDescript());
		if ( ! program->GetSecurity().IsNull() )
			SetSecurity(program->GetSecurity());
		if ( ! program->GetStorflag().IsNull() )
			SetStorflag(program->GetStorflag());
		if ( ! program->GetLoginfo().IsNull() )
			SetLoginfo(program->GetLoginfo());
		if ( ! program->GetParaflag().IsNull() )
			SetParaflag(program->GetParaflag());
	}
}


TBProgram :: ~TBProgram ()
{
	// Delete all pointer members.

}

void TBProgram :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&progKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&progKey, icc_f.progKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&security, icc_f.security);
	AddField((OSDataType *)&storflag, icc_f.storflag);
	AddField((OSDataType *)&loginfo, icc_f.loginfo);
	AddField((OSDataType *)&paraflag, icc_f.paraflag);
}

#include "TBProject.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBProject :: TBProject () : Table(icc_t.Project, 7, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBProject :: TBProject (const pTBProject project) : Table ((Table *) project)
{
	AddAllFields();

	if (project)
	{
		if ( ! project->GetProKey().IsNull() )
			SetProKey(project->GetProKey());
		if ( ! project->GetCode()->IsNull() )
			SetCode(project->GetCode());
		if ( ! project->GetName()->IsNull() )
			SetName(project->GetName());
		if ( ! project->GetDescript()->IsNull() )
			SetDescript(project->GetDescript());
		if ( ! project->GetEsttime()->IsNull() )
			SetEsttime(project->GetEsttime());
		if ( ! project->GetProKeyp().IsNull() )
			SetProKeyp(project->GetProKeyp());
		if ( ! project->GetWatcKey().IsNull() )
			SetWatcKey(project->GetWatcKey());
	}
}


TBProject :: ~TBProject ()
{
	// Delete all pointer members.

}

void TBProject :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&proKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&proKey, icc_f.proKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&esttime, icc_f.esttime);
	AddField((OSDataType *)&proKeyp, icc_f.proKeyp);
	AddField((OSDataType *)&watcKey, icc_f.watcKey);
}

#include "TBProtoTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBProtoTp :: TBProtoTp () : Table(icc_t.ProtoTp, 3, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBProtoTp :: TBProtoTp (const pTBProtoTp protoTp) : Table ((Table *) protoTp)
{
	AddAllFields();

	if (protoTp)
	{
		if ( ! protoTp->GetProtKey().IsNull() )
			SetProtKey(protoTp->GetProtKey());
		if ( ! protoTp->GetCode()->IsNull() )
			SetCode(protoTp->GetCode());
		if ( ! protoTp->GetName()->IsNull() )
			SetName(protoTp->GetName());
	}
}


TBProtoTp :: ~TBProtoTp ()
{
	// Delete all pointer members.

}

void TBProtoTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&protKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&protKey, icc_f.protKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
}

#include "TBPrparDs.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBPrparDs :: TBPrparDs () : Table(icc_t.PrparDs, 10, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBPrparDs :: TBPrparDs (const pTBPrparDs prparDs) : Table ((Table *) prparDs)
{
	AddAllFields();

	if (prparDs)
	{
		if ( ! prparDs->GetPardKey().IsNull() )
			SetPardKey(prparDs->GetPardKey());
		if ( ! prparDs->GetCode()->IsNull() )
			SetCode(prparDs->GetCode());
		if ( ! prparDs->GetName()->IsNull() )
			SetName(prparDs->GetName());
		if ( ! prparDs->GetEsttime()->IsNull() )
			SetEsttime(prparDs->GetEsttime());
		if ( ! prparDs->GetTotime()->IsNull() )
			SetTotime(prparDs->GetTotime());
		if ( ! prparDs->GetOpname()->IsNull() )
			SetOpname(prparDs->GetOpname());
		if ( ! prparDs->GetDescript()->IsNull() )
			SetDescript(prparDs->GetDescript());
		if ( ! prparDs->GetVersion().IsNull() )
			SetVersion(prparDs->GetVersion());
		if ( ! prparDs->GetHydKey().IsNull() )
			SetHydKey(prparDs->GetHydKey());
		if ( ! prparDs->GetProgKey().IsNull() )
			SetProgKey(prparDs->GetProgKey());
	}
}


TBPrparDs :: ~TBPrparDs ()
{
	// Delete all pointer members.

}

void TBPrparDs :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&pardKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&pardKey, icc_f.pardKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&esttime, icc_f.esttime);
	AddField((OSDataType *)&totime, icc_f.totime);
	AddField((OSDataType *)&opname, icc_f.opname);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&version, icc_f.version);
	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&progKey, icc_f.progKey);
}

#include "TBPrparMx.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBPrparMx :: TBPrparMx () : Table(icc_t.PrparMx, 6, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBPrparMx :: TBPrparMx (const pTBPrparMx prparMx) : Table ((Table *) prparMx)
{
	AddAllFields();

	if (prparMx)
	{
		if ( ! prparMx->GetPrmxKey().IsNull() )
			SetPrmxKey(prparMx->GetPrmxKey());
		if ( ! prparMx->GetCode()->IsNull() )
			SetCode(prparMx->GetCode());
		if ( ! prparMx->GetName()->IsNull() )
			SetName(prparMx->GetName());
		if ( ! prparMx->GetDescript()->IsNull() )
			SetDescript(prparMx->GetDescript());
		if ( ! prparMx->GetPardKey().IsNull() )
			SetPardKey(prparMx->GetPardKey());
		if ( ! prparMx->GetPartKey().IsNull() )
			SetPartKey(prparMx->GetPartKey());
	}
}


TBPrparMx :: ~TBPrparMx ()
{
	// Delete all pointer members.

}

void TBPrparMx :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&prmxKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&prmxKey, icc_f.prmxKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&pardKey, icc_f.pardKey);
	AddField((OSDataType *)&partKey, icc_f.partKey);
}

#include "TBPrrunDs.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBPrrunDs :: TBPrrunDs () : Table(icc_t.PrrunDs, 18, 1, 1, TABLE_SELECT_VIEW)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBPrrunDs :: TBPrrunDs (const pTBPrrunDs prrunDs) : Table ((Table *) prrunDs)
{
	AddAllFields();

	if (prrunDs)
	{
		if ( ! prrunDs->GetPrdKey().IsNull() )
			SetPrdKey(prrunDs->GetPrdKey());
		if ( ! prrunDs->GetCode()->IsNull() )
			SetCode(prrunDs->GetCode());
		if ( ! prrunDs->GetName()->IsNull() )
			SetName(prrunDs->GetName());
		if ( ! prrunDs->GetEsttime()->IsNull() )
			SetEsttime(prrunDs->GetEsttime());
		if ( ! prrunDs->GetOpname()->IsNull() )
			SetOpname(prrunDs->GetOpname());
		if ( ! prrunDs->GetSeqno().IsNull() )
			SetSeqno(prrunDs->GetSeqno());
		if ( ! prrunDs->GetExeok()->IsNull() )
			SetExeok(prrunDs->GetExeok());
		if ( ! prrunDs->GetDescript()->IsNull() )
			SetDescript(prrunDs->GetDescript());
		if ( ! prrunDs->GetVersion().IsNull() )
			SetVersion(prrunDs->GetVersion());
		if ( ! prrunDs->GetFunKey().IsNull() )
			SetFunKey(prrunDs->GetFunKey());
		if ( ! prrunDs->GetGrupKey().IsNull() )
			SetGrupKey(prrunDs->GetGrupKey());
		if ( ! prrunDs->GetLdfKey().IsNull() )
			SetLdfKey(prrunDs->GetLdfKey());
		if ( ! prrunDs->GetPardKey().IsNull() )
			SetPardKey(prrunDs->GetPardKey());
		if ( ! prrunDs->GetProgKey().IsNull() )
			SetProgKey(prrunDs->GetProgKey());
		if ( ! prrunDs->GetPainKey().IsNull() )
			SetPainKey(prrunDs->GetPainKey());
		if ( ! prrunDs->GetSimKey().IsNull() )
			SetSimKey(prrunDs->GetSimKey());
		if ( ! prrunDs->GetTdsKey().IsNull() )
			SetTdsKey(prrunDs->GetTdsKey());
		if ( ! prrunDs->GetWatcKey().IsNull() )
			SetWatcKey(prrunDs->GetWatcKey());
	}
}


TBPrrunDs :: ~TBPrrunDs ()
{
	// Delete all pointer members.

}

void TBPrrunDs :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&prdKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&prdKey, icc_f.prdKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&esttime, icc_f.esttime);
	AddField((OSDataType *)&opname, icc_f.opname);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&exeok, icc_f.exeok);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&version, icc_f.version);
	AddField((OSDataType *)&funKey, icc_f.funKey);
	AddField((OSDataType *)&grupKey, icc_f.grupKey);
	AddField((OSDataType *)&ldfKey, icc_f.ldfKey);
	AddField((OSDataType *)&pardKey, icc_f.pardKey);
	AddField((OSDataType *)&progKey, icc_f.progKey);
	AddField((OSDataType *)&painKey, icc_f.painKey);
	AddField((OSDataType *)&simKey, icc_f.simKey);
	AddField((OSDataType *)&tdsKey, icc_f.tdsKey);
	AddField((OSDataType *)&watcKey, icc_f.watcKey);
}

#include "TBRecadr.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBRecadr :: TBRecadr () : Table(icc_t.Recadr, 12, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBRecadr :: TBRecadr (const pTBRecadr recadr) : Table ((Table *) recadr)
{
	AddAllFields();

	if (recadr)
	{
		if ( ! recadr->GetOpunKey().IsNull() )
			SetOpunKey(recadr->GetOpunKey());
		if ( ! recadr->GetProtKey().IsNull() )
			SetProtKey(recadr->GetProtKey());
		if ( ! recadr->GetSeqno().IsNull() )
			SetSeqno(recadr->GetSeqno());
		if ( ! recadr->GetAddress()->IsNull() )
			SetAddress(recadr->GetAddress());
		if ( ! recadr->GetAperak().IsNull() )
			SetAperak(recadr->GetAperak());
		if ( ! recadr->GetApplId()->IsNull() )
			SetApplId(recadr->GetApplId());
		if ( ! recadr->GetValftime()->IsNull() )
			SetValftime(recadr->GetValftime());
		if ( ! recadr->GetValttime()->IsNull() )
			SetValttime(recadr->GetValttime());
		if ( ! recadr->GetReceiver().IsNull() )
			SetReceiver(recadr->GetReceiver());
		if ( ! recadr->GetTimeLimit().IsNull() )
			SetTimeLimit(recadr->GetTimeLimit());
		if ( ! recadr->GetUtcOffsetExportDates().IsNull() )
			SetUtcOffsetExportDates(recadr->GetUtcOffsetExportDates());
		if ( ! recadr->GetExportQueue()->IsNull() )
			SetExportQueue(recadr->GetExportQueue());
	}
}


TBRecadr :: ~TBRecadr ()
{
	// Delete all pointer members.

}

void TBRecadr :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&protKey);
	AddKeyField((OSDataType *)&seqno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&protKey, icc_f.protKey);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&address, icc_f.address);
	AddField((OSDataType *)&aperak, icc_f.aperak);
	AddField((OSDataType *)&applId, icc_f.applId);
	AddField((OSDataType *)&valftime, icc_f.valftime);
	AddField((OSDataType *)&valttime, icc_f.valttime);
	AddField((OSDataType *)&receiver, icc_f.receiver);
	AddField((OSDataType *)&timeLimit, icc_f.timeLimit);
	AddField((OSDataType *)&utcOffsetExportDates, icc_f.utcOffsetExportDates);
	AddField((OSDataType *)&exportQueue, icc_f.exportQueue);
}

#include "TBRefTrobject.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBRefTrobject :: TBRefTrobject () : Table(icc_t.RefTrobject, 3, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBRefTrobject :: TBRefTrobject (const pTBRefTrobject refTrobject) : Table ((Table *) refTrobject)
{
	AddAllFields();

	if (refTrobject)
	{
		if ( ! refTrobject->GetTradeKey().IsNull() )
			SetTradeKey(refTrobject->GetTradeKey());
		if ( ! refTrobject->GetOpunKey().IsNull() )
			SetOpunKey(refTrobject->GetOpunKey());
		if ( ! refTrobject->GetTrcode()->IsNull() )
			SetTrcode(refTrobject->GetTrcode());
	}
}


TBRefTrobject :: ~TBRefTrobject ()
{
	// Delete all pointer members.

}

void TBRefTrobject :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&tradeKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&tradeKey, icc_f.tradeKey);
	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
}

#include "TBReporTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBReporTp :: TBReporTp () : Table(icc_t.ReporTp, 2, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBReporTp :: TBReporTp (const pTBReporTp reporTp) : Table ((Table *) reporTp)
{
	AddAllFields();

	if (reporTp)
	{
		if ( ! reporTp->GetReptKey()->IsNull() )
			SetReptKey(reporTp->GetReptKey());
		if ( ! reporTp->GetGmsmod()->IsNull() )
			SetGmsmod(reporTp->GetGmsmod());
	}
}


TBReporTp :: ~TBReporTp ()
{
	// Delete all pointer members.

}

void TBReporTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&reptKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&reptKey, icc_f.reptKey);
	AddField((OSDataType *)&gmsmod, icc_f.gmsmod);
}

#include "TBRepvlCt.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBRepvlCt :: TBRepvlCt () : Table(icc_t.RepvlCt, 3, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBRepvlCt :: TBRepvlCt (const pTBRepvlCt repvlCt) : Table ((Table *) repvlCt)
{
	AddAllFields();

	if (repvlCt)
	{
		if ( ! repvlCt->GetCcmeKey().IsNull() )
			SetCcmeKey(repvlCt->GetCcmeKey());
		if ( ! repvlCt->GetTimsKey().IsNull() )
			SetTimsKey(repvlCt->GetTimsKey());
		if ( ! repvlCt->GetMaxrep().IsNull() )
			SetMaxrep(repvlCt->GetMaxrep());
	}
}


TBRepvlCt :: ~TBRepvlCt ()
{
	// Delete all pointer members.

}

void TBRepvlCt :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&ccmeKey);
	AddKeyField((OSDataType *)&timsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&ccmeKey, icc_f.ccmeKey);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&maxrep, icc_f.maxrep);
}

#include "TBReservoi.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBReservoi :: TBReservoi () : Table(icc_t.Reservoi, 10, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBReservoi :: TBReservoi (const pTBReservoi reservoi) : Table ((Table *) reservoi)
{
	AddAllFields();

	if (reservoi)
	{
		if ( ! reservoi->GetHydKey().IsNull() )
			SetHydKey(reservoi->GetHydKey());
		if ( ! reservoi->GetMxvol().IsNull() )
			SetMxvol(reservoi->GetMxvol());
		if ( ! reservoi->GetHrv().IsNull() )
			SetHrv(reservoi->GetHrv());
		if ( ! reservoi->GetLrv().IsNull() )
			SetLrv(reservoi->GetLrv());
		if ( ! reservoi->GetDescript()->IsNull() )
			SetDescript(reservoi->GetDescript());
		if ( ! reservoi->GetRestKey().IsNull() )
			SetRestKey(reservoi->GetRestKey());
		if ( ! reservoi->GetInflowperiod().IsNull() )
			SetInflowperiod(reservoi->GetInflowperiod());
		if ( ! reservoi->GetOwnerPct().IsNull() )
			SetOwnerPct(reservoi->GetOwnerPct());
		if ( ! reservoi->GetEnerfact().IsNull() )
			SetEnerfact(reservoi->GetEnerfact());
		if ( ! reservoi->GetRefAltitude().IsNull() )
			SetRefAltitude(reservoi->GetRefAltitude());
	}
}


TBReservoi :: ~TBReservoi ()
{
	// Delete all pointer members.

}

void TBReservoi :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&hydKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&mxvol, icc_f.mxvol);
	AddField((OSDataType *)&hrv, icc_f.hrv);
	AddField((OSDataType *)&lrv, icc_f.lrv);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&restKey, icc_f.restKey);
	AddField((OSDataType *)&inflowperiod, icc_f.inflowperiod);
	AddField((OSDataType *)&ownerPct, icc_f.ownerPct);
	AddField((OSDataType *)&enerfact, icc_f.enerfact);
	AddField((OSDataType *)&refAltitude, icc_f.refAltitude);
}

#include "TBResulVl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBResulVl :: TBResulVl () : Table(icc_t.ResulVl, 5, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBResulVl :: TBResulVl (const pTBResulVl resulVl) : Table ((Table *) resulVl)
{
	AddAllFields();

	if (resulVl)
	{
		if ( ! resulVl->GetRevlKey().IsNull() )
			SetRevlKey(resulVl->GetRevlKey());
		if ( ! resulVl->GetCvalue()->IsNull() )
			SetCvalue(resulVl->GetCvalue());
		if ( ! resulVl->GetRvalue().IsNull() )
			SetRvalue(resulVl->GetRvalue());
		if ( ! resulVl->GetIvalue().IsNull() )
			SetIvalue(resulVl->GetIvalue());
		if ( ! resulVl->GetTimsKey().IsNull() )
			SetTimsKey(resulVl->GetTimsKey());
	}
}


TBResulVl :: ~TBResulVl ()
{
	// Delete all pointer members.

}

void TBResulVl :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&revlKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&revlKey, icc_f.revlKey);
	AddField((OSDataType *)&cvalue, icc_f.cvalue);
	AddField((OSDataType *)&rvalue, icc_f.rvalue);
	AddField((OSDataType *)&ivalue, icc_f.ivalue);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
}

#include "TBResvoTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBResvoTp :: TBResvoTp () : Table(icc_t.ResvoTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBResvoTp :: TBResvoTp (const pTBResvoTp resvoTp) : Table ((Table *) resvoTp)
{
	AddAllFields();

	if (resvoTp)
	{
		if ( ! resvoTp->GetRestKey().IsNull() )
			SetRestKey(resvoTp->GetRestKey());
		if ( ! resvoTp->GetCode()->IsNull() )
			SetCode(resvoTp->GetCode());
		if ( ! resvoTp->GetName()->IsNull() )
			SetName(resvoTp->GetName());
		if ( ! resvoTp->GetDescript()->IsNull() )
			SetDescript(resvoTp->GetDescript());
	}
}


TBResvoTp :: ~TBResvoTp ()
{
	// Delete all pointer members.

}

void TBResvoTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&restKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&restKey, icc_f.restKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBSecpoint.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBSecpoint :: TBSecpoint () : Table(icc_t.Secpoint, 7, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBSecpoint :: TBSecpoint (const pTBSecpoint secpoint) : Table ((Table *) secpoint)
{
	AddAllFields();

	if (secpoint)
	{
		if ( ! secpoint->GetHydKey().IsNull() )
			SetHydKey(secpoint->GetHydKey());
		if ( ! secpoint->GetRelX().IsNull() )
			SetRelX(secpoint->GetRelX());
		if ( ! secpoint->GetRelY().IsNull() )
			SetRelY(secpoint->GetRelY());
		if ( ! secpoint->GetRelZ().IsNull() )
			SetRelZ(secpoint->GetRelZ());
		if ( ! secpoint->GetRelDist().IsNull() )
			SetRelDist(secpoint->GetRelDist());
		if ( ! secpoint->GetSecpKey().IsNull() )
			SetSecpKey(secpoint->GetSecpKey());
		if ( ! secpoint->GetSubsKey().IsNull() )
			SetSubsKey(secpoint->GetSubsKey());
	}
}


TBSecpoint :: ~TBSecpoint ()
{
	// Delete all pointer members.

}

void TBSecpoint :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&hydKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&relX, icc_f.relX);
	AddField((OSDataType *)&relY, icc_f.relY);
	AddField((OSDataType *)&relZ, icc_f.relZ);
	AddField((OSDataType *)&relDist, icc_f.relDist);
	AddField((OSDataType *)&secpKey, icc_f.secpKey);
	AddField((OSDataType *)&subsKey, icc_f.subsKey);
}

#include "TBSensor.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBSensor :: TBSensor () : Table(icc_t.Sensor, 15, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBSensor :: TBSensor (const pTBSensor sensor) : Table ((Table *) sensor)
{
	AddAllFields();

	if (sensor)
	{
		if ( ! sensor->GetFcno().IsNull() )
			SetFcno(sensor->GetFcno());
		if ( ! sensor->GetFsno().IsNull() )
			SetFsno(sensor->GetFsno());
		if ( ! sensor->GetSensno().IsNull() )
			SetSensno(sensor->GetSensno());
		if ( ! sensor->GetCode()->IsNull() )
			SetCode(sensor->GetCode());
		if ( ! sensor->GetName()->IsNull() )
			SetName(sensor->GetName());
		if ( ! sensor->GetDescript()->IsNull() )
			SetDescript(sensor->GetDescript());
		if ( ! sensor->GetAltitude().IsNull() )
			SetAltitude(sensor->GetAltitude());
		if ( ! sensor->GetLarDate()->IsNull() )
			SetLarDate(sensor->GetLarDate());
		if ( ! sensor->GetBloccode().IsNull() )
			SetBloccode(sensor->GetBloccode());
		if ( ! sensor->GetTsexp().IsNull() )
			SetTsexp(sensor->GetTsexp());
		if ( ! sensor->GetScalFac().IsNull() )
			SetScalFac(sensor->GetScalFac());
		if ( ! sensor->GetByteprvl().IsNull() )
			SetByteprvl(sensor->GetByteprvl());
		if ( ! sensor->GetHihvmode()->IsNull() )
			SetHihvmode(sensor->GetHihvmode());
		if ( ! sensor->GetMemeKey().IsNull() )
			SetMemeKey(sensor->GetMemeKey());
		if ( ! sensor->GetTimsKey().IsNull() )
			SetTimsKey(sensor->GetTimsKey());
	}
}


TBSensor :: ~TBSensor ()
{
	// Delete all pointer members.

}

void TBSensor :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&fcno);
	AddKeyField((OSDataType *)&fsno);
	AddKeyField((OSDataType *)&sensno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&fcno, icc_f.fcno);
	AddField((OSDataType *)&fsno, icc_f.fsno);
	AddField((OSDataType *)&sensno, icc_f.sensno);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&altitude, icc_f.altitude);
	AddField((OSDataType *)&larDate, icc_f.larDate);
	AddField((OSDataType *)&bloccode, icc_f.bloccode);
	AddField((OSDataType *)&tsexp, icc_f.tsexp);
	AddField((OSDataType *)&scalFac, icc_f.scalFac);
	AddField((OSDataType *)&byteprvl, icc_f.byteprvl);
	AddField((OSDataType *)&hihvmode, icc_f.hihvmode);
	AddField((OSDataType *)&memeKey, icc_f.memeKey);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
}

#include "TBSign.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBSign :: TBSign () : Table(icc_t.Sign, 7, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBSign :: TBSign (const pTBSign sign) : Table ((Table *) sign)
{
	AddAllFields();

	if (sign)
	{
		if ( ! sign->GetSigncode()->IsNull() )
			SetSigncode(sign->GetSigncode());
		if ( ! sign->GetName()->IsNull() )
			SetName(sign->GetName());
		if ( ! sign->GetPasswd()->IsNull() )
			SetPasswd(sign->GetPasswd());
		if ( ! sign->GetBid24().IsNull() )
			SetBid24(sign->GetBid24());
		if ( ! sign->GetBidrp().IsNull() )
			SetBidrp(sign->GetBidrp());
		if ( ! sign->GetBidwm().IsNull() )
			SetBidwm(sign->GetBidwm());
		if ( ! sign->GetOpunKey().IsNull() )
			SetOpunKey(sign->GetOpunKey());
	}
}


TBSign :: ~TBSign ()
{
	// Delete all pointer members.

}

void TBSign :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&signcode);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&signcode, icc_f.signcode);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&passwd, icc_f.passwd);
	AddField((OSDataType *)&bid24, icc_f.bid24);
	AddField((OSDataType *)&bidrp, icc_f.bidrp);
	AddField((OSDataType *)&bidwm, icc_f.bidwm);
	AddField((OSDataType *)&opunKey, icc_f.opunKey);
}

#include "TBSimuler.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBSimuler :: TBSimuler () : Table(icc_t.Simuler, 9, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBSimuler :: TBSimuler (const pTBSimuler simuler) : Table ((Table *) simuler)
{
	AddAllFields();

	if (simuler)
	{
		if ( ! simuler->GetSimKey().IsNull() )
			SetSimKey(simuler->GetSimKey());
		if ( ! simuler->GetCode()->IsNull() )
			SetCode(simuler->GetCode());
		if ( ! simuler->GetDescript()->IsNull() )
			SetDescript(simuler->GetDescript());
		if ( ! simuler->GetOpname()->IsNull() )
			SetOpname(simuler->GetOpname());
		if ( ! simuler->GetStartime()->IsNull() )
			SetStartime(simuler->GetStartime());
		if ( ! simuler->GetEndtime()->IsNull() )
			SetEndtime(simuler->GetEndtime());
		if ( ! simuler->GetExetime()->IsNull() )
			SetExetime(simuler->GetExetime());
		if ( ! simuler->GetExeok()->IsNull() )
			SetExeok(simuler->GetExeok());
		if ( ! simuler->GetAltKey().IsNull() )
			SetAltKey(simuler->GetAltKey());
	}
}


TBSimuler :: ~TBSimuler ()
{
	// Delete all pointer members.

}

void TBSimuler :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&simKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&simKey, icc_f.simKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&opname, icc_f.opname);
	AddField((OSDataType *)&startime, icc_f.startime);
	AddField((OSDataType *)&endtime, icc_f.endtime);
	AddField((OSDataType *)&exetime, icc_f.exetime);
	AddField((OSDataType *)&exeok, icc_f.exeok);
	AddField((OSDataType *)&altKey, icc_f.altKey);
}

#include "TBSpparDs.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBSpparDs :: TBSpparDs () : Table(icc_t.SpparDs, 8, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBSpparDs :: TBSpparDs (const pTBSpparDs spparDs) : Table ((Table *) spparDs)
{
	AddAllFields();

	if (spparDs)
	{
		if ( ! spparDs->GetPainKey().IsNull() )
			SetPainKey(spparDs->GetPainKey());
		if ( ! spparDs->GetCode()->IsNull() )
			SetCode(spparDs->GetCode());
		if ( ! spparDs->GetName()->IsNull() )
			SetName(spparDs->GetName());
		if ( ! spparDs->GetEsttime()->IsNull() )
			SetEsttime(spparDs->GetEsttime());
		if ( ! spparDs->GetOpname()->IsNull() )
			SetOpname(spparDs->GetOpname());
		if ( ! spparDs->GetDescript()->IsNull() )
			SetDescript(spparDs->GetDescript());
		if ( ! spparDs->GetVersion().IsNull() )
			SetVersion(spparDs->GetVersion());
		if ( ! spparDs->GetProgKey().IsNull() )
			SetProgKey(spparDs->GetProgKey());
	}
}


TBSpparDs :: ~TBSpparDs ()
{
	// Delete all pointer members.

}

void TBSpparDs :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&painKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&painKey, icc_f.painKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&esttime, icc_f.esttime);
	AddField((OSDataType *)&opname, icc_f.opname);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&version, icc_f.version);
	AddField((OSDataType *)&progKey, icc_f.progKey);
}

#include "TBStacoprr.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBStacoprr :: TBStacoprr () : Table(icc_t.Stacoprr, 2, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBStacoprr :: TBStacoprr (const pTBStacoprr stacoprr) : Table ((Table *) stacoprr)
{
	AddAllFields();

	if (stacoprr)
	{
		if ( ! stacoprr->GetPrdKey().IsNull() )
			SetPrdKey(stacoprr->GetPrdKey());
		if ( ! stacoprr->GetStdsKey().IsNull() )
			SetStdsKey(stacoprr->GetStdsKey());
	}
}


TBStacoprr :: ~TBStacoprr ()
{
	// Delete all pointer members.

}

void TBStacoprr :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&prdKey);
	AddKeyField((OSDataType *)&stdsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&prdKey, icc_f.prdKey);
	AddField((OSDataType *)&stdsKey, icc_f.stdsKey);
}

#include "TBStatempl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBStatempl :: TBStatempl () : Table(icc_t.Statempl, 8, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBStatempl :: TBStatempl (const pTBStatempl statempl) : Table ((Table *) statempl)
{
	AddAllFields();

	if (statempl)
	{
		if ( ! statempl->GetSttpKey().IsNull() )
			SetSttpKey(statempl->GetSttpKey());
		if ( ! statempl->GetCode()->IsNull() )
			SetCode(statempl->GetCode());
		if ( ! statempl->GetName()->IsNull() )
			SetName(statempl->GetName());
		if ( ! statempl->GetDescript()->IsNull() )
			SetDescript(statempl->GetDescript());
		if ( ! statempl->GetMaxvalue().IsNull() )
			SetMaxvalue(statempl->GetMaxvalue());
		if ( ! statempl->GetMinvalue().IsNull() )
			SetMinvalue(statempl->GetMinvalue());
		if ( ! statempl->GetProgKey().IsNull() )
			SetProgKey(statempl->GetProgKey());
		if ( ! statempl->GetSttpKea().IsNull() )
			SetSttpKea(statempl->GetSttpKea());
	}
}


TBStatempl :: ~TBStatempl ()
{
	// Delete all pointer members.

}

void TBStatempl :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&sttpKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&sttpKey, icc_f.sttpKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&maxvalue, icc_f.maxvalue);
	AddField((OSDataType *)&minvalue, icc_f.minvalue);
	AddField((OSDataType *)&progKey, icc_f.progKey);
	AddField((OSDataType *)&sttpKea, icc_f.sttpKea);
}

#include "TBStateDs.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBStateDs :: TBStateDs () : Table(icc_t.StateDs, 8, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBStateDs :: TBStateDs (const pTBStateDs stateDs) : Table ((Table *) stateDs)
{
	AddAllFields();

	if (stateDs)
	{
		if ( ! stateDs->GetStdsKey().IsNull() )
			SetStdsKey(stateDs->GetStdsKey());
		if ( ! stateDs->GetName()->IsNull() )
			SetName(stateDs->GetName());
		if ( ! stateDs->GetTotime()->IsNull() )
			SetTotime(stateDs->GetTotime());
		if ( ! stateDs->GetVersion().IsNull() )
			SetVersion(stateDs->GetVersion());
		if ( ! stateDs->GetEsttime()->IsNull() )
			SetEsttime(stateDs->GetEsttime());
		if ( ! stateDs->GetOpname()->IsNull() )
			SetOpname(stateDs->GetOpname());
		if ( ! stateDs->GetHydKey().IsNull() )
			SetHydKey(stateDs->GetHydKey());
		if ( ! stateDs->GetProgKey().IsNull() )
			SetProgKey(stateDs->GetProgKey());
	}
}


TBStateDs :: ~TBStateDs ()
{
	// Delete all pointer members.

}

void TBStateDs :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&stdsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&stdsKey, icc_f.stdsKey);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&totime, icc_f.totime);
	AddField((OSDataType *)&version, icc_f.version);
	AddField((OSDataType *)&esttime, icc_f.esttime);
	AddField((OSDataType *)&opname, icc_f.opname);
	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&progKey, icc_f.progKey);
}

#include "TBTarifdef.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTarifdef :: TBTarifdef () : Table(icc_t.Tarifdef, 7, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTarifdef :: TBTarifdef (const pTBTarifdef tarifdef) : Table ((Table *) tarifdef)
{
	AddAllFields();

	if (tarifdef)
	{
		if ( ! tarifdef->GetTarfKey().IsNull() )
			SetTarfKey(tarifdef->GetTarfKey());
		if ( ! tarifdef->GetSeqno().IsNull() )
			SetSeqno(tarifdef->GetSeqno());
		if ( ! tarifdef->GetCode()->IsNull() )
			SetCode(tarifdef->GetCode());
		if ( ! tarifdef->GetName()->IsNull() )
			SetName(tarifdef->GetName());
		if ( ! tarifdef->GetCalcdef()->IsNull() )
			SetCalcdef(tarifdef->GetCalcdef());
		if ( ! tarifdef->GetDescript()->IsNull() )
			SetDescript(tarifdef->GetDescript());
		if ( ! tarifdef->GetInfo()->IsNull() )
			SetInfo(tarifdef->GetInfo());
	}
}


TBTarifdef :: ~TBTarifdef ()
{
	// Delete all pointer members.

}

void TBTarifdef :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&tarfKey);
	AddKeyField((OSDataType *)&seqno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&tarfKey, icc_f.tarfKey);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&calcdef, icc_f.calcdef);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&info, icc_f.info);
}

#include "TBTariff.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTariff :: TBTariff () : Table(icc_t.Tariff, 6, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTariff :: TBTariff (const pTBTariff tariff) : Table ((Table *) tariff)
{
	AddAllFields();

	if (tariff)
	{
		if ( ! tariff->GetOpunKey().IsNull() )
			SetOpunKey(tariff->GetOpunKey());
		if ( ! tariff->GetTrcode()->IsNull() )
			SetTrcode(tariff->GetTrcode());
		if ( ! tariff->GetIdxDate()->IsNull() )
			SetIdxDate(tariff->GetIdxDate());
		if ( ! tariff->GetAdjDate()->IsNull() )
			SetAdjDate(tariff->GetAdjDate());
		if ( ! tariff->GetAdjRef()->IsNull() )
			SetAdjRef(tariff->GetAdjRef());
		if ( ! tariff->GetNxtAdj()->IsNull() )
			SetNxtAdj(tariff->GetNxtAdj());
	}
}


TBTariff :: ~TBTariff ()
{
	// Delete all pointer members.

}

void TBTariff :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&idxDate, icc_f.idxDate);
	AddField((OSDataType *)&adjDate, icc_f.adjDate);
	AddField((OSDataType *)&adjRef, icc_f.adjRef);
	AddField((OSDataType *)&nxtAdj, icc_f.nxtAdj);
}

#include "TBTdscotim.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTdscotim :: TBTdscotim () : Table(icc_t.Tdscotim, 6, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTdscotim :: TBTdscotim (const pTBTdscotim tdscotim) : Table ((Table *) tdscotim)
{
	AddAllFields();

	if (tdscotim)
	{
		if ( ! tdscotim->GetTdsKey().IsNull() )
			SetTdsKey(tdscotim->GetTdsKey());
		if ( ! tdscotim->GetTimsKey().IsNull() )
			SetTimsKey(tdscotim->GetTimsKey());
		if ( ! tdscotim->GetRvalue().IsNull() )
			SetRvalue(tdscotim->GetRvalue());
		if ( ! tdscotim->GetRvalue2().IsNull() )
			SetRvalue2(tdscotim->GetRvalue2());
		if ( ! tdscotim->GetTswiRef().IsNull() )
			SetTswiRef(tdscotim->GetTswiRef());
		if ( ! tdscotim->GetTsteRef().IsNull() )
			SetTsteRef(tdscotim->GetTsteRef());
	}
}


TBTdscotim :: ~TBTdscotim ()
{
	// Delete all pointer members.

}

void TBTdscotim :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&tdsKey);
	AddKeyField((OSDataType *)&timsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&tdsKey, icc_f.tdsKey);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&rvalue, icc_f.rvalue);
	AddField((OSDataType *)&rvalue2, icc_f.rvalue2);
	AddField((OSDataType *)&tswiRef, icc_f.tswiRef);
	AddField((OSDataType *)&tsteRef, icc_f.tsteRef);
}

#include "TBTimcohyd.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTimcohyd :: TBTimcohyd () : Table(icc_t.Timcohyd, 4, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTimcohyd :: TBTimcohyd (const pTBTimcohyd timcohyd) : Table ((Table *) timcohyd)
{
	AddAllFields();

	if (timcohyd)
	{
		if ( ! timcohyd->GetHydKey().IsNull() )
			SetHydKey(timcohyd->GetHydKey());
		if ( ! timcohyd->GetTimsKey().IsNull() )
			SetTimsKey(timcohyd->GetTimsKey());
		if ( ! timcohyd->GetRvalue().IsNull() )
			SetRvalue(timcohyd->GetRvalue());
		if ( ! timcohyd->GetTicoKey().IsNull() )
			SetTicoKey(timcohyd->GetTicoKey());
	}
}


TBTimcohyd :: ~TBTimcohyd ()
{
	// Delete all pointer members.

}

void TBTimcohyd :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&hydKey);
	AddKeyField((OSDataType *)&timsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&rvalue, icc_f.rvalue);
	AddField((OSDataType *)&ticoKey, icc_f.ticoKey);
}

#include "TBTimcoTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTimcoTp :: TBTimcoTp () : Table(icc_t.TimcoTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTimcoTp :: TBTimcoTp (const pTBTimcoTp timcoTp) : Table ((Table *) timcoTp)
{
	AddAllFields();

	if (timcoTp)
	{
		if ( ! timcoTp->GetTicoKey().IsNull() )
			SetTicoKey(timcoTp->GetTicoKey());
		if ( ! timcoTp->GetCode()->IsNull() )
			SetCode(timcoTp->GetCode());
		if ( ! timcoTp->GetName()->IsNull() )
			SetName(timcoTp->GetName());
		if ( ! timcoTp->GetDescript()->IsNull() )
			SetDescript(timcoTp->GetDescript());
	}
}


TBTimcoTp :: ~TBTimcoTp ()
{
	// Delete all pointer members.

}

void TBTimcoTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&ticoKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&ticoKey, icc_f.ticoKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBTimcovcg.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTimcovcg :: TBTimcovcg () : Table(icc_t.Timcovcg, 7, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTimcovcg :: TBTimcovcg (const pTBTimcovcg timcovcg) : Table ((Table *) timcovcg)
{
	AddAllFields();

	if (timcovcg)
	{
		if ( ! timcovcg->GetTimsKey().IsNull() )
			SetTimsKey(timcovcg->GetTimsKey());
		if ( ! timcovcg->GetValFact().IsNull() )
			SetValFact(timcovcg->GetValFact());
		if ( ! timcovcg->GetCorFact().IsNull() )
			SetCorFact(timcovcg->GetCorFact());
		if ( ! timcovcg->GetValKey().IsNull() )
			SetValKey(timcovcg->GetValKey());
		if ( ! timcovcg->GetCorKey().IsNull() )
			SetCorKey(timcovcg->GetCorKey());
		if ( ! timcovcg->GetValcok().IsNull() )
			SetValcok(timcovcg->GetValcok());
		if ( ! timcovcg->GetCorcok().IsNull() )
			SetCorcok(timcovcg->GetCorcok());
	}
}


TBTimcovcg :: ~TBTimcovcg ()
{
	// Delete all pointer members.

}

void TBTimcovcg :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&timsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&valFact, icc_f.valFact);
	AddField((OSDataType *)&corFact, icc_f.corFact);
	AddField((OSDataType *)&valKey, icc_f.valKey);
	AddField((OSDataType *)&corKey, icc_f.corKey);
	AddField((OSDataType *)&valcok, icc_f.valcok);
	AddField((OSDataType *)&corcok, icc_f.corcok);
}

#include "TBTimeser.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTimeser :: TBTimeser () : Table(icc_t.Timeser, 34, 1, 1, TABLE_SELECT_VIEW)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTimeser :: TBTimeser (const pTBTimeser timeser) : Table ((Table *) timeser)
{
	AddAllFields();

	if (timeser)
	{
		if ( ! timeser->GetTimsKey().IsNull() )
			SetTimsKey(timeser->GetTimsKey());
		if ( ! timeser->GetTscode()->IsNull() )
			SetTscode(timeser->GetTscode());
		if ( ! timeser->GetName()->IsNull() )
			SetName(timeser->GetName());
		if ( ! timeser->GetDescript()->IsNull() )
			SetDescript(timeser->GetDescript());
		if ( ! timeser->GetDocindex().IsNull() )
			SetDocindex(timeser->GetDocindex());
		if ( ! timeser->GetEsttime()->IsNull() )
			SetEsttime(timeser->GetEsttime());
		if ( ! timeser->GetBastime()->IsNull() )
			SetBastime(timeser->GetBastime());
		if ( ! timeser->GetStartime()->IsNull() )
			SetStartime(timeser->GetStartime());
		if ( ! timeser->GetFintime()->IsNull() )
			SetFintime(timeser->GetFintime());
		if ( ! timeser->GetLvdate()->IsNull() )
			SetLvdate(timeser->GetLvdate());
		if ( ! timeser->GetLcdate()->IsNull() )
			SetLcdate(timeser->GetLcdate());
		if ( ! timeser->GetLapDate()->IsNull() )
			SetLapDate(timeser->GetLapDate());
		if ( ! timeser->GetTimsVsc()->IsNull() )
			SetTimsVsc(timeser->GetTimsVsc());
		if ( ! timeser->GetDatasize().IsNull() )
			SetDatasize(timeser->GetDatasize());
		if ( ! timeser->GetSteptype().IsNull() )
			SetSteptype(timeser->GetSteptype());
		if ( ! timeser->GetCurvetyp().IsNull() )
			SetCurvetyp(timeser->GetCurvetyp());
		if ( ! timeser->GetVersion().IsNull() )
			SetVersion(timeser->GetVersion());
		if ( ! timeser->GetReferenc().IsNull() )
			SetReferenc(timeser->GetReferenc());
		if ( ! timeser->GetHvhcmode()->IsNull() )
			SetHvhcmode(timeser->GetHvhcmode());
		if ( ! timeser->GetFilename()->IsNull() )
			SetFilename(timeser->GetFilename());
		if ( ! timeser->GetParaflag().IsNull() )
			SetParaflag(timeser->GetParaflag());
		if ( ! timeser->GetGrupKey().IsNull() )
			SetGrupKey(timeser->GetGrupKey());
		if ( ! timeser->GetHydKey().IsNull() )
			SetHydKey(timeser->GetHydKey());
		if ( ! timeser->GetPrdKey().IsNull() )
			SetPrdKey(timeser->GetPrdKey());
		if ( ! timeser->GetTsprKey().IsNull() )
			SetTsprKey(timeser->GetTsprKey());
		if ( ! timeser->GetTscaKey().IsNull() )
			SetTscaKey(timeser->GetTscaKey());
		if ( ! timeser->GetTsquKey().IsNull() )
			SetTsquKey(timeser->GetTsquKey());
		if ( ! timeser->GetTsinKey().IsNull() )
			SetTsinKey(timeser->GetTsinKey());
		if ( ! timeser->GetTsorKey().IsNull() )
			SetTsorKey(timeser->GetTsorKey());
		if ( ! timeser->GetTserKey().IsNull() )
			SetTserKey(timeser->GetTserKey());
		if ( ! timeser->GetUnmeKey().IsNull() )
			SetUnmeKey(timeser->GetUnmeKey());
		if ( ! timeser->GetRcKey().IsNull() )
			SetRcKey(timeser->GetRcKey());
		if ( ! timeser->GetConvertusinglocaltime().IsNull() )
			SetConvertusinglocaltime(timeser->GetConvertusinglocaltime());
		if ( ! timeser->GetUseRegistrationHandling().IsNull() )
			SetUseRegistrationHandling(timeser->GetUseRegistrationHandling());
	}
}


TBTimeser :: ~TBTimeser ()
{
	// Delete all pointer members.

}

void TBTimeser :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&timsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&tscode, icc_f.tscode);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&docindex, icc_f.docindex);
	AddField((OSDataType *)&esttime, icc_f.esttime);
	AddField((OSDataType *)&bastime, icc_f.bastime);
	AddField((OSDataType *)&startime, icc_f.startime);
	AddField((OSDataType *)&fintime, icc_f.fintime);
	AddField((OSDataType *)&lvdate, icc_f.lvdate);
	AddField((OSDataType *)&lcdate, icc_f.lcdate);
	AddField((OSDataType *)&lapDate, icc_f.lapDate);
	AddField((OSDataType *)&timsVsc, icc_f.timsVsc);
	AddField((OSDataType *)&datasize, icc_f.datasize);
	AddField((OSDataType *)&steptype, icc_f.steptype);
	AddField((OSDataType *)&curvetyp, icc_f.curvetyp);
	AddField((OSDataType *)&version, icc_f.version);
	AddField((OSDataType *)&referenc, icc_f.referenc);
	AddField((OSDataType *)&hvhcmode, icc_f.hvhcmode);
	AddField((OSDataType *)&filename, icc_f.filename);
	AddField((OSDataType *)&paraflag, icc_f.paraflag);
	AddField((OSDataType *)&grupKey, icc_f.grupKey);
	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&prdKey, icc_f.prdKey);
	AddField((OSDataType *)&tsprKey, icc_f.tsprKey);
	AddField((OSDataType *)&tscaKey, icc_f.tscaKey);
	AddField((OSDataType *)&tsquKey, icc_f.tsquKey);
	AddField((OSDataType *)&tsinKey, icc_f.tsinKey);
	AddField((OSDataType *)&tsorKey, icc_f.tsorKey);
	AddField((OSDataType *)&tserKey, icc_f.tserKey);
	AddField((OSDataType *)&unmeKey, icc_f.unmeKey);
	AddField((OSDataType *)&rcKey, icc_f.rcKey);
	AddField((OSDataType *)&convertusinglocaltime, icc_f.convertusinglocaltime);
	AddField((OSDataType *)&useRegistrationHandling, icc_f.useRegistrationHandling);
}

#include "TBTimseDs.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTimseDs :: TBTimseDs () : Table(icc_t.TimseDs, 8, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTimseDs :: TBTimseDs (const pTBTimseDs timseDs) : Table ((Table *) timseDs)
{
	AddAllFields();

	if (timseDs)
	{
		if ( ! timseDs->GetTdsKey().IsNull() )
			SetTdsKey(timseDs->GetTdsKey());
		if ( ! timseDs->GetCode()->IsNull() )
			SetCode(timseDs->GetCode());
		if ( ! timseDs->GetName()->IsNull() )
			SetName(timseDs->GetName());
		if ( ! timseDs->GetEsttime()->IsNull() )
			SetEsttime(timseDs->GetEsttime());
		if ( ! timseDs->GetOpname()->IsNull() )
			SetOpname(timseDs->GetOpname());
		if ( ! timseDs->GetDescript()->IsNull() )
			SetDescript(timseDs->GetDescript());
		if ( ! timseDs->GetVersion().IsNull() )
			SetVersion(timseDs->GetVersion());
		if ( ! timseDs->GetProgKey().IsNull() )
			SetProgKey(timseDs->GetProgKey());
	}
}


TBTimseDs :: ~TBTimseDs ()
{
	// Delete all pointer members.

}

void TBTimseDs :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&tdsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&tdsKey, icc_f.tdsKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&esttime, icc_f.esttime);
	AddField((OSDataType *)&opname, icc_f.opname);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&version, icc_f.version);
	AddField((OSDataType *)&progKey, icc_f.progKey);
}

#include "TBTmarkar.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTmarkar :: TBTmarkar () : Table(icc_t.Tmarkar, 4, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTmarkar :: TBTmarkar (const pTBTmarkar tmarkar) : Table ((Table *) tmarkar)
{
	AddAllFields();

	if (tmarkar)
	{
		if ( ! tmarkar->GetYearweek().IsNull() )
			SetYearweek(tmarkar->GetYearweek());
		if ( ! tmarkar->GetDescript()->IsNull() )
			SetDescript(tmarkar->GetDescript());
		if ( ! tmarkar->GetMartKey()->IsNull() )
			SetMartKey(tmarkar->GetMartKey());
		if ( ! tmarkar->GetTratKey()->IsNull() )
			SetTratKey(tmarkar->GetTratKey());
	}
}


TBTmarkar :: ~TBTmarkar ()
{
	// Delete all pointer members.

}

void TBTmarkar :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&yearweek);
	AddKeyField((OSDataType *)&martKey);
	AddKeyField((OSDataType *)&tratKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&yearweek, icc_f.yearweek);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&martKey, icc_f.martKey);
	AddField((OSDataType *)&tratKey, icc_f.tratKey);
}

#include "TBTrafo2w.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTrafo2w :: TBTrafo2w () : Table(icc_t.Trafo2w, 4, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTrafo2w :: TBTrafo2w (const pTBTrafo2w trafo2w) : Table ((Table *) trafo2w)
{
	AddAllFields();

	if (trafo2w)
	{
		if ( ! trafo2w->GetElecKey().IsNull() )
			SetElecKey(trafo2w->GetElecKey());
		if ( ! trafo2w->GetRefpower().IsNull() )
			SetRefpower(trafo2w->GetRefpower());
		if ( ! trafo2w->GetRefvoltp().IsNull() )
			SetRefvoltp(trafo2w->GetRefvoltp());
		if ( ! trafo2w->GetRefvolts().IsNull() )
			SetRefvolts(trafo2w->GetRefvolts());
	}
}


TBTrafo2w :: ~TBTrafo2w ()
{
	// Delete all pointer members.

}

void TBTrafo2w :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&elecKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&elecKey, icc_f.elecKey);
	AddField((OSDataType *)&refpower, icc_f.refpower);
	AddField((OSDataType *)&refvoltp, icc_f.refvoltp);
	AddField((OSDataType *)&refvolts, icc_f.refvolts);
}

#include "TBTranslin.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTranslin :: TBTranslin () : Table(icc_t.Translin, 1, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTranslin :: TBTranslin (const pTBTranslin translin) : Table ((Table *) translin)
{
	AddAllFields();

	if (translin)
	{
		if ( ! translin->GetElecKey().IsNull() )
			SetElecKey(translin->GetElecKey());
	}
}


TBTranslin :: ~TBTranslin ()
{
	// Delete all pointer members.

}

void TBTranslin :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&elecKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&elecKey, icc_f.elecKey);
}

#include "TBTrareTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTrareTp :: TBTrareTp () : Table(icc_t.TrareTp, 7, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTrareTp :: TBTrareTp (const pTBTrareTp trareTp) : Table ((Table *) trareTp)
{
	AddAllFields();

	if (trareTp)
	{
		if ( ! trareTp->GetTratKey()->IsNull() )
			SetTratKey(trareTp->GetTratKey());
		if ( ! trareTp->GetColour()->IsNull() )
			SetColour(trareTp->GetColour());
		if ( ! trareTp->GetMartKey()->IsNull() )
			SetMartKey(trareTp->GetMartKey());
		if ( ! trareTp->GetCode()->IsNull() )
			SetCode(trareTp->GetCode());
		if ( ! trareTp->GetVlcode().IsNull() )
			SetVlcode(trareTp->GetVlcode());
		if ( ! trareTp->GetSeqno().IsNull() )
			SetSeqno(trareTp->GetSeqno());
		if (!trareTp->GetEicCode()->IsNull())
			SetEicCode(trareTp->GetEicCode());
	}
}


TBTrareTp :: ~TBTrareTp ()
{
	// Delete all pointer members.

}

void TBTrareTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&tratKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&tratKey, icc_f.tratKey);
	AddField((OSDataType *)&colour, icc_f.colour);
	AddField((OSDataType *)&martKey, icc_f.martKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&vlcode, icc_f.vlcode);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&eicCode, icc_f.eicCode);
}

#include "TBTrbusbar.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTrbusbar :: TBTrbusbar () : Table(icc_t.Trbusbar, 3, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTrbusbar :: TBTrbusbar (const pTBTrbusbar trbusbar) : Table ((Table *) trbusbar)
{
	AddAllFields();

	if (trbusbar)
	{
		if ( ! trbusbar->GetOpunKey().IsNull() )
			SetOpunKey(trbusbar->GetOpunKey());
		if ( ! trbusbar->GetTrcode()->IsNull() )
			SetTrcode(trbusbar->GetTrcode());
		if ( ! trbusbar->GetSnitt().IsNull() )
			SetSnitt(trbusbar->GetSnitt());
	}
}


TBTrbusbar :: ~TBTrbusbar ()
{
	// Delete all pointer members.

}

void TBTrbusbar :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&snitt, icc_f.snitt);
}

#include "TBTrdreport.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTrdreport :: TBTrdreport () : Table(icc_t.Trdreport, 4, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTrdreport :: TBTrdreport (const pTBTrdreport trdreport) : Table ((Table *) trdreport)
{
	AddAllFields();

	if (trdreport)
	{
		if ( ! trdreport->GetOpunKey().IsNull() )
			SetOpunKey(trdreport->GetOpunKey());
		if ( ! trdreport->GetRpcode()->IsNull() )
			SetRpcode(trdreport->GetRpcode());
		if ( ! trdreport->GetSystemfl().IsNull() )
			SetSystemfl(trdreport->GetSystemfl());
		if ( ! trdreport->GetTrrpKey().IsNull() )
			SetTrrpKey(trdreport->GetTrrpKey());
	}
}


TBTrdreport :: ~TBTrdreport ()
{
	// Delete all pointer members.

}

void TBTrdreport :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&rpcode);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&rpcode, icc_f.rpcode);
	AddField((OSDataType *)&systemfl, icc_f.systemfl);
	AddField((OSDataType *)&trrpKey, icc_f.trrpKey);
}

#include "TBTrdrpTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTrdrpTp :: TBTrdrpTp () : Table(icc_t.TrdrpTp, 3, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTrdrpTp :: TBTrdrpTp (const pTBTrdrpTp trdrpTp) : Table ((Table *) trdrpTp)
{
	AddAllFields();

	if (trdrpTp)
	{
		if ( ! trdrpTp->GetTrrpKey().IsNull() )
			SetTrrpKey(trdrpTp->GetTrrpKey());
		if ( ! trdrpTp->GetCode()->IsNull() )
			SetCode(trdrpTp->GetCode());
		if ( ! trdrpTp->GetName()->IsNull() )
			SetName(trdrpTp->GetName());
	}
}


TBTrdrpTp :: ~TBTrdrpTp ()
{
	// Delete all pointer members.

}

void TBTrdrpTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&trrpKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&trrpKey, icc_f.trrpKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
}

#include "TBTrelsbid.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTrelsbid :: TBTrelsbid () : Table(icc_t.Trelsbid, 15, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTrelsbid :: TBTrelsbid (const pTBTrelsbid trelsbid) : Table ((Table *) trelsbid)
{
	AddAllFields();

	if (trelsbid)
	{
		if ( ! trelsbid->GetOpunKey().IsNull() )
			SetOpunKey(trelsbid->GetOpunKey());
		if ( ! trelsbid->GetTrcode()->IsNull() )
			SetTrcode(trelsbid->GetTrcode());
		if ( ! trelsbid->GetUnmeKey().IsNull() )
			SetUnmeKey(trelsbid->GetUnmeKey());
		if ( ! trelsbid->GetMinBidQuantity().IsNull() )
			SetMinBidQuantity(trelsbid->GetMinBidQuantity());
		if ( ! trelsbid->GetMaxBidQuantity().IsNull() )
			SetMaxBidQuantity(trelsbid->GetMaxBidQuantity());
		if ( ! trelsbid->GetFactor().IsNull() )
			SetFactor(trelsbid->GetFactor());
		if ( ! trelsbid->GetPriceStrategy().IsNull() )
			SetPriceStrategy(trelsbid->GetPriceStrategy());
		if ( ! trelsbid->GetFirstPriceStrategy().IsNull() )
			SetFirstPriceStrategy(trelsbid->GetFirstPriceStrategy());
		if ( ! trelsbid->GetInsertVolume0().IsNull() )
			SetInsertVolume0(trelsbid->GetInsertVolume0());
		if ( ! trelsbid->GetCorrectInvalidBid().IsNull() )
			SetCorrectInvalidBid(trelsbid->GetCorrectInvalidBid());
		if ( ! trelsbid->GetTimestepDiffHandling().IsNull() )
			SetTimestepDiffHandling(trelsbid->GetTimestepDiffHandling());
		if ( ! trelsbid->GetDoReduction().IsNull() )
			SetDoReduction(trelsbid->GetDoReduction());
		if ( ! trelsbid->GetReductionMethod().IsNull() )
			SetReductionMethod(trelsbid->GetReductionMethod());
		if ( ! trelsbid->GetReduceNPrices().IsNull() )
			SetReduceNPrices(trelsbid->GetReduceNPrices());
	}
}


TBTrelsbid :: ~TBTrelsbid ()
{
	// Delete all pointer members.

}

void TBTrelsbid :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&unmeKey, icc_f.unmeKey);
	AddField((OSDataType *)&minBidQuantity, icc_f.minBidQuantity);
	AddField((OSDataType *)&maxBidQuantity, icc_f.maxBidQuantity);
	AddField((OSDataType *)&factor, icc_f.factor);
	AddField((OSDataType *)&priceStrategy, icc_f.priceStrategy);
	AddField((OSDataType *)&firstPriceStrategy, icc_f.firstPriceStrategy);
	AddField((OSDataType *)&insertVolume0, icc_f.insertVolume0);
	AddField((OSDataType *)&correctInvalidBid, icc_f.correctInvalidBid);
	AddField((OSDataType *)&timestepDiffHandling, icc_f.timestepDiffHandling);
	AddField((OSDataType *)&doReduction, icc_f.doReduction);
	AddField((OSDataType *)&reductionMethod, icc_f.reductionMethod);
	AddField((OSDataType *)&reduceNPrices, icc_f.reduceNPrices);
	AddField((OSDataType *)&portfolioId, icc_f.portfolioId);
}

#include "TBTrobcomm.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTrobcomm :: TBTrobcomm () : Table(icc_t.Trobcomm, 4, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTrobcomm :: TBTrobcomm (const pTBTrobcomm trobcomm) : Table ((Table *) trobcomm)
{
	AddAllFields();

	if (trobcomm)
	{
		if ( ! trobcomm->GetOpunKey().IsNull() )
			SetOpunKey(trobcomm->GetOpunKey());
		if ( ! trobcomm->GetTrcode()->IsNull() )
			SetTrcode(trobcomm->GetTrcode());
		if ( ! trobcomm->GetSeqno().IsNull() )
			SetSeqno(trobcomm->GetSeqno());
		if ( ! trobcomm->GetDescript()->IsNull() )
			SetDescript(trobcomm->GetDescript());
	}
}


TBTrobcomm :: ~TBTrobcomm ()
{
	// Delete all pointer members.

}

void TBTrobcomm :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);
	AddKeyField((OSDataType *)&seqno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBTrobject.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTrobject :: TBTrobject () : Table(icc_t.Trobject, 10, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTrobject :: TBTrobject (const pTBTrobject trobject) : Table ((Table *) trobject)
{
	AddAllFields();

	if (trobject)
	{
		if ( ! trobject->GetTrcode()->IsNull() )
			SetTrcode(trobject->GetTrcode());
		if ( ! trobject->GetName()->IsNull() )
			SetName(trobject->GetName());
		if ( ! trobject->GetOpunKey().IsNull() )
			SetOpunKey(trobject->GetOpunKey());
		if ( ! trobject->GetDircKey()->IsNull() )
			SetDircKey(trobject->GetDircKey());
		if ( ! trobject->GetElecKey().IsNull() )
			SetElecKey(trobject->GetElecKey());
		if ( ! trobject->GetElnoKey().IsNull() )
			SetElnoKey(trobject->GetElnoKey());
		if ( ! trobject->GetHydKey().IsNull() )
			SetHydKey(trobject->GetHydKey());
		if ( ! trobject->GetTimsKey().IsNull() )
			SetTimsKey(trobject->GetTimsKey());
		if ( ! trobject->GetTrotKey()->IsNull() )
			SetTrotKey(trobject->GetTrotKey());
		if ( ! trobject->GetPcodeKey().IsNull() )
			SetPcodeKey(trobject->GetPcodeKey());
	}
}


TBTrobject :: ~TBTrobject ()
{
	// Delete all pointer members.

}

void TBTrobject :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&dircKey, icc_f.dircKey);
	AddField((OSDataType *)&elecKey, icc_f.elecKey);
	AddField((OSDataType *)&elnoKey, icc_f.elnoKey);
	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&trotKey, icc_f.trotKey);
	AddField((OSDataType *)&pcodeKey, "PCODE_KEY");
}

#include "TBTrobjTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTrobjTp :: TBTrobjTp () : Table(icc_t.TrobjTp, 3, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTrobjTp :: TBTrobjTp (const pTBTrobjTp trobjTp) : Table ((Table *) trobjTp)
{
	AddAllFields();

	if (trobjTp)
	{
		if ( ! trobjTp->GetTrotKey()->IsNull() )
			SetTrotKey(trobjTp->GetTrotKey());
		if ( ! trobjTp->GetEnumval().IsNull() )
			SetEnumval(trobjTp->GetEnumval());
		if ( ! trobjTp->GetName()->IsNull() )
			SetName(trobjTp->GetName());
	}
}


TBTrobjTp :: ~TBTrobjTp ()
{
	// Delete all pointer members.

}

void TBTrobjTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&trotKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&trotKey, icc_f.trotKey);
	AddField((OSDataType *)&enumval, icc_f.enumval);
	AddField((OSDataType *)&name, icc_f.name);
}

#include "TBTrocotro.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTrocotro :: TBTrocotro () : Table(icc_t.Trocotro, 4, 4, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTrocotro :: TBTrocotro (const pTBTrocotro trocotro) : Table ((Table *) trocotro)
{
	AddAllFields();

	if (trocotro)
	{
		if ( ! trocotro->GetOpunKea().IsNull() )
			SetOpunKea(trocotro->GetOpunKea());
		if ( ! trocotro->GetTrcode1()->IsNull() )
			SetTrcode1(trocotro->GetTrcode1());
		if ( ! trocotro->GetOpunKey().IsNull() )
			SetOpunKey(trocotro->GetOpunKey());
		if ( ! trocotro->GetTrcode2()->IsNull() )
			SetTrcode2(trocotro->GetTrcode2());
	}
}


TBTrocotro :: ~TBTrocotro ()
{
	// Delete all pointer members.

}

void TBTrocotro :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKea);
	AddKeyField((OSDataType *)&trcode1);
	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode2);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKea, icc_f.opunKea);
	AddField((OSDataType *)&trcode1, icc_f.trcode1);
	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode2, icc_f.trcode2);
}

#include "TBTroption.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTroption :: TBTroption () : Table(icc_t.Troption, 7, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTroption :: TBTroption (const pTBTroption troption) : Table ((Table *) troption)
{
	AddAllFields();

	if (troption)
	{
		if ( ! troption->GetOpunKey().IsNull() )
			SetOpunKey(troption->GetOpunKey());
		if ( ! troption->GetTrcode()->IsNull() )
			SetTrcode(troption->GetTrcode());
		if ( ! troption->GetOptprice().IsNull() )
			SetOptprice(troption->GetOptprice());
		if ( ! troption->GetOpttime()->IsNull() )
			SetOpttime(troption->GetOpttime());
		if ( ! troption->GetDeclared().IsNull() )
			SetDeclared(troption->GetDeclared());
		if ( ! troption->GetDecltime()->IsNull() )
			SetDecltime(troption->GetDecltime());
		if ( ! troption->GetBuslKey().IsNull() )
			SetBuslKey(troption->GetBuslKey());
	}
}


TBTroption :: ~TBTroption ()
{
	// Delete all pointer members.

}

void TBTroption :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&optprice, icc_f.optprice);
	AddField((OSDataType *)&opttime, icc_f.opttime);
	AddField((OSDataType *)&declared, icc_f.declared);
	AddField((OSDataType *)&decltime, icc_f.decltime);
	AddField((OSDataType *)&buslKey, icc_f.buslKey);
}

#include "TBTrprod.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTrprod :: TBTrprod () : Table(icc_t.Trprod, 4, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTrprod :: TBTrprod (const pTBTrprod trprod) : Table ((Table *) trprod)
{
	AddAllFields();

	if (trprod)
	{
		if ( ! trprod->GetOpunKey().IsNull() )
			SetOpunKey(trprod->GetOpunKey());
		if ( ! trprod->GetTrcode()->IsNull() )
			SetTrcode(trprod->GetTrcode());
		if ( ! trprod->GetImpPersist().IsNull() )
			SetImpPersist(trprod->GetImpPersist());
		if ( ! trprod->GetUnmeKey().IsNull() )
			SetUnmeKey(trprod->GetUnmeKey());
	}
}


TBTrprod :: ~TBTrprod ()
{
	// Delete all pointer members.

}

void TBTrprod :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&opunKey);
	AddKeyField((OSDataType *)&trcode);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&opunKey, icc_f.opunKey);
	AddField((OSDataType *)&trcode, icc_f.trcode);
	AddField((OSDataType *)&impPersist, icc_f.impPersist);
	AddField((OSDataType *)&unmeKey, icc_f.unmeKey);
}

#include "TBTsapmCc.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTsapmCc :: TBTsapmCc () : Table(icc_t.TsapmCc, 4, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTsapmCc :: TBTsapmCc (const pTBTsapmCc tsapmCc) : Table ((Table *) tsapmCc)
{
	AddAllFields();

	if (tsapmCc)
	{
		if ( ! tsapmCc->GetCcmeKey().IsNull() )
			SetCcmeKey(tsapmCc->GetCcmeKey());
		if ( ! tsapmCc->GetTimsKey().IsNull() )
			SetTimsKey(tsapmCc->GetTimsKey());
		if ( ! tsapmCc->GetPriority().IsNull() )
			SetPriority(tsapmCc->GetPriority());
		if ( ! tsapmCc->GetAction().IsNull() )
			SetAction(tsapmCc->GetAction());
	}
}


TBTsapmCc :: ~TBTsapmCc ()
{
	// Delete all pointer members.

}

void TBTsapmCc :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&ccmeKey);
	AddKeyField((OSDataType *)&timsKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&ccmeKey, icc_f.ccmeKey);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&priority, icc_f.priority);
	AddField((OSDataType *)&action, icc_f.action);
}

#include "TBTscaTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTscaTp :: TBTscaTp () : Table(icc_t.TscaTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTscaTp :: TBTscaTp (const pTBTscaTp tscaTp) : Table ((Table *) tscaTp)
{
	AddAllFields();

	if (tscaTp)
	{
		if ( ! tscaTp->GetTscaKey().IsNull() )
			SetTscaKey(tscaTp->GetTscaKey());
		if ( ! tscaTp->GetCode()->IsNull() )
			SetCode(tscaTp->GetCode());
		if ( ! tscaTp->GetName()->IsNull() )
			SetName(tscaTp->GetName());
		if ( ! tscaTp->GetDescript()->IsNull() )
			SetDescript(tscaTp->GetDescript());
	}
}


TBTscaTp :: ~TBTscaTp ()
{
	// Delete all pointer members.

}

void TBTscaTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&tscaKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&tscaKey, icc_f.tscaKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBMeterVl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBMeterVl :: TBMeterVl () : Table(icc_t.MeterVl, 8, 3, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBMeterVl :: TBMeterVl (const pTBMeterVl meterVl) : Table ((Table *) meterVl)
{
	AddAllFields();

	if (meterVl)
	{
		if ( ! meterVl->GetTimsKey().IsNull() )
			SetTimsKey(meterVl->GetTimsKey());
		if ( ! meterVl->GetDatetim()->IsNull() )
			SetDatetim(meterVl->GetDatetim());
		if ( ! meterVl->GetTsvalue().IsNull() )
			SetTsvalue(meterVl->GetTsvalue());
		if ( ! meterVl->GetTsstatus().IsNull() )
			SetTsstatus(meterVl->GetTsstatus());
		if ( ! meterVl->GetVlcode().IsNull() )
			SetVlcode(meterVl->GetVlcode());
		if ( ! meterVl->GetReadKey().IsNull() )
			SetReadKey(meterVl->GetReadKey());
		if ( ! meterVl->GetReptKey().IsNull() )
			SetReptKey(meterVl->GetReptKey());
		if ( ! meterVl->GetReasKey().IsNull() )
			SetReasKey(meterVl->GetReasKey());
	}
}


TBMeterVl :: ~TBMeterVl ()
{
	// Delete all pointer members.

}

void TBMeterVl :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&timsKey);
	AddKeyField((OSDataType *)&datetim);
	AddKeyField((OSDataType *)&vlcode);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&datetim, icc_f.datetim);
	AddField((OSDataType *)&tsvalue, icc_f.tsvalue);
	AddField((OSDataType *)&tsstatus, icc_f.tsstatus);
	AddField((OSDataType *)&vlcode, icc_f.vlcode);
	AddField((OSDataType *)&readKey, icc_f.readKey);
	AddField((OSDataType *)&reptKey, icc_f.reptKey);
	AddField((OSDataType *)&reasKey, icc_f.reasKey);
}

#include "TBTscoupl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTscoupl :: TBTscoupl () : Table(icc_t.Tscoupl, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTscoupl :: TBTscoupl (const pTBTscoupl tscoupl) : Table ((Table *) tscoupl)
{
	AddAllFields();

	if (tscoupl)
	{
		if ( ! tscoupl->GetCoplKey().IsNull() )
			SetCoplKey(tscoupl->GetCoplKey());
		if ( ! tscoupl->GetCompKey().IsNull() )
			SetCompKey(tscoupl->GetCompKey());
		if ( ! tscoupl->GetCompTyp().IsNull() )
			SetCompTyp(tscoupl->GetCompTyp());
		if ( ! tscoupl->GetTimsKey().IsNull() )
			SetTimsKey(tscoupl->GetTimsKey());
	}
}


TBTscoupl :: ~TBTscoupl ()
{
	// Delete all pointer members.

}

void TBTscoupl :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&coplKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&coplKey, icc_f.coplKey);
	AddField((OSDataType *)&compKey, icc_f.compKey);
	AddField((OSDataType *)&compTyp, icc_f.compTyp);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
}

#include "TBTseriTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTseriTp :: TBTseriTp () : Table(icc_t.TseriTp, 7, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTseriTp :: TBTseriTp (const pTBTseriTp tseriTp) : Table ((Table *) tseriTp)
{
	AddAllFields();

	if (tseriTp)
	{
		if ( ! tseriTp->GetTserKey().IsNull() )
			SetTserKey(tseriTp->GetTserKey());
		if ( ! tseriTp->GetCode()->IsNull() )
			SetCode(tseriTp->GetCode());
		if ( ! tseriTp->GetName()->IsNull() )
			SetName(tseriTp->GetName());
		if ( ! tseriTp->GetDescript()->IsNull() )
			SetDescript(tseriTp->GetDescript());
		if ( ! tseriTp->GetEdiCode()->IsNull() )
			SetEdiCode(tseriTp->GetEdiCode());
		if ( ! tseriTp->GetTsType()->IsNull() )
			SetTsType(tseriTp->GetTsType());
		if ( ! tseriTp->GetAccType().IsNull() )
			SetAccType(tseriTp->GetAccType());
	}
}


TBTseriTp :: ~TBTseriTp ()
{
	// Delete all pointer members.

}

void TBTseriTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&tserKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&tserKey, icc_f.tserKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&ediCode, icc_f.ediCode);
	AddField((OSDataType *)&tsType, icc_f.tsType);
	AddField((OSDataType *)&accType, icc_f.accType);
}

#include "TBTsfunVl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTsfunVl :: TBTsfunVl () : Table(icc_t.TsfunVl, 6, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTsfunVl :: TBTsfunVl (const pTBTsfunVl tsfunVl) : Table ((Table *) tsfunVl)
{
	AddAllFields();

	if (tsfunVl)
	{
		if ( ! tsfunVl->GetTsfnKey().IsNull() )
			SetTsfnKey(tsfunVl->GetTsfnKey());
		if ( ! tsfunVl->GetArg1().IsNull() )
			SetArg1(tsfunVl->GetArg1());
		if ( ! tsfunVl->GetArg2().IsNull() )
			SetArg2(tsfunVl->GetArg2());
		if ( ! tsfunVl->GetArg3().IsNull() )
			SetArg3(tsfunVl->GetArg3());
		if ( ! tsfunVl->GetFnVal().IsNull() )
			SetFnVal(tsfunVl->GetFnVal());
		if ( ! tsfunVl->GetTimsKey().IsNull() )
			SetTimsKey(tsfunVl->GetTimsKey());
	}
}


TBTsfunVl :: ~TBTsfunVl ()
{
	// Delete all pointer members.

}

void TBTsfunVl :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&tsfnKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&tsfnKey, icc_f.tsfnKey);
	AddField((OSDataType *)&arg1, icc_f.arg1);
	AddField((OSDataType *)&arg2, icc_f.arg2);
	AddField((OSDataType *)&arg3, icc_f.arg3);
	AddField((OSDataType *)&fnVal, icc_f.fnVal);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
}

#include "TBTsintTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTsintTp :: TBTsintTp () : Table(icc_t.TsintTp, 6, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTsintTp :: TBTsintTp (const pTBTsintTp tsintTp) : Table ((Table *) tsintTp)
{
	AddAllFields();

	if (tsintTp)
	{
		if ( ! tsintTp->GetTsinKey().IsNull() )
			SetTsinKey(tsintTp->GetTsinKey());
		if ( ! tsintTp->GetCode()->IsNull() )
			SetCode(tsintTp->GetCode());
		if ( ! tsintTp->GetName()->IsNull() )
			SetName(tsintTp->GetName());
		if ( ! tsintTp->GetUnit().IsNull() )
			SetUnit(tsintTp->GetUnit());
		if ( ! tsintTp->GetNunit().IsNull() )
			SetNunit(tsintTp->GetNunit());
		if ( ! tsintTp->GetDescript()->IsNull() )
			SetDescript(tsintTp->GetDescript());
	}
}


TBTsintTp :: ~TBTsintTp ()
{
	// Delete all pointer members.

}

void TBTsintTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&tsinKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&tsinKey, icc_f.tsinKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&unit, icc_f.unit);
	AddField((OSDataType *)&nunit, icc_f.nunit);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBTsorgTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTsorgTp :: TBTsorgTp () : Table(icc_t.TsorgTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTsorgTp :: TBTsorgTp (const pTBTsorgTp tsorgTp) : Table ((Table *) tsorgTp)
{
	AddAllFields();

	if (tsorgTp)
	{
		if ( ! tsorgTp->GetTsorKey().IsNull() )
			SetTsorKey(tsorgTp->GetTsorKey());
		if ( ! tsorgTp->GetCode()->IsNull() )
			SetCode(tsorgTp->GetCode());
		if ( ! tsorgTp->GetName()->IsNull() )
			SetName(tsorgTp->GetName());
		if ( ! tsorgTp->GetDescript()->IsNull() )
			SetDescript(tsorgTp->GetDescript());
	}
}


TBTsorgTp :: ~TBTsorgTp ()
{
	// Delete all pointer members.

}

void TBTsorgTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&tsorKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&tsorKey, icc_f.tsorKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBTsorgVl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTsorgVl :: TBTsorgVl () : Table(icc_t.TsorgVl, 7, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTsorgVl :: TBTsorgVl (const pTBTsorgVl tsorgVl) : Table ((Table *) tsorgVl)
{
	AddAllFields();

	if (tsorgVl)
	{
		if ( ! tsorgVl->GetTimsKey().IsNull() )
			SetTimsKey(tsorgVl->GetTimsKey());
		if ( ! tsorgVl->GetDatetim()->IsNull() )
			SetDatetim(tsorgVl->GetDatetim());
		if ( ! tsorgVl->GetCode()->IsNull() )
			SetCode(tsorgVl->GetCode());
		if ( ! tsorgVl->GetTsvalue().IsNull() )
			SetTsvalue(tsorgVl->GetTsvalue());
		if ( ! tsorgVl->GetQuality().IsNull() )
			SetQuality(tsorgVl->GetQuality());
		if ( ! tsorgVl->GetValMeth().IsNull() )
			SetValMeth(tsorgVl->GetValMeth());
		if ( ! tsorgVl->GetCorMeth().IsNull() )
			SetCorMeth(tsorgVl->GetCorMeth());
	}
}


TBTsorgVl :: ~TBTsorgVl ()
{
	// Delete all pointer members.

}

void TBTsorgVl :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&timsKey);
	AddKeyField((OSDataType *)&datetim);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&datetim, icc_f.datetim);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&tsvalue, icc_f.tsvalue);
	AddField((OSDataType *)&quality, icc_f.quality);
	AddField((OSDataType *)&valMeth, icc_f.valMeth);
	AddField((OSDataType *)&corMeth, icc_f.corMeth);
}

#include "TBTsproduc.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTsproduc :: TBTsproduc () : Table(icc_t.Tsproduc, 5, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTsproduc :: TBTsproduc (const pTBTsproduc tsproduc) : Table ((Table *) tsproduc)
{
	AddAllFields();

	if (tsproduc)
	{
		if ( ! tsproduc->GetTsprKey().IsNull() )
			SetTsprKey(tsproduc->GetTsprKey());
		if ( ! tsproduc->GetCode()->IsNull() )
			SetCode(tsproduc->GetCode());
		if ( ! tsproduc->GetName()->IsNull() )
			SetName(tsproduc->GetName());
		if ( ! tsproduc->GetDescript()->IsNull() )
			SetDescript(tsproduc->GetDescript());
		if ( ! tsproduc->GetPhonenum()->IsNull() )
			SetPhonenum(tsproduc->GetPhonenum());
	}
}


TBTsproduc :: ~TBTsproduc ()
{
	// Delete all pointer members.

}

void TBTsproduc :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&tsprKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&tsprKey, icc_f.tsprKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&phonenum, icc_f.phonenum);
}

#include "TBTsquTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTsquTp :: TBTsquTp () : Table(icc_t.TsquTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTsquTp :: TBTsquTp (const pTBTsquTp tsquTp) : Table ((Table *) tsquTp)
{
	AddAllFields();

	if (tsquTp)
	{
		if ( ! tsquTp->GetTsquKey().IsNull() )
			SetTsquKey(tsquTp->GetTsquKey());
		if ( ! tsquTp->GetCode()->IsNull() )
			SetCode(tsquTp->GetCode());
		if ( ! tsquTp->GetName()->IsNull() )
			SetName(tsquTp->GetName());
		if ( ! tsquTp->GetDescript()->IsNull() )
			SetDescript(tsquTp->GetDescript());
	}
}


TBTsquTp :: ~TBTsquTp ()
{
	// Delete all pointer members.

}

void TBTsquTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&tsquKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&tsquKey, icc_f.tsquKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBTsrefwei.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTsrefwei :: TBTsrefwei () : Table(icc_t.Tsrefwei, 10, 4, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTsrefwei :: TBTsrefwei (const pTBTsrefwei tsrefwei) : Table ((Table *) tsrefwei)
{
	AddAllFields();

	if (tsrefwei)
	{
		if ( ! tsrefwei->GetCcmeKey().IsNull() )
			SetCcmeKey(tsrefwei->GetCcmeKey());
		if ( ! tsrefwei->GetTimsKey().IsNull() )
			SetTimsKey(tsrefwei->GetTimsKey());
		if ( ! tsrefwei->GetTimsRef().IsNull() )
			SetTimsRef(tsrefwei->GetTimsRef());
		if ( ! tsrefwei->GetStamonth().IsNull() )
			SetStamonth(tsrefwei->GetStamonth());
		if ( ! tsrefwei->GetConstval().IsNull() )
			SetConstval(tsrefwei->GetConstval());
		if ( ! tsrefwei->GetBacktime().IsNull() )
			SetBacktime(tsrefwei->GetBacktime());
		if ( ! tsrefwei->GetOffset().IsNull() )
			SetOffset(tsrefwei->GetOffset());
		if ( ! tsrefwei->GetFactor().IsNull() )
			SetFactor(tsrefwei->GetFactor());
		if ( ! tsrefwei->GetValcok().IsNull() )
			SetValcok(tsrefwei->GetValcok());
		if ( ! tsrefwei->GetCorcok().IsNull() )
			SetCorcok(tsrefwei->GetCorcok());
	}
}


TBTsrefwei :: ~TBTsrefwei ()
{
	// Delete all pointer members.

}

void TBTsrefwei :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&ccmeKey);
	AddKeyField((OSDataType *)&timsKey);
	AddKeyField((OSDataType *)&timsRef);
	AddKeyField((OSDataType *)&stamonth);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&ccmeKey, icc_f.ccmeKey);
	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&timsRef, icc_f.timsRef);
	AddField((OSDataType *)&stamonth, icc_f.stamonth);
	AddField((OSDataType *)&constval, icc_f.constval);
	AddField((OSDataType *)&backtime, icc_f.backtime);
	AddField((OSDataType *)&offset, icc_f.offset);
	AddField((OSDataType *)&factor, icc_f.factor);
	AddField((OSDataType *)&valcok, icc_f.valcok);
	AddField((OSDataType *)&corcok, icc_f.corcok);
}

#include "TBTsstat.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTsstat :: TBTsstat () : Table(icc_t.Tsstat, 8, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTsstat :: TBTsstat (const pTBTsstat tsstat) : Table ((Table *) tsstat)
{
	AddAllFields();

	if (tsstat)
	{
		if ( ! tsstat->GetTimsKey().IsNull() )
			SetTimsKey(tsstat->GetTimsKey());
		if ( ! tsstat->GetTimstamp()->IsNull() )
			SetTimstamp(tsstat->GetTimstamp());
		if ( ! tsstat->GetTsmean().IsNull() )
			SetTsmean(tsstat->GetTsmean());
		if ( ! tsstat->GetTsvar1().IsNull() )
			SetTsvar1(tsstat->GetTsvar1());
		if ( ! tsstat->GetTsvar2().IsNull() )
			SetTsvar2(tsstat->GetTsvar2());
		if ( ! tsstat->GetTsmin().IsNull() )
			SetTsmin(tsstat->GetTsmin());
		if ( ! tsstat->GetTsmax().IsNull() )
			SetTsmax(tsstat->GetTsmax());
		if ( ! tsstat->GetTsforget().IsNull() )
			SetTsforget(tsstat->GetTsforget());
	}
}


TBTsstat :: ~TBTsstat ()
{
	// Delete all pointer members.

}

void TBTsstat :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&timsKey);
	AddKeyField((OSDataType *)&timstamp);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&timsKey, icc_f.timsKey);
	AddField((OSDataType *)&timstamp, icc_f.timstamp);
	AddField((OSDataType *)&tsmean, icc_f.tsmean);
	AddField((OSDataType *)&tsvar1, icc_f.tsvar1);
	AddField((OSDataType *)&tsvar2, icc_f.tsvar2);
	AddField((OSDataType *)&tsmin, icc_f.tsmin);
	AddField((OSDataType *)&tsmax, icc_f.tsmax);
	AddField((OSDataType *)&tsforget, icc_f.tsforget);
}

#include "TBTsstatus.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTsstatus :: TBTsstatus () : Table(icc_t.Tsstatus, 5, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTsstatus :: TBTsstatus (const pTBTsstatus tsstatus) : Table ((Table *) tsstatus)
{
	AddAllFields();

	if (tsstatus)
	{
		if ( ! tsstatus->GetTstrKey().IsNull() )
			SetTstrKey(tsstatus->GetTstrKey());
		if ( ! tsstatus->GetExtval()->IsNull() )
			SetExtval(tsstatus->GetExtval());
		if ( ! tsstatus->GetSeqno().IsNull() )
			SetSeqno(tsstatus->GetSeqno());
		if ( ! tsstatus->GetPattern().IsNull() )
			SetPattern(tsstatus->GetPattern());
		if ( ! tsstatus->GetMask().IsNull() )
			SetMask(tsstatus->GetMask());
	}
}


TBTsstatus :: ~TBTsstatus ()
{
	// Delete all pointer members.

}

void TBTsstatus :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&tstrKey);
	AddKeyField((OSDataType *)&seqno);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&tstrKey, icc_f.tstrKey);
	AddField((OSDataType *)&extval, icc_f.extval);
	AddField((OSDataType *)&seqno, icc_f.seqno);
	AddField((OSDataType *)&pattern, icc_f.pattern);
	AddField((OSDataType *)&mask, icc_f.mask);
}

#include "TBTstradef.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBTstradef :: TBTstradef () : Table(icc_t.Tstradef, 5, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBTstradef :: TBTstradef (const pTBTstradef tstradef) : Table ((Table *) tstradef)
{
	AddAllFields();

	if (tstradef)
	{
		if ( ! tstradef->GetTstrKey().IsNull() )
			SetTstrKey(tstradef->GetTstrKey());
		if ( ! tstradef->GetName()->IsNull() )
			SetName(tstradef->GetName());
		if ( ! tstradef->GetProtKey().IsNull() )
			SetProtKey(tstradef->GetProtKey());
		if ( ! tstradef->GetDescript()->IsNull() )
			SetDescript(tstradef->GetDescript());
		if ( ! tstradef->GetDirect().IsNull() )
			SetDirect(tstradef->GetDirect());
	}
}


TBTstradef :: ~TBTstradef ()
{
	// Delete all pointer members.

}

void TBTstradef :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&tstrKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&tstrKey, icc_f.tstrKey);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&protKey, icc_f.protKey);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&direct, icc_f.direct);
}

#include "TBUnfacVl.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBUnfacVl :: TBUnfacVl () : Table(icc_t.UnfacVl, 5, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBUnfacVl :: TBUnfacVl (const pTBUnfacVl unfacVl) : Table ((Table *) unfacVl)
{
	AddAllFields();

	if (unfacVl)
	{
		if ( ! unfacVl->GetUnmeKey().IsNull() )
			SetUnmeKey(unfacVl->GetUnmeKey());
		if ( ! unfacVl->GetUnmeKea().IsNull() )
			SetUnmeKea(unfacVl->GetUnmeKea());
		if ( ! unfacVl->GetFactor().IsNull() )
			SetFactor(unfacVl->GetFactor());
		if ( ! unfacVl->GetName()->IsNull() )
			SetName(unfacVl->GetName());
		if ( ! unfacVl->GetDescript()->IsNull() )
			SetDescript(unfacVl->GetDescript());
	}
}


TBUnfacVl :: ~TBUnfacVl ()
{
	// Delete all pointer members.

}

void TBUnfacVl :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&unmeKey);
	AddKeyField((OSDataType *)&unmeKea);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&unmeKey, icc_f.unmeKey);
	AddField((OSDataType *)&unmeKea, icc_f.unmeKea);
	AddField((OSDataType *)&factor, icc_f.factor);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBUnmeaTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBUnmeaTp :: TBUnmeaTp () : Table(icc_t.UnmeaTp, 6, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBUnmeaTp :: TBUnmeaTp (const pTBUnmeaTp unmeaTp) : Table ((Table *) unmeaTp)
{
	AddAllFields();

	if (unmeaTp)
	{
		if ( ! unmeaTp->GetUnmeKey().IsNull() )
			SetUnmeKey(unmeaTp->GetUnmeKey());
		if ( ! unmeaTp->GetCode()->IsNull() )
			SetCode(unmeaTp->GetCode());
		if ( ! unmeaTp->GetName()->IsNull() )
			SetName(unmeaTp->GetName());
		if ( ! unmeaTp->GetDescript()->IsNull() )
			SetDescript(unmeaTp->GetDescript());
		if ( ! unmeaTp->GetUnittype()->IsNull() )
			SetUnittype(unmeaTp->GetUnittype());
		if ( ! unmeaTp->GetEdiCode()->IsNull() )
			SetEdiCode(unmeaTp->GetEdiCode());
	}
}


TBUnmeaTp :: ~TBUnmeaTp ()
{
	// Delete all pointer members.

}

void TBUnmeaTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&unmeKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&unmeKey, icc_f.unmeKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&unittype, icc_f.unittype);
	AddField((OSDataType *)&ediCode, icc_f.ediCode);
}

#include "TBUnmeaVd.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBUnmeaVd :: TBUnmeaVd () : Table(icc_t.UnmeaVd, 3, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBUnmeaVd :: TBUnmeaVd (const pTBUnmeaVd unmeaVd) : Table ((Table *) unmeaVd)
{
	AddAllFields();

	if (unmeaVd)
	{
		if ( ! unmeaVd->GetUnvdKey().IsNull() )
			SetUnvdKey(unmeaVd->GetUnvdKey());
		if ( ! unmeaVd->GetTserKey().IsNull() )
			SetTserKey(unmeaVd->GetTserKey());
		if ( ! unmeaVd->GetUnmeKey().IsNull() )
			SetUnmeKey(unmeaVd->GetUnmeKey());
	}
}


TBUnmeaVd :: ~TBUnmeaVd ()
{
	// Delete all pointer members.

}

void TBUnmeaVd :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&unvdKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&unvdKey, icc_f.unvdKey);
	AddField((OSDataType *)&tserKey, icc_f.tserKey);
	AddField((OSDataType *)&unmeKey, icc_f.unmeKey);
}

#include "TBUserprog.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBUserprog :: TBUserprog () : Table(icc_t.Userprog, 4, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBUserprog :: TBUserprog (const pTBUserprog userprog) : Table ((Table *) userprog)
{
	AddAllFields();

	if (userprog)
	{
		if ( ! userprog->GetUserKey().IsNull() )
			SetUserKey(userprog->GetUserKey());
		if ( ! userprog->GetProgKey().IsNull() )
			SetProgKey(userprog->GetProgKey());
		if ( ! userprog->GetFngracc().IsNull() )
			SetFngracc(userprog->GetFngracc());
		if ( ! userprog->GetGrupKey().IsNull() )
			SetGrupKey(userprog->GetGrupKey());
	}
}


TBUserprog :: ~TBUserprog ()
{
	// Delete all pointer members.

}

void TBUserprog :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&userKey);
	AddKeyField((OSDataType *)&progKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&userKey, icc_f.userKey);
	AddField((OSDataType *)&progKey, icc_f.progKey);
	AddField((OSDataType *)&fngracc, icc_f.fngracc);
	AddField((OSDataType *)&grupKey, icc_f.grupKey);
}

#include "TBWatercou.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBWatercou :: TBWatercou () : Table(icc_t.Watercou, 7, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBWatercou :: TBWatercou (const pTBWatercou watercou) : Table ((Table *) watercou)
{
	AddAllFields();

	if (watercou)
	{
		if ( ! watercou->GetWatcKey().IsNull() )
			SetWatcKey(watercou->GetWatcKey());
		if ( ! watercou->GetCode()->IsNull() )
			SetCode(watercou->GetCode());
		if ( ! watercou->GetName()->IsNull() )
			SetName(watercou->GetName());
		if ( ! watercou->GetVuNo()->IsNull() )
			SetVuNo(watercou->GetVuNo());
		if ( ! watercou->GetMainrive()->IsNull() )
			SetMainrive(watercou->GetMainrive());
		if ( ! watercou->GetDescript()->IsNull() )
			SetDescript(watercou->GetDescript());
		if ( ! watercou->GetArea().IsNull() )
			SetArea(watercou->GetArea());
	}
}


TBWatercou :: ~TBWatercou ()
{
	// Delete all pointer members.

}

void TBWatercou :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&watcKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&watcKey, icc_f.watcKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&vuNo, icc_f.vuNo);
	AddField((OSDataType *)&mainrive, icc_f.mainrive);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&area, icc_f.area);
}

#include "TBWtfcohyr.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBWtfcohyr :: TBWtfcohyr () : Table(icc_t.Wtfcohyr, 2, 2, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBWtfcohyr :: TBWtfcohyr (const pTBWtfcohyr wtfcohyr) : Table ((Table *) wtfcohyr)
{
	AddAllFields();

	if (wtfcohyr)
	{
		if ( ! wtfcohyr->GetWtrfKey().IsNull() )
			SetWtrfKey(wtfcohyr->GetWtrfKey());
		if ( ! wtfcohyr->GetRelKey().IsNull() )
			SetRelKey(wtfcohyr->GetRelKey());
	}
}


TBWtfcohyr :: ~TBWtfcohyr ()
{
	// Delete all pointer members.

}

void TBWtfcohyr :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&wtrfKey);
	AddKeyField((OSDataType *)&relKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&wtrfKey, icc_f.wtrfKey);
	AddField((OSDataType *)&relKey, icc_f.relKey);
}

#include "TBWtgate.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBWtgate :: TBWtgate () : Table(icc_t.Wtgate, 12, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBWtgate :: TBWtgate (const pTBWtgate wtgate) : Table ((Table *) wtgate)
{
	AddAllFields();

	if (wtgate)
	{
		if ( ! wtgate->GetHydKey().IsNull() )
			SetHydKey(wtgate->GetHydKey());
		if ( ! wtgate->GetWtgateno().IsNull() )
			SetWtgateno(wtgate->GetWtgateno());
		if ( ! wtgate->GetLenght().IsNull() )
			SetLenght(wtgate->GetLenght());
		if ( ! wtgate->GetToplevel().IsNull() )
			SetToplevel(wtgate->GetToplevel());
		if ( ! wtgate->GetBotlevel().IsNull() )
			SetBotlevel(wtgate->GetBotlevel());
		if ( ! wtgate->GetDesignno().IsNull() )
			SetDesignno(wtgate->GetDesignno());
		if ( ! wtgate->GetTunnarea().IsNull() )
			SetTunnarea(wtgate->GetTunnarea());
		if ( ! wtgate->GetDescript()->IsNull() )
			SetDescript(wtgate->GetDescript());
		if ( ! wtgate->GetWtgtKey().IsNull() )
			SetWtgtKey(wtgate->GetWtgtKey());
		if ( ! wtgate->GetConstval().IsNull() )
			SetConstval(wtgate->GetConstval());
		if ( ! wtgate->GetCorrfact().IsNull() )
			SetCorrfact(wtgate->GetCorrfact());
		if ( ! wtgate->GetStartdelay().IsNull() )
			SetStartdelay(wtgate->GetStartdelay());
	}
}


TBWtgate :: ~TBWtgate ()
{
	// Delete all pointer members.

}

void TBWtgate :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&hydKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&wtgateno, icc_f.wtgateno);
	AddField((OSDataType *)&lenght, icc_f.lenght);
	AddField((OSDataType *)&toplevel, icc_f.toplevel);
	AddField((OSDataType *)&botlevel, icc_f.botlevel);
	AddField((OSDataType *)&designno, icc_f.designno);
	AddField((OSDataType *)&tunnarea, icc_f.tunnarea);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&wtgtKey, icc_f.wtgtKey);
	AddField((OSDataType *)&constval, icc_f.constval);
	AddField((OSDataType *)&corrfact, icc_f.corrfact);
	AddField((OSDataType *)&startdelay, icc_f.startdelay);
}

#include "TBWtgtTp.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBWtgtTp :: TBWtgtTp () : Table(icc_t.WtgtTp, 4, 1, 1, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBWtgtTp :: TBWtgtTp (const pTBWtgtTp wtgtTp) : Table ((Table *) wtgtTp)
{
	AddAllFields();

	if (wtgtTp)
	{
		if ( ! wtgtTp->GetWtgtKey().IsNull() )
			SetWtgtKey(wtgtTp->GetWtgtKey());
		if ( ! wtgtTp->GetCode()->IsNull() )
			SetCode(wtgtTp->GetCode());
		if ( ! wtgtTp->GetName()->IsNull() )
			SetName(wtgtTp->GetName());
		if ( ! wtgtTp->GetDescript()->IsNull() )
			SetDescript(wtgtTp->GetDescript());
	}
}


TBWtgtTp :: ~TBWtgtTp ()
{
	// Delete all pointer members.

}

void TBWtgtTp :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&wtgtKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&wtgtKey, icc_f.wtgtKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&name, icc_f.name);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBWtroujun.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBWtroujun :: TBWtroujun () : Table(icc_t.Wtroujun, 3, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBWtroujun :: TBWtroujun (const pTBWtroujun wtroujun) : Table ((Table *) wtroujun)
{
	AddAllFields();

	if (wtroujun)
	{
		if ( ! wtroujun->GetHydKey().IsNull() )
			SetHydKey(wtroujun->GetHydKey());
		if ( ! wtroujun->GetCode()->IsNull() )
			SetCode(wtroujun->GetCode());
		if ( ! wtroujun->GetDescript()->IsNull() )
			SetDescript(wtroujun->GetDescript());
	}
}


TBWtroujun :: ~TBWtroujun ()
{
	// Delete all pointer members.

}

void TBWtroujun :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&hydKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&descript, icc_f.descript);
}

#include "TBWtroute.h"
#include "iccTables.h"


// C o n s t r u c t o r s

TBWtroute :: TBWtroute () : Table(icc_t.Wtroute, 11, 1, 0, TABLE)
{



	AddAllFields();

	// Initialize all pointer members to NULL to avoid calling Set
	// functions with garbage pointers that cause core dump in delete.

}


TBWtroute :: TBWtroute (const pTBWtroute wtroute) : Table ((Table *) wtroute)
{
	AddAllFields();

	if (wtroute)
	{
		if ( ! wtroute->GetHydKey().IsNull() )
			SetHydKey(wtroute->GetHydKey());
		if ( ! wtroute->GetWtroutno().IsNull() )
			SetWtroutno(wtroute->GetWtroutno());
		if ( ! wtroute->GetCode()->IsNull() )
			SetCode(wtroute->GetCode());
		if ( ! wtroute->GetHeadloss().IsNull() )
			SetHeadloss(wtroute->GetHeadloss());
		if ( ! wtroute->GetMxflow().IsNull() )
			SetMxflow(wtroute->GetMxflow());
		if ( ! wtroute->GetStcont().IsNull() )
			SetStcont(wtroute->GetStcont());
		if ( ! wtroute->GetEndcont().IsNull() )
			SetEndcont(wtroute->GetEndcont());
		if ( ! wtroute->GetTimdelay().IsNull() )
			SetTimdelay(wtroute->GetTimdelay());
		if ( ! wtroute->GetDescript()->IsNull() )
			SetDescript(wtroute->GetDescript());
		if ( ! wtroute->GetWtrfKey().IsNull() )
			SetWtrfKey(wtroute->GetWtrfKey());
		if ( ! wtroute->GetWtroKey().IsNull() )
			SetWtroKey(wtroute->GetWtroKey());
	}
}


TBWtroute :: ~TBWtroute ()
{
	// Delete all pointer members.

}

void TBWtroute :: AddAllFields( void )
{
	// Add all key fields to the Table base class' key field list.

	AddKeyField((OSDataType *)&hydKey);


	// Add all member fields to the Table base class' field list.
	// Note that AddField also initializes the field names of
	// ALL the fields in the object.

	AddField((OSDataType *)&hydKey, icc_f.hydKey);
	AddField((OSDataType *)&wtroutno, icc_f.wtroutno);
	AddField((OSDataType *)&code, icc_f.code);
	AddField((OSDataType *)&headloss, icc_f.headloss);
	AddField((OSDataType *)&mxflow, icc_f.mxflow);
	AddField((OSDataType *)&stcont, icc_f.stcont);
	AddField((OSDataType *)&endcont, icc_f.endcont);
	AddField((OSDataType *)&timdelay, icc_f.timdelay);
	AddField((OSDataType *)&descript, icc_f.descript);
	AddField((OSDataType *)&wtrfKey, icc_f.wtrfKey);
	AddField((OSDataType *)&wtroKey, icc_f.wtroKey);
}
