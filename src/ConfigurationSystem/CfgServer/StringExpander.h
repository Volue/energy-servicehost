//---------------------------------------------------------------------------

#ifndef StringExpanderH
#define StringExpanderH
//---------------------------------------------------------------------------
#include <string>
#include <map>


/** The CfgMap is a pure stl based map of strings.
 * used as a dictionary of configuration variable name
 * and the value */
typedef std::map<std::string,std::string> CfgMap;

/** StringExpander takes care of
  * algorithms for expanding a string that contains
  * %XX% with the contents of XX as found in a
  * specified dictionary (map).
  *
  * Example:
  * If the map contains the following:
  *   A=Alpha
  *   B=%A%Beta
  * and the meta character is '%'
  * the string expander would be expand the value of B to "AlphaBeta".
  */
class StringExpander {

	/** The meta character that surrounds the
	 *  symbol to be expanded. */
	char metaChar;

	/** To ensure a robust, but efficient algorithm
	 *  we count number of expansions pr. string to
	 *  avoid infinite loops (recursive expansion) */
	size_t maxExpandCount;// to limit expansion.

public:
	/** construct a StringExpander, default arguments gives
	 *  metaChar equal to % and infinite loop guard count =10 */
	StringExpander(char metaCharA='%',size_t maxExpandCountA=10)
		:metaChar(metaCharA),maxExpandCount(maxExpandCountA) {

	}
	/** expandString x using map m, count number of items that
	 *  is found, but not expanded in x */
	bool expandString(std::string &x,CfgMap& m,size_t& nMissedMapping);

	/** expand supplied map m using itself as dictionary */
	size_t expandMap(CfgMap&m);
};

#endif
