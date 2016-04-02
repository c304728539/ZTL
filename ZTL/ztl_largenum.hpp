#ifndef ZTL_LARGENUM_HPP
#define ZTL_LARGENUM_HPP

#include<iostream>
#include<sstream>
#include"ztl_vector.hpp"
#include"ztl_string.hpp"
#include<cassert>
#include<cctype>
namespace ztl {

	enum LARGE_NUM_SIGN {
		positive = 0,
		negative = 1
	};

	using BitType = short;

	class LargeNum;
	class LargeFloat;


	LargeNum opposite(const LargeNum& num);
	LargeFloat opposite(const LargeFloat& num);

	LargeNum absolute(const LargeNum& num);

	using dataType = vector<BitType>;

	template<typename T, typename P = T>
	struct IComputableAdd;
	template<typename T, typename P = T>
	struct IComputableSubtract;
	template<typename T, typename P = T>
	struct IComputableMultiply;
	template<typename T, typename P = T>
	struct IComputableDivide;
	template<typename T, typename P = T>
	struct IComputableMod;

#define MAKE_ICOMPUTABLE_FUNDA(CN,O,P) \
	template<typename T> \
	struct CN<T, P> { \
		virtual T& operator##O##=(P num) { \
			return *this O##= T(num); \
		} \
		\
		virtual T& operator##O##=(const T& num) = 0;\
		\
		T operator##O##(const T& num) const {	\
			T ret(*dynamic_cast<const T*>(this));	\
			ret O##= num;	\
			return ret;	\
		}	\
		\
		T operator##O##(P num) const { \
			T ret(*dynamic_cast<const T*>(this)); \
			ret O##= num; \
			return ret; \
		} \
	};

	MAKE_ICOMPUTABLE_FUNDA(IComputableAdd, +, int)
	MAKE_ICOMPUTABLE_FUNDA(IComputableAdd, +, unsigned int)
	MAKE_ICOMPUTABLE_FUNDA(IComputableAdd, +, float)
	MAKE_ICOMPUTABLE_FUNDA(IComputableSubtract, -, int)
	MAKE_ICOMPUTABLE_FUNDA(IComputableSubtract, -, unsigned int)
	MAKE_ICOMPUTABLE_FUNDA(IComputableSubtract, -, float)
	MAKE_ICOMPUTABLE_FUNDA(IComputableMultiply, *, int)
	MAKE_ICOMPUTABLE_FUNDA(IComputableMultiply, *, unsigned int)
	MAKE_ICOMPUTABLE_FUNDA(IComputableMultiply, *, float)
	MAKE_ICOMPUTABLE_FUNDA(IComputableDivide, /, int)
	MAKE_ICOMPUTABLE_FUNDA(IComputableDivide, /, unsigned int)
	MAKE_ICOMPUTABLE_FUNDA(IComputableDivide, / , float)
	MAKE_ICOMPUTABLE_FUNDA(IComputableMod, %, int)
	MAKE_ICOMPUTABLE_FUNDA(IComputableMod, %, unsigned int)

	template<typename T, typename P>
	struct IComputable :
		public IComputableAdd<T, P>,
		public IComputableSubtract<T, P>,
		public IComputableMultiply<T, P>,
		public IComputableDivide<T, P>,
		public IComputableMod<T, P> {
	};

	template<typename T>
	struct IComputable<T, float>:
		public IComputableAdd<T, float>,
		public IComputableSubtract<T, float>,
		public IComputableMultiply<T, float>,
		public IComputableDivide<T, float>
	{
	};

	class LargeNumUnsigned:
		public IComputable<LargeNumUnsigned, unsigned int>
	{
		friend class LargeNum;
		friend class LargeFloat;
	private:
		using self = LargeNumUnsigned;
		dataType data;

		void _contrust(const string& num) {
			data.clear();
			for (size_t start = 0; start != num.length(); ++start)
				data.push_back(num[start] - '0');
		}
	public:
		LargeNumUnsigned() :data({ 0 }) {}

		LargeNumUnsigned(unsigned int num) {
			assign(num);
		}

		LargeNumUnsigned(const string& num) {
			_contrust(num);
		}

		virtual LargeNumUnsigned& operator+=(const LargeNumUnsigned& num) override {
			this->data = DataAdd(this->data, num.data);
			return *this;
		}

		virtual LargeNumUnsigned& operator*=(const LargeNumUnsigned& num) override {
			if (num.data == dataType({ 1,0 }))
				this->data.push_back(0);
			else
				this->data = DataMultiply(this->data, num.data);
			return *this;
		}

		virtual LargeNumUnsigned& operator/=(const LargeNumUnsigned& num) override {
			this->data = DataDivide(this->data, num.data).first;
			return *this;
		}

		virtual LargeNumUnsigned& operator%=(const LargeNumUnsigned& num) override {
			this->data = DataDivide(this->data, num.data).second;
			return *this;
		}

		virtual LargeNumUnsigned& operator*=(unsigned int num) override {
			if (10 == num)
				this->data.push_back(0);
			else {
				*this*=num;
			}
			return *this;
		}

		//call this function must be sure that *this great than num
		virtual LargeNumUnsigned& operator-=(const LargeNumUnsigned& num) override {
			this->data = DataSubtract(this->data, num.data);
			return *this;
		}

	public:
		LargeNum Subtract_s(const LargeNumUnsigned& num) const;

		LargeNumUnsigned& operator=(const LargeNumUnsigned& rhs) {
			assign(rhs);
			return *this;
		}

		void assign(const LargeNumUnsigned& rhs) {
			this->data = rhs.data;
		}

		LargeNumUnsigned& operator=(unsigned int num) {
			assign(num);
			return *this;
		}

		void assign(unsigned int num) {
			unsigned int num_copy = num;
			size_t bits = 0;
			do
			{
				num_copy /= 10;
				bits++;
			} while (0 != num_copy);
			data.resize(bits);
			do
			{
				data[--bits] = num % 10;
				num /= 10;
			} while (0 != num);
		}

		void zero() {
			this->data.resize(1);
			this->data[0] = 0;
		}

		bool IsZero() const {
			return (0 == this->data.size()) && (0 == this->data[0]);
		}

		size_t lenght() const {
			return data.size();
		}

		friend std::ostream& operator<<(std::ostream& os, const LargeNumUnsigned& num) {
			for (auto cit = num.data.cbegin(); cit != num.data.cend(); ++cit)
				os << *cit;
			return os;
		}

	public:
		friend bool operator<(const LargeNumUnsigned& n1, const LargeNumUnsigned& n2) {
				return -1 == CompareData(n1.data, n2.data);
		}

		friend bool operator>(const LargeNumUnsigned& n1, const LargeNumUnsigned& n2) {
			return n2 < n1;
		}

		friend bool operator==(const LargeNumUnsigned& n1, const LargeNumUnsigned& n2) {
			return 0 == CompareData(n1.data, n2.data);
		}

		friend int numcmp(const LargeNumUnsigned& n1, const LargeNumUnsigned& n2) {
			return CompareData(n1.data, n2.data);
		}

	private:
		static void trim(dataType& num) {
			auto it_earse = num.begin();
			while (0 == *it_earse &&num.end() != it_earse)
				++it_earse;
			num.erase(num.begin(), it_earse);
			if (0 == num.size())
				num.push_back(0);
		}

		static int CompareData(const dataType& d1, const dataType& d2)
		{
			if (d1.size() < d2.size()) return -1;
			else if (d1.size() > d2.size()) return 1;
			else
				for (auto rit1 = d1.cbegin(), rit2 = d2.cbegin(); rit1 != d1.cend() && rit2 != d2.cend(); ++rit1, ++rit2)
				{
					if (*rit1 < *rit2) return -1;
					else if (*rit1 > *rit2) return 1;
					//else continue;
				}
			return 0;
		}

		static dataType DataAdd(const dataType& d1, const dataType& d2) {
			dataType newdata;
			size_t newlen = ztl_max(d1.size(), d2.size());
			newdata.resize(newlen);
			auto rit1 = d1.crbegin();
			auto rit2 = d2.crbegin();

			short addr = 0;
			while (rit1 != d1.crend() && rit2 != d2.crend())
			{
				addr = *rit1++ + *rit2++ + addr;
				newdata[--newlen] = addr % 10;
				addr /= 10;
			}
			while (rit1 != d1.crend())
			{
				addr = *rit1++ + addr;
				newdata[--newlen] = addr % 10;
				addr /= 10;
			}
			while (rit2 != d2.crend())
			{
				addr = *rit2++ + addr;
				newdata[--newlen] = addr % 10;
				addr /= 10;
			}
			if (1 == addr)
				newdata.insert(newdata.begin(), addr);
			return newdata;
		}

		//call this function must be sure that d1 great than d2
		static dataType DataSubtract(const dataType& d1, const dataType& d2)
		{
			dataType newdata(d1);
			size_t newlen = newdata.size();
			auto rit1 = newdata.rbegin();
			auto rit2 = d2.crbegin();

			short addr = 0;
			while (/*rit1 != newdata.rend() &&*/ rit2 != d2.crend())
			{
				addr = *rit1++ - *rit2++;
				if (addr >= 0)
					newdata[--newlen] = addr % 10;
				else
				{
					size_t offset = 2;
					while (0 == newdata[newlen - offset])
					{
						newdata[newlen - offset++] = 9;
					}
					--newdata[newlen - offset];
					newdata[--newlen] = addr + 10;
				}
			}
			trim(newdata);
			return newdata;
		}

		static dataType DataMultiply(const dataType& d1, const dataType& d2) {
			dataType newdata;
			for (auto it_num = d2.cbegin(); it_num != d2.cend(); ++it_num) {
				newdata.push_back(0);
				dataType datatemp = { 0,0 };
				for (auto bit = d1.cbegin(); bit != d1.cend(); ++bit)
				{
					datatemp.push_back(0);
					short r = *it_num * *bit;
					datatemp[datatemp.size() - 1] += r % 10;
					if (datatemp[datatemp.size() - 1] >= 10)
					{
						datatemp[datatemp.size() - 1] -= 10;
						++datatemp[datatemp.size() - 2];
					}
					r /= 10;
					if (r)
					{
						datatemp[datatemp.size() - 2] += r;
						if (datatemp[datatemp.size() - 2] >= 10)
						{
							datatemp[datatemp.size() - 2] -= 10;
							++datatemp[datatemp.size() - 3];
						}
					}
				}
				newdata = DataAdd(newdata, datatemp);
			}
			trim(newdata);
			return newdata;
		}

		static pair<dataType, dataType> DataDivide(const dataType& dividend, const dataType& divisor) {
			dataType divisor_c(divisor);
			trim(divisor_c);
			int len = dividend.size() - divisor_c.size();
			dataType remainder(dividend);
			dataType result;
			divisor_c.resize(remainder.size());
			for (int i = 0; i <= len; ++i)
			{
				short begin = 0, end = 10, mid = 5;
				do {
					dataType r = move(DataMultiply(divisor_c, { mid }));
					int cr = CompareData(r, remainder);
					if (1 == cr)
						end = mid;
					else
						begin = mid;
					mid = (begin + end) / 2;
				} while (begin + 1 != end);
				result.push_back(begin);
				remainder = DataSubtract(remainder, DataMultiply(divisor_c, { begin }));
				divisor_c.pop_back();
			}
			trim(result);
			trim(remainder);
			return make_pair(move(result), move(remainder));
		}

	};
	
	class LargeNum :
		public IComputable<LargeNum, int> {
		friend class LargeNumUnsigned;
		friend class LargeFloat;
	private:
		LargeNumUnsigned data;
		int sign = positive;

		void _construct(const string& num) {
			if (num[0] == '-')
				sign = negative;
			else if (num[0] == '+')
				sign = positive;
			data._contrust(isdigit(num[0]) ? num : (num.substr(1, num.length() - 1)));
		}

		void printsign(std::ostream& os) const {
			if (sign) os << '-';
		}

	public:
		LargeNum() = default;

		LargeNum(int num) :sign(num>0?positive: negative),data(num>0 ? num : -num) {}

		LargeNum(unsigned int num):sign(positive), data(num){}

		LargeNum(const string& num){
			_construct(num);
		}
		
		size_t lenght() const {
			return data.lenght();
		}

		virtual LargeNum& operator+=(const LargeNum& num) override {
			if (sign == num.sign) {
				this->data += num.data;
			}
			else
			{
				int r = numcmp(this->data, num.data);
				if (-1 == r)
				{
					LargeNumUnsigned numcpy(num.data);
					numcpy -= this->data;
					this->data = numcpy;
					sign = sign ^ negative;
				}
				else if (1 == r)
				{
					this->data -= num.data;
					//sign has no change
				}
				else
				{
					this->data.zero();
					sign = positive;
				}
			}
			return *this;
		}

		virtual LargeNum& operator-=(const LargeNum& num) override {
			LargeNum opp_num = ztl::opposite(num);
			return operator+=(opp_num);
		}


		LargeNum& operator*=(int num) {
			if (num < 0) {
				this->opposite(); 
				num = -num;
			}
			this->data *= num;
			return *this;
		}

		virtual LargeNum& operator*=(const LargeNum& num) override {
			this->data *= num.data;
			sign = sign ^ num.sign;
			return *this;
		}

		virtual LargeNum& operator/=(const LargeNum& num) override {
			//LargeNum newnum;
			this->data/= num.data;
			sign = sign ^ num.sign;
			return *this;
		}

		virtual LargeNum& operator%=(const LargeNum& num) override {
			this->data %= num.data;
			//mod operator will not change sign
			return *this;
		}

		void opposite() {
			sign = sign ^ negative;
		}

		void absolute() {
			sign = positive;
		}

		friend std::ostream& operator<<(std::ostream& os, const LargeNum& num) {
			num.printsign(os);
			os << num.data;
			return os;
		}

		operator bool() const {
			return false == data.IsZero();
		}

		friend bool operator<(const LargeNum& n1, const LargeNum& n2) {
			if (negative == n1.sign && positive == n2.sign)
				return true;
			else if (positive == n1.sign && negative == n2.sign)
				return false;
			else if (positive == n1.sign && positive == n2.sign)
				return -1 == numcmp(n1.data, n2.data);
			else if (negative == n1.sign && negative == n2.sign)
				return 1 == numcmp(n1.data, n2.data);
		}

		friend bool operator>(const LargeNum& n1, const LargeNum& n2){
			return n2 < n1;
		}

		friend bool operator==(const LargeNum& n1, const LargeNum& n2) {
			return n1.sign == n2.sign && n1.data == n2.data;
		}

	private:
		
	};

	LargeNum LargeNumUnsigned::Subtract_s(const LargeNumUnsigned& num) const {
		LargeNum ret;
		int r = CompareData(this->data, num.data);
		if (1 == r)
		{
			ret.sign = positive;
			ret.data.data = DataSubtract(this->data, num.data);
		}
		else if (-1 == r)
		{
			ret.sign = negative;
			ret.data.data = DataSubtract(num.data, this->data);
		}
		return ret;
	}

	LargeNum opposite(const LargeNum& num) {
		LargeNum ret(num);
		ret.opposite();
		return ret;
	}

	LargeNum absolute(const LargeNum& num) {
		LargeNum ret(num);
		ret.absolute();
		return ret;
	}

	class LargeFloat: public IComputable<LargeFloat,float>
		{
	private:
		unsigned int decimal = 0;
		LargeNum data;
		const dataType& DataR() const { return data.data.data; }
		dataType& DataR() { return data.data.data; }

		void printsign(std::ostream& os) const
		{
			data.printsign(os);
		}
	private:
		void trim() {
			while (decimal && 0 == DataR()[data.lenght() - 1])
				DataR().pop_back(), --decimal;
			auto it = DataR().begin();
			while (distance(it ,DataR().end()) > static_cast<int>(decimal + 1) && 0 == *it)
				++it;
			DataR().erase(DataR().begin(), it);
		}

		void _construst(const string& str){
			string str_c(str);
			auto pos = str_c.find('.');
			if(string::npos!=pos)
				decimal = str_c.size() - pos - 1;
			str_c.erase(pos, 1);
			data._construct(str_c);
		}
	public:
		LargeFloat() {

		}

		LargeFloat(const string& str) {
			_construst(str);
		}

		LargeFloat(float num) {
			std::ostringstream oss;
			oss << num;
			_construst(string(oss.str().data()));
		}

		void opposite() {
			data.opposite();
		}

		virtual LargeFloat& operator+=(const LargeFloat& num) override {
			LargeFloat num_c(num);
			if (this->decimal < num_c.decimal)
			{
				do
				{
					this->data *= 10;
				} while (++this->decimal != num_c.decimal);
			}
			else if (this->decimal > num_c.decimal)
			{
				do
				{
					num_c.data *= 10;
				} while (this->decimal != ++num_c.decimal);
			}
			this->data += num_c.data;
			trim();
			return *this;
		}

		virtual LargeFloat& operator-=(const LargeFloat& num) override {
			LargeFloat opp_num = ztl::opposite(num);
			return *this += opp_num;
		}

		virtual LargeFloat& operator*=(const LargeFloat& num) override {
			data *= num.data;
			decimal += num.decimal;
			trim();
			return *this;
		}

		virtual LargeFloat& operator/=(const LargeFloat& num) override {
			data /= num.data;
			if (num.decimal > this->decimal)
				DataR().insert_n(DataR().end(), num.decimal - this->decimal, 0), this->decimal = 0;
			else
				this->decimal -= num.decimal;
			trim();
			return *this;
		}
		
		friend std::ostream& operator<<(std::ostream& os, const LargeFloat& numf)
		{
			numf.printsign(os);
			size_t n = numf.DataR().size();
			if (numf.decimal == n)
				os << '0';
			for (auto it = numf.DataR().cbegin(); it != numf.DataR().cend(); ++it)
			{
				if (numf.decimal == n--)
					os << '.';
				os << *it;
			}
			if (numf.decimal == 0)
				os << ".0";
			return os;
		}
	};

	LargeFloat opposite(const LargeFloat& num) {
		LargeFloat ret(num);
		ret.opposite();
		return ret;
	}

}

#endif