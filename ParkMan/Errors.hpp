#pragma once

/*======================================================================================================================*/

#include <iostream>
#include <cstring>
#include <cerrno>
#include <sstream>

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Error printing class. Equivalent to "perror()" in C.
 * *************************************************************************************************************
 */

class ErrorStreamWrapper_c
 {
  private:
    std::ostringstream buffer; /* Accumulates your message components                                       */
    int saved_errno;           /* Captures errno immediately when created                                   */
  public:
    ErrorStreamWrapper_c();    /* Capture errno right at creation before any stream operations change it.   */
    ~ErrorStreamWrapper_c();   /* Destructor executes at the end of the statement, printing everything out. */

template <typename T>
   ErrorStreamWrapper_c& operator <<(const T& msg)
    {
     buffer << msg;
     return *this;
    }
    // ErrorStreamWrapper_c& operator <<(const int& msg);
    // ErrorStreamWrapper_c& operator <<(const double& msg);
    // ErrorStreamWrapper_c& operator <<(const std::string& msg);
    // ErrorStreamWrapper_c& operator <<(const char* const& msg);
 };


/*======================================================================================================================*/

/**
 * @brief A helper to easily create the temporary stream object.
 * 
 * @code
 * ErrorStreamWrapper_c perr();
 * @code
 */
ErrorStreamWrapper_c perr();

/*======================================================================================================================*/

