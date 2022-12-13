#include <atlbase.h>
#include <atlcom.h>
#include <comdef.h>
#include <atlconv.h>
#include <PDVariantUtility.h>
#include <memory>
#include <string>
using namespace std;
string GetStringFromSafeArray(VARIANT theSafeArray,int i) {
	_bstr_t bnull;
	const PowelVariantUtility::VVAccessor<_bstr_t> theArray(theSafeArray,bnull);
	_bstr_t valueString = theArray[i];
	std::auto_ptr<char> nameChar(_com_util::ConvertBSTRToString(valueString));
	return string(nameChar.get());
}