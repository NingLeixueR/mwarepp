///        Copyright 2016 libo. All rights reserved
///   (Home at https://github.com/NingLeixueR/middleware/)

#ifndef SEGMENTATION_PACK_H
#define SEGMENTATION_PACK_H

#include <thread.hpp>
#include <pool/singleton_pool.hpp>

#include "middleware/socket_io/lpthread.h"
#include "middleware/socket_asio/protocol_head_handle_base.h"

#include <cstdint>
#include <unordered_map>
#include <function.hpp>
#include <thread.hpp>

namespace middleware{
	namespace tools{
		
		/**
		 *  保存不完整数据
		 */
		struct  not_recv
		{
			uint32_t m_size;
			char  m_dataarr[1024];
			char* m_data;
			not_recv():
				m_data( m_dataarr + server_head::POS::END_POS )
			{}
		};


		MAKE_POOL( not_recv );

#define GET_LEN( DATA )  *( (uint32_t*)(DATA) )   
#define PROTOCOL_BUFFER_MAX_SIZE   (1024)

		typedef unsigned long IP_ADDRESS_TYPE;

		template <typename T>
		class  segmentation_pack
		{
			typedef std::unordered_map< IP_ADDRESS_TYPE , not_recv*  >  type_ump;
			type_ump* m_ump;
			boost::function<bool(T,char*,uint32_t)>* m_logic_fun;
			boost::mutex* m_lock;

			bool every_seg( T aithis,char*& aidata , uint32_t& aidatalen , bool& airet )

			{

				char* ldata_copy = aidata;
				uint32_t ldatalen_copy = aidatalen;


				//获取len
				uint32_t llen;
				if( ldatalen_copy > sizeof( uint32_t ) )
				{
					llen = GET_LEN( ldata_copy );
					//ldata_copy += sizeof( uint16_t );
					//ldatalen_copy -= sizeof( uint16_t );
				}
				else
				{
					return false; /* 数据已经分割完成 */
				}

				/* 检验数据len是否合法 */
				if( llen > PROTOCOL_BUFFER_MAX_SIZE || llen == 0  )
				{
					airet = false;
				}

				if( ldatalen_copy >= llen )
				{
					aidatalen = ldatalen_copy- llen;
					aidata =  ldata_copy;
					aidata += llen;
					airet = (*m_logic_fun)( aithis , ldata_copy , llen );
					return true;

				}
				else
				{
					return false;
				}
			}


			/* hash中有存货 */
			bool every_seg( T aithis,type_ump::iterator& itor , char*& aidata , uint32_t& aidatalen , bool& airet)
			{
				//char* ldata_copy = aidata;
				//uint32_t ldatalen_copy = aidatalen;

				//获取len
				uint32_t llen;
				if( itor->second->m_size >= sizeof( uint32_t ) )
				{
					llen = GET_LEN( itor->second->m_data );
					//ltemp = itor->second->m_size/* - sizeof( uint16_t )*/;
				}
				else
				{
					if( itor->second->m_size + aidatalen > sizeof( uint32_t ) )
					{
						uint32_t litemp = sizeof( uint32_t ) - itor->second->m_size;
						memcpy( &( itor->second->m_data[ itor->second->m_size ] ) , aidata , litemp  );
						itor->second->m_size = sizeof( uint32_t );

						aidata += litemp;
						aidatalen -= litemp;
						llen = GET_LEN( itor->second->m_data );
					}
					else
					{
						return false;
					}
				}

				/* 检验数据len是否合法 */
				if( llen > PROTOCOL_BUFFER_MAX_SIZE || llen == 0  )
				{
					airet = false;
				}

				/* 数据真实总长度 llen */
				/* 已接收数据长度 itor->second->m_size + ldatalen_copy */
				if( llen <= itor->second->m_size + aidatalen )
				{
					/* 还需要多少数据组成 lhavedatalen */
					uint32_t lhavedatalen = llen - itor->second->m_size;
					memcpy( &( itor->second->m_data[ itor->second->m_size ] ) , aidata , lhavedatalen  );
					aidata += lhavedatalen;
					aidatalen -= lhavedatalen;
					airet = (*m_logic_fun)( aithis , itor->second->m_data , itor->second->m_size );

					return true;
				}
				else/* 没接收全 */
				{
					memcpy( &( itor->second->m_data[ itor->second->m_size ] ) , aidata , aidatalen  );
					aidata += aidatalen;
					aidatalen = 0 ;
					return false;
				}


			}




			segmentation_pack();
			segmentation_pack( const segmentation_pack&);
			bool segmentation_data( T aithis,IP_ADDRESS_TYPE aiip , char* aidata , uint32_t aidatalen )

			{
				char* ldata_copy = aidata;
				uint32_t ldatalen_copy = aidatalen;
				bool lbret1 = true;
				bool lbret2 = true;

				while( 1 )
				{
					lbret2 = every_seg( aithis , ldata_copy , ldatalen_copy , lbret1);
					if( !lbret1 )/* 数据错误 */
					{
						return false;
					}

					if( !lbret2 )
					{
						if( ldatalen_copy != 0 )
						{
							/* 依赖stl 关联容器 insert返回值 */
							type_ump::iterator& itor = m_ump->insert( std::make_pair( aiip , MALLOC_POOL( not_recv ) ) ).first;
							memcpy( itor->second->m_data , ldata_copy , ldatalen_copy );
							itor->second->m_size = ldatalen_copy;
						}
						else
						{
							/* 释放方式 */
							/* 1.直接释放全部资源*/
							type_ump::iterator itor;
							if( find_ump( itor , aiip ) )
							{
								FREE_POOL( not_recv , itor->second );
								m_ump->erase( itor );
							}
							///////////////* 2.只释放资源 不释放容器中的key*/
							//////////////type_ump::iterator itor;
							//////////////if( find_ump( itor , aiip ) )
							//////////////{
							//////////////	FREE_POOL( not_recv , itor->second );
							//////////////	itor->second = NULL;
							//////////////如果这样需要 做检查
							//////////////}

							///////////////* 啥也不做  留给调用端 他自己调用 */


						}
						break;
					}


				}
				return true;
			}


			bool segmentation_data( T aithis,type_ump::iterator& itor , IP_ADDRESS_TYPE aiip , char* aidata , uint32_t aidatalen )
			{
				char* ldata_copy = aidata;
				uint32_t ldatalen_copy = aidatalen;
				bool lbret1 = true;
				bool lbret2 = true;

				lbret2 = every_seg( aithis , itor , ldata_copy , ldatalen_copy , lbret1);
				if( !lbret1 )/* 数据错误 */
				{
					FREE_POOL( not_recv , itor->second );
					m_ump->erase( itor );
					return false;
				}


				if( !lbret2 )/* 说明已经无可用数据 */
				{
					return true;
				}

				return segmentation_data( aithis , aiip , ldata_copy , ldatalen_copy );

			}

		public:
			segmentation_pack( boost::function<bool(T,char*,uint32_t)> ailogic_fun ):
				m_logic_fun( new boost::function<bool(T,char*,uint32_t)>() ),
				m_ump( new type_ump() ),
				m_lock( new boost::mutex() )
			{
				*m_logic_fun = ailogic_fun;
			}


			~segmentation_pack(){
				delete m_ump;
				delete m_lock;
				delete m_logic_fun;
			}


			/* 查找ip对应的资源*/
			bool find_ump( type_ump::iterator& itor , IP_ADDRESS_TYPE aiip  ){
				itor = m_ump->find( aiip );
				return itor != m_ump->end() ;
			}


			bool segmentation(T aithis,IP_ADDRESS_TYPE aiip , char* aidata , uint32_t aidatalen)
			{
				boost::mutex::scoped_lock lock(*m_lock);

				type_ump::iterator itor;
				bool lbret1 = true;
				bool lbret2 = true;

				if(  find_ump( itor , aiip ) )
				{
					/* 找到 */
					return segmentation_data( aithis , itor, aiip , aidata , aidatalen );
				}
				else
				{
					if( aidatalen >= protocol_server_head::POS::END_POS - server_head::POS::END_POS )
					{
						return segmentation_data( aithis , aiip , aidata , aidatalen );
					}
					else
					{
						return false;
					}
				}

			}
			/* 强制删除某个ip和其占用的资源*/
			void close( IP_ADDRESS_TYPE aiip ){
				boost::mutex::scoped_lock lock(*m_lock);

				if( m_ump == NULL)
				{
					return ;
				}

				type_ump::iterator itor = m_ump->find( aiip );
				if( itor != m_ump->end() )
				{
					FREE_POOL( not_recv , itor->second );
					m_ump->erase( itor );
				}
			}
		};

	} //namespace tools
} //namespace middleware
#endif //SEGMENTATION_PACK_H

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */

