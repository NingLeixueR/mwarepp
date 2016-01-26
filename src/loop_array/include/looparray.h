﻿#ifndef LOOP_ARRAY_H
#define LOOP_ARRAY_H

#include "loop_array_write_callback_helper.h"

/*
	内部使用looparray实现的 模块通信组件
	单向通信
*/
class module_communicate
{
	loop_array*			m_la_handle;
	uint32_t			m_buffer_size;
	uint32_t			m_every_once_max_size;
	FUN_READ_CALLBACK	m_readfun;
	bool				m_startthread;		//是否开启线程支持

	loop_array_write_callback_helper m_callback_helper;


	bool send_run(char* ap, uint32_t& aplen)
	{
		return m_callback_helper.write_once(ap, aplen);
	}
public:
	
	module_communicate(uint32_t apbuffersize, uint32_t aieverymaxsize,FUN_READ_CALLBACK aireadfun,/* FUN_WRITE_CALLBACK aiwritefun,*/ bool apstartthread):
		m_buffer_size( apbuffersize ),
		m_every_once_max_size( aieverymaxsize ),
		m_readfun(aireadfun),
		m_startthread(apstartthread)
	{
		m_la_handle = new_loop_array(
			m_buffer_size,
			m_every_once_max_size,
			aireadfun,
			boost::bind(&module_communicate::send_run, this, _1, _2)
			);

		start_run(m_la_handle, m_startthread);
	}

	/* 发送字符串 */
	bool send( char* ap , uint32_t aplen )
	{
		return m_callback_helper.push_write_once(ap, aplen);
	}

	/*关闭通信组件*/
	bool close()
	{
		delete_loop_array(m_la_handle);
		return true;
	}


	/* 对其他格式的支持 */
	

};



#include <vector>
using namespace std;

class encode
{
public:
	bool in(char* ap, uint32_t aplen)
	{

	}
};


class decode
{};











#endif  //LOOP_ARRAY_H