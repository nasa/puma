#ifndef _GLX_ERR_H_
#define _GLX_ERR_H_

#include "ostr.h"

#include <cstdlib>


#ifndef PERROR
# define PERROR(m) {							\
    std::cerr<<"==========================="<<std::endl;		\
    std::cerr<<"ERR: Mesg: ";						\
    perror(m);								\
    std::cerr<<"ERR: File: ["<<__FILE__<<"]"<<std::endl;		\
    std::cerr<<"ERR: Func: ["<<__PRETTY_FUNCTION__<<"]"<<std::endl;	\
    std::cerr<<"ERR: Line: ["<<__LINE__<<"]"<<std::endl;		\
    std::cerr<<"==========================="<<std::endl;		\
  }
#endif

#ifndef ERROR
# define ERROR(m) {							\
    std::cerr<<"==========================="<<std::endl;		\
    std::cerr<<"ERR: Mesg: " << m << std::endl;				\
    std::cerr<<"ERR: File: ["<<__FILE__<<"]"<<std::endl;		\
    std::cerr<<"ERR: Func: ["<<__PRETTY_FUNCTION__<<"]"<<std::endl;	\
    std::cerr<<"ERR: Line: ["<<__LINE__<<"]"<<std::endl;		\
    std::cerr<<"==========================="<<std::endl;		\
  }
#endif

#ifndef THROW
# define THROW(m) {							\
    std::cerr<<"==========================="<<std::endl;		\
    std::cerr<<"ERR: Mesg: " << m << std::endl;				\
    std::cerr<<"ERR: File: ["<<__FILE__<<"]"<<std::endl;		\
    std::cerr<<"ERR: Func: ["<<__PRETTY_FUNCTION__<<"]"<<std::endl;	\
    std::cerr<<"ERR: Line: ["<<__LINE__<<"]"<<std::endl;		\
    std::cerr<<"==========================="<<std::endl;		\
    throw std::runtime_error(m);					\
  }
#endif
#ifndef DIE
# define DIE(m) {							\
    std::cerr<<"==========================="<<std::endl;		\
    std::cerr<<"ERR: Mesg: " << m << std::endl;				\
    std::cerr<<"ERR: File: ["<<__FILE__<<"]"<<std::endl;		\
    std::cerr<<"ERR: Func: ["<<__PRETTY_FUNCTION__<<"]"<<std::endl;	\
    std::cerr<<"ERR: Line: ["<<__LINE__<<"]"<<std::endl;		\
    std::cerr<<"==========================="<<std::endl;		\
    exit(-1);								\
  }
#endif

#ifndef ETHROW
# define ETHROW(m) {							\
    std::cerr<<"==========================="<<std::endl;		\
    std::cerr<<"ERR: Mesg: ";						\
    perror(m);								\
    std::cerr<<"ERR: File: ["<<__FILE__<<"]"<<std::endl;		\
    std::cerr<<"ERR: Func: ["<<__PRETTY_FUNCTION__<<"]"<<std::endl;	\
    std::cerr<<"ERR: Line: ["<<__LINE__<<"]"<<std::endl;		\
    std::cerr<<"==========================="<<std::endl;		\
    throw std::runtime_error(m);					\
  }
#endif

#ifndef DIE
# define DIE(m) {							\
    std::cerr<<"==========================="<<std::endl;		\
    std::cerr<<"ERR: Mesg: " << m << std::endl;				\
    std::cerr<<"ERR: File: ["<<__FILE__<<"]"<<std::endl;		\
    std::cerr<<"ERR: Func: ["<<__PRETTY_FUNCTION__<<"]"<<std::endl;	\
    std::cerr<<"ERR: Line: ["<<__LINE__<<"]"<<std::endl;		\
    std::cerr<<"==========================="<<std::endl;		\
    exit(0);								\
}
#endif

#endif
