#pragma once
/*
	Utility for accessing variants that
	might be vectors of variants, or basic types.

	Learning/Documentation by examples can be found in
	 <projectroot>/ABB/PowelSpiderCOM/PowelSpiderInterface.cpp

  Created by:Sigbjørn Helset
	(c)Powel ASA 2003

  Include files needed before this one is listed below
	#include <atlcom.h>
	#include <comdef.h>
	#include <atlconv.h>
*/
namespace PowelVariantUtility {
	template <class T>
	VARIANT createSafeArray(long n, VARTYPE variantType, T* ptrToFill) { /*
		o Create a variant containing a safearray of n elements,
		  each element of type variantType. Initvalue of each
		  element is set to *ptrToFill.
		  If ptrToFill is zero, the arrayelements will have the
		  initial value as set in SaveArrayCreate.
		returns a the variant with ref. to the safeArray.
		*/
		VARIANT safeArray;
		SAFEARRAYBOUND dim;dim.lLbound=0;dim.cElements=n; // cElements is unsigned, but our n is limited by SafeArrayPutElement taking signed value
		SAFEARRAY *sa = SafeArrayCreate(variantType, 1, &dim);
		VariantInit(&safeArray);
		safeArray.vt = VT_ARRAY | variantType;
		safeArray.parray = sa;
		if (ptrToFill)
			for (long i=0;i<n;i++)
				::SafeArrayPutElement(sa, &i, (void*)ptrToFill);
		return safeArray;
	}
	inline bool isVT(VARTYPE vt, VARTYPE t) {
		return (vt&VT_TYPEMASK) == t ? true : false;
	}
	template <class T>
	class VVElement {
		VARIANT& v;
		long i; // Must be signed, since lower bound can be negative!
		T nullv;
	public:
		VVElement(VARIANT &v_, long i_, T nullv_) :v(v_), i(i_), nullv(nullv_) {};
		void operator=(const T& x) {
			operator=(_variant_t(x));
		}
		void operator=(_variant_t x) {
			if (v.vt & VT_ARRAY) {
				SAFEARRAY FAR* a = v.vt&VT_BYREF ? *v.pparray : v.parray;
				HRESULT result = E_INVALIDARG;
				UINT vdimension = ::SafeArrayGetDim(a);
				if (vdimension > 0) {
					long v_i = a->rgsabound[0].lLbound + i;
					if (isVT(v.vt, VT_I4)) {
						long v_ = long(x);result = SafeArrayPutElement(a, &v_i, &v_);
					} else if (isVT(v.vt, VT_R8)) {
						double v_ = double(x);result = SafeArrayPutElement(a, &v_i, &v_);
					} else if (isVT(v.vt, VT_DATE)) {
						DATE v_ = DATE(x);result = SafeArrayPutElement(a, &v_i, &v_);
					} else if (isVT(v.vt, VT_R4)) {
						float v_ = float(x);result = SafeArrayPutElement(a, &v_i, &v_);
					} else if (isVT(v.vt, VT_BOOL)) {
						BOOL v_ = bool(x);result = SafeArrayPutElement(a, &v_i, &v_);
					} else if (isVT(v.vt, VT_BSTR)) {
						_bstr_t v_ = _bstr_t(x);
						result = SafeArrayPutElement(a, &v_i, (void*)static_cast<wchar_t*>(v_));
					} else if (isVT(v.vt, VT_VARIANT)) {
						VARIANT vx = x.Detach();
						result = SafeArrayPutElement(a, &v_i, (void*)&vx);
						VariantClear(&vx);
					} //else  not supported, issue invalid arg.
				} //else empty array, issue invalid arg.
				_com_util::CheckError(result);
			} else {// not an array, just assign.
				VARIANT vx = x.Detach();
				VariantCopy(&v, &vx);
				VariantClear(&vx);
			}
		}
		operator T ();// need VVaccessor for implementation of this one, ref. def later in same file,
		_variant_t operator()(){ return static_cast<T>(*this); }
	};

	template < class T >
	class VVAccessor { /*
		utility to access  data of type T
		from a variant (std::vector) of T or compatible type
		*/
		VARIANT v;
		T NullVal;
	public:
		VVAccessor(VARIANT v_, T NullVal_) :v(v_), NullVal(NullVal_) {}
		VVElement<T> operator[] (long i) {
			return VVElement<T>(v, i, NullVal);
		}
		T operator[](long i) const {
			T res = NullVal;
			if (v.vt & VT_ARRAY) {
				SAFEARRAY FAR* a = v.vt&VT_BYREF ? *v.pparray : v.parray;
				HRESULT result = E_INVALIDARG;
				UINT vdimension = ::SafeArrayGetDim(a);
				if (vdimension > 0) {
					long v_i = a->rgsabound[0].lLbound + i;
					if (isVT(v.vt, VT_I4)) {
						long v_;result = SafeArrayGetElement(a, &v_i, &v_);res = T(_variant_t(v_));
					} else if (isVT(v.vt, VT_R8)) {
						double v_;result = SafeArrayGetElement(a, &v_i, &v_);res = T(_variant_t(v_));
					} else if (isVT(v.vt, VT_DATE)) {
						DATE v_;result = SafeArrayGetElement(a, &v_i, &v_);res = T(_variant_t(v_));
					} else if (isVT(v.vt, VT_R4)) {
						float v_;result = SafeArrayGetElement(a, &v_i, &v_);res = T(_variant_t(v_));
					} else if (isVT(v.vt, VT_BOOL)) {
						BOOL v_;result = SafeArrayGetElement(a, &v_i, &v_);res = T(_variant_t(bool(v_ ? true : false)));
					} else if (isVT(v.vt, VT_BSTR)) {
						BSTR bv_ = 0;
						result = SafeArrayGetElement(a, &v_i, &bv_);
						_bstr_t bv(bv_, false);
						_variant_t vbv(bv);
						res = T(vbv);
					} else if (isVT(v.vt, VT_VARIANT)) {
						VARIANT vv_;
						result = SafeArrayGetElement(a, &v_i, &vv_);
						if (vv_.vt&VT_ARRAY)
							res = T(*((T*)&vv_));
						else
							res = T(_variant_t(vv_, false));
					}
				} //else empty array, issue invalid arg.
				_com_util::CheckError(result);
			} else if (v.vt != VT_EMPTY) {
				res = T(_variant_t(v));
			}
			return res;
		}
		unsigned long size() const {
			if (v.vt & VT_ARRAY) {
				SAFEARRAY FAR* a = v.vt&VT_BYREF ? *v.pparray : v.parray;
				UINT vdimension = ::SafeArrayGetDim(a);
				if (vdimension > 0)
					return a->rgsabound[0].cElements;
				else //empty array
					return 0;
			} else if (v.vt == VT_EMPTY || v.vt == VT_NULL) {
				return 0;
			} else
				return 1;
		}
		unsigned long resize(unsigned long n) {
			if (v.vt & VT_ARRAY) {
				SAFEARRAY FAR* a = v.vt&VT_BYREF ? *v.pparray : v.parray;
				SAFEARRAYBOUND newSize;
				newSize.lLbound = 0;
				newSize.cElements = n;
				_com_util::CheckError(::SafeArrayRedim(a, &newSize));
				return n;
			} else {
				return 1;
			}
		}
		unsigned long size(unsigned long n) { return resize(n); }
	};


	template <class T>
	VVElement<T>::operator T () {
		const VVAccessor<T> vv(v, nullv);
		return vv[i];
	}
};