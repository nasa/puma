#ifndef _SWAPIT_H_
#define _SWAPIT_H_

#include <sys/types.h> // u_int32_t, etc
#include <unistd.h>    // read()
#include <stdexcept>   // std::runtime_error
#include <cstring>    // memcpy()
#include <vector>
#include <cassert>


namespace p_bin {
  // NO NUMTHREADS!!!
  inline void swap16(void* buf, ssize_t numWords){
    const u_int16_t lobyte=(u_int16_t)0x00ff;
    const u_int16_t hibyte=(u_int16_t)0xff00;
    const u_int16_t bitsperbyte=(u_int16_t)8;
    register ssize_t i;
    register u_int16_t* src = reinterpret_cast<u_int16_t*>(buf);

#pragma omp parallel for
    for(i = 0 ; i < numWords ; ++i ){
      src[i] = (u_int16_t)
	((u_int16_t)((src[i] & lobyte)<<bitsperbyte) |
	 (u_int16_t)((src[i] & hibyte)>>bitsperbyte));
    }
  }

  // NO NUMTHREADS!!!
  inline void swap32(void* buf, ssize_t numWords){
    register ssize_t i;
    register u_int32_t* src = reinterpret_cast<u_int32_t*>(buf);

#pragma omp parallel for
    for(i = 0 ; i < numWords ; ++i ){
      src[i] = ((src[i] & 0x000000ff) << 24) |
	((src[i] & 0x0000ff00) << 8) |
	((src[i] & 0x00ff0000) >> 8) |
	((src[i] & 0xff000000) >> 24);
    }
  }

  inline void swap(std::vector<u_int32_t>& buf){
    swap32(&buf[0],buf.size());
  }
  inline void swap(std::vector<int32_t>& buf){
    swap32((u_int32_t*)&buf[0],buf.size());
  }
  inline void swap(std::vector<float>& buf){
    swap32((u_int32_t*)&buf[0],buf.size());
  }

  // NO NUMTHREADS!!!
  inline void swap64(void* buf, ssize_t numWords){
    register ssize_t i;
    register u_int64_t* src = reinterpret_cast<u_int64_t*>(buf);

#pragma omp parallel for
    for(i = 0 ; i < numWords ; ++i ){
      src[i] = ((u_int64_t)((u_int64_t)src[i] & 0x00000000000000ffULL)<<56ULL | \
		(u_int64_t)((u_int64_t)src[i] & 0x000000000000ff00ULL)<<40ULL | \
		(u_int64_t)((u_int64_t)src[i] & 0x0000000000ff0000ULL)<<24ULL | \
		(u_int64_t)((u_int64_t)src[i] & 0x00000000ff000000ULL)<< 8ULL | \
		(u_int64_t)((u_int64_t)src[i] & 0x000000ff00000000ULL)>> 8ULL | \
		(u_int64_t)((u_int64_t)src[i] & 0x0000ff0000000000ULL)>>24ULL | \
		(u_int64_t)((u_int64_t)src[i] & 0x00ff000000000000ULL)>>40ULL | \
		(u_int64_t)((u_int64_t)src[i] & 0xff00000000000000ULL)>>56ULL );
    }
  }
  inline void swap(std::vector<u_int64_t>& buf){
    swap64((u_int64_t*)&buf[0],buf.size());
  }
  inline void swap(std::vector<int64_t>& buf){
    swap64((u_int64_t*)&buf[0],buf.size());
  }
  inline void swap(std::vector<double>& buf){
    swap64((u_int64_t*)&buf[0],buf.size());
  }

  inline double swap(double val){
    u_int64_t bits;
    memcpy(&bits,&val,sizeof(double));
    p_bin::swap64(&bits,1);
    memcpy(&val,&bits,sizeof(double));
    return val;
  }

  inline float swap(float val){
    u_int32_t bits;
    memcpy(&bits,&val,sizeof(float));
    p_bin::swap32(&bits,1);
    memcpy(&val,&bits,sizeof(float));
    return val;
  }

  inline u_int64_t swap(u_int64_t val){
    u_int64_t bits;
    memcpy(&bits,&val,sizeof(u_int64_t));
    p_bin::swap64(&bits,1);
    memcpy(&val,&bits,sizeof(u_int64_t));
    return val;
  }
  inline u_int64_t swap(int64_t val){
    u_int64_t bits;
    memcpy(&bits,&val,sizeof(u_int64_t));
    p_bin::swap64(&bits,1);
    memcpy(&val,&bits,sizeof(u_int64_t));
    return val;
  }

  inline u_int32_t swap(u_int32_t val){
    u_int32_t bits;
    memcpy(&bits,&val,sizeof(u_int32_t));
    p_bin::swap32(&bits,1);
    memcpy(&val,&bits,sizeof(u_int32_t));
    return val;
  }
  inline u_int32_t swap(int32_t val){
    u_int32_t bits;
    memcpy(&bits,&val,sizeof(u_int32_t));
    p_bin::swap32(&bits,1);
    memcpy(&val,&bits,sizeof(u_int32_t));
    return val;
  }

  inline u_int16_t swap(u_int16_t val){
    u_int16_t bits;
    memcpy(&bits,&val,sizeof(u_int16_t));
    p_bin::swap16(&bits,1);
    memcpy(&val,&bits,sizeof(u_int16_t));
    return val;
  }
  inline u_int16_t swap(int16_t val){
    u_int16_t bits;
    memcpy(&bits,&val,sizeof(u_int16_t));
    p_bin::swap16(&bits,1);
    memcpy(&val,&bits,sizeof(u_int16_t));
    return val;
  }

  inline u_int32_t getbareword(int fin, bool swap=false)
    {
      u_int32_t word;
      ssize_t res=read(fin,&word,sizeof(u_int32_t));
      if( res<=0 ) throw std::logic_error("getbareword() failed");
      if( swap ) p_bin::swap32(&word, 1);
      return word;
    }

  inline u_int32_t getrecord(int fin, bool swap=false)
    {
      return getbareword(fin,swap);
    }

  inline u_int32_t getword(int fin, bool swap=false)
    {
      u_int32_t recbgn=getrecord(fin,swap);
      size_t words=recbgn/sizeof(u_int32_t);
      if( words != 1 ) throw std::runtime_error("getword(): [rec>4] => larger than one word to read");
      u_int32_t res = getbareword(fin,swap);
      u_int32_t recend=getrecord(fin,swap);
      if( recbgn != recend ) throw std::runtime_error("getword(): rec bgn/end mismatch");
      return res;
    }

  template <typename T> inline void getbare(int fin, T* buf,
					    size_t bytes, bool swap=false)
    {
      const size_t MAXBYTE=2147483648L;
      size_t bytesRead=0,bytesLeft=bytes;
      unsigned char* dst=reinterpret_cast<unsigned char*>(buf);
      while( bytesLeft ){
	size_t req = bytesLeft>MAXBYTE ? MAXBYTE : bytesLeft;
	ssize_t res = read(fin,&dst[bytesRead],req);
	if( res <= 0) throw std::runtime_error("getbare() failed");
	bytesLeft -= res;
	bytesRead += res;
      }

      if( swap ) {
	size_t N=bytes/sizeof(T);
	for(size_t i=0;i<N;++i)
      buf[i]=p_bin::swap(buf[i]);
      }
    }
};

#endif
