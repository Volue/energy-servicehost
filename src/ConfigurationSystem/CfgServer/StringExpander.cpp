//---------------------------------------------------------------------------

#include "stdafx.h"
#include <stdio.h>
#pragma hdrstop

#include "StringExpander.h"

//---------------------------------------------------------------------------
using namespace std;
//#pragma package(smart_init)

bool StringExpander::expandString(string &x,CfgMap& m,size_t& nMissedMapping) {
	bool changed=false;
	size_t n= x.size();
	size_t pos=0;
	const char metaChar='%';
	size_t replacementCounter=0;
	nMissedMapping=0;
	while (pos<n && replacementCounter <10){
		size_t b= x.find(metaChar,pos);
		if(b != string::npos && b+2<n) {
			size_t e= x.find(metaChar,b+2);
			if( e!= string::npos ) {
				string key= x.substr(b+1,e-b-1);
				//-- validate key, should be [a..zA..Z0..9_]
				bool validKey=true;
				for(size_t j=0;j<key.size() && validKey;j++) {
					register int c=key[j];
					validKey= isalpha(c)|| isdigit(c) ||(c=='_'|| c=='.'||c=='-');
				}
				if(validKey) {
					CfgMap::iterator v= m.find(key);
					if(v != m.end()) {
						x.replace(b,e-b+1,(*v).second);
						pos=b;// search for more replacements.
						n=x.size();//recalc size:todo: n += (e-b+1) - second.size
						replacementCounter++;
						changed=true;
					} else {
						nMissedMapping++;
						pos=e+1;// no replacement found, continue
					}
				} else {
					pos=e;
				}
			} else
				break;//-- end of string, no more metaC
		} else //-- end of string, no more metaC
			break;
	}
	return changed;
}


size_t StringExpander::expandMap(CfgMap&m) {
	size_t retryCount=0;
	size_t notExpanded=0;
	size_t changeCount=0;
	do {
		notExpanded=0;
		changeCount=0;
		for(CfgMap::iterator i=m.begin();i!=m.end();i++) {
			string x= (*i).second;
			size_t missedMapping=0;
			if( expandString(x,m,missedMapping)) {
				(*i).second=x;
				changeCount++;
			}
			if(missedMapping)
				notExpanded++;
		}
		retryCount++;
	} while (changeCount && retryCount<maxExpandCount);
	return notExpanded;
}
