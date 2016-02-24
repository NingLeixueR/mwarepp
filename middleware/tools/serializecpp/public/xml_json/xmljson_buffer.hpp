﻿///        Copyright 2016 libo. All rights reserved
///   (Home at https://github.com/NingLeixueR/middleware/)

#ifndef XMLJSON_BUFFER_HPP
#define XMLJSON_BUFFER_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace middleware {
  namespace tools {

    struct xmljson_buffer
    {
      virtual const char* get_data() = 0;
      
      uint32_t get_len()
      {
        return m_str.length();
      }

      uint32_t get_uselen()
      {
        return get_len();
      }
      /** unserializecpp */
      virtual void reset(const char* ap, uint32_t aplen) = 0;

      xmljson_buffer(){}

      ~xmljson_buffer(){}
      
      inline void add_node(const char* aikey, boost::property_tree::ptree& apt)
      {
        m_root.push_back(make_pair(aikey, apt));
      }

      template <typename T>
      inline void add_single(const char* aikey, const T& aivalues)
      {
        m_root.put<T>(aikey, aivalues);
      }

      inline void clear()
      {
        m_root.clear();
      }

      inline void reset()
      {
        clear();
      }

      template <typename T>
      void get(const char* apkey, T& airet)
      {
        airet = m_root.get<T>(apkey);
      }

      template <typename T>
      void get(boost::property_tree::ptree& aptree, const char* apkey, T& aivalues)
      {
        aivalues = aptree.get<T>(apkey);
      }

      template <typename T>
      void get(const char* apkey1, const char* apkey2, T& aivalues)
      {
        aivalues = m_root.get_child(apkey1).get<T>(apkey2);
      }

      boost::property_tree::ptree& get_child(const char* aikey)
      {
        return m_root.get_child(aikey);
      }

    protected:
      boost::property_tree::ptree m_root;
      std::string m_str;
    };

  } //namespace middleware
} //namespace tools

#endif //XMLJSON_BUFFER_HPP

/* vim: set expandtab ts=2 sw=2 sts=2 tw=100: */