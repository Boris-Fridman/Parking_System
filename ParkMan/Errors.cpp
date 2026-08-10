#include "Errors.hpp"



/*Capture errno right at creation before any stream operations change it */
ErrorStreamWrapper_c::ErrorStreamWrapper_c() 
 : buffer(), saved_errno(errno)
 {

 }

/* Destructor executes at the end of the statement, printing everything out */
ErrorStreamWrapper_c::~ErrorStreamWrapper_c()
  {
   /* Formats exactrly like: std::cerr<<message<<": "<<strerror(errno)<<"\n\r"; */
   std::cerr<<buffer.str()<<": "<< std::strerror(saved_errno) <<"\n\r";
  }


// ErrorStreamWrapper_c& ErrorStreamWrapper_c::operator <<(const int& msg)
//  {
//   buffer << msg;
//   return *this;
//  }

// ErrorStreamWrapper_c& ErrorStreamWrapper_c::operator <<(const double& msg)
//  {
//   buffer << msg;
//   return *this;
//  }

// ErrorStreamWrapper_c& ErrorStreamWrapper_c::operator <<(const std::string& msg)
//  {
//   buffer << msg;
//   return *this;
//  }

// ErrorStreamWrapper_c& ErrorStreamWrapper_c::operator <<(const char* const& msg)
//  {
//   buffer << msg;
//   return *this;
//  }



/* A helper to easily create the temporary stream object */
ErrorStreamWrapper_c perr()
 {
  return ErrorStreamWrapper_c();
 }


