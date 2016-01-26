﻿#ifndef UNSERIALIZECPP_H
#define UNSERIALIZECPP_H



class unserializecpp_base
{
	static void pop(void* aptarget,void* apsource,uint32_t aicopysize)
	{
		memcpy(aptarget,apsource,aicopysize);
	}

	/* map pop */
	template <typename T_DATA>
	static uint32_t pop_map(char* ap, uint32_t aplen, T_DATA& aivaluesarr)
	{
		typedef T_DATA::value_type::first_type		first_type;
		typedef T_DATA::value_type::second_type		second_type;

		if( aplen < sizeof(uint16_t) )
		{
				return 0;
		}

		uint16_t larraysize;
		pop( ap, aplen, larraysize);
		ap += sizeof(uint16_t);
		uint32_t lsize = sizeof(first_type) * larraysize + sizeof(second_type) * larraysize + sizeof(uint16_t);

		std::pair<first_type,second_type> ltempkey;
		for( uint16_t i = 0 ; i < larraysize;++i)
		{
			unserializecpp_base::pop( (void*)&(ltempkey.first), ap, sizeof(first_type));
			ap += sizeof(first_type);
			aplen -= sizeof(first_type);
			unserializecpp_base::pop( (void*)&(ltempkey.second), ap, sizeof(second_type));
			ap += sizeof(second_type);
			aplen -= sizeof(second_type);
			aivaluesarr.insert( ltempkey );
		}
		return lsize;
	}

	/* set pop */
	template <typename T_DATA>
	static uint32_t pop_set(char* ap, uint32_t aplen, T_DATA& aivaluesarr)
	{
		if( aplen < sizeof(uint16_t) )
		{
				return 0;
		}

		uint16_t larraysize;
		pop( ap, aplen, larraysize);
		ap += sizeof(uint16_t);
		uint32_t lsize = sizeof(uint16_t) + larraysize * sizeof(T_DATA::value_type);

	

		T_DATA::value_type ltemp;
		for( uint16_t i = 0 ; i < larraysize;++i)
		{
			unserializecpp_base::pop( (void*)(&ltemp), ap, sizeof(T_DATA::value_type));
			ap += sizeof(T_DATA::value_type);
			aivaluesarr.insert( ltemp );
		}

		return lsize;
	}

public:
	
	/*
		内置类型
		int
		float
		double
		....
		和整个没有指针成员的struct结构
	*/
	template <typename T_DATA>
	static uint32_t pop(char* ap, uint32_t aplen, T_DATA& aivalues)
	{

		if( aplen < sizeof(T_DATA) )
		{
				return 0;
		}

		unserializecpp_base::pop( &aivalues, ap, sizeof(T_DATA));
		return sizeof(T_DATA);
	}

	/*数组*/
	template <typename T_DATA>
	static uint32_t pop(char* ap, uint32_t aplen, T_DATA* aivaluesarr, uint32_t aivaluesarrsize)
	{
		
		if( aplen < sizeof(uint16_t) )
		{
				return 0;
		}

		uint16_t larraysize;
		pop( ap, aplen, larraysize);
		ap += sizeof(uint16_t);
		uint32_t lsize = sizeof(uint16_t) + larraysize * sizeof(T_DATA);

		if( larraysize > aivaluesarrsize || lsize > aplen )
		{
			return 0;
		}

		unserializecpp_base::pop( (void*)(aivaluesarr), ap, larraysize * sizeof(T_DATA));

		return lsize;
	}


	template <typename T_DATA>
	static uint32_t pop(char* ap, uint32_t aplen, std::vector<T_DATA>& aivaluesarr)
	{
		if( aplen < sizeof(uint16_t) )
		{
				return 0;
		}

		uint16_t larraysize;
		pop( ap, aplen, larraysize);
		ap += sizeof(uint16_t);
		uint32_t lsize = sizeof(uint16_t) + larraysize * sizeof(T_DATA);

		if( lsize > aplen )
		{
			return 0;
		}

		aivaluesarr.resize( larraysize );

		unserializecpp_base::pop( (void*)(aivaluesarr.data()), ap, larraysize * sizeof(T_DATA));
		return lsize;
	}

	/*std::string*/
	static uint32_t pop(char* ap, uint32_t aplen, std::string& aivaluesarr)
	{
		if( aplen < sizeof(uint16_t) )
		{
				return 0;
		}

		uint16_t larraysize;
		pop( ap, aplen, larraysize);
		ap += sizeof(uint16_t);
		uint32_t lsize = sizeof(uint16_t) + larraysize;

		if( lsize > aplen )
		{
			return 0;
		}

		aivaluesarr.resize( larraysize );

		unserializecpp_base::pop( (void*)(aivaluesarr.data()), ap, larraysize);
		return lsize;
	}

	

#define DEF_POP_SET_TYPE( TYPE )    \
	template <typename T_DATA>\
	static uint32_t pop(char* ap, uint32_t aplen, TYPE<T_DATA>& aivaluesarr)\
	{\
		return pop_set( ap, aplen, aivaluesarr);\
	}

#define DEF_POP_MAP_TYPE( TYPE )  \
	template <typename T_DATA1,typename T_DATA2>\
	static uint32_t pop(char* ap, uint32_t aplen, TYPE<T_DATA1,T_DATA2>& aivaluesarr)\
	{\
		return pop_map( ap, aplen, aivaluesarr );\
	}

#if 0
	template <typename T_DATA>
	static uint32_t pop(char* ap, uint32_t aplen, std::set<T_DATA>& aivaluesarr)
	{
		return pop_set( ap, aplen, aivaluesarr);
	}

	/* hash set*/
	template <typename T_DATA>
	static uint32_t pop(char* ap, uint32_t aplen, std::unordered_set<T_DATA>& aivaluesarr)
	{
		return pop_set( ap, aplen, aivaluesarr);
	}
#else
	DEF_POP_SET_TYPE(std::set)
	DEF_POP_SET_TYPE(std::unordered_set)
#endif

#if 0
	template <typename T_DATA1,typename T_DATA2>
	static uint32_t pop(char* ap, uint32_t aplen, std::map<T_DATA1,T_DATA2>& aivaluesarr)
	{
		return pop_map( ap, aplen, aivaluesarr );
	}
	template <typename T_DATA1,typename T_DATA2>
	static uint32_t pop(char* ap, uint32_t aplen, std::unordered_map<T_DATA1,T_DATA2>& aivaluesarr)
	{
		return pop_map( ap, aplen, aivaluesarr );
	}
#else
	DEF_POP_MAP_TYPE( std::map )
	DEF_POP_MAP_TYPE( std::unordered_map )
#endif
   
};



/*
	反序列化类
*/
class unserializecpp
{
	/* 返回可用空间 */
	static uint32_t get_have_len(serializecpp_buffer* mp_buffer_data)
	{
			return mp_buffer_data->get_len()- mp_buffer_data->get_uselen();
	}
public:
	//基础
	template <typename T_DATA>
	static bool pop(serializecpp_buffer* mp_buffer_data,T_DATA& aivalues)
	{
		uint32_t lretvalues = unserializecpp_base::pop( mp_buffer_data->get_nowpos_buffer(), unserializecpp::get_have_len(mp_buffer_data), aivalues );
		mp_buffer_data->get_uselen() += lretvalues;
		return (lretvalues==0)?false:true;
	}

	template <typename T_DATA>
	static bool pop(serializecpp_buffer* mp_buffer_data,const T_DATA* aivalues, uint32_t ailen)
	{
		uint32_t lretvalues = unserializecpp_base::pop( mp_buffer_data->get_nowpos_buffer(), unserializecpp::get_have_len(mp_buffer_data), aivalues, ailen );
		mp_buffer_data->get_uselen() += lretvalues;
		return (lretvalues==0)?false:true;
	}
};







#endif //UNSERIALIZECPP_H