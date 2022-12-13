#ifndef _stream_formats_h_
#define _stream_formats_h_

#define INT_AS_HEX(intVar) std::setbase(16) << std::setfill('0') << std::setw(sizeof(intVar) * 2) << intVar << std::setbase(10)
#define SEQ_IDX(intVar) std::setfill(' ') << std::setw(3) << intVar << ". "
#define HIGH_PREC(doubleVar) std::setfill(' ') << std::setw(16) << std::setprecision(12) << doubleVar

#endif _stream_formats_h_
