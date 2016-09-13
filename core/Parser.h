// -*- C++ -*-
// 

#pragma once
#include "base.h"

namespace ACT
{
   struct ParserBase
   {
      ParserBase () : partialReadCount (0) { }
      virtual ~ParserBase () { }

      u8 partialReadBuffer [128]; // partial read buffer
      u32 partialReadCount;
   };

   // Parser, binary
   struct BinParser : public ParserBase
   {
      struct Observer
      {
         virtual ~Observer () { }
         virtual void onMessage (u32 msgLen, const u8* data) = 0;
      };

      enum class State
      {
         RLB1, // reading length byte 1
         RLB2, // reading length byte 2
         RMSG // reading msg...
      };

      BinParser (Observer & o) :
         observer (o),
         state (State::RLB1),
         msgLen (0),
         msgBytesRead (0) { }

      void parse (const u8*, u32);

      Observer & observer;

      State state;
      u32 msgLen; // msg length, read from header
      u32 msgBytesRead; // no of bytes read, ("current message")
   };

   // Parser, ascii/text
   struct TextParser : public ParserBase
   {
      struct Observer
      {
         virtual ~Observer () { }
         virtual void onMessage (const u8* data, u32 len) = 0;
      };

      TextParser (Observer & o) : observer (o) { }
      void parse (const u8*, u32);

      Observer & observer;
   };
   
}
