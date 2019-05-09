#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <climits>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>
using namespace std;
class InvalidRangeException
{
};

class CRange;
class CRangeList;

class CRange {
	private:
	long long int low;
	long long int high;
	public:
	CRange () {}
	CRange (const long long int & lo, const long long int & hi) {
		if (hi < lo) throw InvalidRangeException();
		low = lo;
		high = hi;
	}
	CRange (const long long int & single) { low = single; high = single; }
	~CRange () {}
	long long int getLow (void) const {return this->low;}
	long long int getHigh(void) const {return this->high;}
	CRangeList operator+ (CRange  a);
	CRangeList operator- (CRange  a);
	void setLow (long long int newLow) {
		this->low = newLow;
	}
	void setHigh (long long int newHigh) {
		this->high = newHigh;
	}
};

bool cmp (const CRange a, const CRange b) {
	return a.getLow() < b.getLow();
}

bool interceptSum (const CRange a, const CRange b) {
	if (a.getHigh() < (b.getLow()-1) || a.getLow() > (b.getHigh()+1)) return false;
	return true;
}

bool interceptSub (const CRange a, const CRange b) {
	if (a.getHigh() < b.getLow() || a.getLow() > b.getHigh()) return false;
	return true;
}

bool cmpIncludes (const CRange & a, const CRange & b) {
	if (b.getLow() >= a.getLow() && b.getHigh() <= a.getHigh()) return true;
	return false;
}
bool cmpIncludesNumber (const CRange & a, const long long int & b) {
	if (b >= a.getLow() && b <= a.getHigh()) return true;
	else return false;
}
bool equal (const CRange & a, const CRange & b) {
	if (a.getLow() == b.getLow() && a.getHigh() == b.getHigh()) return true;
	else return false;
}


class CRangeList {
	private:
	vector<CRange> intervals;
	public:
	// constructor
	// Includes long long / range
	// += range / range list
	// -= range / range list
	// = range / range list
	// operator ==
	// operator !=
	// operator <<
	CRangeList () {}
	~CRangeList () {}
	void Add (CRange & a) {
		this->intervals.push_back(a);
	}
	void Insert (CRange & a) {
		size_t size = this->intervals.size();
		size_t cnt = 0;
		vector<CRange>::iterator it = this->intervals.begin();
		for (size_t i = 0; i < size; i++) {
			if (interceptSum(this->intervals[i], a)) {
				cnt++;
				for (size_t k = i+1; k < size; k++) {
					if (interceptSum (this->intervals[k], a)) cnt++;
					else break;
				}
				// merge these and then call the function on merged element
				// then return here
				Merge (a, i, cnt, it);
				return;
			}
			else {
				if (!cmp(this->intervals[i], a)){
					this->intervals.insert(it, a);
					return;
				} // insert in front of it
			}
			it++;
		}
		// if we are here then add the element at the end
		this->intervals.push_back(a);
	}
	void Merge (CRange & a, size_t position, size_t cnt, vector<CRange>::iterator & it) {
		if (a.getLow() < this->intervals[position].getLow()) this->intervals[position].setLow(a.getLow());
		if (a.getHigh() > this->intervals[position+cnt-1].getHigh()) this->intervals[position].setHigh(a.getHigh());
		else this->intervals[position].setHigh(this->intervals[position+cnt-1].getHigh());
		it++;
		for (size_t i = position; i < position+cnt-1; i++) {
			this->intervals.erase(it);
			it++;
		}
	}
	void Delete (CRange & a) {
		size_t size = this->intervals.size();
		size_t cnt = 0;
		vector<CRange>::iterator it = this->intervals.begin();
		for (size_t i = 0; i < size; i++) {
			if (interceptSub(this->intervals[i], a)) {
				cnt++;
				for (size_t k = i+1; k < size; k++) {
					if (interceptSub (this->intervals[k], a)) cnt++;
					else break;
				}
				// merge these and then call the function on merged element
				// then return here
				Split (a, i, cnt, it);
				return;
			}
			else {
				if (!cmp(this->intervals[i], a)) return;
			}
			it++;
		}
		return;
	}
	void Split (CRange & a, size_t position, size_t cnt, vector<CRange>::iterator & it) {
		if (a.getLow() <= this->intervals[position].getLow()) {
			if (a.getHigh() >= this->intervals[position+cnt-1].getHigh()) {
				// delete everything
				for (size_t i = position; i < position+cnt; i++) {
					this->intervals.erase(it);
					it++;
				}
				return;
			}
			else {
				this->intervals[position].setLow(a.getHigh()+1);
				this->intervals[position].setHigh(this->intervals[position+cnt-1].getHigh());
				// delete everything except the current element
				it++;
				for (size_t i = position; i < position+cnt-1; i++) {
					this->intervals.erase(it);
					it++;
				}
				return;
			}
		}
		else {
			if (a.getHigh() >= this->intervals[position+cnt-1].getHigh()) {
				this->intervals[position].setHigh(a.getLow()-1);
				// delete everything except the current element
				it++;
				for (size_t i = position; i < position+cnt-1; i++) {
					this->intervals.erase(it);
					it++;
				}
				return;
			}
			else {
				CRange newRange = CRange (this->intervals[position].getLow(), a.getLow()-1);
				this->intervals[position].setLow(a.getHigh()+1);
				this->intervals[position].setHigh(this->intervals[position+cnt-1].getHigh());
				vector<CRange>::iterator it2 = it;
				it++;
				for (size_t i = position; i < position+cnt-1; i++) {
					this->intervals.erase(it);
					it++;
				}
				this->intervals.insert(it2, newRange);
				return;
			}
		}
	}
	bool Includes (const long long int & number) const {
		size_t size = this->intervals.size();
		long long int m, l = 0, r = size-1;
		while (l <= r) {
			m = (l + r) / 2;
			if (number >= this->intervals[m].getLow() && number <= this->intervals[m].getHigh()) return true;
			if (number < this->intervals[m].getLow()) r=m-1;
			else l=m+1;
		}
		return false;
	}
	bool Includes (const CRange & interval) const {
		size_t size = this->intervals.size();
		long long int m, l = 0, r = size-1;
		while (l <= r) {
			m = (l + r) / 2;
			if (interval.getLow() >= this->intervals[m].getLow() && interval.getHigh() <= this->intervals[m].getHigh()) return true;
			if (interceptSub (this->intervals[m], interval))return false;
			if (cmp (this->intervals[m], interval)) l = m +1;
			else r = m - 1;
		}
		return false;
	}
	CRangeList operator+ (CRange a) {
		Insert(a);
		return *this;
	}
	CRangeList operator+ (CRangeList list) {
		size_t size = list.intervals.size();
		for (size_t i = 0; i < size; i++) {
			*this + list.intervals[i];
		}
		return *this;
	}
	CRangeList operator- (CRange a) {
		Delete(a);
		return *this;
	}
	CRangeList operator- (CRangeList list) {
		size_t size = list.intervals.size();
		for (size_t i = 0; i < size; i++) {
			*this - list.intervals[i];
		}
		return *this;
	}
	CRangeList operator+= (CRange interval) {
		return *this + interval;
	}
	CRangeList operator+= (CRangeList list) {
		return *this + list;
	}
	CRangeList operator-= (CRange interval) {
		return *this - interval;
	}
	CRangeList operator-= (CRangeList list) {
		return *this - list;
	}
	void operator= (CRange interval) {
		if (!this->intervals.empty()) this->intervals.clear();
		this->intervals.push_back(interval);
	}
	void operator= (CRangeList list) {
		if (!this->intervals.empty()) this->intervals.clear();
		this->intervals = list.intervals;
	}
	friend bool operator== (const CRangeList & list1, const CRangeList & list2) {
		size_t size1 = list1.intervals.size();
		size_t size2 = list2.intervals.size();
		if (size1 != size2) return false;
		for (size_t i = 0; i < size1; i++) {
			if (list1.intervals[i].getLow() != list2.intervals[i].getLow()) return false;
			if (list1.intervals[i].getHigh() != list2.intervals[i].getHigh()) return false;
		}
		return true;
	}
	friend bool operator!= (const CRangeList & list1, const CRangeList & list2) {
		size_t size1 = list1.intervals.size();
		size_t size2 = list2.intervals.size();
		if (size1 != size2) return true;
		for (size_t i = 0; i < size1; i++) {
			if (list1.intervals[i].getLow() != list2.intervals[i].getLow()) return true;
			if (list1.intervals[i].getHigh() != list2.intervals[i].getHigh()) return true;
		}
		return false;
	}
	friend ostream & operator<< (ostream & oss, const CRangeList & list) {
		long long int size = list.intervals.size();
		long long int cnt = 0;
		oss << "{";
		for (long long int i = 0; i < size; i++) {
			if (list.intervals[i].getLow() == list.intervals[i].getHigh()) {
				oss << list.intervals[i].getLow();
			}
			else {
				oss << "<" << list.intervals[i].getLow() << ".." << list.intervals[i].getHigh() << ">";
			}
			if (cnt <= size-2) { oss << ","; cnt++; }
		}
		oss << "}";
		return oss;
	}
};

CRangeList CRange::operator+ (CRange  a) {
	CRange newRange;
	CRangeList newList;
	if (a.low == this->low && a.high == this->high) {
		newList.Add(*this);
		return newList;
	}
	if (interceptSum (*this, a)) {
		if (a.low < this->low) newRange.low = a.low;
		else newRange.low = this->low;

		if (a.high > this->high) newRange.high = a.high;
		else newRange.high = this->high;
		newList.Add(newRange);
		return newList;
	}
	else {
		if (cmp (*this, a)) {
			newList.Add(*this);
			newList.Add(a);
			return newList;
		}
		else {
			newList.Add(a);
			newList.Add(*this);
			return newList;
		}
	}
}

CRangeList CRange::operator- (CRange  a) {
	CRangeList newList;
	CRange newRange;

	if (interceptSub (*this, a)) {
		if (a.low < this->low) {
			if (a.high > this->high) {
				return newList;
			}
			else {
				this->low = a.high+1;
				newList.Add (*this);
				return newList;
			}
		}
		else {
			if (a.high > this->high) {
				this->high = a.low-1;
				newList.Add (*this);
				return newList;
			}
			else {
				newRange.low = a.high +1;
				newRange.high = this->high;
				this->high = a.low-1;
				newList.Add (*this);
				newList.Add (newRange);
				return newList;
			}
		}
	}
	else {
		newList.Add(*this);
		return newList;
	}
}

string             toString                                ( const CRangeList& x )
{
  ostringstream oss;
  oss << x;
  return oss . str ();
}

int                main                                    ( void )
{
  CRangeList a, b;

  assert ( sizeof ( CRange ) <= 2 * sizeof ( long long ) );
  a = CRange ( 5, 10 );
  a += CRange ( 25, 100 );
  assert ( toString ( a ) == "{<5..10>,<25..100>}" );
  a += CRange ( -5, 0 );
  a += CRange ( 8, 50 );
  assert ( toString ( a ) == "{<-5..0>,<5..100>}" );
  a += CRange ( 101, 105 ) + CRange ( 120, 150 ) + CRange ( 160, 180 ) + CRange ( 190, 210 );
  assert ( toString ( a ) == "{<-5..0>,<5..105>,<120..150>,<160..180>,<190..210>}" );
  a += CRange ( 106, 119 ) + CRange ( 152, 158 );
  assert ( toString ( a ) == "{<-5..0>,<5..150>,<152..158>,<160..180>,<190..210>}" );
  a += CRange ( -3, 170 );
  a += CRange ( -30, 1000 );
  assert ( toString ( a ) == "{<-30..1000>}" );
  b = CRange ( -500, -300 ) + CRange ( 2000, 3000 ) + CRange ( 700, 1001 );
  a += b;
  assert ( toString ( a ) == "{<-500..-300>,<-30..1001>,<2000..3000>}" );
  a -= CRange ( -400, -400 );
  assert ( toString ( a ) == "{<-500..-401>,<-399..-300>,<-30..1001>,<2000..3000>}" );
  a -= CRange ( 10, 20 ) + CRange ( 900, 2500 ) + CRange ( 30, 40 ) + CRange ( 10000, 20000 );
  assert ( toString ( a ) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}" );
  try
  {
    a += CRange ( 15, 18 ) + CRange ( 10, 0 ) + CRange ( 35, 38 );
    assert ( "Exception not thrown" == NULL );
  }
  catch ( const InvalidRangeException & e )
  {
  }
  catch ( ... )
  {
    assert ( "Invalid exception thrown" == NULL );
  }
  assert ( toString ( a ) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}" );
  b = a;
  assert ( a == b );
  assert ( !( a != b ) );
  b += CRange ( 2600, 2700 );
  assert ( toString ( b ) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}" );
  assert ( a == b );
  assert ( !( a != b ) );
  b += CRange ( 15, 15 );
  assert ( toString ( b ) == "{<-500..-401>,<-399..-300>,<-30..9>,15,<21..29>,<41..899>,<2501..3000>}" );
  assert ( !( a == b ) );
  assert ( a != b );
  assert ( b . Includes ( 15 ) );
  assert ( b . Includes ( 2900 ) );
  assert ( b . Includes ( CRange ( 15, 15 ) ) );
  assert ( b . Includes ( CRange ( -350, -350 ) ) );
  assert ( b . Includes ( CRange ( 100, 200 ) ) );
  assert ( !b . Includes ( CRange ( 800, 900 ) ) );
  assert ( !b . Includes ( CRange ( -1000, -450 ) ) );
  assert ( !b . Includes ( CRange ( 0, 500 ) ) );
  a += CRange ( -10000, 10000 ) + CRange ( 10000000, 1000000000 );
  assert ( toString ( a ) == "{<-10000..10000>,<10000000..1000000000>}" );
  b += a;
  assert ( toString ( b ) == "{<-10000..10000>,<10000000..1000000000>}" );
  b -= a;
  assert ( toString ( b ) == "{}" );
  b += CRange ( 0, 100 ) + CRange ( 200, 300 ) - CRange ( 150, 250 ) + CRange ( 160, 180 ) - CRange ( 170, 170 );
  assert ( toString ( b ) == "{<0..100>,<160..169>,<171..180>,<251..300>}" );
  b -= CRange ( 10, 90 ) - CRange ( 20, 30 ) - CRange ( 40, 50 ) - CRange ( 60, 90 ) + CRange ( 70, 80 );
  assert ( toString ( b ) == "{<0..9>,<20..30>,<40..50>,<60..69>,<81..100>,<160..169>,<171..180>,<251..300>}" );
  return 0;
}
